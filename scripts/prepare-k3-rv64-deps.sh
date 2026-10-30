#!/usr/bin/env bash
# Build ASN.1 compiler locally under ext/asn1c; fetch SIMDe headers under ext/simde.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EXT="$ROOT/ext"
ASN_SRC="$EXT/asn1c/source-oai-pinned"
ASN_BUILD="$EXT/asn1c/build-oai-pinned"
ASN_INSTALL="$EXT/asn1c/install-oai-pinned"
ASN_PATCH="$EXT/asn1c/asn1c-rv64-anonymous-exprs.patch"
SIMDE_SRC="$EXT/simde/source"
die() { printf 'ERROR: %s\n' "$*" >&2; exit 1; }
[[ "$(uname -m)" == riscv64 ]] || die "Run this on K3 riscv64; got $(uname -m)."
for cmd in git autoreconf automake autoconf libtoolize make gcc patch; do
  command -v "$cmd" >/dev/null || die "Missing build tool: $cmd (install OS build tools, then retry)."
done
mkdir -p "$EXT/asn1c" "$EXT/simde"
if [[ ! -d "$ASN_SRC/.git" ]]; then
  git clone https://github.com/mouse07410/asn1c.git "$ASN_SRC"
fi
if [[ "$(git -C "$ASN_SRC" rev-parse HEAD)" != 940dd5fa9f3917913fd487b13dfddfacd0ded06e ]]; then
  git -C "$ASN_SRC" fetch --depth 1 origin 940dd5fa9f3917913fd487b13dfddfacd0ded06e
  git -C "$ASN_SRC" checkout --detach 940dd5fa9f3917913fd487b13dfddfacd0ded06e
fi
if ! grep -q 'tmparg.expr == NULL' "$ASN_SRC/libasn1fix/asn1fix.c"; then
  patch -p1 -d "$ASN_SRC" < "$ASN_PATCH"
fi
if [[ ! -d "$SIMDE_SRC/.git" ]]; then
  git clone --depth 1 https://github.com/simd-everywhere/simde-no-tests.git "$SIMDE_SRC"
fi
git -C "$SIMDE_SRC" fetch --depth 1 origin 1c68d9ad60bf63f3fb527c4ee3b2319d828ffcc6
git -C "$SIMDE_SRC" checkout --detach 1c68d9ad60bf63f3fb527c4ee3b2319d828ffcc6
ln -sfn source "$EXT/simde/simde"
cd "$ASN_SRC"
autoreconf -iv
mkdir -p "$ASN_BUILD"
cd "$ASN_BUILD"
CFLAGS="-O0 -march=rv64gc_zba -mabi=lp64d -fno-strict-aliasing" \
LDFLAGS="-march=rv64gc_zba -mabi=lp64d" \
  "$ASN_SRC/configure" --prefix="$ASN_INSTALL" --disable-shared
make -j"$(nproc)"
make install
"$ASN_INSTALL/bin/asn1c" -version
test -f "$EXT/simde/simde/x86/gfni.h"
printf 'Dependencies ready under %s/ext (no /usr or /opt installation performed).\n' "$ROOT"
