/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openair1/PHY/TOOLS/tools_defs.h"
#include "openair1/PHY/MODULATION/phy_ofdm_mod.h"

static int check(int n, int cp, int offset, int pattern)
{
  enum { symbols = 3, guard = 16 };
  int *input = NULL, *actual_base = NULL, *expected = NULL, *transformed = NULL;
  const size_t count = symbols * (n + cp);
  if (posix_memalign((void **)&input, 64, symbols * n * sizeof(int)) ||
      posix_memalign((void **)&actual_base, 64, (count + guard + offset) * sizeof(int)) ||
      posix_memalign((void **)&expected, 64, count * sizeof(int)) ||
      posix_memalign((void **)&transformed, 64, n * sizeof(int)))
    abort();
  memset(input, 0, symbols * n * sizeof(int));
  memset(actual_base, 0x5a, (count + guard + offset) * sizeof(int));
  if (pattern == 1) {
    input[n] = 0x01000200;
    input[2 * n - 1] = 0xff00fe00;
  } else if (pattern == 2) {
    for (int i = 0; i < symbols * n; ++i)
      input[i] = (int)((unsigned)(i * 37 % 1024) | ((unsigned)(i * 71 % 1024) << 16));
  } else if (pattern == 3) {
    input[symbols * n - 1] = 1;
  }
  int *actual = actual_base + offset;
  for (int s = 0; s < symbols; ++s) {
    idft(get_idft(n), (int16_t *)(input + s * n), (int16_t *)transformed, 1);
    memcpy(expected + s * (n + cp), transformed + n - cp, cp * sizeof(int));
    memcpy(expected + s * (n + cp) + cp, transformed, n * sizeof(int));
  }
  PHY_ofdm_mod(input, actual, n, symbols, cp, CYCLIC_PREFIX);
  int fail = memcmp(expected, actual, count * sizeof(int)) != 0;
  for (int k = 0; k < offset; ++k)
    fail |= actual_base[k] != 0x5a5a5a5a;
  for (int k = 0; k < guard; ++k)
    fail |= actual[count + k] != 0x5a5a5a5a;
  if (fail)
    fprintf(stderr, "OFDM mismatch n=%d cp=%d offset=%d pattern=%d\n", n, cp, offset, pattern);
  free(input); free(actual_base); free(expected); free(transformed);
  return fail;
}

int main(void)
{
  load_dftslib();
  const int sizes[] = {128, 256, 512, 1024, 2048};
  int failures = 0, cases = 0;
  for (unsigned i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i)
    for (int long_cp = 0; long_cp < 2; ++long_cp)
      for (int offset = 0; offset < 4; ++offset)
        for (int pattern = 0; pattern < 4; ++pattern) {
          failures += check(sizes[i], sizes[i] / 16 + long_cp * 4, offset, pattern);
          cases++;
        }
  printf("OFDM zero/mixed/occupied symbols: %d cases, %d failures\n", cases, failures);
  return failures != 0;
}
