  #ifndef SKY_CONF_H
    #define SKY_CONF_H  



 #include  <stdint.h>

   /*   Types de base */
   typedef uint16_t clock_time_t;
typedef uint16_t uip_stats_t;

 /*Clock configuration    */

 #define CLOCK_CONF_SECOND 128  
    



     /*Timer configuration    */
    #define RTIMER_ARCH_SECOND 32768

    /*Network configuration    */     


#define NETSTACK_CONF_WITH_IPV6 1
#define UIP_CONF_IPV6 1

    /* RPL configuration*/  

	  #define RPL_CONF_OF_OCP RPL_OCP_MRHOF
  

	/*Radio configuration  */    

#define CC2420_CONF_AUTOACK 1
#define CC2420_CONF_CHANNEL 26
#define CC2420_CONF_CCA_THRESH  -45
   

      /*    Memory configuration */
#define CFS_CONF_OFFSET_TYPE uint32_t

      /*     Log configuration  */
  #define LOG_CONF_LEVEL_MAC LOG_LEVEL_INFO
#define LOG_CONF_LEVEL_FRAMER LOG_LEVEL_INFO
  #define LOG_CONF_LEVEL_IPV6 LOG_LEVEL_INFO
       #define LOG_CONF_LEVEL_RPL LOG_LEVEL_INFO   

#define LOG_CONF_LEVEL_TCPIP LOG_LEVEL_INFO
 #define LOG_CONF_LEVEL_MAC LOG_LEVEL_INFO
  #define LOG_CONF_LEVEL_6LOWPAN LOG_LEVEL_INFO    



#endif    /*SKY_CONF_H*/
