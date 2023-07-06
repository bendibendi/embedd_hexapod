/*定义运动学逆解*/
#ifndef _TRAIL_H_
#define _TRAIL_H_

#include"hexapod.h"

//移动到固定状状态
void to_state_co(float coord[6][3],const struct hexapod* hexa_p,float roll,float pitch,float yaw,float tz);

//移动到固定状态，自稳定状态下
void to_state_stable_co(float coord[6][3],const struct hexapod* hexa_p,float roll,float pitch,float yaw);

//四足转六足形态
void four2six_step(float coord[6][3],const struct hexapod* hexa_p,int i,int N);

//六足转四足形态
void six2four_step(float coord[6][3],const struct hexapod* hexa_p,int i,int N);

//改变六足足端高度
void change_z_step(float coord[6][3],const struct hexapod* hexa_p,float start_z,float end_z,int i,int N);

//抬腿，六足模式
void lift_6lg_step(float coord[6][3],const struct hexapod* hexa_p,int mode,int step,int N);

//放腿，六足模式
void drop_6lg_step(float coord[6][3],const struct hexapod* hexa_p,int mode,int step,int N);

//前进，mode决定使用哪两组角，i是一步中的第几个插值点，N是一步的总插值点数(N必须是双数)，theta是与前方的夹角
void straight_6lg_step(float coord[6][3],const struct hexapod* hexa_p,float theta,int mode,int step,int N);

//定半径转弯
void turn_6lg_step(float coord[6][3],const struct hexapod* hexa_p,float R,float theta,int mode,int step,int N);

//四足直行，首先移动四条腿
void straight_4lg_leg_step(float coord[4][3],const struct hexapod* hexa_p,float theta,int leg,int step,int N);

//四足直行，移动整个身体
void straight_4lg_body_step(float coord[4][3],const struct hexapod* hexa_p,float theta,int step,int N);

//四足旋转，首先移动四条腿
void turn_4lg_leg_step(float coord[4][3],const struct hexapod* hexa_p,float R,float theta,int leg,int step,int N);

//四足直行，移动整个身体
void turn_4lg_body_step(float coord[4][3],const struct hexapod* hexa_p,float R,float theta,int step,int N);

void move_point_plane_turn(float coord[3],float x0,float y0,float R,float theta,float height,int i,int N);

void move_point_plane(float coord[3],float x0,float y0,float x1,float y1,float height,int i,int N);

#endif
