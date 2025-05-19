#include "net/routing/rpl-lite/rpl-mqos-metrics.h"
#include "sys/log.h"
    #include "sys/ctimer.h"
#include  <string.h>

    #define LOG_MODULE "RPL-MQoS"

#define LOG_LEVEL LOG_LEVEL_INFO    


   


     /*  Pool de métriques   */
static rpl_mqos_metrics_t metrics_pool[MQOS_METRICS_POOL_SIZE];
static uint8_t metrics_pool_index=0;

     /*     Cache de métriques    */
  typedef struct   {
rpl_parent_t  *parent;
      rpl_mqos_metrics_t metrics;
uint32_t timestamp;
   }   mqos_cache_entry_t;

    static mqos_cache_entry_t metrics_cache[MQOS_METRICS_CACHE_SIZE];
	  static uint8_t cache_index   =0;

/*   Timer pour la mise à jour des métriques*/

 static struct ctimer metrics_update_timer;


/*   Fonctions privées*/   

    static void metrics_update_callback(void  *ptr);

static uint32_t calculate_weighted_sum(const rpl_mqos_metrics_t*metrics);  

   

     /*  Initialisation du module     */   

   void
   rpl_mqos_metrics_init(void)
    {   

 /*     Initialisation du pool  */

memset(metrics_pool, 0, sizeof(metrics_pool));
  metrics_pool_index   =   0;

    


      /*  Initialisation du cache */

memset(metrics_cache,  0,  sizeof(metrics_cache));
cache_index=0;
   

    /*   Configuration du timer de mise à jour  */
ctimer_set(&metrics_update_timer,   MQOS_METRICS_UPDATE_INTERVAL, 

             metrics_update_callback,NULL);
     }


   /*    Allocation depuis le pool  */
    rpl_mqos_metrics_t*
rpl_mqos_metrics_alloc(void)
     {
   rpl_mqos_metrics_t*metrics     =         &metrics_pool[metrics_pool_index];
       metrics_pool_index =     (metrics_pool_index   +       1)     % MQOS_METRICS_POOL_SIZE;    


    


      memset(metrics,   0,   sizeof(rpl_mqos_metrics_t));
return metrics;

      }
   

     /*   Libération des métriques  */
void
rpl_mqos_metrics_free(rpl_mqos_metrics_t *metrics)
{
    

/*   Le pool est circulaire, pas besoin de libération explicite  */
   

  (void)metrics;
  }

/*Calcul du délai   */ 

uint16_t  



      rpl_mqos_metrics_calculate_delay(rpl_parent_t *parent)
  {
       uint16_t delay =       0;
  

      /*    Calcul du délai bout-en-bout */
	  if(parent    !=     NULL){

     /*   Délai de propagation   */    

        delay+=parent->link_metric;
 


      /*Délai de traitement*/
       delay +=  10;/*10ms par défaut     */
    
    /*Délai de file d'attente */

 delay    +=parent->queue_delay;
}   
   

  
      return delay;
    }


/* Calcul de la gigue*/

uint16_t

    rpl_mqos_metrics_calculate_jitter(rpl_parent_t  *parent)
    {  

 


static uint16_t last_delay      =0;
uint16_t current_delay   =     rpl_mqos_metrics_calculate_delay(parent);
      uint16_t jitter=0;   


    

  
     if(last_delay   >       0){
/*   Calcul de la variation de délai   */
       jitter   =      (current_delay     >       last_delay)? 
            (current_delay     -   last_delay)   : 
            (last_delay -   current_delay);
 }
    


last_delay       =current_delay;

 return jitter;

   }  

    

     /* Calcul de la fiabilité  */
  uint8_t
rpl_mqos_metrics_calculate_reliability(rpl_parent_t     *parent)

{
uint8_t reliability     =100;
  

if(parent     !=   NULL)  {
      /*  Calcul basé sur le taux de perte  */
      uint16_t loss_rate   =   parent->link_metric;
   if(loss_rate     >         0){

 reliability   = 100       -   (loss_rate     *         100/   256);

    }
  }  

  


  return reliability;

}

/*  Calcul de la bande passante   */
     uint16_t
rpl_mqos_metrics_calculate_bandwidth(rpl_parent_t    *parent)
     {
 uint16_t bandwidth  =250;      /*  250 kbps par défaut*/
    

       if(parent   !=   NULL){
     /*Ajustement basé sur la qualité du lien  */
bandwidth=(bandwidth         *      (256-     parent->link_metric))   >>    8;
     }  


  
 return bandwidth;    

        }   



/*    Mise à jour des métriques  */
void

    rpl_mqos_metrics_update(rpl_parent_t  *parent)
   {  

       if(parent    ==     NULL){
 return;

}
   


rpl_mqos_metrics_t   *metrics   =   rpl_mqos_metrics_alloc();    


  
     /*Calcul des métriques  */
metrics->etx       =     parent->link_metric;   

    

metrics->delay         =   rpl_mqos_metrics_calculate_delay(parent);  


 metrics->jitter   =rpl_mqos_metrics_calculate_jitter(parent);


metrics->reliability   =       rpl_mqos_metrics_calculate_reliability(parent);
        metrics->bandwidth=    rpl_mqos_metrics_calculate_bandwidth(parent);
   metrics->last_update     =   clock_time();

      /*Mise à jour du cache*/
rpl_mqos_metrics_cache_update(parent);
  }
 


 /*  Calcul du rang pondéré    */
uint16_t

rpl_mqos_metrics_calculate_rank(const rpl_mqos_metrics_t *metrics)   

   {

	 if(metrics    ==   NULL)     {
   return 0;
         }   


   /*Calcul optimisé avec décalages binaires  */

uint32_t weighted_sum =   calculate_weighted_sum(metrics);
  
     /*    Normalisation */
return(uint16_t)(weighted_sum  >>    4);        /*     Division par 16*/
}



/*Calcul de la somme pondérée  */
  static uint32_t   


  calculate_weighted_sum(const rpl_mqos_metrics_t*metrics)
  {  

   uint32_t sum   =0;
  
      /* ETX (poids 4)*/
        sum   += metrics->etx   *   MQOS_DEFAULT_ETX_WEIGHT;
 


 /*   Délai(poids 3)   */

sum+=metrics->delay     *     MQOS_DEFAULT_DELAY_WEIGHT;
  
  /*   Gigue    (poids 2)      */
sum    += metrics->jitter     * MQOS_DEFAULT_JITTER_WEIGHT;
  
/*Fiabilité  (poids 3)  */

   sum  +=(100         -   metrics->reliability)*   MQOS_DEFAULT_RELIABILITY_WEIGHT;
  

      /*     Bande passante(poids 2)*/
         sum+=   (250-     metrics->bandwidth)           *   MQOS_DEFAULT_BANDWIDTH_WEIGHT;
  
  

       return sum;
}


/*  Mise à jour du cache */

void
       rpl_mqos_metrics_cache_update(rpl_parent_t   *parent)

    {
 if(parent==     NULL) {
return; 

}

/*   Recherche dans le cache   */
  for(uint8_t i=   0;i         <   MQOS_METRICS_CACHE_SIZE;i++)     {
	  if(metrics_cache[i].parent    ==   parent)    {
  /*  Mise à jour des métriques existantes  */
          metrics_cache[i].metrics =    *rpl_mqos_metrics_alloc();    

         metrics_cache[i].timestamp     =   clock_time();
            return;
      }
      }    


       /*  Ajout dans le cache*/

         metrics_cache[cache_index].parent=      parent;
       metrics_cache[cache_index].metrics=       *rpl_mqos_metrics_alloc();
metrics_cache[cache_index].timestamp= clock_time();
cache_index=     (cache_index    +1)    % MQOS_METRICS_CACHE_SIZE;
}        

    

/*   Récupération depuis le cache*/
rpl_mqos_metrics_t*
    rpl_mqos_metrics_cache_get(rpl_parent_t    *parent)
    {
if(parent==NULL)       {  

   return NULL;
}

   /*   Recherche dans le cache   */
for(uint8_t i      =0;    i<       MQOS_METRICS_CACHE_SIZE;    i++){
       if(metrics_cache[i].parent==parent){
	   /*Vérification de la fraîcheur des données     */   
  

   

        if(clock_time()-metrics_cache[i].timestamp   <       MQOS_METRICS_UPDATE_INTERVAL){
return    &metrics_cache[i].metrics;  

         }
    break;

  }   
 

    }    



        return NULL;
        }

   /*   Callback de mise à jour périodique    */
static void
metrics_update_callback(void*ptr)    


{

 /*Mise à jour des métriques pour tous les parents   */
rpl_parent_t*parent;
 for(parent   =rpl_parent_head();parent  !=NULL;parent   =rpl_parent_next(parent)){
rpl_mqos_metrics_update(parent);
}


/*   Reprogrammation du timer*/
 ctimer_reset(&metrics_update_timer);   



}    