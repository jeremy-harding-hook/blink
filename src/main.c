#include "libopencm3/stm32/f4/rcc.h"
#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>


#ifdef SEMIHOSTING
#include <stdio.h>
#endif

#ifdef SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

#define LED_PORT GPIOA
#define LED_BIT GPIO_ALL 

#if 0
void tim2_isr(void){
	if(!timer_get_flag(TIM2, TIM_SR_CC1IF))
		return;
	timer_clear_flag(TIM2, TIM_SR_CC1IF);
	gpio_toggle(LED_PORT, LED_BIT);
}
#endif

int main(void) {
	
	int i;
#ifdef SEMIHOSTING
	initialise_monitor_handles();
#else
	/* do something... */
#endif

	/* set up output led */
	rcc_periph_clock_enable(RCC_GPIOA); 
	gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_BIT);
	/*gpio_set_output_options(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, LED_BIT); */
	/* gpio_set(LED_PORT, LED_BIT); */

#if 0
	/* set up clock */
	rcc_periph_clock_enable(RCC_TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT_MUL_4, TIM_CR1_CMS_EDGE,
			TIM_CR1_DIR_UP);
	timer_set_period(TIM2, 1000);
	timer_continuous_mode(TIM2);
	timer_set_oc_value(TIM2, TIM_OC1, 900);
	timer_enable_counter(TIM2);
		timer_enable_irq(TIM2, TIM_DIER_CC1IE);
#endif

#ifdef SEMIHOSTING
	printf("hello world!\n");
#endif

	while(1){
		gpio_toggle(LED_PORT, LED_BIT);
		for(i = 0; i < 1000000; i++){
			__asm__("nop");
		}	
	}

	return 0;
}
