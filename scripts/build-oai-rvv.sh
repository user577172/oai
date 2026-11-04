#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLCHAIN="$ROOT/ext/toolchain/rv64-gcc-16"
BUILD="$ROOT/cmake_targets/ran_build/rv64-rvv-release"
ARCH="${OAI_RISCV_ARCH:-rv64gcv_zba}"
export PATH="$TOOLCHAIN/bin:$PATH"
export PKG_CONFIG_SYSROOT_DIR="$TOOLCHAIN/sysroot"
export PKG_CONFIG_LIBDIR="$TOOLCHAIN/sysroot/usr/lib/riscv64-linux-gnu/pkgconfig:$TOOLCHAIN/sysroot/usr/share/pkgconfig"
FLAGS="-march=$ARCH -mabi=lp64d -fno-var-tracking-assignments -I$ROOT/ext/simde"
taskset -c "${OAI_BUILD_CPUS:-0-7}" "$TOOLCHAIN/bin/cmake" -S "$ROOT" -B "$BUILD" -G Ninja \
  -DCMAKE_MAKE_PROGRAM="$TOOLCHAIN/bin/ninja" \
  -DCMAKE_C_COMPILER="$TOOLCHAIN/bin/gcc" \
  -DCMAKE_CXX_COMPILER="$TOOLCHAIN/bin/g++" \
  -DPKG_CONFIG_EXECUTABLE="$TOOLCHAIN/bin/pkg-config" \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="$FLAGS" -DCMAKE_CXX_FLAGS="$FLAGS" \
  -DCMAKE_EXE_LINKER_FLAGS="-march=$ARCH -mabi=lp64d" \
  -DASN1C_EXEC="$ROOT/ext/asn1c/install-oai-pinned/bin/asn1c" \
  -DOAI_RISCV_ARCH="$ARCH" -DOAI_RISCV_ABI=lp64d -DOAI_ENABLE_RVV=ON \
  -DOAI_USRP=ON -DENABLE_LDPC_CUDA=OFF -DAVX2=OFF -DAVX512=OFF \
  -DENABLE_PHYSIM_TESTS=ON -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if (( $# == 0 )); then set -- nr-softmodem nr-uesoftmodem params_libconfig oai_usrpdevif dfts ldpc ldpctest; fi
taskset -c "${OAI_BUILD_CPUS:-0-7}" "$TOOLCHAIN/bin/cmake" --build "$BUILD" --target "$@" -j "${OAI_BUILD_JOBS:-8}"
