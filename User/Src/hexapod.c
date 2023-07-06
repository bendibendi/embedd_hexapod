#include "hexapod.h"
#include "math.h"
#include "stdio.h"

//机器人参数初始化
void Hexapod_Init(struct hexapod* hex_p,
		const float width1,
		const float width2,
		const float len,
		const float lg1,
		const float lg2,
		const float lg3,
		const float loc,
		const float theta,
		const float height,
		const float span,
		const float span_height)
{
	(*hex_p).start=1;//开机
	(*hex_p).width1=width1;
	(*hex_p).width2=width2;
	(*hex_p).len=len;
	(*hex_p).lg1=lg1;
	(*hex_p).lg2=lg2;
	(*hex_p).lg3=lg3;
	(*hex_p).loc=loc;
	(*hex_p).theta=theta;
	(*hex_p).height=height;
	(*hex_p).roll=0;
	(*hex_p).pitch=0;
	(*hex_p).yaw=0;
	(*hex_p).tx=0;
	(*hex_p).ty=0;
	(*hex_p).tz=-height;
	(*hex_p).mode=1;
	(*hex_p).mode_6lg=2;
	(*hex_p).span=span;
	(*hex_p).span_height=span_height;
	//feet_6lg_up
	((*hex_p).feet_6lg_up)[0][0]=len+loc*sin(theta);
	((*hex_p).feet_6lg_up)[0][1]=width1+loc*cos(theta);
	((*hex_p).feet_6lg_up)[1][0]=0;
	((*hex_p).feet_6lg_up)[1][1]=width2+loc;
	((*hex_p).feet_6lg_up)[2][0]=-len-loc*sin(theta);
	((*hex_p).feet_6lg_up)[2][1]=width1+loc*cos(theta);
	((*hex_p).feet_6lg_up)[3][0]=len+loc*sin(theta);
	((*hex_p).feet_6lg_up)[3][1]=-width1-loc*cos(theta);
	((*hex_p).feet_6lg_up)[4][0]=0;
	((*hex_p).feet_6lg_up)[4][1]=-width2-loc;
	((*hex_p).feet_6lg_up)[5][0]=-len-loc*sin(theta);
	((*hex_p).feet_6lg_up)[5][1]=-width1-loc*cos(theta);
	int i;
	for(i=0;i<6;i++)
		((*hex_p).feet_6lg_up)[i][2]=0;
	//feet_4lg_up
	int rela[4]={0,2,3,5};
	for(i=0;i<4;i++){
		((*hex_p).feet_4lg_up)[i][0]=((*hex_p).feet_6lg_up)[rela[i]][0];
		((*hex_p).feet_4lg_up)[i][1]=((*hex_p).feet_6lg_up)[rela[i]][1];
		((*hex_p).feet_4lg_up)[i][2]=((*hex_p).feet_6lg_up)[rela[i]][2];
	}
	//feet_6lg
	for(i=0;i<6;i++){
		((*hex_p).feet_6lg)[i][0]=((*hex_p).feet_6lg_up)[i][0];
		((*hex_p).feet_6lg)[i][1]=((*hex_p).feet_6lg_up)[i][1];
		((*hex_p).feet_6lg)[i][2]=-height;
	}
	//feet_6lg
	for(i=0;i<4;i++){
		((*hex_p).feet_4lg)[i][0]=((*hex_p).feet_4lg_up)[i][0];
		((*hex_p).feet_4lg)[i][1]=((*hex_p).feet_4lg_up)[i][1];
		((*hex_p).feet_4lg)[i][2]=-height;
	}
	//joint_6lg
	((*hex_p).joint_6lg)[0][0]=len;
	((*hex_p).joint_6lg)[0][1]=width1;
	((*hex_p).joint_6lg)[1][0]=0;
	((*hex_p).joint_6lg)[1][1]=width2;
	((*hex_p).joint_6lg)[2][0]=-len;
	((*hex_p).joint_6lg)[2][1]=width1;
	((*hex_p).joint_6lg)[3][0]=len;
	((*hex_p).joint_6lg)[3][1]=-width1;
	((*hex_p).joint_6lg)[4][0]=0;
	((*hex_p).joint_6lg)[4][1]=-width2;
	((*hex_p).joint_6lg)[5][0]=-len;
	((*hex_p).joint_6lg)[5][1]=-width1;
	for(i=0;i<6;i++)
		((*hex_p).joint_6lg)[i][2]=0;
	//joint_4lg
	for(i=0;i<4;i++){
		((*hex_p).joint_4lg)[i][0]=((*hex_p).joint_6lg)[rela[i]][0];
		((*hex_p).joint_4lg)[i][1]=((*hex_p).joint_6lg)[rela[i]][1];
		((*hex_p).joint_4lg)[i][2]=((*hex_p).joint_6lg)[rela[i]][2];
	}

}


