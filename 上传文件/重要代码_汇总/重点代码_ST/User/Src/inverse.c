/*蜘蛛机器人的运动学求逆*/
#include "math.h"
#include "hexapod.h"
#include "inverse.h"

//输入的是六只脚相对于主结构中心点的坐标，输出是18个舵机对应的角度
void get_theta_6lg(float angle[6][3],const float coord[6][3],const struct hexapod* spider)
{
	float lg1=(*spider).lg1;
	float lg2=(*spider).lg2;
	float lg3=(*spider).lg3;
	float relative_coord[6][3];
	int i,j;
	float LR,alR,al1,al2;
	//每只脚对应各自关节的坐标初始化
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			relative_coord[i][j]=coord[i][j]-((*spider).joint_6lg)[i][j];
		}
	}
	//左腿的角度
	for(i=0;i<3;i++){
		angle[i][0]=atan2(-relative_coord[i][0],relative_coord[i][1]);
		LR=sqrt(pow(relative_coord[i][2],2)+pow(sqrt(pow(relative_coord[i][0],2)+pow(relative_coord[i][1],2))-lg1,2));
        alR=atan2(-relative_coord[i][2],sqrt(pow(relative_coord[i][0],2)+pow(relative_coord[i][1],2))-lg1);
        al1=acos((pow(LR,2)+pow(lg2,2)-pow(lg3,2))/(2*lg2*LR));
        al2=acos((pow(LR,2)+pow(lg3,2)-pow(lg2,2))/(2*lg3*LR));
        angle[i][1]=alR-al1;
        angle[i][2]=al1+al2-M_PI/2;
	}
	//右腿的角度（左右腿的坐标计算以及定义有一定差别）
	for(i=3;i<6;i++){
		angle[i][0]=atan2(relative_coord[i][0],-relative_coord[i][1]);
		LR=sqrt(pow(relative_coord[i][2],2)+pow(sqrt(pow(relative_coord[i][0],2)+pow(relative_coord[i][1],2))-lg1,2));
        alR=atan2(-relative_coord[i][2],sqrt(pow(relative_coord[i][0],2)+pow(relative_coord[i][1],2))-lg1);
        al1=acos((pow(LR,2)+pow(lg2,2)-pow(lg3,2))/(2*lg2*LR));
        al2=acos((pow(LR,2)+pow(lg3,2)-pow(lg2,2))/(2*lg3*LR));
        angle[i][1]=-alR+al1;
        angle[i][2]=-al1-al2+M_PI/2;
	}
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			angle[i][j]=angle[i][j]/M_PI*180.0;
		}
	}
}

//输入的是六只脚相对于主结构中心点的坐标，输出是18个舵机对应的角度
void get_theta_4lg(float angle[4][3],const float coord[4][3],const struct hexapod* spider)
{
	float lg1=(*spider).lg1;
	float lg2=(*spider).lg2;
	float lg3=(*spider).lg3;
	float relative_coord[4][3];
	int i,j;
	float LR,alR,al1,al2;
	//每只脚对应各自关节的坐标初始化
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			relative_coord[i][j]=coord[i][j]-((*spider).joint_4lg)[i][j];
		}
	}
	//左腿的角度
	for(i=0;i<2;i++){
		angle[i][0]=atan2(-relative_coord[i][0],relative_coord[i][1]);
		LR=sqrt(pow(relative_coord[i][2],2)+pow(sqrt(pow(relative_coord[i][0],2)+pow(relative_coord[i][1],2))-lg1,2));
		alR=atan2(-relative_coord[i][2],sqrt(pow(relative_coord[i][0],2)+pow(relative_coord[i][1],2))-lg1);
		al1=acos((pow(LR,2)+pow(lg2,2)-pow(lg3,2))/(2*lg2*LR));
		al2=acos((pow(LR,2)+pow(lg3,2)-pow(lg2,2))/(2*lg3*LR));
		angle[i][1]=alR-al1;
		angle[i][2]=al1+al2-M_PI/2;
	}
	//右腿的角度（左右腿的坐标计算以及定义有一定差别）
	for(i=2;i<4;i++){
		angle[i][0]=atan2(relative_coord[i][0],-relative_coord[i][1]);
		LR=sqrt(pow(relative_coord[i][2],2)+pow(sqrt(pow(relative_coord[i][0],2)+pow(relative_coord[i][1],2))-lg1,2));
        alR=atan2(-relative_coord[i][2],sqrt(pow(relative_coord[i][0],2)+pow(relative_coord[i][1],2))-lg1);
        al1=acos((pow(LR,2)+pow(lg2,2)-pow(lg3,2))/(2*lg2*LR));
        al2=acos((pow(LR,2)+pow(lg3,2)-pow(lg2,2))/(2*lg3*LR));
        angle[i][1]=-alR+al1;
        angle[i][2]=-al1-al2+M_PI/2;
	}
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			angle[i][j]=angle[i][j]/M_PI*180.0;
		}
	}

}
