/*定义运动学逆解*/
#ifndef _PATHPLAN_H_
#define _PATHPLAN_H_

#include"hexapod.h"

extern float ruler;
extern int to_point[20];

void pathplan(struct hexapod* hexa_p);
void get_theta_dis(float *p_theta,float *p_dis,int x_old,int y_old,int x,int y,int x_new,int y_new);


#endif
