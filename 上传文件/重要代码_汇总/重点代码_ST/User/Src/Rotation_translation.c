/*定义了旋转和平移的函数，由一组坐标得到另一组坐标*/
#include "math.h"
#include "hexapod.h"
#include "Rotation_translation.h"

/*旋转是指对坐标进行旋转，而不是将坐标系进行旋转*/
//平移
void cotrs_t(const float* coord,float* exp_coord,float tx,float ty,float tz)
{
	*(exp_coord)=*(coord)+tx;
	*(exp_coord+1)=*(coord+1)+ty;
	*(exp_coord+2)=*(coord+2)+tz;
}

//绕x轴旋转
void cotrs_x(const float* coord,float* exp_coord,float roll)
{
	float temp_y=*(coord+1),temp_z=*(coord+2);
	*(exp_coord)=*(coord);
	*(exp_coord+1)=temp_y*cos(roll)-temp_z*sin(roll);
	*(exp_coord+2)=temp_y*sin(roll)+temp_z*cos(roll);
}

//绕y轴旋转
void cotrs_y(const float* coord,float* exp_coord,float pitch)
{
	float temp_x=*(coord),temp_z=*(coord+2);
	*(exp_coord)=temp_x*cos(pitch)+temp_z*sin(pitch);
	*(exp_coord+1)=*(coord+1);
	*(exp_coord+2)=-temp_x*sin(pitch)+temp_z*cos(pitch);
}

//绕z轴旋转
void cotrs_z(const float* coord,float* exp_coord,float yaw)
{
	float temp_x=*(coord),temp_y=*(coord+1);
	*(exp_coord)=temp_x*cos(yaw)-temp_y*sin(yaw);
	*(exp_coord+1)=temp_x*sin(yaw)+temp_y*cos(yaw);
	*(exp_coord+2)=*(coord+2);
}

//按照z,y,x的顺序旋转
void cotrs_xyz(const float* coord,float* exp_coord,float roll,float pitch,float yaw)
{
	float temp_x=*(coord),temp_y=*(coord+1),temp_z=*(coord+2);
	*(exp_coord)  =temp_x*(cos(pitch)*cos(yaw))
			      +temp_y*(-cos(pitch)*sin(yaw))
				  +temp_z*(sin(pitch));
	*(exp_coord+1)=temp_x*(cos(roll)*sin(yaw)+sin(roll)*sin(pitch)*cos(yaw))
				  +temp_y*(cos(roll)*cos(yaw)-sin(roll)*sin(pitch)*sin(yaw))
				  +temp_z*(-sin(roll)*cos(pitch));
	*(exp_coord+2)=temp_x*(sin(roll)*sin(yaw)-cos(roll)*sin(pitch)*cos(yaw))
			      +temp_y*(sin(roll)*cos(yaw)+cos(roll)*sin(pitch)*sin(yaw))
				  +temp_z*(cos(roll)*cos(pitch));
}

//旋转加平移
void cotrs(const float* coord,float* exp_coord,float roll,float pitch,float yaw,float tx,float ty,float tz)
{
	float temp_x=*(coord),temp_y=*(coord+1),temp_z=*(coord+2);
	*(exp_coord)  =temp_x*(cos(pitch)*cos(yaw))
				  +temp_y*(-cos(pitch)*sin(yaw))
				  +temp_z*(sin(pitch))
				  +tx;
	*(exp_coord+1)=temp_x*(cos(roll)*sin(yaw)+sin(roll)*sin(pitch)*cos(yaw))
				  +temp_y*(cos(roll)*cos(yaw)-sin(roll)*sin(pitch)*sin(yaw))
				  +temp_z*(-sin(roll)*cos(pitch))
				  +ty;
	*(exp_coord+2)=temp_x*(sin(roll)*sin(yaw)-cos(roll)*sin(pitch)*cos(yaw))
				  +temp_y*(sin(roll)*cos(yaw)+cos(roll)*sin(pitch)*sin(yaw))
				  +temp_z*(cos(roll)*cos(pitch))
				  +tz;
}

//六足**************************
//平移
void cotrs_t_6lg(float coord[6][3],float tx,float ty,float tz)
{
	float temp_coord[6][3];
	int i,j;
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<6;i++){
		cotrs_t(temp_coord[i],coord[i],tx,ty,tz);
	}
}


//绕x轴旋转
void cotrs_x_6lg(float coord[6][3],float roll)
{
	float temp_coord[6][3];
	int i,j;
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<6;i++){
		cotrs_x(temp_coord[i],coord[i],roll);
	}
}

//绕y轴旋转
void cotrs_y_6lg(float coord[6][3],float pitch)
{
	float temp_coord[6][3];
	int i,j;
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<6;i++){
		cotrs_y(temp_coord[i],coord[i],pitch);
	}
}

//绕z轴旋转
void cotrs_z_6lg(float coord[6][3],float yaw)
{
	float temp_coord[6][3];
	int i,j;
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<6;i++){
		cotrs_z(temp_coord[i],coord[i],yaw);
	}
}

//按照z,y,x的顺序旋转
void cotrs_xyz_6lg(float coord[6][3],float roll,float pitch,float yaw)
{
	float temp_coord[6][3];
	int i,j;
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<6;i++){
		cotrs_xyz(temp_coord[i],coord[i],roll,pitch,yaw);
	}
}

//旋转加平移
void cotrs_6lg(float coord[6][3],float roll,float pitch,float yaw,float tx,float ty,float tz)
{
	float temp_coord[6][3];
	int i,j;
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<6;i++){
		cotrs(temp_coord[i],coord[i],roll,pitch,yaw,tx,ty,tz);
	}
}

//四足*******************
//平移
void cotrs_t_4lg(float coord[4][3],float tx,float ty,float tz)
{
	float temp_coord[4][3];
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<4;i++){
		cotrs_t(temp_coord[i],coord[i],tx,ty,tz);
	}
}


//绕x轴旋转
void cotrs_x_4lg(float coord[4][3],float roll)
{
	float temp_coord[4][3];
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<4;i++){
		cotrs_x(temp_coord[i],coord[i],roll);
	}
}

//绕y轴旋转
void cotrs_y_4lg(float coord[4][3],float pitch)
{
	float temp_coord[4][3];
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<4;i++){
		cotrs_y(temp_coord[i],coord[i],pitch);
	}
}

//绕z轴旋转
void cotrs_z_4lg(float coord[4][3],float yaw)
{
	float temp_coord[4][3];
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<4;i++){
		cotrs_z(temp_coord[i],coord[i],yaw);
	}
}

//按照z,y,x的顺序旋转
void cotrs_xyz_4lg(float coord[4][3],float roll,float pitch,float yaw)
{
	float temp_coord[4][3];
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<4;i++){
		cotrs_xyz(temp_coord[i],coord[i],roll,pitch,yaw);
	}
}

//旋转加平移
void cotrs_4lg(float coord[4][3],float roll,float pitch,float yaw,float tx,float ty,float tz)
{
	float temp_coord[4][3];
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			temp_coord[i][j]=coord[i][j];
		}
	}
	for(i=0;i<4;i++){
		cotrs(temp_coord[i],coord[i],roll,pitch,yaw,tx,ty,tz);
	}
}
