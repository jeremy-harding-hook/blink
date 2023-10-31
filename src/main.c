#include "libopencm3/stm32/f4/rcc.h"
#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include "stdio.h"
#include "utils/string.h"
#include "usart/usart.h"

#define HARD_STUFF

#define LED_PORT GPIOA
#define LED_BIT GPIO5 

/* Note: these don't require any setup, but don't try to use them for other
 * features as they are needed for the debugger to connect */
#define SWD_PORT GPIOA
#define SWD_BIT (GPIO13 | GPIO14)

#define INPUT_LENGTH_MAX 11
#if 0
void tim2_isr(void){
	if(!timer_get_flag(TIM2, TIM_SR_CC1IF))
		return;
	timer_clear_flag(TIM2, TIM_SR_CC1IF);
	gpio_toggle(LED_PORT, LED_BIT);
}
#endif

int main(void) {
	/* set up output led */
	rcc_periph_clock_enable(RCC_GPIOA); 
	gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_BIT);

	usart_setup();
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

	console_puts("Hello world!\n");
	console_puts("This is USART2 speaking!\n");
	console_puts(
			"Please note that all commands must be 10 characters or less\n");

	while(1){
		char user_command[INPUT_LENGTH_MAX];
		console_gets(user_command, INPUT_LENGTH_MAX);
		if(!strcmp(user_command, "toggle")){
			gpio_toggle(LED_PORT, LED_BIT);
		}
		else if(!strcmp(user_command, "help")){
			console_puts("Type 'toggle' in order to turn the led on or off.\n");
		}
		else{
			console_puts("Command not recognised!\n");
		}
	}

	return 0;
}
