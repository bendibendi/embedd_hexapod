import mqtt from "../../utils/mqtt.min.js";

Page({
  data: {
    base64ImgUrl:"../../mqtt.png",
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
    // 切换按键
    num:1,
  },

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
      this.setValue("conenctBtnText", "连接中...");
      const clientId = new Date().getTime();
      this.data.client = mqtt.connect(`wxs://${this.data.host}:8084/mqtt`, {
        ...this.data.mqttOptions,
        clientId,
      });

      this.data.client.on("connect", () => {
        wx.showToast({
          title: "连接成功",
        });
        this.setValue("conenctBtnText", "连接成功");

        this.data.client.on("message", (topic, payload) => {
          // wx.showModal({
          //   content: `收到消息 - Topic: ${topic}，Payload: ${payload}`,
          //   showCancel: false,
          // });
          const currMsg = this.data.receivedMsg ? `<br/>${payload}` : payload;
          this.setValue("receivedMsg", currMsg);//this.data.receivedMsg.concat(currMsg));
        
          // 写文件
          var base64 = wx.arrayBufferToBase64(currMsg);
          // console.log(base64);
          // const base64ImgUrl = "data:image/png;base64," + base64;
          this.setValue("base64ImgUrl", "data:image/png;base64," + base64)
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
      wx.showModal({
        content: `成功订阅主题：${this.data.subTopic}`,
        showCancel: false,
      });
      return;
    }
    wx.showToast({
      title: "请先点击连接",
      icon: "error",
    });
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
    let msg = this.createJsonData(command,value);
    this.setValue("pubTopic", topic);
    this.setValue("pubMsg", msg);
    if (this.data.client) {
      this.data.client.publish(this.data.pubTopic, this.data.pubMsg);
      return;
    }
    else {
      wx.showToast({
        title: "请先点击连接",
        icon: "error",
      });
    }
  },

  // 转换成json格式
  createJsonData(command, value) {
    let jsonData = {
      command: command,
      value: JSON.parse(value)
    };
    return JSON.stringify(jsonData);
  },

  // 发送滑动条
  slideChange(e) {
    let topic = e.currentTarget.dataset.topic;
    let command = e.currentTarget.dataset.command;
    let value = e.detail.value;
    let msg = this.createJsonData(command, value);
    this.setValue("pubTopic", topic);
    this.setValue("pubMsg", msg);
    if (this.data.client) {
      console.log(this.data.pubMsg);
      console.log(this.data.pubTopic);
      this.data.client.publish(this.data.pubTopic, this.data.pubMsg);
      return;
    }
    else {
      wx.showToast({
        title: "请先点击连接",
        icon: "error",
      });
    }
  },  

  // 发送切换按键
  Click1() {
    if (this.data.num == 2) {
      this.setData({
        num: 1
      });
      let command = "切换形态";
      let value = 1;
      let msg = this.createJsonData(command, value);
      this.setValue("pubTopic", "wechatCommand");
      this.setValue("pubMsg", msg);
      if (this.data.client) {
        console.log(this.data.pubMsg);
        console.log(this.data.pubTopic);
        this.data.client.publish(this.data.pubTopic, this.data.pubMsg);
        return;
      }
      else {
        wx.showToast({
          title: "请先点击连接",
          icon: "error",
        });
      }
    };
  },
  Click2() {
    if (this.data.num == 1) {
      this.setData({
        num: 2
      });
      let command = "切换形态";
      let value = 2;
      let msg = this.createJsonData(command, value);
      this.setValue("pubTopic", "wechatCommand");
      this.setValue("pubMsg", msg);
      if (this.data.client) {
        console.log(this.data.pubMsg);
        console.log(this.data.pubTopic);
        this.data.client.publish(this.data.pubTopic, this.data.pubMsg);
        return;
      }
      else {
        wx.showToast({
          title: "请先点击连接",
          icon: "error",
        });
      }
    };
  },
  
  // 控制机器人运动的圆轨

});
