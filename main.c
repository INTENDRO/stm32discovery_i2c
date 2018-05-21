
#include <stm32f4xx.h>


void init_i2c3(void) //SCL: PA8(AF4) SDA: PC9(AF4)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOAEN;
    
    GPIOA->OTYPER |= GPIO_OTYPER_OT8;
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8_Msk;
    GPIOA->AFR[1] |= GPIO_AFRH_AFSEL8_2;
    GPIOA->MODER &= ~GPIO_MODER_MODE8_Msk;
    GPIOA->MODER |= GPIO_MODER_MODE8_1;
    
    GPIOC->OTYPER |= GPIO_OTYPER_OT9;
    GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL9_Msk;
    GPIOC->AFR[1] |= GPIO_AFRH_AFSEL9_2;
    GPIOC->MODER &= ~GPIO_MODER_MODE9_Msk;
    GPIOC->MODER |= GPIO_MODER_MODE9_1;
    
    I2C3->CR1 = 0;
    I2C3->CR2 = 0x0010;
    I2C3->CCR = 80;
    I2C3->TRISE = 17;
    I2C3->CR1 |= I2C_CR1_PE;
}

int main(void)
{
    init_i2c3();
    
    
    while(1)
    {
        
    }
    
}
