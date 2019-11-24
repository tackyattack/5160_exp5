#ifndef _S_EOS_H
#define _S_EOS_H

#include "main.h"
#include "PORT.H"

#define MAX_DELAY (15)
#define MS_PER_TICK (10)
#define MS_PER_DELAY_SETTING (60)
#define DELAY_SETTING_TICK (MS_PER_DELAY_SETTING/MS_PER_TICK)

void sEOS_init(uint8_t interval_ms);

#endif
