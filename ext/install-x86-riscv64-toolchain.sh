#!/usr/bin/env bash
# Install an x86_64-hosted RISC-V cross compiler and target sysroot entirely
# below ext/toolchain. Nothing is installed into /usr.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PREFIX="${OAI_CROSS_PREFIX:-$ROOT/ext/toolchain/x86_64-riscv64}"
SYSROOT="$PREFIX/sysroot"
DOWNLOADS="$PREFIX/downloads"
APTROOT="$PREFIX/apt-riscv64"
ASN_SRC="$ROOT/ext/asn1c/source-oai-pinned"
ASN_BUILD="$ROOT/ext/asn1c/build-host-x86_64"
ASN_INSTALL="$ROOT/ext/asn1c/install-host-x86_64"
ASN_PATCH="$ROOT/ext/asn1c/asn1c-rv64-anonymous-exprs.patch"

die() { printf 'ERROR: %s\n' "$*" >&2; exit 1; }
[[ "$(uname -m)" == x86_64 ]] || die "Run this installer on an x86_64 Linux host."
for cmd in apt-get apt-cache dpkg-deb git patch make autoreconf gcc g++ cmake ninja pkg-config; do
  command -v "$cmd" >/dev/null || die "Missing x86 host utility: $cmd"
done

mkdir -p "$PREFIX" "$SYSROOT" "$DOWNLOADS/host" "$DOWNLOADS/target"

printf 'Resolving x86-hosted cross compiler packages...\n'
host_roots=(binutils-riscv64-linux-gnu gcc-riscv64-linux-gnu g++-riscv64-linux-gnu cpp-riscv64-linux-gnu qemu-user-static)
mapfile -t host_packages < <(
  apt-cache depends --recurse --no-recommends --no-suggests \
    --no-conflicts --no-breaks --no-replaces --no-enhances "${host_roots[@]}" \
    | sed -n 's/^[[:space:]]*Depends: //p; /^[[:alnum:]][[:alnum:].+:-]*$/p' \
    | sed 's/:any$//' | grep -v '^<' | sort -u
)
(
  cd "$DOWNLOADS/host"
  apt-get download "${host_packages[@]}"
)
for deb in "$DOWNLOADS/host"/*.deb; do dpkg-deb -x "$deb" "$PREFIX"; done

printf 'Downloading the Ubuntu 24.04 riscv64 target sysroot...\n'
mkdir -p "$APTROOT/etc/apt" "$APTROOT/var/lib/apt/lists/partial" \
  "$APTROOT/var/cache/apt/archives/partial" "$APTROOT/var/lib/dpkg"
: > "$APTROOT/var/lib/dpkg/status"
cat > "$APTROOT/etc/apt/sources.list" <<'EOF'
deb [arch=riscv64] http://ports.ubuntu.com/ubuntu-ports noble main universe
deb [arch=riscv64] http://ports.ubuntu.com/ubuntu-ports noble-updates main universe
deb [arch=riscv64] http://ports.ubuntu.com/ubuntu-ports noble-security main universe
EOF
apt_opts=(
  -o "Dir::Etc::sourcelist=$APTROOT/etc/apt/sources.list"
  -o "Dir::Etc::sourceparts=-"
  -o "Dir::State::status=$APTROOT/var/lib/dpkg/status"
  -o "Dir::State::lists=$APTROOT/var/lib/apt/lists"
  -o "Dir::Cache::archives=$DOWNLOADS/target"
  -o APT::Architecture=riscv64
  -o APT::Architectures=riscv64
)
apt-get "${apt_opts[@]}" update
target_packages=(
  libc6-dev linux-libc-dev libstdc++-13-dev
  libconfig++-dev libsctp-dev libfftw3-dev libusb-1.0-0-dev libuhd-dev
  libboost-dev libboost-system-dev libboost-thread-dev
  libboost-program-options-dev libboost-filesystem-dev
  libssl-dev libyaml-cpp-dev libblas-dev liblapacke-dev
  libreadline-dev zlib1g-dev
)
apt-get "${apt_opts[@]}" -y --download-only --no-install-recommends install "${target_packages[@]}"
for deb in "$DOWNLOADS/target"/*.deb; do dpkg-deb -x "$deb" "$SYSROOT"; done

real_gcc="$(find "$PREFIX/usr/bin" -maxdepth 1 -type f -name 'riscv64-linux-gnu-gcc-[0-9]*' | sort -V | tail -1)"
real_gxx="$(find "$PREFIX/usr/bin" -maxdepth 1 -type f -name 'riscv64-linux-gnu-g++-[0-9]*' | sort -V | tail -1)"
[[ -x "$real_gcc" && -x "$real_gxx" ]] || die "Cross GCC/G++ was not extracted correctly."
mkdir -p "$PREFIX/bin"
cat > "$PREFIX/bin/gcc" <<EOF
#!/usr/bin/env bash
exec "$real_gcc" --sysroot="$SYSROOT" "\$@"
EOF
cat > "$PREFIX/bin/g++" <<EOF
#!/usr/bin/env bash
exec "$real_gxx" --sysroot="$SYSROOT" "\$@"
EOF
cat > "$PREFIX/bin/pkg-config" <<EOF
#!/usr/bin/env bash
export PKG_CONFIG_SYSROOT_DIR="$SYSROOT"
export PKG_CONFIG_LIBDIR="$SYSROOT/usr/lib/riscv64-linux-gnu/pkgconfig:$SYSROOT/usr/lib/pkgconfig:$SYSROOT/usr/share/pkgconfig"
exec "$(command -v pkg-config)" "\$@"
EOF
chmod +x "$PREFIX/bin/gcc" "$PREFIX/bin/g++" "$PREFIX/bin/pkg-config"
for tool in ar as ld nm objcopy objdump ranlib readelf size strings strip; do
  src="$PREFIX/usr/bin/riscv64-linux-gnu-$tool"
  [[ ! -x "$src" ]] || ln -sfn "../usr/bin/riscv64-linux-gnu-$tool" "$PREFIX/bin/riscv64-linux-gnu-$tool"
done
qemu="$(find "$PREFIX" -type f -name 'qemu-riscv64-static' | head -1 || true)"
[[ -z "$qemu" ]] || ln -sfn "../${qemu#$PREFIX/}" "$PREFIX/bin/qemu-riscv64-static"

cat > "$PREFIX/oai-riscv64-toolchain.cmake" <<EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(CMAKE_SYSROOT "$SYSROOT")
set(CMAKE_C_COMPILER "$PREFIX/bin/gcc")
set(CMAKE_CXX_COMPILER "$PREFIX/bin/g++")
set(CMAKE_ASM_COMPILER "$PREFIX/bin/gcc")
set(CMAKE_AR "$PREFIX/bin/riscv64-linux-gnu-ar")
set(CMAKE_RANLIB "$PREFIX/bin/riscv64-linux-gnu-ranlib")
set(CMAKE_STRIP "$PREFIX/bin/riscv64-linux-gnu-strip")
set(CMAKE_FIND_ROOT_PATH "$SYSROOT")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
EOF
if [[ -x "$PREFIX/bin/qemu-riscv64-static" ]]; then
  printf 'set(CMAKE_CROSSCOMPILING_EMULATOR "%s;-L;%s")\n' \
    "$PREFIX/bin/qemu-riscv64-static" "$SYSROOT" >> "$PREFIX/oai-riscv64-toolchain.cmake"
fi

printf 'Building the ASN.1 generator for the x86 host...\n'
git -C "$ROOT" submodule update --init --depth 1 ext/asn1c/source-oai-pinned
git -C "$ASN_SRC" checkout --detach 940dd5fa9f3917913fd487b13dfddfacd0ded06e
if ! grep -q 'tmparg.expr == NULL' "$ASN_SRC/libasn1fix/asn1fix.c"; then
  patch -p1 -d "$ASN_SRC" < "$ASN_PATCH"
fi
(
  cd "$ASN_SRC"
  autoreconf -iv
)
mkdir -p "$ASN_BUILD"
(
  cd "$ASN_BUILD"
  CFLAGS='-O2 -fno-strict-aliasing' "$ASN_SRC/configure" --prefix="$ASN_INSTALL" --disable-shared
  make -j"$(nproc)"
  make install
)

"$PREFIX/bin/gcc" -dumpmachine | grep -q '^riscv64' || die "Cross compiler self-check failed."
"$ASN_INSTALL/bin/asn1c" -version
printf '\nCross environment ready:\n  compiler: %s\n  sysroot:  %s\n  asn1c:    %s\n' \
  "$PREFIX/bin/gcc" "$SYSROOT" "$ASN_INSTALL/bin/asn1c"
printf 'Next: ./scripts/build-oai-native.sh\n'
