# XSAI RVV PHY integration

Source: `/home/ubuntu/XSAI_AIRAN/vendor/openairinterface5g`, commit `58194a5` of the parent repository.
Target: `/home/ubuntu/openairinterface5g`, branch `riscv`.

The integration carries the RVV LDPC check-node, fused bit-node/message passing,
LLR hard-decision packing, 128-bit code generators, and DFT/IDFT kernels from
XSAI. The LDPC output-pointer ABI remains compatible with the target OAI version.
Generator dependencies and their declared output directories are included.

The original scalar LDPC implementation is selected by the corresponding scalar include files when the vector extension is disabled. This keeps the no-V path unchanged.

The target OAI version split PHY OFDM modulation into `PHY_ofdm_mod.c`.
The native 2048-point RVV output path is integrated there. Its receive wrapper
already passes unaligned input through to the DFT library. The RVV DFT wrapper
accepts naturally aligned complex samples for size 2048, and retains stricter
alignment for the other kernels.

`oai_dfts_rvv.h` is the source header. `oai_dfts.c` selects the XSAI implementation
for a vector ISA. `oai_dfts_scalar.inc` preserves the target's original non-vector
implementation. The transform-size tables follow the selected backend; the XSAI
backend supports the sizes declared in its original tables.

Build the vector profile with:

```sh
bash scripts/build-oai-rvv.sh dfts ldpc ldpctest rvv_dft_test
```

The separate output directory is `cmake_targets/ran_build/rv64-rvv-release`.
`OAI_ENABLE_RVV=ON` and `-march=rv64gcv_zba` select this backend. The original
no-V launcher and build directory remain separate. To build the vector gNB/UE,
run `bash scripts/build-oai-rvv.sh` without target arguments. This does not start
services or change the running RF configuration.

Verification records are in `/home/ubuntu/rvv-port-coverage.json` and the
`/home/ubuntu/rvv-*.log` files. `rvv.dft` calls the actual loaded DFT library and
checks sparse complex input against a mathematical transform at ten sizes,
including 2048-point buffers aligned to four but not 32 bytes. The LDPC physim
cases exercise the actual encoder and decoder. Numerical/module tests do not
establish RF connectivity or throughput.

## Observed verification results

The RVV module build completed; 21 CTest cases completed with no failures. Four additional LDPC rate cases each decoded 30 blocks with zero block errors. The original scalar gNB linked successfully, and scalar BG1/BG2 checks each decoded ten blocks with zero block errors. The full vector gNB/UE build and RF communication were not exercised.
Evidence: `/home/ubuntu/rvv-port-evidence`.

## Active RF deployment

The no-V build directory and no-V build/launch entry points were removed. Use `scripts/ran-gnb.sh` or `scripts/ran-gnb.sh`. The vector gNB full build and a real n78 B210/Windows UE test completed: UE 12.1.1.130 reached UPF 12.1.1.1 with six of six replies (32–36 ms). Evidence: `/home/ubuntu/rf-evidence-rvv-20261103`.

## Script entry points

`scripts` contains exactly three entry points: `build-oai-rvv.sh` (V), `build-oai-no-v.sh` (no V), and `ran-gnb.sh` (start/check/status/log/stop/restart/recover/stop-all). Running `ran-gnb.sh` without an argument defaults to start; it uses the RVV build and ensures the local core is ready. For a no-V run, stop the existing gNB first and set `K3_OAI_BUILD` to the absolute `cmake_targets/ran_build/rv64-no-v-release` path. The dependency preparation utility is at `tools/k3/prepare-k3-rv64-deps.sh`. No backup scripts remain in `scripts`.
