/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/network/iota_packet.h"

void iota_packet_build(iota_packet_t* const packet, size_t const length,
                       char const* const host, uint16_t const port,
                       protocol_type_t const protocol) {
  if (packet == NULL) {
    return;
  }
  packet->length = length;
  if (host != NULL) {
    strcpy(packet->source.host, host);
  }
  packet->source.port = port;
  packet->source.protocol = protocol;
}

void iota_packet_set_transaction(iota_packet_t* const packet,
                                 iota_transaction_t const tx) {
  flex_trit_t tx_flex_trits[FLEX_TRIT_SIZE_8019];
  trit_t tx_trits[NUM_TRITS_SERIALIZED_TRANSACTION];

  if (packet == NULL || tx == NULL) {
    return;
  }

  // Tracked via #166: Efficient flex_trit_array_to_bytes
  transaction_serialize_on_flex_trits(tx, tx_flex_trits);
  flex_trit_array_to_int8(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                          tx_flex_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                          NUM_TRITS_SERIALIZED_TRANSACTION);
  trits_to_bytes(tx_trits, packet->content, NUM_TRITS_SERIALIZED_TRANSACTION);
}