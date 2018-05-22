#include <stm32f4xx.h>

void wait_1ms(uint16_t u16Factor) // using timer 4
{
	uint16_t i;

	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->CR1 = 0x0000;
	TIM4->CR2 = 0x0000;
	TIM4->SMCR = 0x0000;
	TIM4->DIER = 0x0000;
	TIM4->CCMR1 = 0x0000;
	TIM4->CCMR2 = 0x0000;
	TIM4->CCER = 0x0000;
	TIM4->PSC = 1;
	TIM4->ARR = 7999;
	TIM4->EGR = TIM_EGR_UG;

	TIM4->SR &= ~TIM_SR_UIF;
	TIM4->CR1 |= TIM_CR1_CEN;

	for(i=0;i<u16Factor;i++)
	{
		while(!(TIM4->SR & TIM_SR_UIF));
		TIM4->SR &= ~TIM_SR_UIF;
	}
	TIM4->CR1 = 0x0000;
}

void init_i2c3(void) //SCL: PA8(AF4) SDA: PC9(AF4)
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

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

	I2C3->CR1 = I2C_CR1_SWRST;
	I2C3->CR1 = 0;
	I2C3->CR2 = 0x0010;
	I2C3->CCR = 80;
	I2C3->TRISE = 17;
	I2C3->CR1 |= I2C_CR1_PE;
}

uint8_t i2c3_start(void)
{
    if(I2C3->SR2 & I2C_SR2_BUSY)
    {
        return 1;
    }
    I2C3->CR1 |= I2C_CR1_START;
    while(!(I2C3->SR1 & I2C_SR1_SB));
    return 0;
}

uint8_t i2c3_send_addr(uint8_t u8Addr, uint8_t read_notwrite)
{
    uint16_t u16Temp;
    
    if(I2C3->SR1 & (I2C_SR1_ARLO | I2C_SR1_BERR))
    {
        return 1;
    }
    
    I2C3->SR1 = ~I2C_SR1_AF; // clear acknowledge fail flag
    I2C3->DR = (u8Addr<<1) | (read_notwrite&0x01); //start addr transmission
    while(1)
    {
        if(I2C3->SR1 & I2C_SR1_ARLO)
        {
            I2C3->SR1 = ~I2C_SR1_ARLO;
            return 2;
        }
        if(I2C3->SR1 & I2C_SR1_BERR)
        {
            I2C3->SR1 = ~I2C_SR1_BERR;
            return 3;
        }
        if(I2C3->SR1 & I2C_SR1_AF)
        {
            I2C3->SR1 = ~I2C_SR1_AF;
            return 4;
        }
        if(I2C3->SR1 & I2C_SR1_ADDR)
        {
            u16Temp = I2C3->SR2;
            //da wiiter mache!!!! luege, öbs im read oder write mode isch (flag TRA)
        }
    }
    
}

void i2c3_hwreset(void)
{
    I2C3->CR1 |= I2C_CR1_SWRST;
    I2C3->CR1 &= ~I2C_CR1_SWRST;
}



int main(void)
{
	volatile uint16_t u16Temp;
	
	init_i2c3();
	u16Temp = I2C3->CR1;
	u16Temp = I2C3->CR2;
	u16Temp = I2C3->CCR;
	u16Temp = I2C3->TRISE;
	u16Temp = I2C3->SR2;
	
	RCC->AHB1ENR = RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOAEN;
	
	GPIOG->MODER &= ~GPIO_MODER_MODE13_Msk;
	GPIOG->MODER |= GPIO_MODER_MODE13_0;
	GPIOG->OTYPER &= ~GPIO_OTYPER_OT13;
	GPIOG->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED13_Msk;
	GPIOG->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
	GPIOG->BSRR = GPIO_BSRR_BR13;
	
	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT0;
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED0_Msk;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0_Msk;
	
	while(!(GPIOA->IDR & GPIO_IDR_ID0));
//	I2C3->CR1 |= I2C_CR1_START;
//	u16Temp = I2C3->SR1;
//	I2C3->DR = 0x82;
//	u16Temp = I2C3->SR1;
//	u16Temp = I2C3->SR2;
//	wait_1ms(10);
//    u16Temp = I2C3->CR1;
//	I2C3->CR1 |= I2C_CR1_STOP;
//    u16Temp = I2C3->CR1;

    u16Temp = i2c3_start();
    
    if(u16Temp == 0)
    {
        GPIOG->ODR |= GPIO_ODR_OD13;
    }
    
    while(1);
	
	while(1)
	{
		GPIOG->ODR ^= GPIO_ODR_OD13;
		wait_1ms(1000);
	}
}
