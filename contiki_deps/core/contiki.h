/**
 * \file
 * Contiki-NG core header file
 * \author BELACEL Madani
 * \date 2023
 */

#ifndef CONTIKI_H_
#define CONTIKI_H_

#include "contiki-conf.h"
#include "sys/cc.h"
#include "sys/clock.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "sys/process.h"
#include "sys/autostart.h"

#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"

#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-nd6.h"

#include "net/routing/rpl-lite/rpl.h"
#include "net/routing/rpl-lite/rpl-conf.h"
#include "net/routing/rpl-lite/rpl-dag-root.h"
#include "net/routing/rpl-lite/rpl-timers.h"
#include "net/routing/rpl-lite/rpl-callbacks.h"

#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-conf.h"
#include "net/mac/tsch/tsch-queue.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "net/mac/tsch/tsch-slot-operation.h"

#include "net/mac/frame802154.h"
#include "net/mac/mac.h"
#include "net/mac/mac-sequence.h"
#include "net/mac/mac-sequence.h"
#include "net/mac/mac-sequence.h"

#include "net/netstack.h"
#include "net/ipv6/sicslowpan.h"
#include "net/ipv6/tcpip.h"

#include "dev/leds.h"
#include "dev/serial-line.h"
#include "dev/slip.h"
#include "dev/watchdog.h"

#include "dev/button-sensor.h"
#include "dev/temperature-sensor.h"
#include "dev/light-sensor.h"
#include "dev/battery-sensor.h"

#include "platform/sky/contiki-sky-main.h"
#include "platform/sky/contiki-sky-platform.h"
#include "platform/sky/contiki-sky-conf.h"

#endif /* CONTIKI_H_ */ 