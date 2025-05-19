/**
 * @file rpl-mqos-example.c
 * @author BELACEL Madani
 * @brief Implémentation d'un exemple d'utilisation de RPL avec support MQoS
 * @version 1.0
 * @date 2024
 * 
 * Ce fichier contient l'implémentation d'un exemple d'utilisation
 * du protocole RPL avec support Multi-path Quality of Service (MQoS)
 * pour les réseaux IoT basés sur Contiki-NG.
 */

// Auteur : BELACEL Madani | Author: BELACEL Madani

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"
#include "net/routing/rpl-lite/rpl.h"
#include "net/routing/rpl-lite/rpl-conf.h"
#include "net/routing/rpl-lite/rpl-dag-root.h"
#include "net/routing/rpl-lite/rpl-neighbor.h"
#include "net/routing/rpl-lite/rpl-timers.h"
#include "net/routing/rpl-lite/mqos/rpl-mqos-conf.h"
#include "net/routing/rpl-lite/mqos/rpl-of-mqos.h"
#include <stdio.h>
#include <string.h>

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_DBG
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678
#define SEND_INTERVAL (10 * CLOCK_SECOND)
#define NEIGHBOR_CHECK_INTERVAL (5 * CLOCK_SECOND)

/* Définition des classes QoS si non définies */
#ifndef MQOS_CONF_CLASS_NORMAL
#define MQOS_CONF_CLASS_NORMAL 0
#endif

static struct simple_udp_connection udp_conn;
static struct etimer periodic_timer;
static struct etimer neighbor_timer;

/* Structure pour stocker les informations QoS */
typedef struct {
    uint8_t priority;
    uint16_t etx;
    uint8_t hop_count;
} qos_info_t;

static qos_info_t qos_info;

PROCESS(rpl_mqos_example_process, "RPL MQOS Example");
AUTOSTART_PROCESSES(&rpl_mqos_example_process);

/* Fonction pour obtenir l'ETX d'un voisin */
static uint16_t
get_neighbor_etx(const uip_ipaddr_t *addr)
{
    rpl_nbr_t *nbr;
    nbr = rpl_nbr_get_from_ipaddr(addr);
    if(nbr != NULL) {
        return rpl_nbr_get_metric(nbr);
    }
    return 0;
}

/* Fonction pour afficher les informations des voisins */
static void
print_neighbor_info(void)
{
    rpl_nbr_t *nbr;
    uip_ipaddr_t ipaddr;
    
    LOG_INFO("=== Neighbor Information ===\n");
    
    for(nbr = rpl_nbr_first(); nbr != NULL; nbr = rpl_nbr_next(nbr)) {
        rpl_nbr_get_ipaddr(nbr, &ipaddr);
        LOG_INFO("Neighbor: ");
        LOG_INFO_6ADDR(&ipaddr);
        LOG_INFO(", ETX: %u, Rank: %u\n",
                rpl_nbr_get_metric(nbr),
                rpl_nbr_get_rank(nbr));
    }
    LOG_INFO("========================\n");
}

static void
udp_rx_callback(struct simple_udp_connection *c,
                const uip_ipaddr_t *sender_addr,
                uint16_t sender_port,
                const uip_ipaddr_t *receiver_addr,
                uint16_t receiver_port,
                const uint8_t *data,
                uint16_t datalen)
{
    rpl_nbr_t *nbr;
    
    LOG_INFO("Received UDP packet from ");
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO("\n");
    
    /* Mettre à jour les informations QoS */
    qos_info.etx = get_neighbor_etx(sender_addr);
    nbr = rpl_nbr_get_from_ipaddr(sender_addr);
    if(nbr != NULL) {
        qos_info.hop_count = rpl_nbr_get_hop_count(nbr);
    } else {
        qos_info.hop_count = 0;
    }
    
    LOG_INFO("QoS Info - ETX: %u, Hop Count: %u\n",
            qos_info.etx, qos_info.hop_count);
}

PROCESS_THREAD(rpl_mqos_example_process, ev, data)
{
    PROCESS_BEGIN();
    
    LOG_INFO("RPL MQOS Example started\n");
    
    /* Initialiser les informations QoS */
    qos_info.priority = MQOS_CONF_CLASS_NORMAL;
    qos_info.etx = 0;
    qos_info.hop_count = 0;
    
    /* Initialiser la connexion UDP */
    simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                       UDP_SERVER_PORT, udp_rx_callback);
    
    /* Configurer les timers */
    etimer_set(&periodic_timer, SEND_INTERVAL);
    etimer_set(&neighbor_timer, NEIGHBOR_CHECK_INTERVAL);
    
    while(1) {
        PROCESS_WAIT_EVENT();
        
        if(ev == PROCESS_EVENT_TIMER) {
            if(data == &periodic_timer) {
                /* Envoyer un paquet UDP avec les informations QoS */
                static char buf[128];
                snprintf(buf, sizeof(buf), "QoS: Priority=%d, ETX=%u, Hops=%u",
                        qos_info.priority, qos_info.etx, qos_info.hop_count);
                
                uip_ipaddr_t dest_ipaddr;
                uip_ip6addr(&dest_ipaddr, 0xfe80, 0, 0, 0, 0, 0, 0, 1);
                
                simple_udp_sendto(&udp_conn, buf, strlen(buf), &dest_ipaddr);
                LOG_INFO("Sent UDP packet with QoS info\n");
                
                etimer_reset(&periodic_timer);
            } else if(data == &neighbor_timer) {
                /* Afficher les informations des voisins */
                print_neighbor_info();
                etimer_reset(&neighbor_timer);
            }
        }
    }
    
    PROCESS_END();
}



