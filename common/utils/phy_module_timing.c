/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#include "phy_module_timing.h"
#include <stdatomic.h>

/* Counters are atomic because symbol/LDPC tasks may execute on different workers. */
typedef struct {
  _Atomic uint64_t calls;
  _Atomic uint64_t total_ns;
  _Atomic uint64_t max_ns;
} phy_timing_counters_t;

static phy_timing_counters_t phy_timing_counters[PHY_TIME_MODULE_COUNT];

void oai_phy_timing_add(phy_timing_module_t module, uint64_t total_ns, uint64_t calls, uint64_t max_ns)
{
  if (!cpu_meas_enabled || (unsigned)module >= PHY_TIME_MODULE_COUNT || calls == 0)
    return;
  phy_timing_counters_t *c = &phy_timing_counters[module];
  uint64_t old_max = atomic_load_explicit(&c->max_ns, memory_order_relaxed);
  while (old_max < max_ns && !atomic_compare_exchange_weak_explicit(
             &c->max_ns, &old_max, max_ns, memory_order_relaxed, memory_order_relaxed)) {}
  atomic_fetch_add_explicit(&c->total_ns, total_ns, memory_order_relaxed);
  atomic_fetch_add_explicit(&c->calls, calls, memory_order_relaxed);
}

phy_timing_snapshot_t oai_phy_timing_snapshot(phy_timing_module_t module)
{
  if ((unsigned)module >= PHY_TIME_MODULE_COUNT)
    return (phy_timing_snapshot_t){0};
  const phy_timing_counters_t *c = &phy_timing_counters[module];
  return (phy_timing_snapshot_t){
      .calls = atomic_load_explicit(&c->calls, memory_order_relaxed),
      .total_ns = atomic_load_explicit(&c->total_ns, memory_order_relaxed),
      .max_ns = atomic_load_explicit(&c->max_ns, memory_order_relaxed)};
}
