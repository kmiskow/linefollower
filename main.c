#include <string.h>
#include "stm32f1xx.h"
#include <stdint.h>
UART_HandleTypeDef uart;
ADC_HandleTypeDef adc;
int sens[8]={0,0,0,0,0,0,0,0};
int min0=3900;
int min1=3900;
int min4=3900;


//void power(int pow,TIM_HandleTypeDef* tim,TIM_OC_InitTypeDef* oc,uint32_t channel){
//	oc->Pulse=pow;
//	HAL_TIM_PWM_ConfigChannel(tim, oc, channel);
//	HAL_TIM_PWM_Start(&tim, channel);
//}





void send_char(char c)
{
	HAL_UART_Transmit(&uart, (uint8_t*)&c, 1, 1000);
}

int __io_putchar(int ch)
{
	if (ch == '\n')
		send_char('\r');
	send_char(ch);
	return ch;
}

int adc_read(uint32_t channel)
{

	ADC_ChannelConfTypeDef adc_ch;
	adc_ch.Channel = channel;
	adc_ch.Rank = ADC_REGULAR_RANK_1;
	adc_ch.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

    HAL_ADC_Start(&adc);
	HAL_ADC_PollForConversion(&adc, 1000);
    return HAL_ADC_GetValue(&adc);
}
void line_position(){
if(adc_read(ADC_CHANNEL_1)>min0)sens[0]=1;else sens[0]=0;
if(adc_read(ADC_CHANNEL_4)>min1)sens[1]=1;else sens[1]=0;
if(adc_read(ADC_CHANNEL_0)>min4)sens[4]=1;else sens[4]=0;
}
void drive(){
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);//input #3 ON/OFF
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);//input #4 ON/OFF
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);//input #1 ON/OFF
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);//input #2 ON/OFF
}





TIM_HandleTypeDef tim4;
int main(void)
{
	SystemCoreClock = 8000000;	// taktowanie 8Mhz
	HAL_Init();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();




	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_2;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpio);


	gpio.Mode = GPIO_MODE_AF_INPUT;
	gpio.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_ANALOG;
	gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1|GPIO_PIN_4 |GPIO_PIN_6|GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &gpio);



	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_8|GPIO_PIN_9;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Pin = GPIO_PIN_1|GPIO_PIN_2;; // input #1 i input #2
	gpio.Mode = GPIO_MODE_OUTPUT_PP; // jako wyjœcie
	gpio.Pull = GPIO_NOPULL; // rezystory podci¹gaj¹ce s¹ wy³¹czone
	gpio.Speed = GPIO_SPEED_FREQ_LOW; // wystarcz¹ nieskie czêstotliwoœci prze³¹czania
	HAL_GPIO_Init(GPIOC, &gpio); // inicjalizacja modu³u GPIO


	gpio.Pin = GPIO_PIN_3|GPIO_PIN_0; // input #3 i input #4
	gpio.Mode = GPIO_MODE_OUTPUT_PP; // jako wyjœcie
	gpio.Pull = GPIO_NOPULL; // rezystory podci¹gaj¹ce s¹ wy³¹czone
	gpio.Speed = GPIO_SPEED_FREQ_LOW; // wystarcz¹ nieskie czêstotliwoœci prze³¹czania
	HAL_GPIO_Init(GPIOC, &gpio); // inicjalizacja modu³u GPIOA


	uart.Instance = USART2;
	uart.Init.BaudRate = 9600;
	uart.Init.WordLength = UART_WORDLENGTH_8B;
	uart.Init.Parity = UART_PARITY_NONE;
	uart.Init.StopBits = UART_STOPBITS_1;
	uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart.Init.OverSampling = UART_OVERSAMPLING_16;
	uart.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&uart);

	RCC_PeriphCLKInitTypeDef adc_clk;
	adc_clk.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	adc_clk.AdcClockSelection = RCC_ADCPCLK2_DIV2;
	HAL_RCCEx_PeriphCLKConfig(&adc_clk);

	adc.Instance = ADC1;
	adc.Init.ContinuousConvMode = DISABLE;
	adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adc.Init.NbrOfConversion = 1;
	adc.Init.DiscontinuousConvMode = DISABLE;
	adc.Init.NbrOfDiscConversion = 1;
	HAL_ADC_Init(&adc);



	tim4.Instance = TIM4;
	tim4.Init.Period = 1000 - 1;
	tim4.Init.Prescaler = 80 - 1;
	tim4.Init.ClockDivision = 0;
	tim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim4.Init.RepetitionCounter = 0;
	tim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_PWM_Init(&tim4);

	TIM_OC_InitTypeDef oc;
	oc.OCMode = TIM_OCMODE_PWM1;
	oc.Pulse = 600;										// Moc PB8 (n/1000)*100%
	oc.OCPolarity = TIM_OCPOLARITY_HIGH;
	oc.OCNPolarity = TIM_OCNPOLARITY_LOW;
	oc.OCFastMode = TIM_OCFAST_ENABLE;
	oc.OCIdleState = TIM_OCIDLESTATE_SET;
	oc.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_3); // Kana³ PB8
	oc.Pulse = 600;//moc pb9
			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_4);//pb9


		HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_4);
		HAL_ADCEx_Calibration_Start(&adc);

	while (1)
	{
		//////////////////////////////////////////////////////STAN CZUJNIKOW//////////////////////////////////////////////////////////////////////
//		 int value = adc_read(ADC_CHANNEL_6);
//		 float v = (float)value * 5 / 4096.0f;
//		 printf("L %d (%.3fV) ", value, v);
//
//		 value = adc_read(ADC_CHANNEL_0);
//		 v = (float)value * 5 / 4096.0f;
//		 printf("LS %d (%.3fV)", value, v);
//
//		 value = adc_read(ADC_CHANNEL_1);
//		 		 v = (float)value * 5 / 4096.0f;
//		 		 printf("S %d (%.3fV)", value, v);
//
//		 value = adc_read(ADC_CHANNEL_4);
//		 v = (float)value * 5 / 4096.0f;
//		 printf("PS %d (%.3fV)", value, v);
//
//		 value = adc_read(ADC_CHANNEL_7);
//		 		 v = (float)value * 5 / 4096.0f;
//		 		 printf(" P %d (%.3fV)\n", value, v);

		 		 ////////////////////////////////////////////////LINEFOLLOWER////////////////////////////////////////////////////////////////////////

		 		 line_position();
		 		 if(sens[0]==1){
		 			oc.Pulse = 650;//moc pb8
		 			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_3);//pb8
		 			oc.Pulse = 650;//moc pb8
		 			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_4);//pb9 	//pb9 prawy
		 			HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_3);
		 			HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_4);
		 			 printf("pierwsza %d\n",oc.Pulse);
		 		 }
		 		 else if(sens[1]==0 && sens[4]==1){				//prawySrodek widzi linie
		 			oc.Pulse = 650;//moc pb8
		 			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_3);//pb8
		 			oc.Pulse = 100;//moc pb8
		 			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_4);//pb9 	//pb9 prawy
		 			HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_3);
		 			HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_4);
		 			 printf("druga   %lu\n",oc.Pulse);
		 		 }

		 		 else if(sens[1]==1 && sens[4]==0){				//lewySrodek widzi linie
		 			oc.Pulse = 100;//moc pb8
		 			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_3);//pb8
		 			oc.Pulse = 650;//moc pb
		 			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_4);//pb9 	//pb9 prawy
		 			HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_3);
		 			HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_4);
		 			 printf("trzecia %lu\n",oc.Pulse);
 		 }
//		 		 else if(sens[1]==1&&sens[0]==1&&sens[4]==1){
//		 			oc.Pulse = 500;//moc pb8
//		 				 			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_3);//pb8
//		 			oc.Pulse = 100;//moc pb8
//		 				 			HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_4);//pb9 	//pb9 prawy
//		 				 			HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_3);
//		 				 			HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_4);
//
//		 			printf("czwarta %d\n",oc.Pulse);
//		 		 }
//		 		 else if(sens[1]==0&&sens[0]==0&&sens[4]==0){
//		 			oc.Pulse = 1000;//moc pb8
//		 				 						HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_3);//pb8
//		 			oc.Pulse = 1000;//moc pb8
//		 				 						HAL_TIM_PWM_ConfigChannel(&tim4, &oc, TIM_CHANNEL_4);//pb9 	//pb9 prawy
//		 				 						HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_3);
//		 				 						HAL_TIM_PWM_Start(&tim4, TIM_CHANNEL_4);
//		 				 						drive();
//		 			printf("piata %d\n",oc.Pulse);
//		 				 		 }
	 drive();









	}
}
