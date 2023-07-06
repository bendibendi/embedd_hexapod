#include "pca9685.h"
#include "math.h"
#include "i2c.h"

uint8_t pca_read(uint8_t startAddress,int num) {
    uint8_t tx[1];
    uint8_t buffer[1];
    tx[0]=startAddress;

    if(num==1){
        HAL_I2C_Master_Transmit(&hi2c1,pca_adrr, tx,1,10000);
        HAL_I2C_Master_Receive(&hi2c1,pca_adrr,buffer,1,10000);
    }
    else{
        HAL_I2C_Master_Transmit(&hi2c2,pca_adrr, tx,1,10000);
        HAL_I2C_Master_Receive(&hi2c2,pca_adrr,buffer,1,10000);
    }

    return buffer[0];
}

void pca_write(uint8_t startAddress, uint8_t buffer,int num) {
    uint8_t tx[2];
    tx[0]=startAddress;
    tx[1]=buffer;
    if(num==1)
    	HAL_I2C_Master_Transmit(&hi2c1,pca_adrr, tx,2,10000);
    else
    	HAL_I2C_Master_Transmit(&hi2c2,pca_adrr, tx,2,10000);

}

void pca_setfreq(float freq)//设置PWM频率
{
		uint8_t prescale,oldmode1,oldmode2,newmode1,newmode2;
		double prescaleval;
		freq *= 0.96;
		prescaleval = 25000000;
		prescaleval /= 4096;
		prescaleval /= freq;
		prescaleval -= 1;
		prescale =floor(prescaleval + 0.5f);
		oldmode1 = pca_read(pca_mode1,1);
		oldmode2 = pca_read(pca_mode1,2);
		newmode1 = (oldmode1&0x7F) | 0x10; // sleep
		newmode2 = (oldmode2&0x7F) | 0x10; // sleep
		pca_write(pca_mode1, newmode1,1); // go to sleep
		pca_write(pca_mode1, newmode2,2); // go to sleep
		pca_write(pca_pre, prescale,1); // set the prescaler
		pca_write(pca_pre, prescale,2); // set the prescaler
		pca_write(pca_mode1, oldmode1,1);
		pca_write(pca_mode1, oldmode2,2);
		HAL_Delay(2);
		pca_write(pca_mode1, oldmode1 | 0xa1,1);
		pca_write(pca_mode1, oldmode2 | 0xa1,2);
}

void pca_setpwm(uint8_t num, uint32_t on, uint32_t off)
{
	if(num<=11){
		pca_write(LED0_ON_L+4*num,on,1);
		pca_write(LED0_ON_H+4*num,on>>8,1);
		pca_write(LED0_OFF_L+4*num,off,1);
		pca_write(LED0_OFF_H+4*num,off>>8,1);
	}
	else{
		pca_write(LED0_ON_L+4*(num-12),on,2);
		pca_write(LED0_ON_H+4*(num-12),on>>8,2);
		pca_write(LED0_OFF_L+4*(num-12),off,2);
		pca_write(LED0_OFF_H+4*(num-12),off>>8,2);
	}
}

void PCA_SPT_Init(float hz)
{
//	IIC_Init();
	pca_write(pca_mode1,0x0,1);
	pca_write(pca_mode1,0x0,2);
	pca_setfreq(hz);//设置PWM频率
	bias[0]=44.0;
	bias[1]=190.0;
	bias[2]=88.0;
	bias[3]=87.0;
	bias[4]=93.0;
	bias[5]=92.0;
	bias[6]=142.0;
	bias[7]=84.0;
	bias[8]=98.0;
	bias[9]=90.0;
	bias[10]=90.0;
	bias[11]=90.0;
	bias[12]=133.0;
	bias[13]=90.0;
	bias[14]=87.0;
	bias[15]=88.0;
	bias[16]=93.0;
	bias[17]=92.0;
	bias[18]=39.0;
	bias[19]=93.0;
	bias[20]=96.0;
	bias[21]=90.0;
	bias[22]=90.0;
	bias[23]=90.0;

}

void PCA_SPT(uint8_t num,float end_angle)
{
	uint32_t off=0;

	off=(uint32_t)(102.4+end_angle*2.275555);
	pca_setpwm(num,0,off);
}


int bias[24];
void SERVO_SPT(uint8_t num,float angle)
{
	int w[24]={-1,1,-1,-1,1,-1,-1,1,-1, 1,1,1   ,   -1,1,-1,-1,1,-1,-1,1,-1,1,1,1};
	//int b[24]={44.0,90.0,88.0,87.0,93.0,92.0,142.0,84.0,98.0,90,90,90   , 133.0,90.0,87.0,88.0,93.0,92.0,39.0,93.0,96.0,   90,90,90};
	//PCA_SPT(num,b[num]+w[num]*angle);
	PCA_SPT(num,bias[num]+w[num]*angle);
}










