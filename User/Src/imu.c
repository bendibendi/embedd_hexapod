/*mpu6050 相关函数以及imu求解*/
#include "i2c.h"
#include "tim.h"
#include "math.h"
#include "stdio.h"
#include "hexapod.h"
#include "sports.h"
#include "mainloop.h"

#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)

#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)

#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)

#define	GYRO_CONFIG		0x1B
//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
/*
0bxx<<3:
00 ~ +-250°
01 ~ +-500°
10 ~ +-1000°
11 ~ +-2000°
*/


#define	ACCEL_CONFIG	0x1C
//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
/*
0bxx<<3:
00 ~ +-2g
01 ~ +-4g
10 ~ +-18g
11 ~ +-16g
*/

#define	ACCEL_XOUT_H	0x3B//加速度计测量值寄存器地址
#define	ACCEL_XOUT_L	0x3C

#define	ACCEL_YOUT_H	0x3D //2 3
#define	ACCEL_YOUT_L	0x3E

#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
/********************/
#define	TEMP_OUT_H		0x41//温度测量值寄存器地址
#define	TEMP_OUT_L		0x42
/*********************/
#define	GYRO_XOUT_H		0x43// 8 9 陀螺仪测量值寄存器地址
#define	GYRO_XOUT_L		0x44

#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46

#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

#define MPU6050_ADDR  	0xD0

#define MPU6050_WHO_AM_I         0x75

#define rad2deg 57.29578

unsigned char IIC_buf[14];

signed short ax;
signed short ay;
signed short az;

signed short gx;
signed short gy;
signed short gz;

signed short ax_cl = 0;
signed short ay_cl = 0;
signed short az_cl = 0;

signed short gx_cl = 0;
signed short gy_cl = 0;
signed short gz_cl = 0;


void IIC_Write(unsigned char dev_addr, unsigned char mem_addr, unsigned char data)
{
	HAL_I2C_Mem_Write(&hi2c3, dev_addr, mem_addr,I2C_MEMADD_SIZE_8BIT, &data, 1, 2);
}

void IIC_Read(unsigned char dev_addr, unsigned char mem_addr, unsigned char *buf, unsigned char len)
{
	HAL_I2C_Mem_Read(&hi2c3, dev_addr, mem_addr,I2C_MEMADD_SIZE_8BIT, buf, len, 2);
}

/*写入初始化MPU6050*/
void MPU6050_Init(void)
{
	unsigned char temp = 0;
reset_MPU6050:
	IIC_Write(MPU6050_ADDR,PWR_MGMT_1,0x80);	//复位MPU6050
	HAL_Delay(50);
	IIC_Write(MPU6050_ADDR,PWR_MGMT_1,0x01);	//唤醒MPU6050，使用x轴PLL为时钟
	IIC_Read(MPU6050_ADDR, MPU6050_WHO_AM_I, &temp, 1);	//读ID
	temp &= 0x7e;
	if(temp != 0x68)
		goto reset_MPU6050;

	IIC_Write(MPU6050_ADDR,SMPLRT_DIV,0x00);	//采样率分频，不分频
	IIC_Write(MPU6050_ADDR,CONFIG,0x00);	//低通滤波器，带宽256~260Hz

	IIC_Write(MPU6050_ADDR,GYRO_CONFIG,(0x00<<3));	//陀螺量程,+-250°
	IIC_Write(MPU6050_ADDR,ACCEL_CONFIG,(0x00<<3));	//加速度计量程,+-2g
}

void MPU6050_Get(void)
{
	IIC_Read(MPU6050_ADDR,ACCEL_XOUT_H,IIC_buf,14);

	ax = (IIC_buf[0]<<8) + IIC_buf[1] - ax_cl;//X轴加速度
	ay = (IIC_buf[2]<<8) + IIC_buf[3] - ay_cl;//Y轴加速度
	az = (IIC_buf[4]<<8) + IIC_buf[5] - az_cl;//Z轴加速度

	gx = (IIC_buf[8]<<8) + IIC_buf[9] - gx_cl;//X轴角速度
	gy = (IIC_buf[10]<<8) + IIC_buf[11] - gy_cl;//Y轴角速度
	gz = (IIC_buf[12]<<8) + IIC_buf[13] - gz_cl;//Z轴角速度
}

/*MPU6050校准函数*/
#define	CL_cnt	128
void MPU6050_calibrate()
{
	unsigned short i;
	signed int temp[6] = {0};
	for(i=0; i<CL_cnt; i++)
	{
		HAL_Delay(10);
		MPU6050_Get();
		temp[0] += ax;
		temp[1] += ay;
		temp[2] += az;
		temp[3] += gx;
		temp[4] += gy;
		temp[5] += gz;
	}
	ax_cl = temp[0]/CL_cnt;
	ay_cl = temp[1]/CL_cnt;
	az_cl = temp[2]/CL_cnt - (0xffff>>2); //平放时z轴有重力加速度g，减去g值
	gx_cl = temp[3]/CL_cnt;
	gy_cl = temp[4]/CL_cnt;
	gz_cl = temp[5]/CL_cnt;
}

/*参数*/
float roll_v , pitch_v;              //换算到x,y轴上的角速度
/*三个状态，先验状态，观测状态，最优估计状态*/
float gyro_roll, gyro_pitch;        //陀螺仪积分计算出的角度，先验状态
float acc_roll, acc_pitch;          //加速度计观测出的角度，观测状态
float k_roll, k_pitch;              //卡尔曼滤波后估计出最优角度，最优估计状态
/*误差协方差矩阵P*/
float e_P[2][2];             //误差协方差矩阵，这里的e_P既是先验估计的P，也是最后更新的P
/*卡尔曼增益K*/
float k_k[2][2];             //这里的卡尔曼增益矩阵K是一个2X2的方阵
/*采样间隔*/
float dt;
#define correct_kg 131.07
#define correct_ka 1673.519


void self_stable(struct hexapod* hexa_p)
{
	/*参数赋初值*/
	dt=0.01;
	roll_v = 0;
	pitch_v = 0;
	gyro_roll = 0;
	gyro_pitch = 0;
	acc_roll = 0;
	acc_pitch = 0;
	k_roll = 0;
	k_pitch = 0;
	e_P[0][0] =1;
	e_P[0][1] =0;
	e_P[1][0] =0;
	e_P[1][1] =1;
	k_k[0][0]=0;
	k_k[0][1]=0;
	k_k[1][0]=0;
	k_k[1][1]=0;

	MPU6050_calibrate();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_Delay(500);
	while(1){
		if((data.command==c_stop)||(data.command==c_self_stable_quit)){
			to_state(hexa_p,0.0,0.0,0.0,(*hexa_p).tz);
			break;
		}
		if(fabs(k_roll)>1.0||fabs(k_pitch)>1.0)
			to_state_stable(hexa_p,k_roll/57.29578,k_pitch/57.29578);
//		printf("k_roll %f\n",k_roll);
//		printf("k_pitch%f\n",k_pitch);
		HAL_Delay(30);

	}
	HAL_TIM_Base_Stop_IT(&htim2);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&htim2))
    {
    	printf("tim\n");
    	/*获取角速度和加速度 */
    	MPU6050_Get();

    	/*step1:计算先验状态*/
    	/*计算x,y轴上的角速度*/
    	roll_v = (gx + ((sin(k_pitch)*sin(k_roll))/cos(k_pitch))*gy + ((sin(k_pitch)*cos(k_roll))/cos(k_pitch))*gz)/correct_kg;//roll轴的角速度
    	pitch_v = (cos(k_roll)*gy - sin(k_roll)*gz)/correct_kg;//pitch轴的角速度
    	gyro_roll = k_roll + dt*roll_v;//先验roll角度
    	gyro_pitch = k_pitch + dt*pitch_v;//先验pitch角度

    	/*step2:计算先验误差协方差矩阵P*/
    	e_P[0][0] = e_P[0][0] + 0.0025;//这里的Q矩阵是一个对角阵且对角元均为0.0025
//    	e_P[0][1] = e_P[0][1] + 0;
//    	e_P[1][0] = e_P[1][0] + 0;
    	e_P[1][1] = e_P[1][1] + 0.0025;

    	/*step3:更新卡尔曼增益K*/
    	k_k[0][0] = e_P[0][0]/(e_P[0][0]+0.3);
//    	k_k[0][1] = 0;
//    	k_k[1][0] = 0;
    	k_k[1][1] = e_P[1][1]/(e_P[1][1]+0.3);

    	/*step4:计算最优估计状态*/
    	/*观测状态*/
    	//roll角度
    	acc_roll = atan((float)ay/az)*rad2deg;
    	//pitch角度
    	acc_pitch = -1*atan((float)ax/ sqrt((float)ay*ay + az*az))*rad2deg;
//    	printf("acc_roll    : %f\n",acc_roll);
//    	printf("acc_pitch   : %f\n",acc_pitch);
    	/*最优估计状态*/
    	k_roll = gyro_roll + k_k[0][0]*(acc_roll - gyro_roll);
    	k_pitch = gyro_pitch + k_k[1][1]*(acc_pitch - gyro_pitch);
//    	printf("k_roll    : %f\n",k_roll);
//    	printf("k_pitch   : %f\n",k_pitch);

    	/*step5:更新协方差矩阵P*/
    	e_P[0][0] = (1 - k_k[0][0])*e_P[0][0];
//    	e_P[0][1] = 0;
//    	e_P[1][0] = 0;
    	e_P[1][1] = (1 - k_k[1][1])*e_P[1][1];


    }
}


