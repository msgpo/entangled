
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 [ITSec Lab]

 *
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file wots.c
\brief MAM2 WOTS layer.
*/
#include "mam/v2/wots.h"

static void wots_calc_pks(isponge *s, trits_t sk_pks, trits_t pk) {
  size_t i, j;
  trits_t sk_part, pks = sk_pks;

  MAM2_ASSERT(trits_size(sk_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sk_part = trits_take(sk_pks, MAM2_WOTS_SK_PART_SIZE);
    sk_pks = trits_drop(sk_pks, MAM2_WOTS_SK_PART_SIZE);

    for (j = 0; j < 26; ++j) sponge_hash(s, sk_part, sk_part);
  }

  sponge_hash(s, pks, pk);
}

static void wots_hash_sign(isponge *s, trits_t sk_sig, trits_t H) {
  size_t i, j;
  trint9_t t = 0;
  trint3_t h;
  trits_t sig_part;

  MAM2_ASSERT(trits_size(sk_sig) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(H) == MAM2_WOTS_HASH_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(H, i * 3));
    t += h;

    for (j = 0; j < 13 + h; ++j) sponge_hash(s, sig_part, sig_part);
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = 0; j < 13 + h; ++j) sponge_hash(s, sig_part, sig_part);
  }
}

static void wots_hash_verify(isponge *s, trits_t sig_pks, trits_t H) {
  size_t i, j;
  trint9_t t = 0;
  trint3_t h;
  trits_t sig_part;

  MAM2_ASSERT(trits_size(sig_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(H) == MAM2_WOTS_HASH_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(H, i * 3));
    t += h;

    for (j = 0; j < 13 - h; ++j) sponge_hash(s, sig_part, sig_part);
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = 0; j < 13 - h; ++j) sponge_hash(s, sig_part, sig_part);
  }
}

trits_t wots_sk_trits(iwots *w) {
  return trits_from_rep(MAM2_WOTS_SK_SIZE, w->sk);
}

MAM2_SAPI void wots_init(iwots *w, isponge *s) {
  MAM2_ASSERT(w);
  MAM2_ASSERT(s);
  w->s = s;
}

MAM2_SAPI void wots_gen_sk(iwots *w, iprng *p, trits_t N) {
  wots_gen_sk3(w, p, N, trits_null(), trits_null());
}

MAM2_SAPI void wots_gen_sk2(iwots *w, iprng *p, trits_t N1, trits_t N2) {
  wots_gen_sk3(w, p, N1, N2, trits_null());
}

MAM2_SAPI void wots_gen_sk3(iwots *w, iprng *p, trits_t N1, trits_t N2,
                            trits_t N3) {
  prng_gen3(p, MAM2_PRNG_DST_WOTSKEY, N1, N2, N3, wots_sk_trits(w));
}

MAM2_SAPI void wots_calc_pk(iwots *w, trits_t pk) {
  MAM2_TRITS_DEF(sk_pks, MAM2_WOTS_SK_SIZE);
  trits_copy(wots_sk_trits(w), sk_pks);
  wots_calc_pks(w->s, sk_pks, pk);
  trits_set_zero(sk_pks);
}

MAM2_SAPI void wots_sign(iwots *w, trits_t H, trits_t sig) {
  trits_copy(wots_sk_trits(w), sig);
  wots_hash_sign(w->s, sig, H);
}

MAM2_SAPI void wots_recover(isponge *s, trits_t H, trits_t sig, trits_t pk) {
  MAM2_TRITS_DEF(sig_pks, MAM2_WOTS_SK_SIZE);

  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  trits_copy(sig, sig_pks);
  wots_hash_verify(s, sig_pks, H);
  sponge_hash(s, sig_pks, pk);
}

MAM2_SAPI bool_t wots_verify(isponge *s, trits_t H, trits_t sig, trits_t pk) {
  MAM2_TRITS_DEF(sig_pk, MAM2_WOTS_PK_SIZE);
  wots_recover(s, H, sig, sig_pk);
  return (0 == trits_cmp_grlex(pk, sig_pk)) ? 1 : 0;
}

MAM2_SAPI err_t wots_create(ialloc *a, iwots *w) {
  err_t e = err_internal_error;
  MAM2_ASSERT(w);
  do {
    memset(w, 0, sizeof(iwots));
    w->sk = mam2_words_alloc(a, MAM2_WORDS(MAM2_WOTS_SK_SIZE));
    err_guard(w->sk, err_bad_alloc);
    e = err_ok;
  } while (0);
  return e;
}

MAM2_SAPI void wots_destroy(ialloc *a, iwots *w) {
  MAM2_ASSERT(w);
  mam2_words_free(a, w->sk);
  w->sk = 0;
}
