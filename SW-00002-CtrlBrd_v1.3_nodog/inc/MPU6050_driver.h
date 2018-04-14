/********************************************************************************
********************************************************************************
* 权限:    杭州南江机器人股份有限公司
* 文件名:    MPU6050_driver.h         
* 功能描述:    
           MPU6050陀螺仪驱动程序头文件
* 版本      作者       时间          状态
* V1.0      sunlf      2016.03.29    创建文件
*****************************************************************************
*****************************************************************************/ 

#ifndef _MPU6050_DRIVER_H
#define _MPU6050_DRIVER_H

/* MPU6050 Register Address ------------------------------------------------------------*/
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz) sample_rate=gyroscope output rate/(1+SMPLRT_DIV)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define INT_PIN_CFG     0x37    //中断配置寄存器
#define INT_ENABLE      0x38
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43    //陀螺仪X轴角速度高16位
#define	GYRO_XOUT_L		0x44	//陀螺仪X轴角速度低16位
#define	GYRO_YOUT_H		0x45    //陀螺仪Y轴角速度高16位
#define	GYRO_YOUT_L		0x46    //陀螺仪Y轴角速度低16位
#define	GYRO_ZOUT_H		0x47    //陀螺仪Z轴角速度高16位
#define	GYRO_ZOUT_L		0x48    //陀螺仪X轴角速度低16位
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)config powermode and clock source
#define	WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据  AR0=0 1101000x x=0 write x=1 read

#define MPU6050_NAME    0x68
/*0x37  INT_PIN_CFG*/
#define MPU6050_INT_LEVEL_ACTIVE_LOW       (1 << 7)  	//INT PIN ACTICE LOW
#define MPU6050_INT_RD_CLEAR               (1 << 4)

/*0x38  INT_ENBALE*/
#define MPU6050_INTERRUPT_DATA_RDY_BIT     (1 << 0)

/*0x6B Power management 1*/
#define MPU6050_CLOCK_INTERNAL          0x00
#define MPU6050_CLOCK_PLL_XGYRO         0x01
#define MPU6050_CLOCK_PLL_YGYRO         0x02
#define MPU6050_CLOCK_PLL_ZGYRO         0x03
#define MPU6050_CLOCK_PLL_EXT32K        0x04
#define MPU6050_CLOCK_PLL_EXT19M        0x05
#define MPU6050_CLOCK_KEEP_RESET        0x07

/*0x1A  configuration*/
#define MPU6050_EXT_SYNC_DISABLED       (0 << 3)
#define MPU6050_EXT_SYNC_TEMP_OUT_L     (1 << 3)
#define MPU6050_EXT_SYNC_GYRO_XOUT_L    (2 << 3)
#define MPU6050_EXT_SYNC_GYRO_YOUT_L    (3 << 3)
#define MPU6050_EXT_SYNC_GYRO_ZOUT_L    (4 << 3)
#define MPU6050_EXT_SYNC_ACCEL_XOUT_L   (5 << 3)
#define MPU6050_EXT_SYNC_ACCEL_YOUT_L   (6 << 3)
#define MPU6050_EXT_SYNC_ACCEL_ZOUT_L   (7 << 3)

#define MPU6050_DLPF_BW_256         0  
#define MPU6050_DLPF_BW_188         1
#define MPU6050_DLPF_BW_98          2
#define MPU6050_DLPF_BW_42          3
#define MPU6050_DLPF_BW_20          4    //对加速度和陀螺仪低通滤波 带宽20Hz
#define MPU6050_DLPF_BW_10          5
#define MPU6050_DLPF_BW_5           6 

/*0x1B Gyroscope configuration*/
#define MPU6050_GYRO_FS_250         (0 << 3)
#define MPU6050_GYRO_FS_500         (1 << 3)
#define MPU6050_GYRO_FS_1000        (2 << 3)
#define MPU6050_GYRO_FS_2000        (3 << 3)
#define MPU6050_XG_YG_ZG_ST_EN      (7 << 5)
#define MPU6050_XG_YG_ZG_ST_DIS     (0 << 5)

/*0x1C Accelerometer configuration*/
#define MPU6050_ACCEL_FS_2          (0 << 3)
#define MPU6050_ACCEL_FS_4          (1 << 3)
#define MPU6050_ACCEL_FS_8          (2 << 3)
#define MPU6050_ACCEL_FS_16         (3 << 3)
#define MPU6050_XA_YA_ZA_ST_EN      (7 << 5)
#define MPU6050_XA_YA_ZA_ST_DIS     (0 << 5)


#define ACC_SEN_SCAN_FACTOR   8192.00  //65536/8g = 8192 LSB/g +-4g
#define GYRO_SEN_SCAN_FACTOR  65.536    //65536/1000 = 65.536 LSB/(°/s) +-500°/s

#define GYRO_SEN_X_OFFSET    20 // 18~21  陀螺仪静止时候X轴ADC偏置
#define GYRO_SEN_Y_OFFSET    48 // 46~53
#define GYRO_SEN_Z_OFFSET    -33// 61 //  温度环境不同0飘值不同
#define ACC_SEN_X_OFFSET     285  //
#define ACC_SEN_Y_OFFSET     220   
#define ACC_SEN_Z_OFFSET     858  //90508192

/***定义陀螺仪加速度计和陀螺仪的ADC结构体***/
typedef struct
{
    short acc_x;   // 传感器采集的原始数据
	short acc_y;
	short acc_z;
	short gyro_x;
	short gyro_y;
	short gyro_z;
}MPU6050_ACC_GYRO_ADC;

/***定义陀螺仪加速度计和陀螺仪的国际单位结构体***/
typedef struct
{
    float ACC_x;   // 单位[g] 加速度计数据
	float ACC_y;
	float ACC_z;
	float GYRO_x;  //单位[°/s] 陀螺仪数据
	float GYRO_y;
	float GYRO_z;  //绕Z轴旋转速度
}MPU6050_ACC_GYRO_MEASUREMENT;

/***滤波后的实际角度 ***/
typedef struct
{
    float acc_x_angel;
	float acc_y_angel;
	float acc_z_angel;
	float gyro_x_angel;    //陀螺仪X轴旋转角度
	float gyro_y_angel;
	float gyro_z_angel;     //单位[°]
}MPU6050_ACC_GYRO_ANGEL;

/***一阶滤波互补参数***/
typedef struct
{
    float acc_factor;
	float gyro_factor;
	
}LOWPASS_1ND_PARAMETER;


extern MPU6050_ACC_GYRO_ADC acc_gyro_adc ;
extern MPU6050_ACC_GYRO_MEASUREMENT acc_gyro_actul ;
extern LOWPASS_1ND_PARAMETER  z_factory ;
extern MPU6050_ACC_GYRO_ANGEL acc_agro_angel;

int InitMPU6050(void);
unsigned int Get_RegData(unsigned char REG_Address);
void Get_Motion_data(MPU6050_ACC_GYRO_ADC *acc_gyro_adc,MPU6050_ACC_GYRO_MEASUREMENT *acc_gyro_actual);
void Lowpass_1nd_caulate(MPU6050_ACC_GYRO_MEASUREMENT meam,LOWPASS_1ND_PARAMETER factory);
unsigned char MPU6050_WHO_AM_I(unsigned char i2c_mode);
void Caulate_adc_offset(void);





#endif //endif _mpu6050_driver_h
