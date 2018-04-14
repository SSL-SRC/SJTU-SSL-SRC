/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    MPU6050_driver.c          
* 功能描述:    
           MPU6050陀螺仪驱动程序
* 版本      作者       时间          状态
* V1.0      sunlf      2016.03.29    创建文件
*****************************************************************************
*****************************************************************************/ 

#include "MPU6050_driver.h"
#include "I2c.h"
#include "math.h"
#include "stm32f4xx.h"


MPU6050_ACC_GYRO_ADC acc_gyro_adc = {0};
MPU6050_ACC_GYRO_MEASUREMENT acc_gyro_actul = {0};

LOWPASS_1ND_PARAMETER  z_factory ={0.02,0.98};
MPU6050_ACC_GYRO_ANGEL acc_agro_angel = {0};

#define SAMPLE_T  0.02   //单位秒  

float angel_z = 0.0;
float angel_x = 0.0;
float angel_y = 0.0;

short adc_offset_buf[10];
int adc_offset_gyro_z;
/*******************************************************************************
* Function Name   : MPU6050_WHO_AM_I
* Description	  : MPU6050芯片是否正常
*					
* Input 		  : None
* Output		  : None
* Return		  : 返回1表示MPU6050正确 返回0表示MPU6050不正常.
*******************************************************************************/
unsigned char MPU6050_WHO_AM_I(unsigned char i2c_mode)
{
   static unsigned char name = 0;
   if(i2c_mode == 0)//IO模拟时序
   {
     i2c_m_rd_byte(SlaveAddress, WHO_AM_I, &name);
	 if(name == MPU6050_NAME)
	 {
	    return  1;
	 }
	 else
	 {
	    return  0;
	 }
   }
   else if(i2c_mode == 1)//I2C复用功能模式
   {
      name = I2C_MPU6050_ByteRead(WHO_AM_I);
	  if(name == MPU6050_NAME)
	  {
	     return  1;
	  }
	  else
	  {
	     return  0;
	  }
	  
   }
   
   	
   return name;  
}

/*******************************************************************************
* Function Name   : InitMPU6050
* Description	  : 初始化MPU6050陀螺仪模块
*					
* Input		      : None
* Output		  : None
* Return		  : 返回1 正常 返回0 失败.
*******************************************************************************/
int InitMPU6050(void)
{
    int rtn = 1; //
    if(MPU6050_WHO_AM_I(0) == 0)
    {
      return 0;
    }
     rtn &= i2c_m_wr_byte(SlaveAddress, PWR_MGMT_1, 0x00);   //sleep mode disable  内部8Mhz时钟
   //rtn &= i2c_m_wr_byte(SlaveAddress, SMPLRT_DIV, 0x07);  //8分频  采样率1khz/8=125hz 8ms
     rtn &= i2c_m_wr_byte(SlaveAddress, SMPLRT_DIV, 0x01); //2分频1khz/2=500hz  
     rtn &= i2c_m_wr_byte(SlaveAddress, CONFIG, MPU6050_EXT_SYNC_DISABLED|MPU6050_DLPF_BW_20);      //FSYNC input disable 20Hz带宽滤波 Fs=1Khz
     rtn &= i2c_m_wr_byte(SlaveAddress, GYRO_CONFIG,MPU6050_XG_YG_ZG_ST_DIS| MPU6050_GYRO_FS_500);  //陀螺仪自测取消 陀螺仪最大量程设置为+-500°/s  
     rtn &= i2c_m_wr_byte(SlaveAddress, ACCEL_CONFIG, MPU6050_XA_YA_ZA_ST_DIS|MPU6050_ACCEL_FS_4); //+-4g 

   
   //配置MPU6050 的中断模式 和中断电平模式
   //rtn &= i2c_m_wr_byte(SlaveAddress, INT_PIN_CFG, MPU6050_INT_LEVEL_ACTIVE_LOW|MPU6050_INT_RD_CLEAR);  //低电平产生中断 
   //rtn &= i2c_m_wr_byte(SlaveAddress, INT_ENABLE, MPU6050_INTERRUPT_DATA_RDY_BIT, MPU6050_INTERRUPT_DATA_RDY_BIT);	//数据转换完成中断

     return rtn;
   

}

/*******************************************************************************
* Function Name   : MPU6050_is_DRY
* Description	  : 可以读取数据 发生中断
*					
* Input 		  : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
unsigned char MPU6050_is_DRY(void)
{
    if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)==Bit_RESET)//低电平有效
	{
	   return 1;
	}
	else 
	{
	   return 0;
	}
		
	  
}

/*******************************************************************************
* Function Name   : Get_RegData
* Description	  : 获取陀螺仪寄存器16位数据
*					
* Input 		  : unsigned char Reg_Address
* Output		  : None
* Return		  : None.
*******************************************************************************/
unsigned int Get_RegData(unsigned char Reg_Address)
{
	unsigned char data_h,data_l;
	//i2c_m_rd_byte(SlaveAddress, Reg_Address, &data_h);
	//i2c_m_rd_byte(SlaveAddress, Reg_Address+1, &data_l);
	//if(MPU6050_is_DRY())
	{
	  data_h = I2C_MPU6050_ByteRead(Reg_Address);
	}
	//if(MPU6050_is_DRY())
	{
	  data_l = I2C_MPU6050_ByteRead(Reg_Address+1);
	}
	
	return (data_h << 8)+ data_l;   //合成数据
}

/*******************************************************************************
* Function Name   : Caulate_adc_offset
* Description	  : 计算陀螺仪禁止时候的adc零漂值
*					
* Input 		  : None
* Output		  : None
* Return		  : None.
*******************************************************************************/
void Caulate_adc_offset(void)
{   

    char i;
	int j;
	for(i = 0; i < 10; i ++)
	{
	   adc_offset_buf[i] = Get_RegData(GYRO_ZOUT_H);
	   for(j = 0; j < 200000 ; j ++);
	}
    for(i = 0; i < 10; i ++)
    {
      adc_offset_gyro_z += adc_offset_buf[i];
    }
	adc_offset_gyro_z = adc_offset_gyro_z/10;
	
	
}

/*******************************************************************************
* Function Name   : Get_Motion_Adc
* Description	  : 获取陀螺仪16位寄存器值
*					
* Input 		  : unsigned char Reg_Address
* Output		  : None
* Return		  : None.
*******************************************************************************/
void Get_Motion_data(MPU6050_ACC_GYRO_ADC *acc_gyro_adc,MPU6050_ACC_GYRO_MEASUREMENT *acc_gyro_actual)
{   
	/*acc_gyro_adc->acc_x = Get_RegData(ACCEL_XOUT_H);
	acc_gyro_adc->acc_y = Get_RegData(ACCEL_YOUT_H);
	acc_gyro_adc->acc_z = Get_RegData(ACCEL_ZOUT_H);
	acc_gyro_adc->gyro_x = Get_RegData(GYRO_XOUT_H);
	acc_gyro_adc->gyro_y = Get_RegData(GYRO_YOUT_H);
	*/
	acc_gyro_adc->gyro_z = Get_RegData(GYRO_ZOUT_H);
  
	/*acc_gyro_actual->ACC_x = (acc_gyro_adc->acc_x - ACC_SEN_X_OFFSET)/ ACC_SEN_SCAN_FACTOR ; //对加速度进行量化，得出单位为g的加速度值
	acc_gyro_actual->ACC_y = (acc_gyro_adc->acc_y - ACC_SEN_Y_OFFSET)/ ACC_SEN_SCAN_FACTOR;
	acc_gyro_actual->ACC_z = (acc_gyro_adc->acc_z -ACC_SEN_Z_OFFSET)/ ACC_SEN_SCAN_FACTOR;
	acc_gyro_actual->GYRO_x = (acc_gyro_adc->gyro_x - GYRO_SEN_X_OFFSET)/ GYRO_SEN_SCAN_FACTOR;//对角速度进行量化，得出单位为°/s的角速度值
	acc_gyro_actual->GYRO_y = (acc_gyro_adc->gyro_y - GYRO_SEN_Y_OFFSET)/ GYRO_SEN_SCAN_FACTOR;
    */
	//acc_gyro_actual->GYRO_z = (acc_gyro_adc->gyro_z -GYRO_SEN_Z_OFFSET )/ GYRO_SEN_SCAN_FACTOR;
	acc_gyro_actual->GYRO_z = (acc_gyro_adc->gyro_z -adc_offset_gyro_z )/ GYRO_SEN_SCAN_FACTOR;
}


/*******************************************************************************
* Function Name   : Lowpass_1nd_caulate
* Description	  : 一阶互补滤波
*					
* Input 		  : unsigned char Reg_Address
* Output		  : None
* Return		  : None.
*******************************************************************************/
void Lowpass_1nd_caulate(MPU6050_ACC_GYRO_MEASUREMENT meam,LOWPASS_1ND_PARAMETER factory)
{

	 
	//angel_z = atan(meam.ACC_y/meam.ACC_x)*180/3.14159;
	/*一阶互补滤波*/
	if(meam.ACC_z !=0)
	{
	   angel_x = atan2(meam.ACC_y,meam.ACC_z)*180/3.14159;
	}
	
	acc_agro_angel.gyro_x_angel =z_factory.acc_factor*angel_x+z_factory.gyro_factor*(acc_agro_angel.gyro_x_angel + meam.GYRO_x*SAMPLE_T);

    if(meam.ACC_z !=0)
	{
	   angel_y = atan2(-meam.ACC_x,meam.ACC_z)*180/3.14159;
	}
	
	acc_agro_angel.gyro_y_angel =z_factory.acc_factor*angel_y+z_factory.gyro_factor*(acc_agro_angel.gyro_y_angel + meam.GYRO_y*SAMPLE_T);

	if(meam.ACC_x !=0)
	{
	   angel_z = atan2(meam.ACC_y,meam.ACC_x)*180/3.14159;
	}
	if((meam.GYRO_z > 1.0) ||(meam.GYRO_z < -1.0))   //1.0*GYRO_SEN_SCAN_FACTOR = 65.36 跳动浮动大于65.36则认为MPU6050旋转了
	{
	 acc_agro_angel.gyro_z_angel =1*(acc_agro_angel.gyro_z_angel + meam.GYRO_z*SAMPLE_T);
	}
	if(acc_agro_angel.gyro_z_angel > 180.0)
	{
	  acc_agro_angel.gyro_z_angel = acc_agro_angel.gyro_z_angel - 360.0;
	}
	else if(acc_agro_angel.gyro_z_angel < -180.0)
		{
		   acc_agro_angel.gyro_z_angel = acc_agro_angel.gyro_z_angel + 360.0;
		}
	
	

	
}

