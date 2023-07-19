const app = getApp()
const Locker = require('../utils/locker.js');
import mqtt from "../../utils/mqtt.min.js";

Page({
  data: {
    lockMsg: '请规划路线',
    min:3,
    max:11,
    value:7,
    min1:10,
    max1:100,
    value1:50,
    value2:[],
  },
   /**
   * 生命周期函数--监听页面显示
   */
  onShow: function() {
    wx.hideHomeButton();  //隐藏home/返回主页按钮
  },
  onLoad: function () {
    let pwd = wx.getStorageSync('locker_pwd') || '12345';
    let res = wx.getSystemInfoSync();
    let ftHeight = res.windowHeight - 410;
    let that = this;
    this.setData({ lockType: pwd == '' ? 1 : 2, ftHeight: ftHeight });

    //
    this.lock = new Locker(this, {
      id: 'canvas',
      lockType: this.data.lockType,
      onTouchEnd: (e,value) => {
        console.log("onTouchEnd: ", e);
        console.log("enav")
        console.log(value);
        this.setData({value2:value})
      },
      onSuccess: (e) => {
        // console.log("onSuccess: ", e);
        // if (e.lockType == "1") {
        //   console.log("密码设置成功，密码为：", e.lockPwd);
        //   that.setData({ lockType: 2 });
        //   that.lock.changeLockType(2);
        // } else {
        //   console.log("密码解锁成功!");
        // }
      }
    },7);
  },
  resetN(n){
    this.lock.resetN(n)  
  },
  // 拖动过程中触发的事件
  sliderchanging(e) {
    var value = e.detail.value;
    this.setData({ value: value })
    this.resetN(value)
  },
  // 完成一次拖动后触发的事件
  sliderchange(e) {
    var value = e.detail.value;
    this.setData({ value: value })
  },

  sliderchanging1(e) {
    var value = e.detail.value;
    this.setData({ value1: value })
  },
  // 完成一次拖动后触发的事件
  sliderchange1(e) {
    var value = e.detail.value;
    this.setData({ value1: value })
  },
  send(e){    
    var sendValue=JSON.stringify(this.data.value2)
    let pages = getCurrentPages();
    let prevPage = pages[pages.length - 2];//上一个页面
    prevPage.publishDataIn("wechatCommand","nop","nop")
    prevPage.publishDataIn("wechatCommand","ruler",this.data.value1)
    prevPage.publishDataIn("wechatCommand","guihua",sendValue)
    wx.navigateBack({
      url: `../work/work?arr=${JSON.stringify(sendValue)}&obj=${JSON.stringify(this.data.value1)}`,//要跳转到的页面路径
    })  
  },
  leave(e){
    // this.publishDataIn("wechatCommand","leaveNav",true);    
    // this.publishDataIn("wechatCommand","leaveNav",true);    
    wx.navigateBack({
      url: '../work/work',//要跳转到的页面路径
    })  
  },

  // mqtt
  setValue(key, value) {
    this.setData({
      [key]: value,
    });
  },  
})