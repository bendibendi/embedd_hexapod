#处理热成像模块和GPS模块
from bmp import BMP
import machine
import time
import network
from umqttsimple import MQTTClient
import ujson
import binascii
import _thread
import struct

adc_pin = machine.Pin(15)
adc = machine.ADC(adc_pin)
# 初始化引脚
adc2_pin = machine.Pin(10)
adc2 = machine.ADC(adc2_pin)

c = MQTTClient("IRsend","124.221.254.13",1883,"admin","public",keepalive=60)#建立一个NOTT客户端
uart = machine.UART(1, baudrate=38400, tx=17, rx=16)

# 检测按钮状态
def check_button_state():
    if adc2.read()>3000:
        return False
    else:
        return True

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

def process_data(payload):
    global data_buf
    a = 0
    temperature_list = [[0] * 32 for _ in range(24)]
    
    if len(payload) == 1544:
        try:
            max_value = float('-inf')  # 初始化最大值为负无穷小
            for y in range(24):
                for x in range(4, 64 + 4, 2):
                    a += 1
                    temp2 = (payload[x + 1 + 64 * y] << 8) | payload[x + 64 * y]
                    temperature = float(temp2) / 100
                    temperature_list[y][int((x - 4) / 2)] = temperature
                    max_value = max(max_value, temperature)  # 更新最大值
            #print("Max=",max_value, end='\t')
            bmp_image = BMP(w=32, h=24)

            # Fill rgbData with temperature values
            for r in range(bmp_image.h):
                for t in range(bmp_image.w):
                    temperature = temperature_list[r][t]
                    bmp_image.rgbData[r][t] = temperature
            #print(bmp_image.rgbData)
            #bmp_image.save_image()
            binary_data = bmp_image.get_binary_code()
            c.publish('ir', binary_data)
            #print(len(binary_data),'bits sent')
            #print(len(bmp_image.rgbData))
            
        except Exception as e:
            print('Error:', e)
        #print(a)
    data_buf = b''  # 清零数据缓冲区
    
def reset_device():
    uart.write('AT+CGNSPWR=1\r\n')
    time.sleep(1)
    uart.read()
    uart.write('AT+CGPS=1,1\r\n')
    time.sleep(1)
    uart.read()    
    
def gps_thread():
    cc = MQTTClient("GPSsend","124.221.254.13",1883,"admin","public",keepalive=120)#建立一个NOTT客户端
    cc.connect()#建立连接
    reset_device()
    uart.write('AT+CGNSCMD=0,"$PMTK251,115200*1F"\r\n')    
    time.sleep(1)
    latitude, longitude = None, None
    #print("ok")
    while True:
        vout = adc.read()  # 返回一个0-4095的整数值，对应0-3.3V
        if vout<3000:
            #print("vout：", vout)
            res = {'command': 'state', 'data': 'Person Ahead'}
            cc.publish('gpsCommand', ujson.dumps(res)) # 将消息发布到 MQTT 主题，由回调函数处理
        data = uart.read(1024)
        if data:
            
            try:
                data = data.decode('utf-8')
                data = data.split('\r\n')
                #print(data)
            except UnicodeError:
                pass
            except AttributeError:
                pass
            for item in data:
                if item.startswith('$GNRMC'):
                    item = item.split(',')
                    if len(item) >= 6:  # 检查列表长度是否大于等于6
                        latitude = item[3]
                        longitude = item[5]
                        # 进一步处理经纬度数据
                    #welse:
                        #print("数据项不完整")
            if latitude and longitude:
                
                formatted_latitude = latitude[:9]
                formatted_longitude = longitude[:10]
                #print(formatted_latitude, formatted_longitude)
                
                res = {'command': 'latitude', 'data': (formatted_latitude)}
                cc.publish('gpsCommand', ujson.dumps(res)) # 将消息发布到 MQTT 主题，由回调函数处理
                res = {'command': 'longitude', 'data': (formatted_longitude)}
                cc.publish('gpsCommand', ujson.dumps(res)) # 将消息发布到 MQTT 主题，由回调函数处理
        time.sleep(0.5)
def main():
    data_buf = b''  # 初始化数据缓冲区
    start_marker = b'\x5a\x5a'
    end_marker = b'\x5a\x5a'
    
    do_connect()#联网
    c.connect()#与服务器建立连接
    uart1 = machine.UART(2, baudrate=115200, tx=3, rx=2,rxbuf=2048)
    _thread.start_new_thread(gps_thread, ())
    send_data = bytearray([0xa5, 0x25, 0x03, 0xcd])
    uart1.write(send_data)
    time.sleep(0.1)
    send_data = bytearray([0xa5, 0x35, 0x02, 0xdc])
    uart1.write(send_data)
    time.sleep(0.1)
    send_data = bytearray([0xa5, 0x65, 0x01, 0x0b])
    uart1.write(send_data)

    while True:
        #time.sleep(0.02)
        uart_data = uart1.read(6000)  # 从串口读取数据
        
        if uart_data:
            data_buf += uart_data  # 在现有数据缓冲区末尾添加新的数据

            # 查找起始标记和结束标记的索引
            start_index = data_buf.find(start_marker)
            if start_index >= 0:
                # 查找结束标记的索引
                end_index = data_buf.find(end_marker, start_index + len(start_marker))
            if start_index >= 0 and end_index >= 0:
                #print(start_index,end="\t")
                #print(end_index)
                # 如果找到起始标记和结束标记
                if start_index < end_index:
                    # 提取从起始标记到结束标记之间的数据，并处理
                    payload = data_buf[start_index:end_index]
                    process_data(payload)
                    data_buf = data_buf[end_index:]
                    #break
                else:
                    # 如果顺序不正确，则删除起始标记之前的数据
                    data_buf = data_buf[start_index:]

if __name__ == "__main__":
    main()



