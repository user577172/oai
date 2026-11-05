/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#ifndef OAI_PHY_MODULE_TIMING_H
#define OAI_PHY_MODULE_TIMING_H

#include "common/utils/time_meas.h"

typedef enum {
  PHY_TIME_DLSCH_ENCODING,
  PHY_TIME_UL_SEGMENTS_DECODING,
  PHY_TIME_PUSCH_CHANNEL_ESTIMATION,
  PHY_TIME_PUSCH_SYMBOL_PROCESSING,
  PHY_TIME_SYMBOL_DFT_RX,
  PHY_TIME_PUSCH_CHANNEL_MEASUREMENT,
  PHY_TIME_PUSCH_LLR_COMPUTATION,
  PHY_TIME_PUSCH_INNER_RECEIVER,
  PHY_TIME_LDPC_CUDA,
  PHY_TIME_MODULE_COUNT
} phy_timing_module_t;

typedef struct {
  uint64_t calls;
  uint64_t total_ns;
  uint64_t max_ns;
} phy_timing_snapshot_t;

/* Process-wide cumulative counters; writers never reset another worker's data. */
void oai_phy_timing_add(phy_timing_module_t module, uint64_t total_ns, uint64_t calls, uint64_t max_ns);
phy_timing_snapshot_t oai_phy_timing_snapshot(phy_timing_module_t module);

static inline uint64_t oai_phy_timing_start(void)
{
  return cpu_meas_enabled ? (uint64_t)clock_gettime_oai() : 0;
}

static inline void oai_phy_timing_stop(phy_timing_module_t module, uint64_t start_ns)
{
  if (start_ns != 0) {
    uint64_t elapsed = (uint64_t)clock_gettime_oai() - start_ns;
    oai_phy_timing_add(module, elapsed, 1, elapsed);
  }
}
#endif
