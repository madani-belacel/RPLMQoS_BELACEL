#ifndef PROJECT_CONF_H_ 


	  #define PROJECT_CONF_H_
  

#include   <stdint.h>


     /*    Configuration de base*/

  #define NETSTACK_CONF_WITH_IPV6 1  


/*     Désactiver les capteurs    */
#define SENSORS_CONF_ENABLE 0    


       /*Configuration RPL*/
  #define RPL_CONF_WITH_PROBING 1

#define RPL_CONF_DIO_INTERVAL_MIN 12
  #define RPL_CONF_DIO_INTERVAL_DOUBLINGS 8  


/*    Configuration des LED    */      


#define LEDS_CONF_LEGACY_API 1
    #define PLATFORM_CONF_PROVIDES_MAIN_LOOP 0

/*     Désactiver les statistiques UIP*/ 

    #define UIP_CONF_STATISTICS 0

/*Configuration des LED pour Cooja   */
  #define PLATFORM_HAS_LEDS 1    

 #define LEDS_GREEN  1    

#define LEDS_YELLOW 2
        #define LEDS_RED    4

   #define LEDS_ALL    7

   

/*     Définition des types manquants*/
typedef unsigned short clock_time_t;     

typedef unsigned short uip_stats_t;  


        /*     Configuration RPL-MQoS  */ 


   #define RPL_CONF_MOP RPL_MOP_STORING_MULTICAST

    #define RPL_CONF_WITH_MULTICAST 1
   #define RPL_CONF_PROBING_INTERVAL   (60 *CLOCK_SECOND)

        /*   Configuration QoS  */
	#define MQOS_CONF_NUM_CLASSES 4

#define MQOS_CONF_CLASS_CRITICAL 0
#define MQOS_CONF_CLASS_PRIORITY 1
   #define MQOS_CONF_CLASS_NORMAL 2
#define MQOS_CONF_CLASS_BACKGROUND 3


    #endif    /*  PROJECT_CONF_H_   */