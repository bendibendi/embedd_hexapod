#Openmv嵌入cnn，基本图像处理以及发送图像
import sensor, image, time, os, tf, uos, gc, pyb
import ustruct
import pyb
usb = pyb.USB_VCP()
sensor.reset()                         # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565)    # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)      # Set frame size to QVGA (320x240)
sensor.set_windowing((240, 240))       # Set 240x240 window.
sensor.skip_frames(time=2000)          # Let the camera adjust.

net = None
# 初始化引脚
pin = pyb.Pin('P2', pyb.Pin.OUT)
pin.high()
try:
    # 加载tf训练模型文件，96*96大小
    net = tf.load("person_detection.tflite", load_to_fb=uos.stat('person_detection.tflite')[6] > (gc.mem_free() - (64*1024)))
except Exception as e:
    #print(e)
    raise Exception('Failed to load "person_detection.tflite", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')


labels = [line.rstrip('\n') for line in open("person_detection.txt")]
uart = pyb.UART(3, 1200000)
#a=0
clock = time.clock()
while True:
    #clock.tick()

    img = sensor.snapshot()
    img1=img
    for obj in net.classify(img, min_scale=1.0, scale_mul=0.8, x_overlap=0.5, y_overlap=0.5):

        #print("**********\nPredictions at [x=%d,y=%d,w=%d,h=%d]" % obj.rect())
        if obj.output()[1]>0.5 :
            # 检测框
            #img.draw_rectangle(obj.rect())
            #img.draw_string(obj.x()+3, obj.y()-1, labels[obj.output().index(max(obj.output()))], scale=3.0)

            #predictions_list = list(zip(labels, obj.output()))

            # 输出每个类别的概率
            #for i in range(len(predictions_list)):
                #print("%s = %f" % (predictions_list[i][0], predictions_list[i][1]))
            # 通知
            pin.low()
        else :
            pin.high()
    #print(clock.fps(), "fps")

    img1.compress(quality=20)
    #a=a+1
    #time.sleep_ms(10)
    #print(a,"\t",len(img1))
    uart.write(img1)
