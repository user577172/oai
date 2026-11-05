/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "openair1/PHY/TOOLS/tools_defs.h"

static int check_transform(int n, int inverse, int offset)
{
  void *in_base = NULL, *out_base = NULL;
  if (posix_memalign(&in_base, 64, (n + 32) * sizeof(c16_t)) ||
      posix_memalign(&out_base, 64, (n + 32) * sizeof(c16_t)))
    return 1;
  memset(in_base, 0, (n + 32) * sizeof(c16_t));
  memset(out_base, 0, (n + 32) * sizeof(c16_t));
  c16_t *in = (c16_t *)in_base + offset;
  c16_t *out = (c16_t *)out_base + offset;
  const int positions[] = {1, 7, n - 3};
  const c16_t values[] = {{1000, -700}, {-500, 900}, {300, 200}};
  for (int j = 0; j < 3; ++j)
    in[positions[j]] = values[j];
  if (inverse)
    idft(get_idft(n), (int16_t *)in, (int16_t *)out, 1);
  else
    dft(get_dft(n), (int16_t *)in, (int16_t *)out, 1);
  double max_error = 0;
  for (int k = 0; k < n; ++k) {
    double re = 0, im = 0;
    for (int j = 0; j < 3; ++j) {
      const double phase = (inverse ? 1 : -1) * 2 * M_PI * k * positions[j] / n;
      re += values[j].r * cos(phase) - values[j].i * sin(phase);
      im += values[j].r * sin(phase) + values[j].i * cos(phase);
    }
    max_error = fmax(max_error, fabs(out[k].r - re / sqrt(n)));
    max_error = fmax(max_error, fabs(out[k].i - im / sqrt(n)));
  }
  printf("%s N=%d offset=%d max_error=%.3f\n", inverse ? "IDFT" : "DFT", n, offset, max_error);
  free(in_base);
  free(out_base);
  return max_error > 12;
}

int main(void)
{
  load_dftslib();
  const int sizes[] = {64, 128, 256, 512, 768, 1024, 1536, 2048, 3072, 4096};
  int errors = 0;
  for (unsigned i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i)
    for (int inverse = 0; inverse < 2; ++inverse)
      errors += check_transform(sizes[i], inverse, 0);
  errors += check_transform(2048, 0, 1);
  errors += check_transform(2048, 1, 1);
  return errors != 0;
}
