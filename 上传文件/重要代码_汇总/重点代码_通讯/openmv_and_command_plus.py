#处理Openmv视觉模块和与小程序端指令通讯
import time
import network
from umqttsimple import MQTTClient
import machine
import ujson
import binascii
import _thread
import struct

button_pin = 10
running = True
lastcommand = 0
lastvalue = 0
# 初始化引脚
button = machine.Pin(button_pin, machine.Pin.IN)
uart1 = machine.UART(1, baudrate=115200, tx=3, rx=2)
uart1.init(115200, bits=8, parity=None, stop=1, rxbuf=2048)
# 检测按钮状态
def check_button_state():
    button_state = button.value()
    return button_state

def uart_send(command_s,value_s):
    # 将两个数据分别打包成字节数组
    byteArray1 = struct.pack("i", command_s)
    byteArray2 = struct.pack("f",value_s)
    #print(type(command_s),'\t',type(value_s))
    # 将两个字节数组连接成一个新的字节数组
    newByteArray = byteArray1 + byteArray2

    # 发送字节数组到串口
    uart1.write(newByteArray)
    
def uart_send_xy(value_s):
    # 定义"x"和"y"的初始值为0
    x = 0
    y = 0

    # 创建一个空的字节数组
    newByteArray = b""
    Array = ""
    # 根据循环次数连接"x"和"y"的数据并打包成字节数组
    for i in range(0,10):
        # 如果"value"中包含当前键，则更新"x"和"y"的值
        if i<len(value_s):
            x = value_s[i][0]
            y = value_s[i][1]

        # 将"x"和"y"的值分别打包成字节数组并连接到newByteArray
        newByteArray += struct.pack("f", x)
        newByteArray += struct.pack("f", y)
        Array += str(x)
        Array += str(y)
        Array += " "
    # 发送字节数组到串口
    uart1.write(newByteArray)
    #print(Array)
    
def convert_command_to_int(command_str):
    command_mapping = {
        'go': 1,                        # 直线移动指令: c_move_strait
        'turn': 2,                      # 转向移动指令: c_move_turn
        'type': 3,                      # 切换模式指令: c_switch_mode
        'speed': 4,                     # 切换速度指令: c_switch_velocity
        'step': 5,                      # 切换跨度指令: c_switch_span
        'roll': 6,                      # 横滚指令: c_roll
        'pitch': 7,                     # 俯仰指令: c_pitch
        'yaw': 8,                       # 偏航指令: c_yaw
        'height': 9,                    # 高度控制指令: c_height
        'x': 10,                        # 平台水平移动指令: c_platform_h
        'y': 11,                        # 平台垂直移动指令: c_platform_v
        
        'stop': 13,                     # 停止指令: c_stop
        'stable': 14,                   # 自稳定模式指令: c_self_stable
        
        'nav': 15,                      # 舵机矫正启动指令: c_servo_start
        'navNum': 16,                   # 伺服矫正指令: c_servo_correct
        'power': 17                     # 启动/停止指令: c_start_stop

    }
    
    return command_mapping.get(command_str, 0)  # 返回对应的编码，如果不存在则返回0

def process_message(json_data):
    global lastcommand, lastvalue
    try:
        # 尝试解析JSON消息
        if "command" in json_data and "value" in json_data:
            command_r = json_data['command']
            value_r = json_data['value']
            command_s = convert_command_to_int(command_r)
            # 直行
            if command_s == 1 or command_s == 2:
                value_s = -float(value_r)
                if lastcommand != command_s or lastvalue - value_s <=0.02 or lastvalue - value_s >=0.02:
                    print("command_s:", command_s, "\tvalue_s:", value_s)
                    uart_send(command_s, value_s)
                    lastcommand = command_s
                    lastvalue = value_s
                                    
            elif command_r == 'ruler':
                #print("command_s ->", 12, "\tvalue_s:", value_r)
                uart_send(command_s, 10*float(value_r))
            elif command_r == 'guihua':
                uart_send_xy(eval(value_r))
            else:
                value_s = float(value_r)
                #print("command_s:", command_s, "\tvalue_s:", value_s)
                uart_send(command_s, value_s)
        
    except ValueError:
        #print("incorrect command format...")
        # JSON解析错误，忽略消息
        pass
    

def sub_cb(topic, msg):
    #print("Received message from", topic.decode(), ":", msg.decode())
    try:
        parsed_msg = ujson.loads(msg.decode('utf-8'))  # 尝试解析接收到的消息
        process_message(parsed_msg)
    except ValueError:
        print("Received non-JSON message:", msg.decode('utf-8'))

def do_connect():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        #print('connecting to network...')
        wlan.connect('114514', 'qazwsxedc12345')
        i = 1
        while not wlan.isconnected():
            #print("正在链接...{}".format(i))
            i += 1
            time.sleep(1)
    #print('network config:', wlan.ifconfig())

def mqtt_thread():
    c = MQTTClient("Commandrecv","124.221.254.13",1883,"admin","public",keepalive=600)#建立一个NOTT客户端
    c.set_callback(sub_cb)#设置回调函数
    c.connect()#建立连接
    #print("ok2")
    c.subscribe(b"wechatCommand") # 订阅主题
    while True:
        c.check_msg() # 接收消息
        #time.sleep(0.005)

def main():
    do_connect()#联网
    cc = MQTTClient("Openmvsend","124.221.254.13",1883,"admin","public",keepalive=60)#建立一个NOTT客户端
    cc.connect()#与服务器建立连接
    #print("ok1")
    _thread.start_new_thread(mqtt_thread, ())# 在独立线程中运行 mqtt_thread() 函数
    uart = machine.UART(2, baudrate=115200, tx=17, rx=16)
    uart.init(1200000, bits=8, parity=None, stop=1, rxbuf=4096)
    DATA = b''
    while True:
        if uart.any():
            received_data = uart.read()
            DATA += received_data
            if DATA[-2:] == b'\xff\xd9' :
                #print(len(DATA))
                #res = {'command': 'photo', 'data': binascii.hexlify(DATA)}
                cc.publish('video', DATA) # 将消息发布到 MQTT 主题，由回调函数处理
                DATA = b''

            

        
if __name__ == "__main__":
    main()

