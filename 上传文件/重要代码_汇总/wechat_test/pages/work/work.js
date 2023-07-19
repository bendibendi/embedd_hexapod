// pages/work/work.js
import mqtt from "../../utils/mqtt.min.js";

const app = getApp();   //引入全局变量
Page({
  /**
   * 页面的初始数据
   */
  data: {
    state: '正常',
    timer: '',

    // video/
    condition: true,
    // video
    videoId: 'video',
    //videoSrc:'../../sources/videos/hometown.mp4',
    videoSrc: 'http://wxsnsdy.tc.qq.com/105/20210/snsdyvideodownload?filekey=30280201010421301f0201690402534804102ca905ce620b1241b726bc41dcff44e00204012882540400&bizid=1023&hy=SH&fileparam=302c020101042530230204136ffd93020457e3c4ff02024ef202031e8d7f02030f42400204045a320a0201000400',

    // 切换底部控件
    switchBoxNum:1,

    // 暂用进度条
    sliderValue: 50,

    // 页面一进度条
    // 速度
    speedValueMax: 3,
    speedValueMin: 1,
    speedValue:2,
    // 步长
    stepValueMax: 80,
    stepValueMin: 50,
    stepValue: 60,
    // 页面一按钮
    // 开机
    powerText:"开机",
    powerValue:false,

    // // control
    // b_top_state: 'default',
    // b_down_state: 'default',
    // b_right_state: 'default',
    // b_left_state: 'default',

    // 摇杆
    StartX: '122',
    StartY: '90',
    leftLooks: '122',
    topLooks: '90',
    StartXLeft: '122',
    StartYLeft: '90',
    leftLooksLeft: '122',
    topLooksLeft: '90',
    //半径
    radius: '100',
    angle: '',
    angleLeft: '',
    latitude:0.0,
    longitude:0.0,


    // 页面二
    // switch 切换形态
    typeBoxNum:1,

    // roll,yaw,pitch
    sliderValue1: 0,
    sliderValue1min: -10,
    sliderValue1max: 10,
    sliderValue2: 0,
    sliderValue2min: -10,
    sliderValue2max: 10,
    sliderValue3: 0,
    sliderValue3min: -10,
    sliderValue3max: 10,

    // 行走高度    
    sliderValue4: 80,
    sliderValue4min: 80,
    sliderValue4max: 200,

    // info

    // mqtt


    base64ImgUrl: "../../fake.jpg",
    irImage: "../../fake.jpg",
    client: null,
    conenctBtnText: "连接",
    host: "www.mqttt.xyz", //"wx.emqxcloud.cn",
    subTopic: "1t",
    pubTopic: "testtopic/miniprogram",
    pubMsg: "Hello! I am from WeChat miniprogram",
    receivedMsg: "",
    mqttOptions: {
      username: "test",
      password: "test",
      reconnectPeriod: 1000, // 1000毫秒，设置为 0 禁用自动重连，两次重新连接之间的间隔时间
      connectTimeout: 30 * 1000, // 30秒，连接超时时间
    },

    // 校正弹出层
    hiddenmodalput:true, 
    showModal:false,
    duojiNum:1,
    duojiAngle:0,

    // 自稳定
    stableValue:false,
    stableText:"开启自稳定",

    // 云台
    sliderx: 0,
    sliderxmin: -15,
    sliderxmax: 15,
    slidery: 0,
    sliderymin: -30,
    sliderymax: 30,
    // 切换视图
    imageIndex:1,

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    
    // this.connect();
    // this.setValue("subTopic", "gpsCommand");
    // this.subscribe();
		// let arr = options.arr
    // let obj = options.obj;
    // if(!arr||!obj){
    //   ;
    // }
    // else{
    //   console.log("arr"+arr)
    //   console.log("obj"+obj)
    //   this.publishDataIn("wechatCommand","nop","nop")
    //   this.publishDataIn("wechatCommand","guihua",arr)
    //   this.publishDataIn("wechatCommand","ruler",obj)
    // }
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {
    // this.queryMultipleNodes();
    this.connect();
    this.setValue("subTopic", "gpsCommand");
    this.subscribe();
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
    this.endTimer()
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
  },

  // video
  videoErrorCallback: function (e) {
    console.log('视频错误信息:' + e.detail.errMsg);
  },


  // 显示信息

  // 切换底部控件
  click_box1() {
    this.setData({
      switchBoxNum: 1
    })
  },
  click_box2() {
    this.setData({
      switchBoxNum: 2
    })
  },
  click_box3() {
    this.setData({
      switchBoxNum: 3
    })
  },
  // 切换形态
  
  pose_box1() {
    this.setData({
      typeBoxNum: 1
    })
    this.publishDataIn("wechatCommand","type",this.data.typeBoxNum);
  },
  pose_box2() {
    this.setData({
      typeBoxNum: 2
    })
    this.publishDataIn("wechatCommand","type",this.data.typeBoxNum);
  },
  pose_box3() {
    this.setData({
      typeBoxNum: 3
    })
    this.publishDataIn("wechatCommand","type",this.data.typeBoxNum);
  },
  
  // 切换页面
  switchPage:function(){
    wx.navigateTo({
      url: '../index/index',
    })
  },
  // 定时器
  startTimer: function (n, time) {
    var that = this;
    this.data.timer = setTimeout(
      function () {
        switch (n) {
          case 'b_t':
            that.setData({ b_top_state: 'default' });
            break;
          case 'b_d':
            that.setData({ b_down_state: 'default' });
            break;
          case 'b_r':
            that.setData({ b_right_state: 'default' });
            break;
          case 'b_l':
            that.setData({ b_left_state: 'default' });
            break;
          default:
            console.log('error-change default');
        }
      }, time);
  },
  /**
   * 结束定时器
   */
  endTimer: function () {
    clearTimeout(this.data.timer)
  },

  // 开机
  powerBtn: function (e) {
    if (this.data.powerValue==false){
      this.setData({
        powerText: "关机",
        powerValue: true
      })
    }
    else {
      this.setData({
        powerText: "开机",
        powerValue: false
      })
    }
    this.publishDataIn("wechatCommand","power",this.data.powerValue)
  },


  // 右摇杆
  //摇杆点击事件
  ImageTouch: function (e) {
    const query = wx.createSelectorQuery()
    query.select('.control-right-yaogan-front').boundingClientRect()
    query.selectViewport().scrollOffset()
    query.exec(function (res) {
      console.log("top:" + res[0].top);
      console.log("left:" + res[0].left);
    })
  },

  //拖动摇杆移动
  ImageTouchMove: function (e) {
    var self = this;
    //e.touches[0].clientX是触碰的位置，需要减h/2,w/2使得图片中心跟随触碰位置
    // format x y
    var rt;
    var rl;
    // 和yaogan子组件对齐
    var touchX = e.touches[0].clientX - 190;
    var touchY = e.touches[0].clientY - 570;
    // console.log("对齐后的px坐标：(" + touchX + "," + touchY + ")");
    // change to rpx
    var h = wx.getSystemInfoSync().windowHeight // 获取可使用窗口高度
    var w = wx.getSystemInfoSync().windowWidth  // 获取可使用窗口宽度
    var windowHeight = (h * (750 / w)); //rpx单位的窗口高度
    touchX = touchX * 750 / w;
    touchY = touchY * windowHeight / h;
    // console.log("对齐后的rpx坐标：(" + touchX + "," + touchY + ")");
    
    // 对齐图片左上角
    touchX=touchX-60;
    touchY=touchY-80;

    // 转化为图片坐标（鼠标中点和图片中点对齐）
    var movePos = self.GetPosition(touchX, touchY);
    // console.log("初始坐标：("+self.data.StartX+","+self.data.StartY+")");
    // console.log("图片坐标：("+movePos.posX + "," + movePos.posY+")");
    self.setData({
      leftLooks: movePos.posX,
      topLooks: movePos.posY
    })
  },

  //松开摇杆复原
  ImageReturn: function (e) {
    var self = this;
    self.setData({
      leftLooks: self.data.StartX,
      topLooks: self.data.StartY,
      angle: ""
    })
    this.publishDataIn("wechatCommand","stop","1");
  },

  // 左摇杆
  //摇杆点击事件
  ImageTouchLeft: function (e) {
    // const query = wx.createSelectorQuery()
    // query.select('.control-right-yaogan-front').boundingClientRect()
    // query.selectViewport().scrollOffset()
    // query.exec(function (res) {
    //   console.log("top:" + res[0].top);
    //   console.log("left:" + res[0].left);
    // })
  },

  //拖动摇杆移动
  ImageTouchMoveLeft: function (e) {
    var self = this;
    //e.touches[0].clientX是触碰的位置，需要减h/2,w/2使得图片中心跟随触碰位置
    // format x y
    var rt;
    var rl;
    // 和yaogan子组件对齐
    var touchX = e.touches[0].clientX;// - 190;
    var touchY = e.touches[0].clientY - 570;
    // console.log("对齐后的px坐标：(" + touchX + "," + touchY + ")");
    // change to rpx
    var h = wx.getSystemInfoSync().windowHeight // 获取可使用窗口高度
    var w = wx.getSystemInfoSync().windowWidth  // 获取可使用窗口宽度
    var windowHeight = (h * (750 / w)); //rpx单位的窗口高度
    touchX = touchX * 750 / w;
    touchY = touchY * windowHeight / h;
    // console.log("对齐后的rpx坐标：(" + touchX + "," + touchY + ")");

    // 对齐图片左上角
    touchX = touchX - 60;
    touchY = touchY - 80;

    // 转化为图片坐标（鼠标中点和图片中点对齐）
    var movePos = self.GetPositionLeft(touchX, touchY);
    // console.log("初始坐标：("+self.data.StartX+","+self.data.StartY+")");
    // console.log("图片坐标：("+movePos.posX + "," + movePos.posY+")");
    self.setData({
      leftLooksLeft: movePos.posX,
      topLooksLeft: movePos.posY
    })
  },

  //松开摇杆复原
  ImageReturnLeft: function (e) {
    var self = this;
    self.setData({
      leftLooksLeft: self.data.StartXLeft,
      topLooksLeft: self.data.StartYLeft,
      angleLeft: ""
    })
    this.publishDataIn("wechatCommand","stop","1");
  },

  //获得触碰位置并且进行数据处理获得触碰位置与拖动范围的交点位置
  GetPosition: function (touchX, touchY) {
    var self = this;
    var DValue_X;
    var Dvalue_Y;
    var Dvalue_Z;
    var imageX;
    var imageY;
    var ratio; 
    // console.log("touchX  " + touchX)
    // console.log("touchY  " + touchY)
    DValue_X = touchX - self.data.StartX;
    Dvalue_Y = touchY - self.data.StartY;
    Dvalue_Z = Math.sqrt(DValue_X * DValue_X + Dvalue_Y * Dvalue_Y);
    self.GetAngle(DValue_X, Dvalue_Y)
    //触碰点在范围内
    if (Dvalue_Z <= self.data.radius) {
      imageX = touchX;
      imageY = touchY;
      imageX = Math.round(imageX);
      imageY = Math.round(imageY);
      var angle=this.data.angle/180*3.1415926;
      this.publishDataIn('wechatCommand', 'go', angle);
      return { posX: imageX, posY: imageY };
    }

    //触碰点在范围外
    else {
      // ratio = self.data.radius / Dvalue_Z;
      // imageX = DValue_X * ratio + this.StartX;
      // imageY = Dvalue_Y * ratio + this.StartY;
      // imageX = Math.round(imageX);
      // imageY = Math.round(imageY);
      return { posX: self.data.StartX, posY: self.data.StartY };
    }
  },
// 左摇杆版本
  GetPositionLeft: function (touchX, touchY) {
    var self = this;
    var DValue_X;
    var Dvalue_Y;
    var Dvalue_Z;
    var imageX;
    var imageY;
    var ratio;
    console.log("touchX  " + touchX)
    console.log("touchY  " + touchY)
    DValue_X = touchX - self.data.StartXLeft;
    Dvalue_Y = touchY - self.data.StartYLeft;
    Dvalue_Z = Math.sqrt(DValue_X * DValue_X + Dvalue_Y * Dvalue_Y);
    self.GetAngleLeft(DValue_X, Dvalue_Y)
    //触碰点在范围内
    if (Dvalue_Z <= self.data.radius) {
      imageX = touchX;
      imageY = touchY;
      imageX = Math.round(imageX);
      imageY = Math.round(imageY);
      var angle=this.data.angleLeft/180*3.1415926
      this.publishDataIn('wechatCommand','turn',angle);
      return { posX: imageX, posY: imageY };
    }

    //触碰点在范围外
    else {
      // ratio = self.data.radius / Dvalue_Z;
      // imageX = DValue_X * ratio + this.StartX;
      // imageY = Dvalue_Y * ratio + this.StartY;
      // imageX = Math.round(imageX);
      // imageY = Math.round(imageY);
      return { posX: self.data.StartXLeft, posY: self.data.StartYLeft };
    }
  },

  //获取角度
  GetAngle: function (DValue_Y, Dvalue_X) {
    var self = this;
    if (Dvalue_X != 0) {
      var angleTan = DValue_Y / -Dvalue_X;
      var ang = Math.atan(angleTan);

      var angs = ang * 180 / 3.14;
      if (Dvalue_X > 0) {
        if(DValue_Y > 0){
          angs+=180;
        }
        else{
          angs=angs-180
        }
      }
      var result = Math.round(angs);
      console.log(result);
      self.setData({
        angle: result
      })
    }
  },
// 左摇杆版本
  GetAngleLeft: function (DValue_Y, Dvalue_X) {
    var self = this;
    if (Dvalue_X != 0) {
      var angleTan = DValue_Y / -Dvalue_X;
      var ang = Math.atan(angleTan);

      var angs = ang * 180 / 3.14;
      if (Dvalue_X > 0) {
        if (DValue_Y > 0) {
          angs += 180;
        }
        else {
          angs = angs - 180
        }
      }
      var result = Math.round(angs);
      console.log(result);
      self.setData({
        angleLeft: result
      })
    }
  },

  // 弹出校正栏
  modalinput: function () {
    this.setData({
      //注意到模态框的取消按钮也是绑定的这个函数，
      //所以这里直接取反hiddenmodalput，也是没有毛病
      hiddenmodalput: !this.data.hiddenmodalput
    })
  },



  // UTILS 工具函数
  rpxTorpx(rpx) {
    let deviceWidth = wx.getSystemInfoSync().windowWidth;	//获取设备屏幕宽度
    let px = (deviceWidth / 750) * Number(rpx);
    return px;
  },

  // px 转换为 rpx ，传参类型是数字（Number）
  pxTorpx(px) {
    let deviceWidth = wx.getSystemInfoSync().windowWidth;	//获取设备屏幕宽度
    let rpx = (750 / Number(px)) * deviceWidth;
    return rpx;
  },

  // 滑杆
  changeSliderValue(e) {
    this.setData({ sliderValue: e.detail.value })
  },

  changingSpeedValue(e) {
    this.setData({ speedValue: e.detail.value })
  },
  changeSpeedValue(e) {
    this.setData({ speedValue: e.detail.value })
    var msg = this.createJsonData('speed', this.data.speedValue);
    this.publishDataIn('wechatCommand', 'speed', e.detail.value);
  },
  changingStepValue(e) {
    this.setData({ stepValue: e.detail.value })
  },
  changeStepValue(e) {
    this.setData({ stepValue: e.detail.value })
    var msg = this.createJsonData('step', this.data.speedValue);
    this.publishDataIn('wechatCommand', 'step', e.detail.value);
  },

  // 查看页面位置
  queryMultipleNodes() {
    const query = wx.createSelectorQuery()
    query.select('.control-bottom-left-yaogan').boundingClientRect(function (res) {
      res.top // #the-id 节点的上边界坐标（相对于显示区域）
      console.log("top")
      console.log(res.top)
      console.log("left")
      console.log(res.left)
    })
    query.selectViewport().scrollOffset(function (res) {
      res.scrollTop // 显示区域的竖直滚动位置
    })
    query.exec()
    console.log("query2")

    query.select('.control-bottom-right-yaogan').boundingClientRect(function (res) {
      res.top // #the-id 节点的上边界坐标（相对于显示区域）
      console.log("top")
      console.log(res.top)
      console.log("left")
      console.log(res.left)
    })
    query.selectViewport().scrollOffset(function (res) {
      res.scrollTop // 显示区域的竖直滚动位置
    })
    query.exec()
  },

  // mqtt
  setValue(key, value) {
    this.setData({
      [key]: value,
    });
  },
  setHost(e) {
    this.setValue("host", e.detail.value);
  },
  setSubTopic(e) {
    this.setValue("subTopic", e.detail.value);
  },
  setPubTopic(e) {
    this.setValue("pubTopic", e.detail.value);
  },
  setPubMsg(e) {
    this.setValue("pubMsg", e.detail.value);
  },
  setRecMsg(msg) {
    this.setValue("receivedMsg", msg);
  },

  connect() {
    // MQTT-WebSocket 统一使用 /path 作为连接路径，连接时需指明，但在 EMQX Cloud 部署上使用的路径为 /mqtt
    // 因此不要忘了带上这个 /mqtt !!!
    // 微信小程序中需要将 wss 协议写为 wxs，且由于微信小程序出于安全限制，不支持 ws 协议
    try {
      // this.setValue("conenctBtnText", "连接中...");
      const clientId = new Date().getTime();
      this.data.client = mqtt.connect(`wxs://${this.data.host}:8084/mqtt`, {
        ...this.data.mqttOptions,
        clientId,
      });

      this.data.client.on("connect", () => {
        // wx.showToast({
        //   title: "连接成功",
        // });
        // this.setValue("conenctBtnText", "连接成功");

        this.data.client.on("message", (topic, payload) => {
          // wx.showModal({
          //   content: `收到消息 - Topic: ${topic}，Payload: ${payload}`,
          //   showCancel: false,
          // });
          if (topic=="video"){
            let url = 'data:image/png;base64,' + wx.arrayBufferToBase64(payload)
            this.setValue("base64ImgUrl", url)
          }
          else if (topic == "ir") {
            let url = 'data:image/png;base64,' + wx.arrayBufferToBase64(payload)
            this.setValue("irImage", url)
          }
          else if (topic == "gpsCommand") {
            // const currMsg = this.data.receivedMsg ? `<br/>${payload}` : payload;
            // // console.log(payload);
            // this.setValue("receivedMsg", currMsg);//this.data.receivedMsg.concat(currMsg));

            // // 写文件
            // var base64 = wx.arrayBufferToBase64(currMsg);
            // // console.log(base64);
            // const base64ImgUrl = "data:image/png;base64," + base64;
            // // this.setValue("base64ImgUrl", "data:image/jpg;base64," + base64ImgUrl)

            // let url = 'data:image/png;base64,' + wx.arrayBufferToBase64(payload)
            // this.setValue("base64ImgUrl", url)
            var json1 = JSON.parse(payload);

            // console.log(json1);

            // console.log(json1['command']);
            // console.log(json1['data'])
            if (json1['command'] == 'longitude') {
              this.setValue('longitude', json1['data'])
            }
            else if (json1['command'] == 'latitude') {
              this.setValue('latitude', json1['data'])
            }
            else if(json1['command'] == 'state'){
              this.setValue('state',json1['data'])
            }
          }
        });

        this.data.client.on("error", (error) => {
          this.setValue("conenctBtnText", "连接");
          console.log("onError", error);
        });

        this.data.client.on("reconnect", () => {
          this.setValue("conenctBtnText", "连接");
          console.log("reconnecting...");
        });

        this.data.client.on("offline", () => {
          this.setValue("conenctBtnText", "连接");
          console.log("onOffline");
        });
        // 更多 MQTT.js 相关 API 请参阅 https://github.com/mqttjs/MQTT.js#api
      });
    } catch (error) {
      this.setValue("conenctBtnText", "连接");
      console.log("mqtt.connect error", error);
    }
  },

  subscribe() {
    if (this.data.client) {
      this.data.client.subscribe(this.data.subTopic);
      this.data.client.subscribe("ir");
      this.data.client.subscribe("video");
      // wx.showModal({
      //   content: `成功订阅主题：${this.data.subTopic}`,
      //   showCancel: false,
      // });
      return;
    }
    else {
      this.connect();
    }
  },

  unsubscribe() {
    if (this.data.client) {
      this.data.client.unsubscribe(this.data.subTopic);
      wx.showModal({
        content: `成功取消订阅主题：${this.data.subTopic}`,
        showCancel: false,
      });
      return;
    }
    wx.showToast({
      title: "请先点击连接",
      icon: "error",
    });
  },

  publish() {
    if (this.data.client) {
      this.data.client.publish(this.data.pubTopic, this.data.pubMsg);
      return;
    }
    wx.showToast({
      title: "请先点击连接",
      icon: "error",
    });
    // this.connect();
  },

  disconnect() {
    this.data.client.end();
    this.data.client = null;
    this.setValue("conenctBtnText", "连接");
    wx.showToast({
      title: "成功断开连接",
    });
  },

  // 特殊功能函数
  publishData(e) {
    let topic = e.currentTarget.dataset.topic;
    let command = e.currentTarget.dataset.command;
    let value = e.currentTarget.dataset.value;
    if(!this.data.cloent){
      this.connect();
    }
    this.publishDataIn(topic,command,value)
  },

  publishDataIn(topic,command,value){
    let msg = this.createJsonData(command, value);
    this.setValue("pubTopic", topic);
    this.setValue("pubMsg", msg);
    if (this.data.client) {
      console.log('publish  '+this.data.pubMsg)
      this.data.client.publish(this.data.pubTopic, this.data.pubMsg);
      return;
    }
    else {
      this.connect();
      // wx.showToast({
      //   title: "请先点击连接",
      //   icon: "error",
      // });
    }
  },

  // 转换成json格式
  createJsonData(command, value) {
    let jsonData = {
      command: command,
      value: value//JSON.parse(value)
    };
    return JSON.stringify(jsonData);
  },
  
  //  弹出框
   /**
  * 控制显示
    */
  eject:function(){
    this.publishDataIn("wechatCommand","nav","1")
    this.setData({
      showModal:true
    })
  },
  /**
  * 点击返回按钮隐藏
  */
  back:function(){
    this.setData({
      showModal:false
    })
    this.publishDataIn("wechatCommand","stop","1")
    this.resetDuoji()
  },
  resetDuoji:function(){

    this.setData({
      duojiAngleNum:0
    })      
    
    this.setData({
      duojiNum:0
    })      
  },
  /**
  * 点击确定按钮获取input值并且关闭弹窗
  */
  send:function(){
    if(this.data.duojiNum>=1&&this.data.duojiNum<=24&&this.data.duojiAngleNum>=1&&this.data.duojiAngleNum<=180){
      var angle=this.data.duojiAngleNum///180*3.141592654      
      wx.showToast({
        title: '发送成功',
        icon: 'none',
      })
      //发送
      this.publishDataIn("wechatCommand","navNum",parseFloat(this.data.duojiNum)*1000+parseFloat(angle));
    }
    else{
      wx.showToast({
        title: '超出范围，请重新输入',
        icon: 'none',
      })
    }
  },

  getDuojiAngleNum:function(e){
    console.log(e.detail.value)
    var v=e.detail.value
    this.setData({
      duojiAngleNum:v
    })      
  },

  getDuojiNum:function(e){
    console.log(e.detail.value)
    var v=e.detail.value
    this.setData({
      duojiNum:v
    })      
  },

  // 自稳定
  stableChange:function(){
    if(this.data.stableValue==false){
      this.setData({
        stableValue:true
      })      
      this.setData({
        stableText:"关闭自稳定"
      })      
      this.publishDataIn("wechatCommand","stable",this.data.stableValue)
    }
    else{
      this.setData({
        stableValue:false
      })      
      this.setData({
        stableText:"开启自稳定"
      })      
      this.publishDataIn("wechatCommand","stop","1")
    }
  },
  changeSliderValue1:function(e){
    this.setData({
      sliderValue1:e.detail.value
    })      
    var angle=this.data.sliderValue1/180*3.1415926
    this.publishDataIn("wechatCommand","roll",angle)
  },
  changeSliderValue2:function(e){
    this.setData({
      sliderValue2:e.detail.value
    })      
    var angle=this.data.sliderValue2/180*3.1415926
    this.publishDataIn("wechatCommand","yaw",angle)
  },
  changeSliderValue3:function(e){
    this.setData({
      sliderValue3:e.detail.value
    })      
    var angle=this.data.sliderValue3/180*3.1415926
    this.publishDataIn("wechatCommand","pitch",angle)
  },
  changeSliderValue4:function(e){
    this.setData({
      sliderValue4:e.detail.value
    })      
    var height=this.data.sliderValue4
    this.publishDataIn("wechatCommand","height",height)
  },
  changeSliderx:function(e){
    this.setData({
      sliderx:e.detail.value
    })      
    this.publishDataIn("wechatCommand","x",this.data.sliderx)
  },
  changeSlidery:function(e){
    this.setData({
      slidery:e.detail.value
    })      
    this.publishDataIn("wechatCommand","y",this.data.slidery)
  },
  enternav:function(){
      // this.publishDataIn("wechatCommand","guihua","enter")
      wx.navigateTo({
        url: '../testBase64/base64',//要跳转到的页面路径
      })  
  },
  switchImage:function(){
    console.log(this.data.imageIndex)
    if(this.data.imageIndex==1){
      this.setValue('imageIndex',2);
      this.setValue('condition',false)
    }
    else{      
      this.setValue('imageIndex',1) 
      this.setValue('condition',true)
    } 
  }
})