#include"stm32f10x.h"

int main(void)
{
	My_I2C_Init();

  while(1)
  {

  }
}
void My_I2C_Init(void)
{
	//1.IO引脚初始化
	//对I2C1进行重映射
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE );
	//对PB8P89初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);//开启时钟

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);//施加复位信号
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);//释放复位信号

	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;

	I2C_Init(I2C1, &I2C_InitStruct);
	I2C_Cmd(I2C1, ENABLE);//使能I2C1
}
int My_I2C_SendBytes(I2C_TypeDef *I2Cx, uint8_t Addr,uint8_t *pData, uint16_t Size)
{
	//1.等待总线空闲
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET);

	//2.发送起始信号
	I2C_GenerateSTART(I2Cx, ENABLE);

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);//检查是否发送完
	//3.寻址阶段
	I2C_Clearflag(I2Cx, I2C_FLAG_AF);//清除标志位

	I2C_SendData(I2Cx, Addr & 0xfe);//发送地址+写信号
	while(1)
	if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET)
	{
		break;
	}
	if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET)
	{

		I2C_GenerateSTOP(I2Cx, ENABLE);//发送停止信号
		return -1;//应答失败
	}
	//清除ADDR标志位
	I2C_ReadRegister(I2Cx, I2C_Register_SR1);
	I2C_ReadRegister(I2Cx, I2C_Register_SR2);

	//4.发送数据阶段
	for(uint16_t i = 0; i < Size; i++)
	{
		while(1)
		{
			if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET)
			{
				I2C1_GenerateSTOP(I2Cx, ENABLE);//发送停止信号
				return -2;//应答失败
			}
    if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == SET)
			{
				break;
			}
		}

		I2C_SendData(I2Cx, pData[i]);
	}
	while(1)
	{
		if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET)
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);//发送停止信号
			return -2;//数据被拒收
		}
		if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == SET)
		{
			break;
		}
	}
	//发送停止位
	I2C_GenerateSTOP(I2Cx, ENABLE);
	return 0;//成功

}
