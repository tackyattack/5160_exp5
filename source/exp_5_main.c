#include <stdio.h>
#include "main.h"
#include "PORT.H"
#include "LED_"

typedef enum {not_pressed, debounce_p, pressed, held, debounce_r} switch_state_t;
typedef struct
{
  uint8_t SW_mask;
  uint8_t debounce_time;
  switch_state_t SW_state;
} SW_values_t;

void switch_init(void)
{

}

void read_switch(SW_values_t *SW_input_p)
{
  if(SW_input_p->SW_state == not_pressed)
  {
    if((input_port&SW_input_p->SW_mask)==0)
    {
      SW_input_p->debounce_time=0;
      SW_input_p->SW_state=debounce_p;
    }
  }
  else if(SW_input_p->SW_state == debounce_p)
  {
    SW_input_p->debounce_time += interval;
    if(SW_input_p->debounce_time>=50)
    {
      if((input_port&SW_input_p->SW_mask)==0)
      {
        SW_input_p->SW_state=pressed;
        // task execute
      }
      else
      {
        SW_input_p->SW_state=not_pressed;
      }
    }
  }
  else if(SW_input_p->SW_state==held)
  {
    if((input_port&SW_input_p->SW_mask)==1)
    {
      SW_input_p->SW_state=debounce_r;
    }
  }
  else if(SW_input_p->SW_state==debounce_r)
  {
    SW_input_p->debounce_time += interval;
    if(SW_input_p->debounce_time>=50)
    {
      if((input_port&SW_input_p->SW_mask)==1)
      {
        SW_input_p->SW_state=not_pressed;
      }
      else
      {
        SW_input_p->SW_state=held;
      }
    }
  }
}

#define MAX_DELAY (15)
#define MS_PER_TICK (10)
uint8_t delay_amount = 0;
uint8_t LED_display = 0;
uint16_t display_tick = 0;

typedef enum {delay_mode, delay_increment, delay_decrement, flash_mode, LR_flash_mode, RL_flash_mode} os_state_t;
os_state_t os_state;

SW_values_t SW1_p, SW2_p, SW3_p, SW4_p;
void sEOS_ISR(void) interrupt Timer_2_overflow using 1
{
  TF2 = 0;

  read_switch(SW1_p);
  read_switch(SW2_p);
  read_switch(SW3_p);
  read_switch(SW4_p);

  switch(os_state)
  {
    case delay_mode:
      LED_number(delay_amount);
      if (SW2_p->SW_state == pressed)
      {
        os_state = delay_increment;
        SW2_p->SW_state = held;
      }
      if (SW3_p->SW_state == pressed)
      {
        os_state = delay_decrement;
        SW3_p->SW_state = held;
      }
      if (SW1_p->SW_state == pressed) os_state = flash_mode;
      if (SW4_p->SW_state == pressed) os_state = flash_mode;
      break;
    case delay_increment:
      if(delay_amount < MAX_DELAY) delay_amount++;
      LED_number(delay_amount);
      os_state = delay_mode;
      break;
    case delay_decrement:
      if(delay_amount > 0) delay_amount--;
      LED_number(delay_amount);
      os_state = delay_mode;
      break;
    case flash_mode:
      LED_number(LED_display);
      LED_display=0;
      // you could change these to include held so you have to press it again to increment
      if (SW2_p->SW_state == pressed) os_state = delay_mode;
      if (SW3_p->SW_state == pressed) os_state = delay_mode;

      if (SW1_p->SW_state == pressed)
      {
        os_state = LR_flash_mode;
        SW1_p->SW_state = held;
      }
      if (SW4_p->SW_state == pressed)
      {
        os_state = RL_flash_mode;
        SW4_p->SW_state = held;
      }
      break;
    case LR_flash_mode:
      display_tick++;
      if(display_tick > delay_amount*MS_PER_TICK)
      {
        display_tick = 0;
        LED_display = LED_display>>1;
        LED_display = LED_display | (0x4);
        if(LED_display == 0xf) LED_display = 0;
      }
      LED_number(LED_display);
      if (SW1_p->SW_state == pressed)
      {
        os_state = flash_mode;
        SW1_p->SW_state = held;
      }
      break;
    case RL_flash_mode:
      display_tick++;
      if(display_tick > delay_amount*MS_PER_TICK)
      {
        display_tick = 0;
        LED_display = LED_display<<1;
        LED_display = LED_display | (0x01);
        if(LED_display == 0xf) LED_display = 0;
      }
      LED_number(LED_display);
      if (SW4_p->SW_state == pressed)
      {
        os_state = flash_mode;
        SW4_p->SW_state = held;
      }
      break;

  }


}


void sEOS_init(uint8_t interval_ms)
{
  T2CON=0;
  uint16_t preload = (uint16_t)(65536 - ((OSC_FREQ*interval_ms)/(OSC_PER_INST*1000UL))));
  RCAP2H=PRELOAD/256;
  RCAP2L=PRELOAD%256;
  TF2=0;
  ET2=1;
  EA=1;
  TR2=1;
}

main()
{
   AUXR=0x0c;   // make all of XRAM available, ALE always on
   if(OSC_PER_INST==6)
   {
       CKCON0=0x01;  // set X2 clock mode
   }
   else if(OSC_PER_INST==12)
   {
       CKCON0=0x00;  // set standard clock mode
   }


}
