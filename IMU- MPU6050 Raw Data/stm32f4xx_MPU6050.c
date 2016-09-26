/*
-------------------------------------------------------SAMPLE PROGGRAM ------------------------------------------------
#include "stm32f4xx_MPU6050.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_tim.h"

int main()
{
	int i=0,Time;
	float x[3],y[3],Anglex=0,Angley=0,Anglez=0;
	Init_Gyroscope();
	Init_usart();
	Init_led();
	TIM_SetCounter(TIM2,0);

	while(1)
	{
		Get_Data(x,y);

		Time=TIM_GetCounter(TIM2);							//taking counter for evaluating Time for one loop
		TIM_SetCounter(TIM2,0);							//resetting counter to one

		Anglex+=(x[0])*Time/400000.0;						//multiplying ang. velocity by Time (dividing by period*10) to grt change of Angle in previous loop
		Angley+=(x[1])*Time/400000.0;						//adding change in Angle to previous Angle
		Anglez+=(x[2])*Time/400000.0;

		Send_data_usart((int)Anglex);
		Send_data_usart((int)Angley);
		Send_data_usart((int)Anglez);

		if(Anglex>0)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_12);
				GPIO_ResetBits(GPIOD,GPIO_Pin_14);
			}

		else
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_14);
				GPIO_ResetBits(GPIOD,GPIO_Pin_12);
			}

		if(Angley>0)
			 {
				 GPIO_SetBits(GPIOD,GPIO_Pin_15);
				 GPIO_ResetBits(GPIOD,GPIO_Pin_13);
			 }

		else
			 {
				 GPIO_SetBits(GPIOD,GPIO_Pin_13);
				 GPIO_ResetBits(GPIOD,GPIO_Pin_15);
			 }
	}
	return(0);
}
 */
#include "stm32f4xx_MPU6050.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_tim.h"


uint8_t I2C1_SLAVE_ADDRESS;

#define I2C1_SLAVE_ADDRESS 0b1101000


void Init_i2c(void)
	{
		 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);                             	//Enable I2c clock
		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE);       	//Enable gpio clock


		 GPIO_InitTypeDef GPIO_Init_Struct;													//Initialize all pins
		 GPIO_Init_Struct.GPIO_OType = GPIO_OType_OD;                                     	//SCLK and SDA must be open drain
		 GPIO_Init_Struct.GPIO_PuPd = GPIO_PuPd_UP;
		 GPIO_Init_Struct.GPIO_Mode = GPIO_Mode_AF;
		 GPIO_Init_Struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		 GPIO_Init_Struct.GPIO_Speed = GPIO_Speed_50MHz;                                  	//Must be greater than a particular value
		 GPIO_Init(GPIOB, &GPIO_Init_Struct);
		 GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);                          	//Connecting those terminals to AF config
		 GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);							//Connecting those terminals to AF config


		 I2C_InitTypeDef I2C_Init_Struct;													//Initialize i2c
		 I2C_Init_Struct.I2C_ClockSpeed = 400000;											//400 kHz (max for i2c)
		 I2C_Init_Struct.I2C_Mode = I2C_Mode_I2C;
		 I2C_Init_Struct.I2C_DutyCycle = I2C_DutyCycle_2;									//50% duty cycle standard
		 I2C_Init_Struct.I2C_OwnAddress1 = 0x00;											//own address doesnt matter if stm is master
		 I2C_Init_Struct.I2C_Ack = I2C_Ack_Disable;											//Acknowledgement is disabled by default can be enabled later
		 I2C_Init_Struct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;            //select according to the address format
		 I2C_Init(I2C1, &I2C_Init_Struct);
		 I2C_Cmd(I2C1,ENABLE);

	}

void Init_led(void)
	{

		GPIO_InitTypeDef GPIO_Init_Struct;
		GPIO_Init_Struct.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;		//Initialize leds (self explainatory)
		GPIO_Init_Struct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init_Struct.GPIO_OType = GPIO_OType_PP;
		GPIO_Init_Struct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init_Struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOD, &GPIO_Init_Struct);

	}

void Init_usart(void)
	{

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);               				//enable clock
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);               				//enable clock

		GPIO_InitTypeDef GPIO_Init_Struct;                                            		//initialize pins
		GPIO_Init_Struct.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init_Struct.GPIO_Mode = GPIO_Mode_AF;                                    		//using as usart
		GPIO_Init_Struct.GPIO_OType = GPIO_OType_PP;
		GPIO_Init_Struct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init_Struct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &GPIO_Init_Struct);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);


		USART_InitTypeDef USART_Init_Struct;
		USART_Init_Struct.USART_BaudRate = 38400;											//self explainatory
		USART_Init_Struct.USART_WordLength = USART_WordLength_8b;
		USART_Init_Struct.USART_StopBits = USART_StopBits_1;
		USART_Init_Struct.USART_Parity = USART_Parity_No;
		USART_Init_Struct.USART_Mode = USART_Mode_Tx;
		USART_Init_Struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(USART2,&USART_Init_Struct);
		USART_Cmd(USART2,ENABLE);         													//enable uart

	}

void Init_tim(void)
	{

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);


		TIM_TimeBaseInitTypeDef TIM_Init_Struct;                                      		//initialize counter
		TIM_Init_Struct.TIM_Prescaler = 39;													//pescalar 40
		TIM_Init_Struct.TIM_CounterMode = TIM_CounterMode_Up;								//self expainatory
		TIM_Init_Struct.TIM_Period = 40000;													//period 40000 (later used to find out Time for one loop
		TIM_Init_Struct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_Init_Struct.TIM_RepetitionCounter = 0x00;
		TIM_TimeBaseInit(TIM2,&TIM_Init_Struct);
		TIM_Cmd(TIM2,ENABLE);

	}

void Init_mpu(void)
	{
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));										//Wait till i2c lines become free


		//basically it is transmitting a data
		//waking up imu mpu6050 from sleep and setting reference clock to x axis of gyroxcope by writting regester no 117 to 00000001


		I2C_GenerateSTART(I2C1, ENABLE);//Generate start (i2c protocol step one)
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//Wait till knowledge bit is recieved from slve and master mode select event happens

		I2C_AcknowledgeConfig(I2C1, ENABLE);

		I2C_Send7bitAddress(I2C1, I2C1_SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);//Slave addr is sent for verification for slave
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//addres 6 bits from datasheet regester no 117 7th bit = logic condition of AD0 pin of imu

		I2C_SendData(I2C1, 0x6B);  //address of register 107 which is to be written(next step of i2c)
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//wait till byte is transmitted

		I2C_SendData(I2C1, 0x01);   //value of register to 0x01(next step of i2c)
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//wait till byte is transmitted
		I2C_GenerateSTOP(I2C1, ENABLE);//last step for i2c protocol


		//setting gyro range to 250 deg per sec (which is default)
		//prcedure is same as before


		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
		I2C_AcknowledgeConfig(I2C1, ENABLE);
		I2C_Send7bitAddress(I2C1, I2C1_SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		I2C_SendData(I2C1, 0x1B);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		I2C_SendData(I2C1, 0x00);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		I2C_GenerateSTOP(I2C1, ENABLE);
	}

uint8_t Rec_data (uint8_t reg)
	{
		uint8_t d;
		I2C_AcknowledgeConfig(I2C1, ENABLE);												//Enable Acknowledgement
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));										//Wait till i2c lines become free

		I2C_GenerateSTART(I2C1, ENABLE);													//Generate start (i2c protocol step one)
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

		I2C_Send7bitAddress(I2C1, I2C1_SLAVE_ADDRESS<<1, I2C_Direction_Transmitter);		//Slave addr is sent for verification for slave
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

		I2C_SendData(I2C1, reg);    														//address of register to be read
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));


		//recieving data

		I2C_GenerateSTART(I2C1, ENABLE);													//Wait till i2c lines become free
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

		I2C_Send7bitAddress(I2C1, I2C1_SLAVE_ADDRESS<<1, I2C_Direction_Receiver);			//Slave addr is sent for verification for slave
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

		I2C_AcknowledgeConfig(I2C1, DISABLE);												//Disable Acknowledgement according to the protocol
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));

		d = I2C_ReceiveData(I2C1);															//recieving latest register value

		I2C_GenerateSTOP(I2C1, ENABLE);
		return (d);
	}

void Calibrate_Gyros(void)
	{
		int x = 0,k=0;
		int16_t GYRO_XOUT_Offset = 0;
		int16_t GYRO_YOUT_Offset = 0;
		int16_t GYRO_ZOUT_Offset = 0;
		uint8_t regadd,Data[6];

		for(x = 0; x<5000; x++)
			{
				regadd=0x43;																//address of the resistor containing latest sensor data
				for(k=0;k<6;k++)															//total six resister for gyroscope
					{
						Data[k]=Rec_data (regadd);
						GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); 	//turn led on
						regadd++;																//address of next resister
					}
																						//among 6 resisters
				GYRO_XOUT_Offset = ((Data[0]<<8)|Data[1]);									//1st two give divided value of actual 16 bit Data o/p of x axis sensor
				GYRO_YOUT_Offset = ((Data[2]<<8)|Data[3]);									//next two give divided value of actual 16 bit Data o/p of y axis sensor
				GYRO_ZOUT_Offset = ((Data[4]<<8)|Data[5]);									//next two give divided value of actual 16 bit Data o/p of z axis sensor
				Offset[0]+=GYRO_XOUT_Offset;
				Offset[1]+=GYRO_YOUT_Offset;												//adding all 5000 value
				Offset[2]+=GYRO_ZOUT_Offset;
			}
		Offset[0] /= 5000;
		Offset[1] /= 5000;																//dividing by 5000 for mean Offset
		Offset[2] /= 5000;
	}

void Calibrate_Accel(void)
	{
		int x = 0,k=0;
		int16_t GYRO_XOUT_Offset = 0;
		int16_t GYRO_YOUT_Offset = 0;
		int16_t GYRO_ZOUT_Offset = 0;
		uint8_t regadd,Data[6];

		for(x = 0; x<5000; x++)
			{
				regadd=0x3B;																//address of the resistor containing latest sensor data
				for(k=0;k<6;k++)															//total six resister for gyroscope
					{
						Data[k]=Rec_data (regadd);
						GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); 	//turn led on
						regadd++;																//address of next resister
					}
																						//among 6 resisters
				GYRO_XOUT_Offset = ((Data[0]<<8)|Data[1]);									//1st two give divided value of actual 16 bit Data o/p of x axis sensor
				GYRO_YOUT_Offset = ((Data[2]<<8)|Data[3]);									//next two give divided value of actual 16 bit Data o/p of y axis sensor
				GYRO_ZOUT_Offset = ((Data[4]<<8)|Data[5]);									//next two give divided value of actual 16 bit Data o/p of z axis sensor
				Offset_accel[0]+=GYRO_XOUT_Offset;
				Offset_accel[1]+=GYRO_YOUT_Offset;												//adding all 5000 value
				Offset_accel[2]+=GYRO_ZOUT_Offset;
			}
		Offset_accel[0] /= 5000;
		Offset_accel[1] /= 5000;																//dividing by 5000 for mean Offset
		Offset_accel[2] /= 5000;
	}

void Send_data_usart(int data)                                                      //function for sending some integer
	{

		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC));
		USART_SendData(USART2,data);
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC));
		USART_SendData(USART2,data>>8);
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC));
		USART_SendData(USART2,data>>16);
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC));
		USART_SendData(USART2,data>>24);

	}

void Init_Gyroscope(void)
{
	Init_tim(); 								//Initialize timer
	Init_i2c();									//Initialize I2C
	Init_mpu();									//Initialize MPU-6050
	Calibrate_Gyros();							//calculate gyroscope error due to noice
	Calibrate_Accel();
}

void Get_Data(float* Omega,float *Accel)
	{
		//long long int count;
		//float Anglex=0.0,Angley=0.0,Anglez=0.0;
		//uint32_t Time=0;
		int k=0,i=0;
		uint8_t regadd=0x43;
		uint8_t Data[6];
		uint16_t Data_uint16;
		//float Omega[3],Accel[3];
		//Init_Gyroscope();

		 for(k=0;k<6;k++)											//reading six resister datas for gyroscope
			 {
				 Data[k]=Rec_data (regadd);
				 regadd++;
			 }

		 for(i=0;i<3;i++)
			 {
				 Data_uint16= ((Data[2*i]<<8)|Data[2*i+1]);		   	//reuniting data
				 int16_t Temp_int16=Data_uint16;
				 int Temp_int=Temp_int16;
				 Temp_int=Temp_int-Offset[i];						//subtracting Offset
				 Omega[i]=(float)Temp_int/131.072;					//dividing by sensitivity(2^16/500) for range (-250 to +250)
			 }

		 regadd=0x3B;
		 for(k=0;k<6;k++)											//reading six resister datas for accelerometer
			 {
				 Data[k]=Rec_data (regadd);
				 regadd++;
			 }

		 for(i=0;i<3;i++)
			 {
				 Data_uint16= ((Data[2*i]<<8)|Data[2*i+1]);		   	//reuniting data
				 int16_t Temp_int16=Data_uint16;
				 int Temp_int=Temp_int16;
				 Temp_int=Temp_int-Offset_accel[i];					//subtracting Offset
				 Accel[i]=(float)Temp_int/16384.0;                  //dividing by the sensitivity
			 }
		 /*
--------------------------------------------------------- to get the present angle------------------------------------------------------------------------------------------
		 //Time=TIM_GetCounter(TIM2);								//taking counter for evaluating Time for one loop
		 //TIM_SetCounter(TIM2,0);									//resetting counter to one
		 Anglex+=(Omega[0])*Time/400000.0;						//multiplying ang. velocity by Time (dividing by period*10) to grt change of Angle in previous loop
		 Angley+=(Omega[1])*Time/400000.0;							//adding change in Angle to previous Angle
		 Anglez+=(Omega[2])*Time/400000.0;
		 */

	}
