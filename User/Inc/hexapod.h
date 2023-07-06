/*定义六足机器人的结构体*/
#ifndef _HEXAPOD_H_
#define _HEXAPOD_H_

struct hexapod{
	//机器人启动
	int   start;                           //机器人主体，前后关节相对于中点的宽
	//机器人物理参数
    float width1;                          //机器人主体，前后关节相对于中点的宽
    float width2;                          //机器人主体，中间关节相对于中点的宽
    float len;                             //机器人主体，前后关节相对于中点的长
    float lg1;                             //腿1长度
    float lg2;                             //腿2长度
    float lg3;                             //腿3长度
    //机器人位置参数
    float loc;                             //六（四）只脚的落脚点与关节的水平距离
    float theta;                           //前后脚与左右方向上的默认夹角
    float height;                          //默认站立的高度
    float roll;                            //六只脚的平面相对于原点（主结构中点）在x方向旋转的角度：默认为0
    float pitch;
    float yaw;
    float tx;
    float ty;
    float tz;                              //六只脚的平面相对于原点（主结构中点）在z方向平移的距离，默认为 -height
    float feet_6lg_up[6][3];               //六足状态下六只脚映射到主结构的默认坐标（存储作用，方便计算）  #前三组对应左脚，后三组对应右脚，从前往后，关节从内到外
    float feet_4lg_up[4][3];               //四足状态下四只脚映射到主结构的默认坐标
    float feet_6lg[6][3];                  //六足状态下六只脚的默认坐标（存储作用，方便计算）
    float feet_4lg[4][3];                  //四足状态下四只脚的默认坐标
    float joint_6lg[6][3];                 //六足状态下六个关节的默认坐标（存储作用，方便计算）
    float joint_4lg[4][3];                 //四足状态下四个关节的默认坐标
    //机器人运动参数
    int mode;                              //机器人模式：1（六足）、2（四足）、3（轮足）
    int mode_6lg;                          //六足模式：0（左2+右1着地）、1（左1+右2着地）、2（六脚着地）
    float span;                            //机器人一步的跨度（一步是指一组腿而不是两组腿）
    float span_height;                     //机器人一步的高度
    int speed;                             //1慢速，2标准，3快速
};

void Hexapod_Init(struct hexapod* hex_p,
				const float width1,
				const float width2,
				const float len,
				const float lg1,
				const float lg2,
				const float lg3,
				const float loc,
				const float tehta,
				const float height,
				const float span,
				const float span_height);
#endif
