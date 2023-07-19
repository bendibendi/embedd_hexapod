/*定义机器人运动的各种足端轨迹*/
//#include "math.h"
#include "hexapod.h"
#include "Rotation_translation.h"

////移动到固定姿态
//void to_state_co(float coord[6][3],const struct hexapod* hexa_p,float roll,float pitch,float yaw,float tz)
//{
//	float coord0[6][3];
//	for(int i=0;i<6;i++){
//		coord0[i][0]=(*hexa_p).feet_6lg_up[i][0];
//		coord0[i][1]=(*hexa_p).feet_6lg_up[i][1];
//		coord0[i][2]=tz;
//	}
//	for(int i=0;i<6;i++)
//		cotrs_xyz(coord0[i],coord[i],roll,pitch,yaw);
//}
//移动到固定姿态
void to_state_co(float coord[6][3],const struct hexapod* hexa_p,float roll,float pitch,float yaw,float tz)
{
	for(int i=0;i<6;i++){
		cotrs_xyz((*hexa_p).feet_6lg_up[i],coord[i],roll,pitch,yaw);
	}
	cotrs_t_6lg(coord,0.0,0.0,(*hexa_p).tz);
}

//移动到固定状态，自稳定状态下
void to_state_stable_co(float coord[6][3],const struct hexapod* hexa_p,float roll,float pitch,float yaw)
{
	float temp_coord[6][3];
	for(int i=0;i<6;i++){
		cotrs_xyz((*hexa_p).feet_6lg_up[i],temp_coord[i],(*hexa_p).roll,(*hexa_p).pitch,(*hexa_p).yaw);
		cotrs_xyz(temp_coord[i],coord[i],roll,pitch,yaw);
	}
	cotrs_t_6lg(coord,0.0,0.0,(*hexa_p).tz);
}

//四足转六足形态
void four2six_step(float coord[6][3],const struct hexapod* hexa_p,int i,int N)
{
	int k;
	for(k=0;k<6;k++){
		coord[k][0]=((*hexa_p).feet_6lg_up)[k][0];
		coord[k][1]=((*hexa_p).feet_6lg_up)[k][1];
		if((k==1)||(k==4))
			coord[k][2]=-(*hexa_p).height*((float)i)/N;
		else
			coord[k][2]=-(*hexa_p).height;
	}
}

//六足转四足形态
void six2four_step(float coord[6][3],const struct hexapod* hexa_p,int i,int N)
{
	int k;
	for(k=0;k<6;k++){
		coord[k][0]=((*hexa_p).feet_6lg_up)[k][0];
		coord[k][1]=((*hexa_p).feet_6lg_up)[k][1];
		if((k==1)||(k==4))
			coord[k][2]=-(*hexa_p).height*(1-((float)i)/N);
		else
			coord[k][2]=-(*hexa_p).height;
	}
}

//改变六足足端高度
void change_z_step(float coord[6][3],const struct hexapod* hexa_p,float start_z,float end_z,int i,int N)
{
	for(int k=0;k<6;k++){
		coord[k][0]=((*hexa_p).feet_6lg_v)[k][0];
		coord[k][1]=((*hexa_p).feet_6lg_v)[k][1];
		coord[k][2]=start_z+(end_z-start_z)*((float)i)/N;
	}
}

//抬腿，六足模式
void lift_6lg_step(float coord[6][3],const struct hexapod* hexa_p,int mode,int step,int N)
{
	if(mode==1){
		for(int k=0;k<6;k++){
			coord[k][0]=((*hexa_p).feet_6lg_up)[k][0];
			coord[k][1]=((*hexa_p).feet_6lg_up)[k][1];
			if(k%2==0)
				coord[k][2]=(*hexa_p).span_height*((float)step)/N+(*hexa_p).tz;
			else
				coord[k][2]=(*hexa_p).tz;
		}
	}
	else{
		for(int k=0;k<6;k++){
			coord[k][0]=((*hexa_p).feet_6lg_up)[k][0];
			coord[k][1]=((*hexa_p).feet_6lg_up)[k][1];
			if(k%2==1)
				coord[k][2]=(*hexa_p).span_height*((float)step)/N+(*hexa_p).tz;
			else
				coord[k][2]=(*hexa_p).tz;
		}
	}
}

//放腿，六足模式
void drop_6lg_step(float coord[6][3],const struct hexapod* hexa_p,int mode,int step,int N)
{
	if(mode==1){
		for(int k=0;k<6;k++){
			coord[k][0]=((*hexa_p).feet_6lg_up)[k][0];
			coord[k][1]=((*hexa_p).feet_6lg_up)[k][1];
			if(k%2==0)
				coord[k][2]=(*hexa_p).span_height*(1-((float)step)/N)+(*hexa_p).tz;
			else
				coord[k][2]=(*hexa_p).tz;
		}
	}
	else{
		for(int k=0;k<6;k++){
			coord[k][0]=((*hexa_p).feet_6lg_up)[k][0];
			coord[k][1]=((*hexa_p).feet_6lg_up)[k][1];
			if(k%2==1)
				coord[k][2]=(*hexa_p).span_height*(1-((float)step)/N)+(*hexa_p).tz;
			else
				coord[k][2]=(*hexa_p).tz;
		}
	}
}

//前进，mode决定使用哪两组角，i是一步中的第几个插值点，N是一步的总插值点数(N必须是双数)，theta是与前方的夹角
void straight_6lg_step(float coord[6][3],const struct hexapod* hexa_p,float theta,int mode,int step,int N)
{
	//theta是从蜘蛛的正前方逆时针的角度，通过调整theta可以改变蜘蛛的直行方向
	int i,j;
	float k;

	/*获取每个角相对于自己中心点的移动轨迹*/
	if(mode ==1){
		//先用左边的两条腿画圈
		if(step<=N/2){
			k=((float)(step+N/2))/(N);
			for(i=0;i<6;i=i+2){
				coord[i][0]=cos(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
				coord[i][1]=sin(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
				coord[i][2]=(*hexa_p).span_height*(2-2*(k-sin(4*M_PI*k)/4/M_PI));
			}
			for(i=1;i<6;i=i+2){
				coord[i][0]=cos(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
				coord[i][1]=sin(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
				coord[i][2]=0;
			}
		}
		else{
			k=((float)(N*1.5-step))/(N);
			for(i=0;i<6;i=i+2){
				coord[i][0]=cos(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
				coord[i][1]=sin(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
				coord[i][2]=0;
			}
			for(i=1;i<6;i=i+2){
				coord[i][0]=cos(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
				coord[i][1]=sin(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
				coord[i][2]=(*hexa_p).span_height*(2-2*(k-sin(4*M_PI*k)/4/M_PI));
			}
		}
	}
	else{
		//先用右边的两条腿画圈
		if(step<=N/2){
			k=((float)(step+N/2))/(N);
			for(i=1;i<6;i=i+2){
				coord[i][0]=cos(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
				coord[i][1]=sin(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
				coord[i][2]=(*hexa_p).span_height*(2-2*(k-sin(4*M_PI*k)/4/M_PI));
			}
			for(i=0;i<6;i=i+2){
				coord[i][0]=cos(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
				coord[i][1]=sin(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
				coord[i][2]=0;
			}
		}
		else{
			k=((float)(N*1.5-step))/(N);
			for(i=1;i<6;i=i+2){
				coord[i][0]=cos(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
				coord[i][1]=sin(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
				coord[i][2]=0;
			}
			for(i=0;i<6;i=i+2){
				coord[i][0]=cos(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
				coord[i][1]=sin(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
				coord[i][2]=(*hexa_p).span_height*(2-2*(k-sin(4*M_PI*k)/4/M_PI));
			}
		}
	}

	/*获取足底相对于主结构中心点的轨迹*/
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			coord[i][j]=coord[i][j]+((*hexa_p).feet_6lg_up)[i][j];
		}
	}

	/*将足端坐标下降到地面*/
	cotrs_t_6lg(coord,0.0,0.0,(*hexa_p).tz);
}

//前进，mode决定使用哪两组角，i是一步中的第几个插值点，N是一步的总插值点数(N必须是双数)，theta是与前方的夹角
void straight_4lg_tort_step(float coord[4][3],const struct hexapod* hexa_p,float theta,int mode,int step,int N)
{
	//theta是从蜘蛛的正前方逆时针的角度，通过调整theta可以改变蜘蛛的直行方向
	int i,j;
	float k;

	/*获取每个角相对于自己中心点的移动轨迹*/
	if(mode ==1){
		//先用左边的两条腿画圈
		if(step<=N/2){
			k=((float)(step+N/2))/(N);
			for(i=0;i<4;i=i+1){
				if(i%3==0){
					coord[i][0]=cos(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
					coord[i][1]=sin(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
					coord[i][2]=(*hexa_p).span_height*(2-2*(k-sin(4*M_PI*k)/4/M_PI));
				}
				else{
					coord[i][0]=cos(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
					coord[i][1]=sin(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
					coord[i][2]=0;
				}
			}
		}
		else{
			k=((float)(N*1.5-step))/(N);
			for(i=0;i<4;i=i+1){
				if(i%3==0){
					coord[i][0]=cos(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
					coord[i][1]=sin(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
					coord[i][2]=0;
				}
				else{
					coord[i][0]=cos(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
					coord[i][1]=sin(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
					coord[i][2]=(*hexa_p).span_height*(2-2*(k-sin(4*M_PI*k)/4/M_PI));
				}
			}
		}
	}
	else{
		//先用右边的两条腿画圈
		if(step<=N/2){
			k=((float)(step+N/2))/(N);
			for(i=0;i<4;i=i+1){
				if(i%3!=0){
					coord[i][0]=cos(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
					coord[i][1]=sin(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
					coord[i][2]=(*hexa_p).span_height*(2-2*(k-sin(4*M_PI*k)/4/M_PI));
				}
				else{
					coord[i][0]=cos(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
					coord[i][1]=sin(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
					coord[i][2]=0;
				}
			}
		}
		else{
			k=((float)(N*1.5-step))/(N);
			for(i=0;i<4;i=i+1){
				if(i%3!=0){
					coord[i][0]=cos(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
					coord[i][1]=sin(theta)*(*hexa_p).span*((k-sin(2*M_PI*k)/2/M_PI)-0.5);
					coord[i][2]=0;
				}
				else{
					coord[i][0]=cos(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
					coord[i][1]=sin(theta)*(*hexa_p).span*(0.5-(k-sin(2*M_PI*k)/2/M_PI));
					coord[i][2]=(*hexa_p).span_height*(2-2*(k-sin(4*M_PI*k)/4/M_PI));
				}
			}
		}
	}

	/*获取足底相对于主结构中心点的轨迹*/
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			coord[i][j]=coord[i][j]+((*hexa_p).feet_4lg_up)[i][j];
		}
	}

	/*将足端坐标下降到地面*/
	cotrs_t_4lg(coord,0.0,0.0,(*hexa_p).tz);
}

inline void cotrs_vs_point(const float coord0[3],float coord[3],float x0,float y0, float theta)
{
	coord[0]=(coord0[0]-x0)*cos(theta)-(coord0[1]-y0)*sin(theta)+x0;
	coord[1]=(coord0[0]-x0)*sin(theta)+(coord0[1]-y0)*cos(theta)+y0;
}

//定半径转弯
void turn_6lg_step(float coord[6][3],const struct hexapod* hexa_p,float R,float theta,int mode,int step,int N)
{

	int i;
	float k=((float)step)/(N);
	float x0=0,y0=R;

	/*获取每个角相对于自己中心点的移动轨迹*/
	if(mode ==1){
		//先用左边的两条腿画圈
		if(step<=N/2){
			for(i=0;i<6;i=i+2){
				cotrs_vs_point(((*hexa_p).feet_6lg_up)[i],coord[i],x0,y0,k*theta);
				coord[i][2]=(*hexa_p).span_height*(1-2*(k-sin(4*M_PI*k)/4/M_PI));
			}
			for(i=1;i<6;i=i+2){
				cotrs_vs_point(((*hexa_p).feet_6lg_up)[i],coord[i],x0,y0,-k*theta);
				coord[i][2]=0;
			}
		}
		else{
			for(i=0;i<6;i=i+2){
				cotrs_vs_point(((*hexa_p).feet_6lg_up)[i],coord[i],x0,y0,(1-k)*theta);
				coord[i][2]=0;
			}
			for(i=1;i<6;i=i+2){
				cotrs_vs_point(((*hexa_p).feet_6lg_up)[i],coord[i],x0,y0,(k-1)*theta);
				coord[i][2]=(*hexa_p).span_height*(-1+2*(k-sin(4*M_PI*k)/4/M_PI));
			}
		}
	}
	else{
		//先用右边的两条腿画圈
		if(step<=N/2){
			for(i=1;i<6;i=i+2){
				cotrs_vs_point(((*hexa_p).feet_6lg_up)[i],coord[i],x0,y0,k*theta);
				coord[i][2]=(*hexa_p).span_height*(1-2*(k-sin(4*M_PI*k)/4/M_PI));
			}
			for(i=0;i<6;i=i+2){
				cotrs_vs_point(((*hexa_p).feet_6lg_up)[i],coord[i],x0,y0,-k*theta);
				coord[i][2]=0;
			}
		}
		else{
			for(i=1;i<6;i=i+2){
				cotrs_vs_point(((*hexa_p).feet_6lg_up)[i],coord[i],x0,y0,(1-k)*theta);
				coord[i][2]=0;
			}
			for(i=0;i<6;i=i+2){
				cotrs_vs_point(((*hexa_p).feet_6lg_up)[i],coord[i],x0,y0,(k-1)*theta);
				coord[i][2]=(*hexa_p).span_height*(-1+2*(k-sin(4*M_PI*k)/4/M_PI));
			}
		}
	}

	/*将足端坐标下降到地面*/
	cotrs_t_6lg(coord,0.0,0.0,(*hexa_p).tz);
}

//移动单条腿,仅在同一水平面上
void move_point_plane(float coord[3],float x0,float y0,float x1,float y1,float height,int i,int N)
{
	float k=((float)i)/N;
	coord[0]=x0+k*(x1-x0);
	coord[1]=y0+k*(y1-y0);
	coord[2]=sqrt(1-(2*k-1)*(2*k-1))*height;
}

//移动单条腿,旋转
void move_point_plane_turn(float coord[3],float x0,float y0,float R,float theta,float height,int i,int N)
{
	float k=((float)i)/N;
	float t=k*theta;
	coord[0]=(x0)*cos(t)-(y0-R)*sin(t);
	coord[1]=(x0)*sin(t)+(y0-R)*cos(t)+R;
	coord[2]=sqrt(1-(2*k-1)*(2*k-1))*height;
}

//四足直行，首先移动四条腿
void straight_4lg_leg_step(float coord[4][3],const struct hexapod* hexa_p,float theta,int leg,int step,int N)
{
	int rela[4]={1,0,3,2};
	move_point_plane(coord[rela[leg]],
			((*hexa_p).feet_4lg)[rela[leg]][0],
			((*hexa_p).feet_4lg)[rela[leg]][1],
			((*hexa_p).feet_4lg)[rela[leg]][0]+cos(theta)*(*hexa_p).span,
			((*hexa_p).feet_4lg)[rela[leg]][1]+sin(theta)*(*hexa_p).span,
			(*hexa_p).span_height*2,
			step, N);
	coord[rela[leg]][2]+=(*hexa_p).tz;
}

//四足直行，移动整个身体
void straight_4lg_body_step(float coord[4][3],const struct hexapod* hexa_p,float theta,int step,int N)
{
	float k=((float)step)/N;
	for(int i=0;i<4;i++){
		coord[i][0]=((*hexa_p).feet_4lg)[i][0]+(1-k)*cos(theta)*(*hexa_p).span*0.7;
		coord[i][1]=((*hexa_p).feet_4lg)[i][1]+(1-k)*sin(theta)*(*hexa_p).span*0.7;
		coord[i][2]=(*hexa_p).tz;
	}
}

//四足旋转，首先移动四条腿
void turn_4lg_leg_step(float coord[4][3],const struct hexapod* hexa_p,float R,float theta,int leg,int step,int N)
{
	int rela[4]={1,0,3,2};
	move_point_plane_turn(coord[rela[leg]],
				((*hexa_p).feet_4lg)[rela[leg]][0],
				((*hexa_p).feet_4lg)[rela[leg]][1],
				R,
				theta,
				(*hexa_p).span_height*0.7,
				step, N);
	coord[rela[leg]][2]+=(*hexa_p).tz;
}

//四足直行，移动整个身体
void turn_4lg_body_step(float coord[4][3],const struct hexapod* hexa_p,float R,float theta,int step,int N)
{
	float t=(1-((float)step)/N)*theta;
	for(int i=0;i<4;i++){
		coord[i][0]=(((*hexa_p).feet_4lg)[i][0])*cos(t)-(((*hexa_p).feet_4lg)[i][1]-R)*sin(t);
		coord[i][1]=(((*hexa_p).feet_4lg)[i][0])*sin(t)+(((*hexa_p).feet_4lg)[i][1]-R)*cos(t)+R;
		coord[i][2]=(*hexa_p).tz;
	}
}






