
#include "mainloop.h"
#include "hexapod.h"
#include "sports.h"
#include "math.h"
#include "pca9685.h"
#include "stm32g4xx_hal.h"
#include "pathplan.h"
#include "imu.h"

struct getins data;

#define deg2rad 0.01745329

void mainloop(struct hexapod* hexa_p)
{
	while(1){
		if((*hexa_p).start!=1){
			//没有启动时等待开机信号，等到开机信号则启动
			if(data.command==c_start_stop && fabs(data.data-1.0)<1e-5){
				//开机时首先从下蹲到站立状态,开机时默认进入六足状态
				stand(hexa_p,(int)(fabs((*hexa_p).tz)));
				//修改状态值
				(*hexa_p).start=1;
				(*hexa_p).mode=1;
				(*hexa_p).mode_6lg=2;
			}
		}
		else{
			switch(data.command){
			case(c_move_strait):
					/*直行命令，如果在六足状态，首先需要调整为默认状态*/
					if((*hexa_p).mode==1){
						//六足模式下首先应该恢复默认状态
						to_state(hexa_p,0.0,0.0,0.0,(*hexa_p).tz);
						straight_6lg(hexa_p);
					}
					else if((*hexa_p).mode==2)
						straight_4lg(hexa_p);//四足状态下直行
					else
						straight_wheel(hexa_p);//轮足状态下直行
					break;
			case(c_move_turn):
					/*如果在六足状态，首先需要调整为默认状态*/
					if((*hexa_p).mode==1){
						//六足模式下首先应该恢复默认状态
						to_state(hexa_p,0.0,0.0,0.0,(*hexa_p).tz);
						turn_6lg(hexa_p);
					}
					else if((*hexa_p).mode==2)
						turn_4lg(hexa_p);//四足状态下直行
					else
						turn_wheel(hexa_p);//轮足状态下直行
					break;
			case(c_switch_mode):
					/*转换步态*/
					if(fabs(data.data-1.0)<1e-5){
						if((*hexa_p).mode==2)
							four2six(hexa_p);
						else if((*hexa_p).mode==3)
							wheel2six(hexa_p);
					}
					else if(fabs(data.data-2.0)<1e-5){
						if((*hexa_p).mode==1){
							to_state(hexa_p,0.0,0.0,0.0,(*hexa_p).tz);
							six2four(hexa_p);
						}
						else if((*hexa_p).mode==3){
							wheel2six(hexa_p);
							six2four(hexa_p);
						}
					}
					else{
						if((*hexa_p).mode==1){
							to_state(hexa_p,0.0,0.0,0.0,(*hexa_p).tz);
							six2wheel(hexa_p);
						}
						else if((*hexa_p).mode==2){
							four2six(hexa_p);
							six2wheel(hexa_p);
						}
					}
					break;
			case(c_switch_velocity):
					(*hexa_p).speed=data.data;
					break;
			case(c_switch_span):
					(*hexa_p).span=data.data;
					break;
			case(c_roll):
					to_state(hexa_p,data.data*deg2rad,(*hexa_p).pitch,(*hexa_p).yaw,(*hexa_p).tz);
					break;
			case(c_pitch):
					to_state(hexa_p,(*hexa_p).roll,data.data*deg2rad,(*hexa_p).yaw,(*hexa_p).tz);
					break;
			case(c_yaw):
					to_state(hexa_p,(*hexa_p).roll,(*hexa_p).pitch,data.data*deg2rad,(*hexa_p).tz);
					break;
			case(c_hieght):
					to_state(hexa_p,(*hexa_p).roll,(*hexa_p).pitch,(*hexa_p).yaw,-data.data);
					break;
			case(c_platform_h):
					SERVO_SPT(11,data.data);
					HAL_Delay(100);
					break;
			case(c_platform_v):
					SERVO_SPT(23,data.data);
					HAL_Delay(100);
					break;
			case(c_path_planning):
					pathplan(hexa_p);
					break;
			case(c_self_stable):
		            self_stable(hexa_p);
					break;
			case(c_servo_start):
					servo_correct(hexa_p);
					break;
			case(c_start_stop):
					if(fabs(data.data-2.0)<1e-5){
						if((*hexa_p).mode==2)
							four2six(hexa_p);
						else if((*hexa_p).mode==3)
							wheel2six(hexa_p);
						crouch(hexa_p,(int)(fabs((*hexa_p).tz)));
						(*hexa_p).start=0;
					}
					break;
			default:
				break;
			}
		}
	}
}
