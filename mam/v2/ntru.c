
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
\file ntru.c
\brief MAM2 NTRU layer.
*/
#include "mam/v2/ntru.h"

MAM2_INLINE MAM2_SAPI trits_t ntru_id_trits(intru *n) {
  return trits_from_rep(MAM2_NTRU_ID_SIZE, n->id);
}

MAM2_INLINE MAM2_SAPI trits_t ntru_sk_trits(intru *n) {
  return trits_from_rep(MAM2_NTRU_SK_SIZE, n->sk);
}

MAM2_SAPI void ntru_gen(intru *n, iprng *p, trits_t N, trits_t pk) {}

MAM2_SAPI void ntru_encr(trits_t pk, iprng *p, trits_t K, trits_t N,
                         trits_t Y) {}

MAM2_SAPI bool_t ntru_decr(intru *n, trits_t Y, trits_t K) { return 0; }
