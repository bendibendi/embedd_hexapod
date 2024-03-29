#ifndef _IMU_H
#define _IMU_H

void MPU6050_Init(void);
void MPU6050_Get(void);

void MPU6050_calibrate();
void self_stable(struct hexapod* hexa_p);

extern unsigned char IIC_buf[14];

extern signed short ax;
extern signed short ay;
extern signed short az;

extern signed short gx;
extern signed short gy;
extern signed short gz;

extern signed short ax_cl;
extern signed short ay_cl;
extern signed short az_cl;

extern signed short gx_cl;
extern signed short gy_cl;
extern signed short gz_cl;

#endif

