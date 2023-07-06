/*定义运动学逆解*/
#ifndef _SPORTS_H_
#define _SPORTS_H_

#include"hexapod.h"

void sports_6lg(const float coord[6][3],const struct hexapod* hexa_p);

void sports_4lg(const float coord[4][3],const struct hexapod* hexa_p);

//抬腿
void lift_6lg(struct hexapod* hexa_p,int mode,int N);

//抬腿
void drop_6lg(struct hexapod* hexa_p,int mode,int N);

//六足形态，从蹲下到站立，插值点个数为N
void stand(struct hexapod* hexa_p ,int N);

//六足形态，从站立到蹲下，插值点个数为N
void crouch(struct hexapod* hexa_p ,int N);

//四足形态转六足形态
void four2six(struct hexapod* hexa_p);

//六足形态转四足形态
void six2four(struct hexapod* hexa_p);

//轮足形态转六足形态
void wheel2six(struct hexapod* hexa_p);

//六足形态转轮足形态
void six2wheel(struct hexapod* hexa_p);

//移动到一个固定状态
void to_state(struct hexapod* hexa_p,float roll,float pitch,float yaw,float tz);

//六足模式下直行一个周期
void straight_6lg(struct hexapod* hexa_p);

//六足模式下旋转
void turn_6lg(struct hexapod* hexa_p);

//四足模式下直行一个周期
void straight_4lg(struct hexapod* hexa_p);

//四足模式下旋转
void turn_4lg(struct hexapod* hexa_p);

//轮足模式下直行
void straight_wheel(struct hexapod* hexa_p);

//轮足模式转弯
void turn_wheel(struct hexapod* hexa_p);

void to_state_stable(struct hexapod* hexa_p,float roll,float pitch);

void servo_correct(struct hexapod* hexa_p);

void get_data_turn(const struct hexapod* hexa_p,float *p_R,float *p_theta);

void getspeed_straight(int wheelspeed[4],int maxspeed,float theta);

void getspeed_turn(int wheelspeed[4],int maxspeed,float R_point,int mode);

#endif
