/*定义运动控制主循环*/
#ifndef _MAINLOOP_H_
#define _MAINLOOP_H_

#include"hexapod.h"

struct getins{
	int command;
	float data;
};//接收数据的结构体格式

extern struct getins data;

/*定义各种命令对应的指令*/
#define c_move_strait 1                //直行
#define c_move_turn   2                //转弯
#define c_switch_mode 3                //转换步态，1六足，2四足，3轮足
#define c_switch_velocity 4            //改变速度 1 2 3档
#define c_switch_span 5                //步长跨度
#define c_roll 6                       //改变roll角
#define c_pitch 7                      //改变pitch角
#define c_yaw 8                        //改变yaw角
#define c_hieght 9                     //改变行走高度
#define c_platform_h 10                //改变舵机云台的水平角度
#define c_platform_v 11                //改变舵机云台的竖直角度
#define c_path_planning 12             //路径规划
#define c_stop  13                     //停止
#define c_self_stable 14               //自稳定
#define c_self_stable_quit 13          //退出自稳定,注意和stop相同
#define c_servo_start  15              //开始舵机矫正，进入程序
#define c_servo_quit   13              //退出舵机矫正
#define c_servo_crooect   16           //舵机矫正参数
#define c_start_stop      17           //1开机，0关机


void mainloop(struct hexapod* hexa_p);




#endif
