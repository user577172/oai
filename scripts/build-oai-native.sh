#!/usr/bin/env bash
# Build scalar/no-RVV riscv64 OAI either natively on a RISC-V host or with the
# project-local x86_64 -> riscv64 cross environment.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
die() { printf 'ERROR: %s\n' "$*" >&2; exit 1; }
HOST_ARCH="$(uname -m)"
SIMDE_INCLUDE="$ROOT/ext/simde"
PROFILE="${OAI_RISCV_PROFILE:-no-v}"
ARCH="${OAI_RISCV_ARCH:-rv64gc_zba}"
ABI="${OAI_RISCV_ABI:-lp64d}"

[[ "$PROFILE" == no-v ]] || die "Only the no-v build profile is supported."
[[ ! "$ARCH" =~ rv64gcv|_v[0-9]|_zve|_zv[a-z] ]] \
  || die "RISC-V V architecture requested for the no-V build: $ARCH"
[[ -f "$SIMDE_INCLUDE/simde/x86/gfni.h" ]] || die "Project-local SIMDe headers are missing."

case "$HOST_ARCH" in
  riscv64)
    MODE=native
    TOOLCHAIN="$ROOT/ext/toolchain/rv64-gcc-16"
    CC_BIN="$TOOLCHAIN/bin/gcc"
    CXX_BIN="$TOOLCHAIN/bin/g++"
    CMAKE_BIN="$TOOLCHAIN/bin/cmake"
    NINJA_BIN="$TOOLCHAIN/bin/ninja"
    PKG_CONFIG_BIN="$TOOLCHAIN/bin/pkg-config"
    ASN1C="$ROOT/ext/asn1c/install-oai-pinned/bin/asn1c"
    SYSROOT="$TOOLCHAIN/sysroot"
    BUILD="${K3_OAI_BUILD:-$ROOT/cmake_targets/ran_build/rv64-no-v-release}"
    cmake_cross_args=()
    ;;
  x86_64|amd64)
    MODE=cross
    TOOLCHAIN="${OAI_CROSS_PREFIX:-$ROOT/ext/toolchain/x86_64-riscv64}"
    CC_BIN="$TOOLCHAIN/bin/gcc"
    CXX_BIN="$TOOLCHAIN/bin/g++"
    CMAKE_BIN="$(command -v cmake || true)"
    NINJA_BIN="$(command -v ninja || true)"
    PKG_CONFIG_BIN="$TOOLCHAIN/bin/pkg-config"
    ASN1C="$ROOT/ext/asn1c/install-host-x86_64/bin/asn1c"
    SYSROOT="$TOOLCHAIN/sysroot"
    BUILD="${K3_OAI_BUILD:-$ROOT/cmake_targets/ran_build/rv64-cross-no-v-release}"
    cmake_cross_args=(-DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN/oai-riscv64-toolchain.cmake")
    ;;
  *) die "Unsupported build host architecture: $HOST_ARCH" ;;
esac

for tool in "$CC_BIN" "$CXX_BIN" "$CMAKE_BIN" "$NINJA_BIN" "$PKG_CONFIG_BIN" "$ASN1C"; do
  [[ -x "$tool" ]] || die "Required build tool is missing: $tool. Run ext/install-x86-riscv64-toolchain.sh on x86."
done
[[ -d "$SYSROOT/usr/include" ]] || die "Target sysroot is incomplete: $SYSROOT"
[[ "$($CC_BIN -dumpmachine)" == riscv64* ]] || die "$CC_BIN is not a RISC-V compiler."

ISA_FLAGS="-march=$ARCH -mabi=$ABI"
export CFLAGS="${CFLAGS:+$CFLAGS }$ISA_FLAGS -fno-var-tracking-assignments -I$SIMDE_INCLUDE"
export CXXFLAGS="${CXXFLAGS:+$CXXFLAGS }$ISA_FLAGS -fno-var-tracking-assignments -I$SIMDE_INCLUDE"
export ASFLAGS="${ASFLAGS:+$ASFLAGS }$ISA_FLAGS"
export LDFLAGS="${LDFLAGS:+$LDFLAGS }$ISA_FLAGS"
export PKG_CONFIG_SYSROOT_DIR="$SYSROOT"
export PKG_CONFIG_LIBDIR="$SYSROOT/usr/lib/riscv64-linux-gnu/pkgconfig:$SYSROOT/usr/lib/pkgconfig:$SYSROOT/usr/share/pkgconfig"
export PATH="$TOOLCHAIN/bin:$PATH"

printf 'OAI root: %s\nBuild mode: %s (%s host -> riscv64 target)\n' "$ROOT" "$MODE" "$HOST_ARCH"
printf 'RV64 ISA: %s; ABI: %s; profile: %s\nCompiler: %s\nSysroot: %s\nASN.1 compiler: %s\n' \
  "$ARCH" "$ABI" "$PROFILE" "$CC_BIN" "$SYSROOT" "$ASN1C"

"$CMAKE_BIN" -S "$ROOT" -B "$BUILD" -G Ninja \
  "${cmake_cross_args[@]}" \
  -DCMAKE_MAKE_PROGRAM="$NINJA_BIN" \
  -DPKG_CONFIG_EXECUTABLE="$PKG_CONFIG_BIN" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER="$CC_BIN" \
  -DCMAKE_CXX_COMPILER="$CXX_BIN" \
  -DCMAKE_C_FLAGS="$CFLAGS" \
  -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
  -DCMAKE_ASM_FLAGS="$ASFLAGS" \
  -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" \
  -DASN1C_EXEC="$ASN1C" \
  -DOAI_RISCV_ARCH="$ARCH" \
  -DOAI_RISCV_ABI="$ABI" \
  -DOAI_USRP=ON \
  -DENABLE_LDPC_CUDA=OFF \
  -DAVX2=OFF \
  -DAVX512=OFF \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

"$CMAKE_BIN" --build "$BUILD" \
  --target nr-softmodem nr-uesoftmodem params_libconfig dfts -j "$(nproc)"

OBJDUMP="$TOOLCHAIN/bin/riscv64-linux-gnu-objdump"
OBJCOPY="$TOOLCHAIN/bin/riscv64-linux-gnu-objcopy"
READELF="$TOOLCHAIN/bin/riscv64-linux-gnu-readelf"
for cmd in "$OBJDUMP" "$OBJCOPY" "$READELF"; do
  [[ -x "$cmd" ]] || die "Missing project-local ELF tool: $cmd"
done
attr_tmp="$(mktemp -d)"
trap 'rm -rf "$attr_tmp"' EXIT
printf 'int oai_scalar_isa_marker(void) { return 0; }\n' > "$attr_tmp/marker.c"
"$CC_BIN" -c "$attr_tmp/marker.c" $ISA_FLAGS -o "$attr_tmp/marker.o"
"$OBJCOPY" --dump-section ".riscv.attributes=$attr_tmp/scalar.attributes" "$attr_tmp/marker.o"
verify_elf() {
  local elf="$1"
  [[ -f "$elf" ]] || die "Expected build artifact not found: $elf"
  if "$OBJDUMP" -d --no-show-raw-insn "$elf" 2>/dev/null \
      | grep -Eiq '^[[:space:]]*[[:xdigit:]]+:[[:space:]]+v[a-z0-9_.]*([[:space:]]|$)'; then
    die "RISC-V V instruction detected in $elf"
  fi
  "$OBJCOPY" --update-section ".riscv.attributes=$attr_tmp/scalar.attributes" "$elf"
  if "$READELF" -A "$elf" 2>/dev/null | grep -Eiq 'Tag_RISCV_arch:.*(_v[0-9]|_zve|_zv)'; then
    die "RVV ELF attribute remains in $elf"
  fi
  printf 'Verified no-V RV64 artifact: %s\n' "$elf"
}
for artifact in nr-softmodem nr-uesoftmodem libparams_libconfig.so libdfts.so; do
  verify_elf "$BUILD/$artifact"
done
if "$CMAKE_BIN" --build "$BUILD" --target help 2>/dev/null | grep -q oai_usrpdevif; then
  "$CMAKE_BIN" --build "$BUILD" --target oai_usrpdevif -j "$(nproc)"
  [[ ! -f "$BUILD/liboai_device.so" ]] || verify_elf "$BUILD/liboai_device.so"
fi
printf 'Build complete. Artifacts: %s\n' "$BUILD"
