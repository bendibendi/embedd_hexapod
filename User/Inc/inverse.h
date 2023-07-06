/*定义运动学逆解*/
#ifndef _INVERSE_H_
#define _INVERSE_H_
#include"hexapod.h"

void get_theta_6lg(float angle[6][3],const float coord[6][3],const struct hexapod* spider);
void get_theta_4lg(float angle[4][3],const float coord[4][3],const struct hexapod* spider);

#endif
