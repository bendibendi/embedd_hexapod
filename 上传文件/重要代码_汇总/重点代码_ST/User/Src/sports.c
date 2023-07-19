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

void sports_6lg(const float coord[6][3],const struct hexapod* hexa_p)
{
	float angle[6][3];
	get_theta_6lg(angle,coord,hexa_p);
	int m,n;
	for(m=0;m<3;m++){
		for(n=0;n<3;n++){
			SERVO_SPT(m*3+n,angle[m][n]);
		}
	}
	for(m=3;m<6;m++){
		for(n=0;n<3;n++){
			SERVO_SPT((m+1)*3+n,angle[m][n]);
		}
	}
}

void sports_4lg(const float coord[4][3],const struct hexapod* hexa_p)
{
	float angle[4][3];
	get_theta_4lg(angle,coord,hexa_p);
	int m,n;
	for(m=0;m<2;m++){
		for(n=0;n<3;n++){
			SERVO_SPT(m*6+n,angle[m][n]);
		}
	}
	for(m=2;m<4;m++){
		for(n=0;n<3;n++){
			SERVO_SPT(m*6+n,angle[m][n]);
		}
	}
}

//六足形态，从蹲下到站立，插值点个数为N
void stand(struct hexapod* hexa_p ,int N)
{
	float coord[6][3];
	for(int i=1;i<=N;i++){
		change_z_step(coord,hexa_p,-10.0,-(*hexa_p).height,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(30);
	}
	(*hexa_p).tz=-(*hexa_p).height;
	iner(hexa_p);
}

//六足形态，从站立到蹲下，插值点个数为N
void crouch(struct hexapod* hexa_p ,int N)
{
	float coord[6][3];
	for(int i=1;i<=N;i++){
		change_z_step(coord,hexa_p,-(*hexa_p).height,-10.0,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(30);
	}
	(*hexa_p).tz=0.0;
}

//向内
void iner(struct hexapod* hexa_p)
{
	float coord[6][3];
	for(int i=0;i<6;i++){
		coord[i][0]=((*hexa_p).feet_6lg_v)[i][0];
		coord[i][1]=((*hexa_p).feet_6lg_v)[i][1];
		coord[i][2]=-(*hexa_p).height;
	}
	float k;
	int N=5;
	int rela[6]={0,2,4,1,3,5};
	for(int i=0;i<6;i++){
		for(int j=1;j<=N;j++){
			k=(float)j/N;
			coord[rela[i]][0]=((*hexa_p).feet_6lg_v)[rela[i]][0]+0.5*k*(((*hexa_p).feet_6lg_up)[rela[i]][0]-((*hexa_p).feet_6lg_v)[rela[i]][0]);
			coord[rela[i]][1]=((*hexa_p).feet_6lg_v)[rela[i]][1]+0.5*k*(((*hexa_p).feet_6lg_up)[rela[i]][1]-((*hexa_p).feet_6lg_v)[rela[i]][1]);
			coord[rela[i]][2]=-(*hexa_p).height+k*20;
			sports_6lg(coord,hexa_p);
			HAL_Delay(30);
		}
		for(int j=1;j<=N;j++){
			k=(float)j/N;
			coord[rela[i]][0]=((*hexa_p).feet_6lg_v)[rela[i]][0]+(0.5*k+0.5)*(((*hexa_p).feet_6lg_up)[rela[i]][0]-((*hexa_p).feet_6lg_v)[rela[i]][0]);
			coord[rela[i]][1]=((*hexa_p).feet_6lg_v)[rela[i]][1]+(0.5*k+0.5)*(((*hexa_p).feet_6lg_up)[rela[i]][1]-((*hexa_p).feet_6lg_v)[rela[i]][1]);
			coord[rela[i]][2]=-(*hexa_p).height+(1-k)*20;
			sports_6lg(coord,hexa_p);
			HAL_Delay(30);
		}
	}
}

//向外
void outer(struct hexapod* hexa_p)
{
	float coord[6][3];
	for(int i=0;i<6;i++){
		coord[i][0]=((*hexa_p).feet_6lg_up)[i][0];
		coord[i][1]=((*hexa_p).feet_6lg_up)[i][1];
		coord[i][2]=-(*hexa_p).height;
	}
	float k;
	int N=5;
	int rela[6]={0,2,4,1,3,5};
	for(int i=0;i<6;i++){
		for(int j=1;j<=N;j++){
			k=(float)j/N;
			coord[rela[i]][0]=((*hexa_p).feet_6lg_up)[rela[i]][0]+0.5*k*(((*hexa_p).feet_6lg_v)[rela[i]][0]-((*hexa_p).feet_6lg_up)[rela[i]][0]);
			coord[rela[i]][1]=((*hexa_p).feet_6lg_up)[rela[i]][1]+0.5*k*(((*hexa_p).feet_6lg_v)[rela[i]][1]-((*hexa_p).feet_6lg_up)[rela[i]][1]);
			coord[rela[i]][2]=-(*hexa_p).height+k*20;
			sports_6lg(coord,hexa_p);
			HAL_Delay(30);
		}
		for(int j=1;j<=N;j++){
			k=(float)j/N;
			coord[rela[i]][0]=((*hexa_p).feet_6lg_up)[rela[i]][0]+(0.5*k+0.5)*(((*hexa_p).feet_6lg_v)[rela[i]][0]-((*hexa_p).feet_6lg_up)[rela[i]][0]);
			coord[rela[i]][1]=((*hexa_p).feet_6lg_up)[rela[i]][1]+(0.5*k+0.5)*(((*hexa_p).feet_6lg_v)[rela[i]][1]-((*hexa_p).feet_6lg_up)[rela[i]][1]);
			coord[rela[i]][2]=-(*hexa_p).height+(1-k)*20;
			sports_6lg(coord,hexa_p);
			HAL_Delay(30);
		}
	}
}

//四足形态转六足形态
void four2six(struct hexapod* hexa_p)
{
	int N=15;
	float coord[6][3];
	for(int i=1;i<=N;i++){
		four2six_step(coord,hexa_p,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(30);
	}
	(*hexa_p).mode=1;
	(*hexa_p).mode_6lg=2;
}

//六足形态转四足形态
void six2four(struct hexapod* hexa_p)
{
	int N=15;
	float coord[6][3];
	for(int i=1;i<=N;i++){
		six2four_step(coord,hexa_p,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(30);
	}
	(*hexa_p).mode=2;
	(*hexa_p).tz=-(*hexa_p).height;
}

//轮足形态转六足形态
void wheel2six(struct hexapod* hexa_p)
{
	int N=30;
	float coord[6][3];
	//放腿
	for(int i=1;i<=2*N;i++){
		change_z_step(coord,hexa_p,-10.0,-200.0,i,N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(20);
	}
	//收起轮足，由于舵机是270°舵机，因此转动角度有所区别
	for(int i=1;i<=N*6/5;i++){
		SERVO_SPT(9,(-60.0+125.0*((float)i)/N));//增大
		SERVO_SPT(10,(60.0-125.0*((float)i)/N));//减小
		SERVO_SPT(21,(60.0-125.0*((float)i)/N));//减小
		SERVO_SPT(22,(-60.0+125.0*((float)i)/N));//增大
		HAL_Delay(10);
	}
	//抬腿
	for(int i=1;i<=N*3/2;i++){
		change_z_step(coord,hexa_p,-200.0,-(*hexa_p).height,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(20);
	}
	iner(hexa_p);
	(*hexa_p).tz=-(*hexa_p).height;
	(*hexa_p).mode=1;
	(*hexa_p).mode_6lg=2;
}

//六足形态转轮足形态
void six2wheel(struct hexapod* hexa_p)
{
	int N=30;
	float coord[6][3];
	outer(hexa_p);
	//放腿
	for(int i=1;i<=N*3/2;i++){
		change_z_step(coord,hexa_p,-(*hexa_p).height,-200.0,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(20);
	}
	//放下轮足，由于舵机是270°舵机，因此转动角度有所区别
	for(int i=1;i<=N*6/5;i++){
		SERVO_SPT(9,(-60.0+125.0*((float)(N-i))/N));
		SERVO_SPT(10,(60.0-125.0*((float)(N-i))/N));
		SERVO_SPT(21,(60.0-125.0*((float)(N-i))/N));
		SERVO_SPT(22,(-60.0+125.0*((float)(N-i))/N));
		HAL_Delay(10);
	}
	//抬腿
	for(int i=1;i<=2*N;i++){
		change_z_step(coord,hexa_p,-200.0,-10.0,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(30);
	}
	(*hexa_p).mode=3;
}

//移动到一个固定状态
void to_state(struct hexapod* hexa_p,float roll,float pitch,float yaw,float tz)
{
	int N1,N2,N3,N4,N;
	const float k_angle=0.01745;//1°
	const float k_height=2;//1mm
	N1=abs((int)((roll-(*hexa_p).roll)/k_angle));
	N2=abs((int)((pitch-(*hexa_p).pitch)/k_angle));
	N3=abs((int)((yaw-(*hexa_p).yaw)/k_angle));
	N4=abs((int)((tz-(*hexa_p).tz)/k_height));
	N=(N1>N2)?N1:N2;
	N=(N>N3)?N:N3;
	N=(N>N4)?N:N4;
	if(N==0)
		return;
	float coord[6][3];
	float temp_roll,temp_pitch,temp_yaw,temp_tz,step_roll,step_pitch,step_yaw,step_tz;
	temp_roll=(*hexa_p).roll;
	temp_pitch=(*hexa_p).pitch;
	temp_yaw=(*hexa_p).yaw;
	temp_tz=(*hexa_p).tz;
	step_roll=(roll-(*hexa_p).roll)/N;
	step_pitch=(pitch-(*hexa_p).pitch)/N;
	step_yaw=(yaw-(*hexa_p).yaw)/N;
	step_tz=(tz-(*hexa_p).tz)/N;
	for(int i=1;i<=N;i++){
		if(((fabs(temp_roll)>M_PI/18)&&(fabs(temp_roll+step_roll)>fabs(temp_roll)))
				||((fabs(temp_pitch)>M_PI/18)&&(fabs(temp_pitch+step_pitch)>fabs(temp_pitch)))
				||((fabs(temp_yaw)>M_PI/18)&&(fabs(temp_yaw+step_yaw)>fabs(temp_yaw)))){
			break;
		}
		temp_roll+=step_roll;
		temp_pitch+=step_pitch;
		temp_yaw+=step_yaw;
		temp_tz+=step_tz;
		to_state_co(coord,hexa_p,temp_roll,temp_pitch,temp_yaw,temp_tz);
		sports_6lg(coord,hexa_p);
		(*hexa_p).roll=temp_roll;
		(*hexa_p).pitch=temp_pitch;
		(*hexa_p).yaw=temp_yaw;
		(*hexa_p).tz=temp_tz;
		HAL_Delay(30);
	}
}

//抬腿
void lift_6lg(struct hexapod* hexa_p,int mode,int N)
{
	float coord[6][3];
	for(int i=1;i<=N;i++){
		lift_6lg_step(coord,hexa_p,mode,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(30);
	}
	(*hexa_p).mode_6lg=1;
}

//抬腿
void drop_6lg(struct hexapod* hexa_p,int mode,int N)
{
	float coord[6][3];
	for(int i=1;i<=N;i++){
		drop_6lg_step(coord,hexa_p,mode,i, N);
		sports_6lg(coord,hexa_p);
		HAL_Delay(30);
	}
	(*hexa_p).mode_6lg=2;
}

//六足模式下直行一个周期
void straight_6lg(struct hexapod* hexa_p)
{
	//根据速度选取插值点个数
	int N;
	if((*hexa_p).speed==1)
		N=20;
	else if((*hexa_p).speed==2)
		N=14;
	else
		N=10;

	/*直行首先抬腿*/
	if((*hexa_p).mode_6lg==2)
		lift_6lg(hexa_p,1,10);

	float coord[6][3];
	while(1){
		/*收到停止信号时退出*/
		if(data.command==c_stop){
			/*放腿*/
			drop_6lg(hexa_p,(*hexa_p).mode_6lg,10);
			break;
		}
		/*直行一个周期*/
		for(int i=1;i<=N;i++){
			straight_6lg_step(coord,hexa_p,data.data,(*hexa_p).mode_6lg,i, N);
			sports_6lg(coord,hexa_p);
			HAL_Delay(30);
		}
		(*hexa_p).mode_6lg=((*hexa_p).mode_6lg+1)%2;
	}
}

void get_data_turn(const struct hexapod* hexa_p,float *p_R,float *p_theta)
{
	float k_R=500.0,r1,r2,r;
	if(data.data>0){
		(*p_R)=k_R*tan((-data.data+M_PI/2)*0.99);
		r1=fabs(((*hexa_p).feet_6lg_up)[1][1]-(*p_R));
		r2=fabs(((*hexa_p).feet_6lg_up)[4][1]-(*p_R));
		r=(r1>r2)?r1:r2;
		(*p_theta)=1.2*(*hexa_p).span/r;
	}
	else{
		(*p_R)=k_R*tan((-data.data-M_PI/2)*0.99);
		r1=fabs(((*hexa_p).feet_6lg_up)[1][1]-(*p_R));
		r2=fabs(((*hexa_p).feet_6lg_up)[4][1]-(*p_R));
		r=(r1>r2)?r1:r2;
		(*p_theta)=-1.2*(*hexa_p).span/r;
	}
}

//六足模式下旋转
void turn_6lg(struct hexapod* hexa_p)
{
	//根据速度选取插值点个数
	int N;
	if((*hexa_p).speed==1)
		N=20;
	else if((*hexa_p).speed==2)
		N=14;
	else
		N=10;

	/*直行首先抬腿*/
	if((*hexa_p).mode_6lg==2)
		lift_6lg(hexa_p,1,10);

	float coord[6][3];
	while(1){
		/*收到停止信号时退出*/
		if(data.command==c_stop){
			/*放腿*/
			drop_6lg(hexa_p,(*hexa_p).mode_6lg,10);
			break;
		}
		float R,theta;
		get_data_turn(hexa_p,&R,&theta);
		/*直行一个周期*/
		for(int i=1;i<=N;i++){
			turn_6lg_step(coord,hexa_p,R,theta,(*hexa_p).mode_6lg,i,N);
			sports_6lg(coord,hexa_p);
			HAL_Delay(30);
		}
		(*hexa_p).mode_6lg=((*hexa_p).mode_6lg+1)%2;
	}
}


//四足模式下直行一个周期
void straight_4lg(struct hexapod* hexa_p)
{
	//根据速度选取插值点个数
	int N;
	if((*hexa_p).speed==1)
		N=16;
	else if((*hexa_p).speed==2)
		N=14;
	else
		N=10;

	(*hexa_p).mode_4lg=1;
	float coord[4][3];
	while(1){
		/*收到停止信号时退出*/
		if(data.command==c_stop){
			/*放腿*/
			drop_6lg(hexa_p,(*hexa_p).mode_6lg,N);
			break;
		}
		/*直行一个周期*/
		for(int i=1;i<=N;i++){
			straight_4lg_tort_step(coord,hexa_p,data.data,(*hexa_p).mode_4lg,i, N);
			sports_4lg(coord,hexa_p);
			HAL_Delay(30);
		}
		(*hexa_p).mode_4lg=((*hexa_p).mode_4lg+1)%2;
	}
}

//四足模式下旋转
void turn_4lg(struct hexapod* hexa_p)
{
	//根据速度选取插值点个数
	int N;
	if((*hexa_p).speed==1)
		N=20;
	else if((*hexa_p).speed==2)
		N=14;
	else
		N=10;

	/*坐标初始化*/
	float coord[4][3];
	for(int i=0;i<4;i++){
		coord[i][0]=((*hexa_p).feet_4lg_up)[i][0];
		coord[i][1]=((*hexa_p).feet_4lg_up)[i][1];
		coord[i][2]=(*hexa_p).tz;
	}

	while(1){
		/*收到停止信号时退出*/
		if(data.command==c_stop)
			break;
		/*直行一个周期*/
		float R,theta;
		get_data_turn(hexa_p,&R,&theta);
		/*移动四条腿*/
		for(int i=0;i<4;i++){
			for(int j=1;j<=N;j++){
				turn_4lg_leg_step(coord,hexa_p,R,theta,i,j,N);
				sports_4lg(coord,hexa_p);
				HAL_Delay(30);
			}
		}
		/*移动整个身体*/
		for(int j=1;j<=N;j++){
			turn_4lg_body_step(coord,hexa_p,R,theta,j,N);
			sports_4lg(coord,hexa_p);
			HAL_Delay(30);
		}
	}
}

//根据角度和最大速度求解出麦克纳姆轮四个轮子的速度
void getspeed_straight(int wheelspeed[4],int maxspeed,float theta)
{
	int rela[4]={0,1,2,3};
	float vx=100*cos(theta);
	float vy=100*sin(theta);
	float speed[4],nowmax,k;
	speed[rela[0]]=vx-vy;
	speed[rela[1]]=vx+vy;
	speed[rela[2]]=vx+vy;
	speed[rela[3]]=vx-vy;
	nowmax=(fabs(speed[0])>fabs(speed[1]))?fabs(speed[0]):fabs(speed[1]);
	nowmax=(fabs(speed[2])>nowmax)?fabs(speed[2]):nowmax;
	nowmax=(fabs(speed[3])>nowmax)?fabs(speed[3]):nowmax;
	k=maxspeed/nowmax;
	for(int i=0;i<4;i++)
		wheelspeed[i]=(int)(speed[i]*k);

	wheelspeed[2]=-wheelspeed[2];
	wheelspeed[3]=-wheelspeed[3];
}

//轮足模式下直行
void straight_wheel(struct hexapod* hexa_p)
{
	int wheelspeed[4]={0,0,0,0};
	int maxspeed;
	if((*hexa_p).speed==1)
		maxspeed=3000;
	else if((*hexa_p).speed==2)
		maxspeed=5500;
	else
		maxspeed=8000;
	while(1){
		/*收到停止信号时退出*/
		if(data.command==c_stop){
			for(int i=0;i<4;i++)
				wheelspeed[i]=0;
			HAL_UART_Transmit(&huart4, (uint8_t *)wheelspeed,16,0xffff);
			break;
		}
		getspeed_straight(wheelspeed,maxspeed,data.data);
		HAL_UART_Transmit(&huart4, (uint8_t *)wheelspeed,16,0xffff);
		HAL_Delay(100);
	}
}

//根据角度和最大速度求解出麦克纳姆轮四个轮子的速度,向前为x张方向，向左为y（R）正方向
void getspeed_turn(int wheelspeed[4],int maxspeed,float R_point,int mode)
{
	int rela[4]={0,1,2,3};
	float L1=100.0;
	float L2=100.0;
	float speed[4],nowmax,k;
	speed[rela[0]]=100*( - mode*(-R_point+L1+L2));
	speed[rela[1]]=100*( + mode*(R_point+L1+L2));
	speed[rela[2]]=100*( - mode*(-R_point+L1+L2));
	speed[rela[3]]=100*( + mode*(R_point+L1+L2));
	nowmax=(fabs(speed[0])>fabs(speed[1]))?fabs(speed[0]):fabs(speed[1]);
	nowmax=(fabs(speed[2])>nowmax)?fabs(speed[2]):nowmax;
	nowmax=(fabs(speed[3])>nowmax)?fabs(speed[3]):nowmax;
	k=maxspeed/nowmax;
	for(int i=0;i<4;i++)
		wheelspeed[i]=(int)(speed[i]*k);
}

//轮足模式转弯
void turn_wheel(struct hexapod* hexa_p)
{
	int wheelspeed[4]={0,0,0,0};
	int maxspeed;
	if((*hexa_p).speed==1)
		maxspeed=3000;
	else if((*hexa_p).speed==2)
		maxspeed=5500;
	else
		maxspeed=8000;
	while(1){
		/*收到停止信号时退出*/
		if(data.command==c_stop){
			for(int i=0;i<4;i++)
				wheelspeed[i]=0;
			HAL_UART_Transmit(&huart4, (uint8_t *)wheelspeed,16,0xffff);
			break;
		}
		float R,theta;
		get_data_turn(hexa_p,&R,&theta);
		getspeed_turn(wheelspeed,maxspeed,R,(int)(theta/(fabs(theta))));
		HAL_UART_Transmit(&huart4, (uint8_t *)wheelspeed,16,0xffff);
		HAL_Delay(100);
	}
}

void to_state_stable(struct hexapod* hexa_p,float roll,float pitch)
{
	float coord[6][3];
	float to_roll,to_pitch;
	to_roll=(fabs(roll)<M_PI/15)?roll:(M_PI/15)*fabs(roll)/roll;
	to_pitch=(fabs(pitch)<M_PI/15)?pitch:(M_PI/15)*fabs(pitch)/pitch;
	to_state_stable_co(coord,hexa_p,to_roll,to_pitch,0.0);

}

void servo_correct(struct hexapod* hexa_p)
{
	//依次将所有舵机调节到90°
	for(int n=0;n<24;n++){
		if(n==9)
			SERVO_SPT(n,-60.0);
		else if(n==10)
			SERVO_SPT(n,60.0);
		else if(n==21)
			SERVO_SPT(n,60.0);
		else if(n==22)
			SERVO_SPT(n,-60.0);
		else
			SERVO_SPT(n,0.0);
		HAL_Delay(300);
	}
	int num;
	float angle;
	while(1){
		if(data.command==c_stop||data.command==c_servo_quit){
			float coord[6][3];
			to_state_co(coord,hexa_p,0.0,0.0,0.0,(*hexa_p).tz);
			sports_6lg(coord,hexa_p);
			SERVO_SPT(9,(-60.0+125.0));
		    HAL_Delay(10);
		    SERVO_SPT(10,(+60.0-125.0));
		    HAL_Delay(10);
		    SERVO_SPT(21,(+60.0-125.0));
		    HAL_Delay(10);
		    SERVO_SPT(22,(-60.0+125.0));
		    HAL_Delay(1000);
			break;
		}
		if(data.command==c_servo_crooect){
			num=(int)(data.data)/1000;
			angle=data.data-1000*num;
			if(num-1==9)
				PCA_SPT((uint8_t)(num-1),angle-60);
			else if(num-1==10)
				PCA_SPT((uint8_t)(num-1),angle+60);
			else if(num-1==21)
				PCA_SPT((uint8_t)(num-1),angle+60);
			else if(num-1==22)
				PCA_SPT((uint8_t)(num-1),angle-60);
			else
				PCA_SPT((uint8_t)(num-1),angle);
			bias[num-1]=angle;
			HAL_Delay(300);
		}
	}




}


