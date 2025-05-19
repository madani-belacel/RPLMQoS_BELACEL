#include "rpl-mqos.h"
#include "rpl-of-mqos.h"
#include "rpl-mqos-conf.h"

#define LOG_MODULE "RPL-MQoS"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Variables globales */
static uint8_t current_traffic_class;
static rpl_mqos_weights_t weights[MQOS_CONF_NUM_CLASSES];

/* Pool de mémoire pour les métriques */
#define MQOS_METRICS_POOL_SIZE 16
static rpl_mqos_metrics_t metrics_pool[MQOS_METRICS_POOL_SIZE];
static uint8_t metrics_pool_index = 0;

/* Structure pour la récupération */
typedef struct {
  uint8_t retry_count;
  uint8_t max_retries;
  uint16_t timeout;
} mqos_recovery_t;

/* Allocation optimisée des métriques */
static inline rpl_mqos_metrics_t *
mqos_metrics_alloc(void)
{
  rpl_mqos_metrics_t *metrics = &metrics_pool[metrics_pool_index];
  metrics_pool_index = (metrics_pool_index + 1) % MQOS_METRICS_POOL_SIZE;
  return metrics;
}

/* Optimisation du calcul pondéré */
static inline uint32_t
calculate_weighted_rank_optimized(const rpl_mqos_metrics_t *metrics, const rpl_mqos_weights_t *weights)
{
  /* Utilisation de décalages binaires pour la division */
  uint32_t total_weight = weights->etx_weight + weights->delay_weight + weights->jitter_weight;
  uint32_t shift = 0;
  
  /* Calcul du décalage optimal */
  while(total_weight > 1) {
    total_weight >>= 1;
    shift++;
  }
  
  /* Calcul optimisé avec décalages */
  return ((metrics->etx * weights->etx_weight +
           metrics->delay * weights->delay_weight +
           metrics->jitter * weights->jitter_weight) >> shift);
}

/* Gestion de la récupération */
static void
mqos_handle_recovery(rpl_parent_t *parent, mqos_recovery_t *recovery)
{
  if(recovery->retry_count < recovery->max_retries) {
    recovery->retry_count++;
    /* Tentative de récupération */
    rpl_mqos_update_metrics(parent);
  } else {
    /* Échec de récupération */
    LOG_WARN("Recovery failed for parent %u\n", parent->dag->instance->instance_id);
  }
}

/* Initialisation du module MQoS */
void
rpl_mqos_init(void)
{
  /* Initialisation des poids par défaut */
  weights[RPL_MQOS_CLASS_CRITICAL] = (rpl_mqos_weights_t){
    .etx_weight = MQOS_CONF_ETX_WEIGHT_CRITICAL,
    .delay_weight = MQOS_CONF_DELAY_WEIGHT_CRITICAL,
    .jitter_weight = MQOS_CONF_JITTER_WEIGHT_CRITICAL
  };
  
  weights[RPL_MQOS_CLASS_PRIORITY] = (rpl_mqos_weights_t){
    .etx_weight = MQOS_CONF_ETX_WEIGHT_PRIORITY,
    .delay_weight = MQOS_CONF_DELAY_WEIGHT_PRIORITY,
    .jitter_weight = MQOS_CONF_JITTER_WEIGHT_PRIORITY
  };
  
  weights[RPL_MQOS_CLASS_NORMAL] = (rpl_mqos_weights_t){
    .etx_weight = MQOS_CONF_ETX_WEIGHT_NORMAL,
    .delay_weight = MQOS_CONF_DELAY_WEIGHT_NORMAL,
    .jitter_weight = MQOS_CONF_JITTER_WEIGHT_NORMAL
  };
  
  weights[RPL_MQOS_CLASS_BACKGROUND] = (rpl_mqos_weights_t){
    .etx_weight = MQOS_CONF_ETX_WEIGHT_BACKGROUND,
    .delay_weight = MQOS_CONF_DELAY_WEIGHT_BACKGROUND,
    .jitter_weight = MQOS_CONF_JITTER_WEIGHT_BACKGROUND
  };
  
  /* Classe de trafic par défaut */
  current_traffic_class = RPL_MQOS_CLASS_NORMAL;
  
  /* Initialisation du pool de métriques */
  memset(metrics_pool, 0, sizeof(metrics_pool));
  metrics_pool_index = 0;
  
  LOG_INFO("RPL MQoS initialized\n");
}

/* Mise à jour des métriques */
void
rpl_mqos_update_metrics(rpl_parent_t *parent)
{
  if(parent == NULL) {
    return;
  }
  
  /* Allocation optimisée des métriques */
  rpl_mqos_metrics_t *metrics = mqos_metrics_alloc();
  
  /* Calcul de l'ETX */
  metrics->etx = parent->link_metric;
  if(metrics->etx > MQOS_CONF_MAX_ETX) {
    metrics->etx = MQOS_CONF_MAX_ETX;
  }
  
  /* Calcul du délai */
  metrics->delay = parent->delay;
  if(metrics->delay > MQOS_CONF_MAX_DELAY) {
    metrics->delay = MQOS_CONF_MAX_DELAY;
  }
  
  /* Calcul de la gigue */
  metrics->jitter = parent->jitter;
  if(metrics->jitter > MQOS_CONF_MAX_JITTER) {
    metrics->jitter = MQOS_CONF_MAX_JITTER;
  }
  
  /* Mise à jour des métriques */
  rpl_mqos_set_etx(parent, metrics->etx);
  rpl_mqos_set_delay(parent, metrics->delay);
  rpl_mqos_set_jitter(parent, metrics->jitter);
  
  LOG_DBG("Updated metrics for parent %u: ETX=%u, Delay=%u, Jitter=%u\n",
          parent->dag->instance->instance_id, metrics->etx, metrics->delay, metrics->jitter);
}

/* Mise à jour des poids */
void
rpl_mqos_update_weights(uint8_t traffic_class)
{
  if(traffic_class >= MQOS_CONF_NUM_CLASSES) {
    return;
  }
  
  current_traffic_class = traffic_class;
  
  LOG_DBG("Updated weights for traffic class %u\n", traffic_class);
}

/* Calcul du rang */
rpl_rank_t
rpl_mqos_calculate_rank(rpl_parent_t *parent, rpl_rank_t base_rank)
{
  if(parent == NULL) {
    return INFINITE_RANK;
  }
  
  /* Allocation optimisée des métriques */
  rpl_mqos_metrics_t *metrics = mqos_metrics_alloc();
  metrics->etx = rpl_mqos_get_etx(parent);
  metrics->delay = rpl_mqos_get_delay(parent);
  metrics->jitter = rpl_mqos_get_jitter(parent);
  metrics->traffic_class = current_traffic_class;
  
  rpl_mqos_weights_t *w = &weights[current_traffic_class];
  
  /* Calcul optimisé du rang pondéré */
  uint32_t weighted_rank = calculate_weighted_rank_optimized(metrics, w);
  
  /* Ajout du rang de base */
  weighted_rank += base_rank;
  
  /* Vérification des limites */
  if(weighted_rank > RPL_INFINITE_RANK) {
    weighted_rank = RPL_INFINITE_RANK;
  }
  
  return (rpl_rank_t)weighted_rank;
}

/* Getters et setters pour la classe de trafic */
uint8_t
rpl_mqos_get_traffic_class(void)
{
  return current_traffic_class;
}

void
rpl_mqos_set_traffic_class(uint8_t traffic_class)
{
  if(traffic_class < MQOS_CONF_NUM_CLASSES) {
    current_traffic_class = traffic_class;
  }
}

/* Getters et setters pour les métriques */
uint16_t
rpl_mqos_get_etx(rpl_parent_t *parent)
{
  return parent ? parent->link_metric : 0;
}

uint16_t
rpl_mqos_get_delay(rpl_parent_t *parent)
{
  return parent ? parent->delay : 0;
}

uint16_t
rpl_mqos_get_jitter(rpl_parent_t *parent)
{
  return parent ? parent->jitter : 0;
}

void
rpl_mqos_set_etx(rpl_parent_t *parent, uint16_t etx)
{
  if(parent) {
    parent->link_metric = etx;
  }
}

void
rpl_mqos_set_delay(rpl_parent_t *parent, uint16_t delay)
{
  if(parent) {
    parent->delay = delay;
  }
}

void
rpl_mqos_set_jitter(rpl_parent_t *parent, uint16_t jitter)
{
  if(parent) {
    parent->jitter = jitter;
  }
}

/* Getters et setters pour les poids */
uint8_t
rpl_mqos_get_etx_weight(uint8_t traffic_class)
{
  return traffic_class < MQOS_CONF_NUM_CLASSES ?
         weights[traffic_class].etx_weight : 0;
}

uint8_t
rpl_mqos_get_delay_weight(uint8_t traffic_class)
{
  return traffic_class < MQOS_CONF_NUM_CLASSES ?
         weights[traffic_class].delay_weight : 0;
}

uint8_t
rpl_mqos_get_jitter_weight(uint8_t traffic_class)
{
  return traffic_class < MQOS_CONF_NUM_CLASSES ?
         weights[traffic_class].jitter_weight : 0;
}

void
rpl_mqos_set_etx_weight(uint8_t traffic_class, uint8_t weight)
{
  if(traffic_class < MQOS_CONF_NUM_CLASSES) {
    weights[traffic_class].etx_weight = weight;
  }
}

void
rpl_mqos_set_delay_weight(uint8_t traffic_class, uint8_t weight)
{
  if(traffic_class < MQOS_CONF_NUM_CLASSES) {
    weights[traffic_class].delay_weight = weight;
  }
}

void
rpl_mqos_set_jitter_weight(uint8_t traffic_class, uint8_t weight)
{
  if(traffic_class < MQOS_CONF_NUM_CLASSES) {
    weights[traffic_class].jitter_weight = weight;
  }
}

/* Callbacks pour les événements de voisinage */
void
rpl_mqos_neighbor_link_callback(rpl_parent_t *parent, int status, int numtx)
{
  if(parent == NULL) {
    return;
  }
  
  /* Structure de récupération */
  static mqos_recovery_t recovery = {
    .retry_count = 0,
    .max_retries = 3,
    .timeout = MQOS_CONF_NEIGHBOR_TIMEOUT
  };
  
  /* Mise à jour des métriques en fonction du statut du lien */
  if(status == MAC_TX_OK) {
    /* Succès de transmission */
    uint16_t etx = (parent->link_metric * 3 + numtx) / 4;
    rpl_mqos_set_etx(parent, etx);
    recovery.retry_count = 0;
  } else {
    /* Échec de transmission */
    uint16_t etx = parent->link_metric + MQOS_CONF_ETX_THRESHOLD;
    if(etx > MQOS_CONF_MAX_ETX) {
      etx = MQOS_CONF_MAX_ETX;
    }
    rpl_mqos_set_etx(parent, etx);
    
    /* Tentative de récupération */
    mqos_handle_recovery(parent, &recovery);
  }
  
  LOG_DBG("Link callback for parent %u: status=%d, numtx=%d, new_etx=%u\n",
          parent->dag->instance->instance_id, status, numtx, parent->link_metric);
}

void
rpl_mqos_neighbor_path_callback(rpl_parent_t *parent, int status)
{
  if(parent == NULL) {
    return;
  }
  
  /* Structure de récupération */
  static mqos_recovery_t recovery = {
    .retry_count = 0,
    .max_retries = 3,
    .timeout = MQOS_CONF_NEIGHBOR_TIMEOUT
  };
  
  /* Mise à jour des métriques en fonction du statut du chemin */
  if(status == RPL_LINK_METRIC_WORSE) {
    /* Dégradation du chemin */
    uint16_t delay = parent->delay + MQOS_CONF_DELAY_THRESHOLD;
    if(delay > MQOS_CONF_MAX_DELAY) {
      delay = MQOS_CONF_MAX_DELAY;
    }
    rpl_mqos_set_delay(parent, delay);
    
    /* Tentative de récupération */
    mqos_handle_recovery(parent, &recovery);
  } else if(status == RPL_LINK_METRIC_BETTER) {
    /* Amélioration du chemin */
    uint16_t delay = parent->delay > MQOS_CONF_DELAY_THRESHOLD ?
                    parent->delay - MQOS_CONF_DELAY_THRESHOLD : 0;
    rpl_mqos_set_delay(parent, delay);
    recovery.retry_count = 0;
  }
  
  LOG_DBG("Path callback for parent %u: status=%d, new_delay=%u\n",
          parent->dag->instance->instance_id, status, parent->delay);
} 