/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_CONF_H__
#define __GOSSIP_CONF_H__

#include <stdint.h>

#include "common/errors.h"
#include "common/trinary/trit_byte.h"

#define PACKET_TX_SIZE 1604
#define PACKET_SIZE (PACKET_TX_SIZE + REQUEST_HASH_SIZE)

#define DEFAULT_UDP_RECEIVER_PORT 14600
#define DEFAULT_TCP_RECEIVER_PORT 15600
#define DEFAULT_MWN MWM
#define DEFAULT_NEIGHBORS NULL
#define DEFAULT_PROBABILITY_SELECT_MILESTONE 0.7
#define DEFAULT_PROBABILITY_REMOVE_REQUEST 0.01

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate the
// IOTA gossip protocol
typedef struct iota_gossip_conf_s {
  // UDP listen port
  uint16_t udp_receiver_port;
  // TCP listen port
  uint16_t tcp_receiver_port;
  // Number of trailing ternary 0s that must appear at the end of a transaction
  // hash. Difficulty can be described as 3^mwm
  uint8_t mwm;
  // Size of the request hash in trits, equivalent to HASH_LENGTH_TRIT - mwm.
  uint8_t request_hash_size_trit;
  // URIs of neighbouring nodes, separated by a space
  char* neighbors;
  // Probability of sending a current milestone request to a neighbour. Value
  // must be in [0,1]
  double p_select_milestone;
  // Probability of removing a transaction from the request queue without
  // requesting it. Value must be in [0,1]
  double p_remove_request;
} iota_gossip_conf_t;

/**
 * Initializes gossip configuration with default values
 *
 * @param conf Gossip configuration variables
 *
 * @return a status code
 */
retcode_t iota_gossip_conf_init(iota_gossip_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_CONF_H__
