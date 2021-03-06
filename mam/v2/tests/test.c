#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unity/unity.h>

#include "mam/v2/alloc.h"
#include "mam/v2/buffers.h"
#include "mam/v2/curl.h"
#include "mam/v2/mam.h"
#include "mam/v2/mss.h"
#include "mam/v2/ntru.h"
#include "mam/v2/pb3.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"

#include "mam/v2/tests/common.h"

#define MAM2_WOTS_PK_SIZE 243
#define MAM2_WOTS_SK_PART_SIZE 162
#define MAM2_WOTS_SK_PART_COUNT 81
#define MAM2_WOTS_SK_SIZE (MAM2_WOTS_SK_PART_SIZE * MAM2_WOTS_SK_PART_COUNT)
#define MAM2_WOTS_SIG_SIZE MAM2_WOTS_SK_SIZE

void test_sponge_hash(size_t Xn, char *X, size_t Yn, char *Y) {
  test_sponge_t _s[1];
  isponge *s = test_sponge_init(_s);

  ialloc *a = 0;
  trits_t tX = trits_alloc(a, 3 * Xn);
  trits_t tY = trits_alloc(a, 3 * Yn);

  sponge_init(s);
  trits_from_str(tX, X);
  sponge_absorb(s, MAM2_SPONGE_CTL_DATA, tX);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, tY);
  trits_to_str(tY, Y);

  trits_free(a, tX);
  trits_free(a, tY);
}

void test_sponge_encr(size_t Kn, char *K, size_t Xn, char *X, size_t Yn,
                      char *Y) {
  test_sponge_t _s[1];
  isponge *s = test_sponge_init(_s);

  ialloc *a = 0;
  trits_t tK = trits_alloc(a, 3 * Kn);
  trits_t tX = trits_alloc(a, 3 * Xn);
  trits_t tY = trits_alloc(a, 3 * Yn);

  sponge_init(s);
  trits_from_str(tK, K);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, tK);
  trits_from_str(tX, X);
  sponge_encr(s, tX, tY);
  trits_to_str(tY, Y);

  trits_free(a, tK);
  trits_free(a, tX);
  trits_free(a, tY);
}

void test_sponge_decr(size_t Kn, char *K, size_t Yn, char *Y, size_t Xn,
                      char *X) {
  test_sponge_t _s[1];
  isponge *s = test_sponge_init(_s);

  ialloc *a = 0;
  trits_t tK = trits_alloc(a, 3 * Kn);
  trits_t tY = trits_alloc(a, 3 * Yn);
  trits_t tX = trits_alloc(a, 3 * Xn);

  sponge_init(s);
  trits_from_str(tK, K);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, tK);
  trits_from_str(tY, Y);
  sponge_decr(s, tY, tX);
  trits_to_str(tX, X);

  trits_free(a, tK);
  trits_free(a, tY);
  trits_free(a, tX);
}

void test_prng_gen(size_t Kn, char *K, size_t Nn, char *N, size_t Yn, char *Y) {
  test_sponge_t _s[1];
  test_prng_t _p[1];
  isponge *s = test_sponge_init(_s);
  iprng *p = test_prng_init(_p, s);

  ialloc *a = 0;
  trits_t tK = trits_alloc(a, 3 * Kn);
  trits_t tN = trits_alloc(a, 3 * Nn);
  trits_t tY = trits_alloc(a, 3 * Yn);

  trits_from_str(tK, K);
  prng_init(p, s, tK);
  trits_from_str(tN, N);
  prng_gen(p, 0, tN, tY);
  trits_to_str(tY, Y);

  trits_free(a, tK);
  trits_free(a, tN);
  trits_free(a, tY);
}

void test_wots_gen_sign(size_t Kn, char *K, size_t Nn, char *N, size_t pkn,
                        char *pk, size_t Hn, char *H, size_t sign, char *sig) {
  test_sponge_t _s[1];
  test_prng_t _p[1];
  test_wots_t _w[1];
  isponge *s = test_sponge_init(_s);
  iprng *p = test_prng_init(_p, s);
  iwots *w = test_wots_init(_w, s);

  ialloc *a = 0;
  trits_t tK = trits_alloc(a, 3 * Kn);
  trits_t tN = trits_alloc(a, 3 * Nn);
  trits_t tpk = trits_alloc(a, 3 * pkn);
  trits_t tH = trits_alloc(a, 3 * Hn);
  trits_t tsig = trits_alloc(a, 3 * sign);

  trits_from_str(tK, K);
  prng_init(p, s, tK);
  trits_from_str(tN, N);
  wots_gen_sk(w, p, tN);
  wots_calc_pk(w, tpk);
  trits_to_str(tpk, pk);

  trits_from_str(tH, H);
  wots_sign(w, tH, tsig);
  trits_to_str(tsig, sig);

  trits_free(a, tK);
  trits_free(a, tN);
  trits_free(a, tpk);
  trits_free(a, tH);
  trits_free(a, tsig);
}

void test() {
  char N[12 + 1] = "9ABCKNLMOXYZ";

  char K[81 + 1] =
      "ABCKNLMOXYZ99AZNODFGWERXCVH"
      "XCVHABCKNLMOXYZ99AZNODFGWER"
      "FGWERXCVHABCKNLMOXYZ99AZNOD";

  char H[78 + 1] =
      "BACKNLMOXYZ99AZNODFGWERXCVH"
      "XCVHABCKNLMOXYZ99AZNODFGWER"
      "FGWERXCVHABCKNLMOXYZ99AZ";

  char X[162 + 1] =
      "ABCKNLMOXYZ99AZNODFGWERXCVH"
      "XCVHABCKNLMOXYZ99AZNODFGWER"
      "FGWERXCVHABCKNLMOXYZ99AZNOD"
      "Z99AZNODFGABCKNLMOXYWERXCVH"
      "AZNODFGXCVKNLMOXHABCYZ99WER"
      "FGWERVHABCKXCNLM9AZNODOXYZ9";

  char *sponge_hash_expected =
      "Z9DDCLPZASLK9BCLPZASLKDVICXESYBIWBHJHQYOKIHNXHZDQVCFGDVIUTDVISKTMDG9EMON"
      "OYXPODPWU";
  char *sponge_encr_expected =
      "ABEZQN99JVWYZAZONRCHKUNKUSKSKSKTMDGQN99JVWYZAZONRCHKUNTYKUNKUSKTMDGQN99J"
      "VWYZAZONRNDAAZNODFGABCKNLMOXYWERXCVHAZNODFGXCVKNLMOXHABCYZ99WERFGWERVHAB"
      "CKXCNLM9AZNODOXYZ9";
  char *sponge_decr_expected =
      "ABSNTANLTJBBAAZPPERLQAVSLPEKVPEFUBITANLTJBBAAZPPERLQAVAPQAVSLPEFUBITANLT"
      "JBBAAZPPEKWZAZNODFGABCKNLMOXYWERXCVHAZNODFGXCVKNLMOXHABCYZ99WERFGWERVHAB"
      "CKXCNLM9AZNODOXYZ9";
  char *prng_gen_expected =
      "99MGDGQN99JVWYZZZNODFGWERXCVHXCVHABCKNLMOXYZ99AZNODFGWERFGWERXCVHABCKNLM"
      "OXYZ99AZNOD99ABCKNLMOXYZA99999999999999999999999999999999999999999999999"
      "999999999999999999";

  bool_t r = 1;
  size_t sponge_hash_Yn = 81;
  size_t sponge_encr_Yn = 162;
  size_t sponge_decr_Xn = 162;
  size_t prng_gen_Yn = 162;
  clock_t clk;

  char sponge_hash_Y[81];
  char sponge_encr_Y[162];
  char sponge_decr_X[162];
  char prng_gen_Y[162];
  char wots_gen_sign_pk[MAM2_WOTS_PK_SIZE / 3];
  char wots_gen_sign_sig[MAM2_WOTS_SIG_SIZE / 3];

  test_sponge_hash(162, X, sponge_hash_Yn, sponge_hash_Y);
  test_sponge_encr(81, K, 162, X, sponge_encr_Yn, sponge_encr_Y);
  test_sponge_decr(81, K, 162, X, sponge_decr_Xn, sponge_decr_X);
  test_prng_gen(81, K, 12, N, prng_gen_Yn, prng_gen_Y);
  test_wots_gen_sign(81, K, 12, N, MAM2_WOTS_PK_SIZE / 3, wots_gen_sign_pk, 78,
                     H, MAM2_WOTS_SIG_SIZE / 3, wots_gen_sign_sig);

  TEST_ASSERT_EQUAL_MEMORY(sponge_hash_Y, sponge_hash_expected, 81);
  TEST_ASSERT_EQUAL_MEMORY(sponge_encr_Y, sponge_encr_expected, 162);
  TEST_ASSERT_EQUAL_MEMORY(sponge_decr_X, sponge_decr_expected, 162);
  TEST_ASSERT_EQUAL_MEMORY(prng_gen_Y, prng_gen_expected, 162);
  // FIXME (@tsvisabo) test dec(enc(X)) = X
  // TODO - test/fix wots_gen_sign_sig
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test);

  return UNITY_END();
}