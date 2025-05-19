#ifndef RPL_MQOS_H
#define RPL_MQOS_H

#include "net/routing/rpl-lite/rpl.h"
#include "net/routing/rpl-lite/rpl-conf.h"
#include "net/routing/rpl-lite/rpl-types.h"
#include "net/routing/rpl-lite/rpl-timers.h"
#include "net/routing/rpl-lite/rpl-dag.h"
#include "net/routing/rpl-lite/rpl-dag-root.h"
#include "net/routing/rpl-lite/rpl-neighbor.h"
#include "net/routing/rpl-lite/rpl-ext-header.h"
#include "net/routing/rpl-lite/rpl-icmp6.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-icmp6.h"
#include "sys/ctimer.h"
#include "sys/log.h"

/* Définition des classes de trafic */
#define RPL_MQOS_CLASS_CRITICAL   0
#define RPL_MQOS_CLASS_PRIORITY   1
#define RPL_MQOS_CLASS_NORMAL     2
#define RPL_MQOS_CLASS_BACKGROUND 3

/* Structure pour les métriques QoS */
typedef struct rpl_mqos_metrics {
  uint16_t etx;
  uint16_t delay;
  uint16_t jitter;
  uint8_t traffic_class;
} rpl_mqos_metrics_t;

/* Structure pour les poids des métriques */
typedef struct rpl_mqos_weights {
  uint8_t etx_weight;
  uint8_t delay_weight;
  uint8_t jitter_weight;
} rpl_mqos_weights_t;

/* Fonctions principales */
void rpl_mqos_init(void);
void rpl_mqos_update_metrics(rpl_parent_t *parent);
void rpl_mqos_update_weights(uint8_t traffic_class);
rpl_rank_t rpl_mqos_calculate_rank(rpl_parent_t *parent, rpl_rank_t base_rank);
uint8_t rpl_mqos_get_traffic_class(void);
void rpl_mqos_set_traffic_class(uint8_t traffic_class);

/* Fonctions de gestion des métriques */
uint16_t rpl_mqos_get_etx(rpl_parent_t *parent);
uint16_t rpl_mqos_get_delay(rpl_parent_t *parent);
uint16_t rpl_mqos_get_jitter(rpl_parent_t *parent);
void rpl_mqos_set_etx(rpl_parent_t *parent, uint16_t etx);
void rpl_mqos_set_delay(rpl_parent_t *parent, uint16_t delay);
void rpl_mqos_set_jitter(rpl_parent_t *parent, uint16_t jitter);

/* Fonctions de gestion des poids */
uint8_t rpl_mqos_get_etx_weight(uint8_t traffic_class);
uint8_t rpl_mqos_get_delay_weight(uint8_t traffic_class);
uint8_t rpl_mqos_get_jitter_weight(uint8_t traffic_class);
void rpl_mqos_set_etx_weight(uint8_t traffic_class, uint8_t weight);
void rpl_mqos_set_delay_weight(uint8_t traffic_class, uint8_t weight);
void rpl_mqos_set_jitter_weight(uint8_t traffic_class, uint8_t weight);

/* Fonctions de gestion des voisins */
void rpl_mqos_neighbor_link_callback(rpl_parent_t *parent, int status, int numtx);
void rpl_mqos_neighbor_path_callback(rpl_parent_t *parent, int status);

#endif /* RPL_MQOS_H */ 