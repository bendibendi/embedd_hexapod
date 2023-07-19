/**
 * canvasId: 画布ID, 默认'canvas'
 * lockType: 1--setting, 2--unlock
 * width: 图案区域宽度，px
 * successColor: 图案设置或解锁成功时,划过的圆周线颜色
 * errorColor: 图案设置或解锁失败时,划过的圆周线颜色
 * cleColor: 9个圆点的圆周线颜色
 * cleCenterColor: 9个原点的中心小点颜色，及连接线颜色
 * storageKey: 保存密码的key
 * onTouchEnd(e): 触摸结束回调
 * onSuccess(e): 密码设置成功、解锁成功回调
 */
var Locker = class {
  constructor(page, opt,n) {
    var obj = opt || {};

    this.page = page;
    this.width = obj.width || 300;
    this.height = this.width;
    this.canvasId = obj.id || 'canvas';
    this.successColor = obj.successColor || '#00c203';
    this.errorColor = obj.errorColor || '#e64340';
    this.cleColor = obj.cleColor || '#3296fa';
    this.cleCenterColor = obj.cleCenterColor || '#3296fa';
    this.lockType = obj.lockType || 1;  //1->setting, 2->unlock
    this.lockStep = 1;
    this.lockMsg = "请规划路线";
    this.storageKey = obj.storageKey || 'locker_password';
    this.onTouchEnd = obj.onTouchEnd ? obj.onTouchEnd : function (e) { };
    this.onSuccess = obj.onSuccess ? obj.onSuccess : function (e) { };
    this.n=n||5
    this.init();
    this.endFlag=false;
  }
  init() {
    // 创建 canvas 绘图上下文（指定 canvasId）
    this.ctx = wx.createCanvasContext(this.canvasId, this);
    this.touchFlag = false;
    this.lastPoint = [];
    this.navValue=[];

    //
    this.createCircles();
    // canvas绑定事件
    this.bindEvent();
    //
    this.page.setData({ lockMsg: this.lockMsg });
    //
    if (this.lockType == 2) {  //读取保存的密码
      this.savedPwd = wx.getStorageSync(this.storageKey) || '';
    }
  }
  changeLockType(lockType = 1) {
    this.lockType = lockType;
    if (this.lockType == 2) {  //读取保存的密码
      this.savedPwd = wx.getStorageSync(this.storageKey) || '';
    }
    this.reset();
  }
  resetN(n){
    this.n=n,
    this.reset()
  }
  // 画圆方法
  drawCircle(x, y, color) {
    color = color || this.cleColor;
    // 设置边框颜色。
    this.ctx.setStrokeStyle(color);
    // 设置线条的宽度。
    this.ctx.setLineWidth(5);
    // 开始创建一个路径，需要调用fill或者stroke才会使用路径进行填充或描边。
    this.ctx.beginPath();
    this.ctx.arc(x, y, this.r, 0, Math.PI * 2, true);
    this.ctx.closePath();
    // 画出当前路径的边框。默认颜色色为黑色。
    this.ctx.stroke();
    // 将之前在绘图上下文中的描述（路径、变形、样式）画到 canvas 中。
    this.ctx.draw(true);
  }

  // 创建解锁点的坐标，根据canvas的大小（默认300px）来平均分配半径
  createCircles() {
    var n = this.n, count = 0;
    // 计算圆半径
    this.r = this.width / ((2 * n + 1) * 2);
    this.arr = [];
    this.restPoint = [];
    var r = this.r;
    var centerX=0;
    var centerY=0;
    // 获取圆心坐标，以及当前圆所代表的数
    for (var i = 0; i < n; i++) {
      for (var j = 0; j < n; j++) {        
        count++;
        var obj = {
          x: j * 4 * r + 3 * r,
          y: i * 4 * r + 3 * r,
          index: count
        };
        if (count == n * n / 2 + 0.5) {
          centerX = obj.x,
          centerY = obj.y
        }
        this.arr.push(obj);
        this.restPoint.push(obj);
      }
    }
    // 清空画布
    this.ctx.clearRect(0, 0, this.width, this.height);
    // 绘制所有的圆
    this.arr.forEach(current => { this.drawCircle(current.x, current.y); });
    this.drawCircle(centerX,centerY,this.successColor)
  }

  // 获取touch点相对于canvas的坐标
  getPosition(e) {
    return {
      x: e.touches[0].x,
      y: e.touches[0].y
    };
  }

  // 绘制触摸点
  drawPoint(x, y, color) {
    color = color || this.cleCenterColor;
    this.ctx.setFillStyle(color); // 注意用set方法
    this.ctx.beginPath();
    this.ctx.arc(x, y, this.r / 2, 0, Math.PI * 2, true);
    this.ctx.closePath();
    this.ctx.fill();
    this.ctx.draw(true);
  }
  drawStatusPoint(color) { // 初始化状态线条
    for (var i = 0; i < this.lastPoint.length; i++) {
      this.drawCircle(this.lastPoint[i].x, this.lastPoint[i].y, color);
    }
  }
  drawLine(po1, po2) {
    this.ctx.beginPath();
    this.ctx.lineWidth = 2;
    this.ctx.moveTo(po1.x, po1.y);
    this.ctx.lineTo(po2.x, po2.y);
    this.ctx.stroke();
    this.ctx.closePath();
    this.ctx.draw(true);
  }
  // 如果po在剩余可用点的圆内，则画触摸点和轨迹线
  drawTrace(po) {
    var n=this.n;
    for (var i = 0; i < this.restPoint.length; i++) {
      var pt = this.restPoint[i];
      var lt = this.lastPoint[this.lastPoint.length - 1]
      // // 列最左
      // if (lt.index % n == 1) {
      //   // 行最上
      //   if (lt.index/n<=1){
      //     // console.log("1,1")
      //     if(pt.index==lt.index+1||pt.index==lt.index+n){
      //       ;
      //     }
      //     else{
      //       continue;
      //     }
      //   }
      //   // 行最下
      //   else if(lt.index/n>n-1){
      //     // console.log("1,N")
      //     if(pt.index==lt.index+1||pt.index==lt.index-n){
      //       ;
      //     }
      //     else{
      //       continue;
      //     }
      //   }
      // }
      // // 列最右
      // else if (lt.index % n == 0) {
      //   // 行最上
      //   if (lt.index / n <= 1) {
      //     // console.log("n,1")
      //     if(pt.index==lt.index-1||pt.index==lt.index+n){
      //       ;
      //     }
      //     else{
      //       continue;
      //     }
      //   }
      //   // 行最下
      //   else if (lt.index / n > n-1) {
      //     // console.log("n,N")
      //     if(pt.index==lt.index-1||pt.index==lt.index-n){
      //       ;
      //     }
      //     else{
      //       continue;
      //     }
      //   }
      // }
      // else{
      //   // console.log("middle")
      //   if(pt.index==lt.index+1||pt.index==lt.index-1||pt.index==lt.index-n||pt.index==lt.index+n){
      //     ;
      //   }
      //   else{
      //     continue;
      //   }
      // }
      if(this.lastPoint.length==11){
        continue;
      }
      if (Math.abs(po.x - pt.x) < this.r && Math.abs(po.y - pt.y) < this.r) {
        this.drawPoint(pt.x, pt.y);
        // console.log(pt.index)
        this.drawLine(this.lastPoint[this.lastPoint.length - 1], pt);
        this.lastPoint.push(pt);
        this.restPoint.splice(i, 1);
        var x=pt.index%(n);
        x=x==0?n:x;
        // console.log(x)
        var y=parseInt((pt.index-1)/n+1)
        var cx=parseInt(n/2+1)
        var cy=parseInt(n/2+1)
        var dx=x-cx
        var dy=cy-y
        // console.log("cx "+dx+" cy "+dy)
        this.navValue.push([dx,dy])
        console.log(this.navValue)
        // if(pt.index==lt.index+1||pt.index==lt.index-1||pt.index==lt.index-n||pt.index==lt.index+n){
        //   if(pt.index==lt.index+1){
        //     this.navValue.push('(0,1)')
        //   }
        //   else if(pt.index==lt.index-1){
        //     this.navValue.push('(0,-1)')
        //   }
        //   else if(pt.index==lt.index-n){
        //     this.navValue.push('(1,0)')
        //   }
        //   else if(pt.index==lt.index+n){
        //     this.navValue.push('(-1,0)')
        //   }
        // }
        break;
      }
    }
  }

  reset() {
    this.lockStep = 1, this.lastPoint = [], this.lockMsg = "请绘制图案";
    this.navValue=[];
    // 绘制圆
    this.createCircles();
    //
    this.page.setData({ lockMsg: this.lockMsg });
  }

  getLockPwd() {
    var str = "";
    for (var i = 0; i < this.lastPoint.length; i++) {
      str += this.lastPoint[i].index + "";
    }
    return str;
  }
  bindEvent() {
    var self = this;
    this.page.onTouchStart = function (e) {
      if (self.lastPoint.length > 0) return;   // 上次绘制没有清理干净
      var po = self.getPosition(e);
      // 坐标是否起始点？
      var i = self.arr.length / 2 - 0.5;
      // 坐标是否在圆内？ 
      if (Math.abs(po.x - self.arr[i].x) < self.r && Math.abs(po.y - self.arr[i].y) < self.r) {
        self.touchFlag = true;
        self.drawPoint(self.arr[i].x, self.arr[i].y);
        self.lastPoint.push(self.arr[i]);
        self.restPoint.splice(i, 1);
      }
    }

    this.page.onTouchMove = function (e) {
      if (self.touchFlag) {
        self.drawTrace(self.getPosition(e));
      }
    }

    this.page.onTouchEnd = function (e) {
      this.touchFlag = false;
      if (self.touchFlag) {
        // self.touchFlag = false;
        // e.lockType = self.lockType;
        // e.lockStep = self.lockStep;
        e.lockPwd = self.getLockPwd();
        // e.navValue=this.navValue
        // console.log("e")
        // console.log(e)
        // console.log(self.navValue)

        self.onTouchEnd(e,self.navValue);

        // 处理连接后的事件
        // console.log("pwd")
        // console.log(e.lockPwd)
        // if (self.lockType == "1") {  // setting pwd
          // if (self.lockStep == 1) {
          //   if (e.lockPwd.length < 4) {
          //     self.lockMsg = "至少绘制4个及以上点";
          //   } else {
          //     self.lockMsg = "请再次绘制图案";
          //     self.lockPwd = e.lockPwd, self.lockStep++;
          //   }
          // } else {
          //   if (e.lockPwd == self.lockPwd) {
          //     self.lockMsg = "图案绘制成功";
          //     self.drawStatusPoint(self.successColor);
              // wx.setStorageSync(self.storageKey, e.lockPwd);
          //     success = true;
          //   } else {
          //     self.drawStatusPoint(self.errorColor);
          //     self.lockMsg = "两次不一致，请重新绘制";
          //   }
          // }
        // } else {  // unlock pwd
        //   if (e.lockPwd == self.savedPwd) {
        //     self.lockMsg = "图案解锁成功";
        //     self.drawStatusPoint(self.successColor);
        //     success = true;
        //   } else {
        //     self.drawStatusPoint(self.errorColor);
        //     self.lockMsg = "图案错误, 请重新绘制";
        //   }
        // }

        self.drawStatusPoint(self.successColor);
        // self.page.setData({ lockMsg: self.lockMsg });
        // setTimeout(function () {
        //   self.lastPoint = [], self.createCircles();
        //   if (success) {
        //     self.onSuccess(e);
        //   }
        // }, 1000);
      }
    }
  }
}
module.exports = Locker;