/*定义机器人运动*/
#include"stdlib.h"
#include "math.h"
#include "hexapod.h"
#include "trail.h"
#include"stdio.h"
#include "stm32g4xx_hal.h"
#include"inverse.h"
#include"pca9685.h"
#include "mainloop.h"
#include "usart.h"
#include "sports.h"

#define deg2rad 0.01745329

//六足模式下直行一个周期
void path_straight_6lg(struct hexapod* hexa_p,float distance)
{
	//根据速度选取插值点个数
	int N=14;
	float step_dis=(*hexa_p).span;
	float remain_dis=distance;
	float do_dis;

	float coord[6][3];
	while(1){
		/*收到停止信号时退出*/
		if(data.command==c_stop){
			/*放腿*/
			drop_6lg(hexa_p,(*hexa_p).mode_6lg,N);
			break;
		}
		if(fabs(remain_dis)<1e-4)
			break;
		if(fabs(remain_dis)>step_dis){
			do_dis=step_dis*(fabs(remain_dis)/remain_dis);
			remain_dis=remain_dis-do_dis;
		}
		else{
			do_dis=remain_dis;
			remain_dis=0.0;
		}
		/*直行一个周期*/
		for(int i=1;i<=N;i++){
			straight_6lg_step(coord,hexa_p,data.data*deg2rad,(*hexa_p).mode_6lg,i, N);
			sports_6lg(coord,hexa_p);
			HAL_Delay(30);
		}
		(*hexa_p).mode_6lg=((*hexa_p).mode_6lg+1)%2;
	}
}

//六足模式下旋转
void path_turn_6lg(struct hexapod* hexa_p,float theta)
{
	//根据速度选取插值点个数
	int N=14;
	float step_theta=M_PI/9;
	float remain_theta=theta;
	float do_theta;

	float coord[6][3];
	while(1){
		/*收到停止信号时退出*/
		if(data.command==c_stop){
			/*放腿*/
			drop_6lg(hexa_p,(*hexa_p).mode_6lg,N);
			break;
		}
		if(fabs(remain_theta)<1e-4)
			break;
		if(fabs(remain_theta)>step_theta){
			do_theta=step_theta*(fabs(remain_theta)/remain_theta);
			remain_theta=remain_theta-do_theta;
		}
		else{
			do_theta=remain_theta;
			remain_theta=0.0;
		}

		for(int i=1;i<=N;i++){
			turn_6lg_step(coord,hexa_p,0.0,remain_theta,(*hexa_p).mode_6lg,i,N);
			sports_6lg(coord,hexa_p);
			HAL_Delay(30);
		}
		(*hexa_p).mode_6lg=((*hexa_p).mode_6lg+1)%2;
	}
}

float ruler;
int to_point[20];

void get_theta_dis(float *p_theta,float *p_dis,int x_old,int y_old,int x,int y,int x_new,int y_new)
{
	int x1,x2,y1,y2;
	x1=x-x_old;
	y1=y-y_old;
	x2=x_new-x;
	y2=y_new-y;
	int dot=x1*x2+y1*y2;
	double coth=(double)dot/(sqrt(x1*x1+y1*y1)*sqrt(x2*x2+y2*y2));
	(*p_theta)=acos(coth);
	if(x1*y2-x2*y1<0)
		(*p_theta)=-(*p_theta);
	(*p_dis)=sqrt(x2*x2+y2*y2)*ruler;
}

void pathplan(struct hexapod* hexa_p)
{
	int N=14;
	/*直行首先抬腿*/
	if((*hexa_p).mode_6lg==2)
		lift_6lg(hexa_p,1,N);
	float theta,distance;
	int x,y,x_new,y_new,x_old,y_old;

	x_old=0;
	y_old=-1;
	x=0;
	y=0;
	for(int i=0;i<10;i++){
		x_new=to_point[2*i];
		y_new=to_point[2*i+1];
		if(abs(x-x_new)<1e-4&&abs(y-y_new)<1e-4)
			continue;
		get_theta_dis(&theta,&distance,x_old,y_old,x,y,x_new,y_new);
		path_turn_6lg(hexa_p,theta);
		path_straight_6lg(hexa_p,distance);
		x_old=x;
		y_old=y;
		x=x_new;
		y=y_new;
	}

	drop_6lg(hexa_p,(*hexa_p).mode_6lg,N);
}


