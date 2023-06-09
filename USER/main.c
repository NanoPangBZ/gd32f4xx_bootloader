#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>

#include "cdc_acm_core.h"

usb_core_driver cdc_acm;

void jump2app( uint32_t app_bin_addr )
{
    typedef void (*app_fn)( void );
    __disable_irq();
    app_fn app = (app_fn)(*(__IO  uint32_t*) (app_bin_addr + 4));
    //取app_bin_addr所在地址的第一个uint32_t为主堆栈指针寄存器的值
    __set_MSP( *(__IO uint32_t*)(app_bin_addr) );
    //jump
    app();
}

void usb_init( void )
{
    usbd_init (&cdc_acm,
#ifdef USE_USB_FS
              USB_CORE_ENUM_FS,
#elif defined(USE_USB_HS)
              USB_CORE_ENUM_HS,
#endif /* USE_USB_FS */
              &cdc_desc,
              &cdc_class);
}

int main(void)
{
    /* configure systick */
    systick_config();

    /* enable the LEDs GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);

    /* configure LED2 GPIO port */
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* reset LED2 GPIO pin */
    gpio_bit_reset(GPIOC, GPIO_PIN_6);

    uint8_t cnt = 0;

    while(1) {
        /* turn on LED2 */
        gpio_bit_set(GPIOC, GPIO_PIN_6);
        delay_1ms(1000);

        /* turn off LED2 */
        gpio_bit_reset(GPIOC, GPIO_PIN_6);
        delay_1ms(1000);

        cnt++;

        if( cnt > 5 )
        {
            jump2app(0x8080000);
        }
    }
}
