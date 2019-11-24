#include "main.h"
#include "PORT.H"
#include "switch.h"
#include "LED_Control.h"
#include "sEOS.h"

uint8_t delay_amount = 1;
uint8_t LED_display = 0;
uint16_t display_tick = 0;

typedef enum {delay_mode, delay_increment, delay_decrement, flash_mode, LR_flash_mode, RL_flash_mode} os_state_t;
os_state_t os_state;
SW_values_t SW1_val;
SW_values_t SW2_val;
SW_values_t SW3_val;
SW_values_t SW4_val;

void sEOS_ISR(void) interrupt Timer_2_Overflow
{
  TF2 = 0;

  read_switch(&SW1_val);
  read_switch(&SW2_val);
  read_switch(&SW3_val);
  read_switch(&SW4_val);

  switch(os_state)
  {
    case delay_mode:
      LED_number(delay_amount);
      if (SW2_val.SW_state == pressed)
      {
        os_state = delay_increment;
        SW2_val.SW_state = held;
      }
      if (SW3_val.SW_state == pressed)
      {
        os_state = delay_decrement;
        SW3_val.SW_state = held;
      }

      if (SW1_val.SW_state == pressed)
      {
        os_state = LR_flash_mode;
        SW1_val.SW_state = held;
        LED_display = 0;
      }
      if (SW4_val.SW_state == pressed)
      {
        os_state = RL_flash_mode;
        SW4_val.SW_state = held;
        LED_display = 0;
      }
      break;
    case delay_increment:
      if(delay_amount < MAX_DELAY) delay_amount++;
      LED_number(delay_amount);
      os_state = delay_mode;
      break;
    case delay_decrement:
      if(delay_amount > 1) delay_amount--;
      LED_number(delay_amount);
      os_state = delay_mode;
      break;
    case flash_mode:
      LED_number(LED_display);
      LED_display=0;

      if (SW2_val.SW_state == pressed)
      {
        SW2_val.SW_state = held;
        os_state = delay_mode;
      }
      if (SW3_val.SW_state == pressed)
      {
        SW3_val.SW_state = held;
        os_state = delay_mode;
      }
      if (SW1_val.SW_state == pressed)
      {
        os_state = LR_flash_mode;
        SW1_val.SW_state = held;
      }
      if (SW4_val.SW_state == pressed)
      {
        os_state = RL_flash_mode;
        SW4_val.SW_state = held;
      }
      break;
    case LR_flash_mode:
      LED_number(LED_display);
      display_tick++;
      if(display_tick > delay_amount*DELAY_SETTING_TICK)
      {
        display_tick = 0;
        if(LED_display >= 0x0f)
        {
          LED_display = 0;
        }
        else
        {
          LED_display = LED_display>>1;
          LED_display = LED_display | (0x08);
        }
      }

      if (SW2_val.SW_state == pressed)
      {
        SW2_val.SW_state = held;
        os_state = delay_mode;
      }
      if (SW3_val.SW_state == pressed)
      {
        SW3_val.SW_state = held;
        os_state = delay_mode;
      }
      if (SW1_val.SW_state == pressed)
      {
        os_state = flash_mode;
        SW1_val.SW_state = held;
      }
      if (SW4_val.SW_state == pressed)
      {
        os_state = RL_flash_mode;
        SW4_val.SW_state = held;
        LED_display = 0;
      }
      break;
    case RL_flash_mode:
      LED_number(LED_display);
      display_tick++;
      if(display_tick > delay_amount*DELAY_SETTING_TICK)
      {
        display_tick = 0;
        if(LED_display >= 0x0f)
        {
          LED_display = 0;
        }
        else
        {
          LED_display = LED_display<<1;
          LED_display = LED_display | (0x01);
        }
      }

      if (SW2_val.SW_state == pressed)
      {
        SW2_val.SW_state = held;
        os_state = delay_mode;
      }
      if (SW3_val.SW_state == pressed)
      {
        SW3_val.SW_state = held;
        os_state = delay_mode;
      }
      if (SW1_val.SW_state == pressed)
      {
        os_state = LR_flash_mode;
        SW1_val.SW_state = held;
        LED_display = 0;
      }
      if (SW4_val.SW_state == pressed)
      {
        os_state = flash_mode;
        SW4_val.SW_state = held;
      }
      break;
  }
}



void sEOS_init(uint8_t interval_ms)
{
  uint16_t preload = (uint16_t)(65536 - ((OSC_FREQ*interval_ms)/(OSC_PER_INST*1000UL)));
  switch_init(&SW1_val, 1<<0);
  switch_init(&SW2_val, 1<<1);
  switch_init(&SW3_val, 1<<2);
  switch_init(&SW4_val, 1<<3);
  os_state = flash_mode;
  LED_display = 0;
  delay_amount = 1;

  T2CON=0;
  RCAP2H=(uint8_t)preload/256;
  RCAP2L=(uint8_t)preload%256;
  TF2=0;
  ET2=1;
  EA=1;
  TR2=1;
}
