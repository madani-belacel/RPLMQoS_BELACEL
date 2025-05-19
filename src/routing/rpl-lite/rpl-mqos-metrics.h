#ifndef RPL_MQOS_METRICS_H
#define RPL_MQOS_METRICS_H

#include "net/routing/rpl-lite/rpl.h"
#include "sys/ctimer.h"

/* Structure pour les métriques QoS */
typedef struct {
  uint16_t etx;           /* Expected Transmission Count */
  uint16_t delay;         /* Délai bout-en-bout en ms */
  uint16_t jitter;        /* Gigue en ms */
  uint8_t reliability;    /* Fiabilité (0-100%) */
  uint16_t bandwidth;     /* Bande passante disponible en kbps */
  uint32_t last_update;   /* Timestamp de dernière mise à jour */
} rpl_mqos_metrics_t;

/* Configuration des métriques */
#define MQOS_METRICS_POOL_SIZE 32
#define MQOS_METRICS_CACHE_SIZE 16
#define MQOS_METRICS_UPDATE_INTERVAL (5 * CLOCK_SECOND)

/* Poids par défaut des métriques */
#define MQOS_DEFAULT_ETX_WEIGHT 4
#define MQOS_DEFAULT_DELAY_WEIGHT 3
#define MQOS_DEFAULT_JITTER_WEIGHT 2
#define MQOS_DEFAULT_RELIABILITY_WEIGHT 3
#define MQOS_DEFAULT_BANDWIDTH_WEIGHT 2

/* Fonctions publiques */
void rpl_mqos_metrics_init(void);
rpl_mqos_metrics_t *rpl_mqos_metrics_alloc(void);
void rpl_mqos_metrics_free(rpl_mqos_metrics_t *metrics);
void rpl_mqos_metrics_update(rpl_parent_t *parent);
uint16_t rpl_mqos_metrics_calculate_rank(const rpl_mqos_metrics_t *metrics);

/* Fonctions de calcul des métriques */
uint16_t rpl_mqos_metrics_calculate_delay(rpl_parent_t *parent);
uint16_t rpl_mqos_metrics_calculate_jitter(rpl_parent_t *parent);
uint8_t rpl_mqos_metrics_calculate_reliability(rpl_parent_t *parent);
uint16_t rpl_mqos_metrics_calculate_bandwidth(rpl_parent_t *parent);

/* Fonctions de cache */
void rpl_mqos_metrics_cache_update(rpl_parent_t *parent);
rpl_mqos_metrics_t *rpl_mqos_metrics_cache_get(rpl_parent_t *parent);

#endif /* RPL_MQOS_METRICS_H */ 