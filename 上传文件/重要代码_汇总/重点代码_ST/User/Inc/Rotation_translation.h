/*定义坐标的平移和旋转*/
#ifndef _ROTATION_H_
#define _ROTATION_H_

#include "math.h"
#include "hexapod.h"

//平移
void cotrs_t(const float* coord,float* exp_coord,float tx,float ty,float tz);
//绕x轴旋转
void cotrs_x(const float* coord,float* exp_coord,float roll);
//绕y轴旋转
void cotrs_y(const float* coord,float* exp_coord,float pitch);
//绕z轴旋转
void cotrs_z(const float* coord,float* exp_coord,float yaw);
//按照z,y,x的顺序旋转
void cotrs_xyz(const float* coord,float* exp_coord,float roll,float pitch,float yaw);
//旋转加平移
void cotrs(const float* coord,float* exp_coord,float roll,float pitch,float yaw,float tx,float ty,float tz);

//平移
void cotrs_t_6lg(float coord[6][3],float tx,float ty,float tz);
//绕x轴旋转
void cotrs_x_6lg(float coord[6][3],float roll);
//绕y轴旋转
void cotrs_y_6lg(float coord[6][3],float pitch);
//绕z轴旋转
void cotrs_z_6lg(float coord[6][3],float yaw);
//按照z,y,x的顺序旋转
void cotrs_xyz_6lg(float coord[6][3],float roll,float pitch,float yaw);
//旋转加平移
void cotrs_6lg(float coord[6][3],float roll,float pitch,float yaw,float tx,float ty,float tz);
//四足*******************
//平移
void cotrs_t_4lg(float coord[4][3],float tx,float ty,float tz);
//绕x轴旋转
void cotrs_x_4lg(float coord[4][3],float roll);
//绕y轴旋转
void cotrs_y_4lg(float coord[4][3],float pitch);
//绕z轴旋转
void cotrs_z_4lg(float coord[4][3],float yaw);
//按照z,y,x的顺序旋转
void cotrs_xyz_4lg(float coord[4][3],float roll,float pitch,float yaw);
//旋转加平移
void cotrs_4lg(float coord[4][3],float roll,float pitch,float yaw,float tx,float ty,float tz);
//针对某一个点进行旋转
void cotrs_vs_point(const float coord[3],float coord_exp[3],float x0,float y0,float theta);

#endif
