/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#include "common/utils/phy_module_timing.h"
#include <pthread.h>
#include <stdio.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "failed: %s at %d\n", #c, __LINE__); return 1; } } while (0)

int cpu_meas_enabled;

static void *worker(void *unused)
{
  (void)unused;
  for (int i = 0; i < 10000; ++i)
    oai_phy_timing_add(PHY_TIME_DLSCH_ENCODING, 1000, 1, 1000);
  return NULL;
}

int main(void)
{
  cpu_meas_enabled = 0;
  CHECK(oai_phy_timing_start() == 0);
  oai_phy_timing_stop(PHY_TIME_DLSCH_ENCODING, 0);
  oai_phy_timing_add(PHY_TIME_DLSCH_ENCODING, 100, 1, 100);
  CHECK(oai_phy_timing_snapshot(PHY_TIME_DLSCH_ENCODING).calls == 0);
  cpu_meas_enabled = 1;
  pthread_t threads[4];
  for (unsigned i = 0; i < 4; ++i)
    CHECK(pthread_create(&threads[i], NULL, worker, NULL) == 0);
  for (unsigned i = 0; i < 4; ++i)
    CHECK(pthread_join(threads[i], NULL) == 0);
  phy_timing_snapshot_t s = oai_phy_timing_snapshot(PHY_TIME_DLSCH_ENCODING);
  CHECK(s.calls == 40000 && s.total_ns == 40000000 && s.max_ns == 1000);
  CHECK(oai_phy_timing_snapshot(PHY_TIME_PUSCH_LLR_COMPUTATION).calls == 0);
  oai_phy_timing_add(PHY_TIME_UL_SEGMENTS_DECODING, 450, 3, 200);
  s = oai_phy_timing_snapshot(PHY_TIME_UL_SEGMENTS_DECODING);
  CHECK(s.calls == 3 && s.total_ns == 450 && s.max_ns == 200);
  uint64_t start = oai_phy_timing_start();
  struct timespec delay = {.tv_nsec = 2000000};
  nanosleep(&delay, NULL);
  oai_phy_timing_stop(PHY_TIME_SYMBOL_DFT_RX, start);
  s = oai_phy_timing_snapshot(PHY_TIME_SYMBOL_DFT_RX);
  CHECK(s.calls == 1 && s.total_ns >= 1000000 && s.total_ns == s.max_ns);
  CHECK(oai_phy_timing_snapshot(PHY_TIME_MODULE_COUNT).calls == 0);
  puts("timing: disabled, concurrent workers, batch merge, monotonic clock and isolation passed");
  return 0;
}
