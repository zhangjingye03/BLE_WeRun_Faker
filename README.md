# BLE_WeRun_Faker
这是一个利用esp32低功耗蓝牙欺骗微信运动步数的项目。

插上电源，按下按键，挑一个合适的步数，打开蓝牙，打开公众号，等待同步，然后排行榜上你就占领封面啦。

特别适用于要求微信步数与体育运动分数挂钩的某些奇葩学校~~（广外）~~

其实类似的方案有很多，可以去淘宝代刷，可以用安卓+(Virtual)Xposed，甚至可以把手机绑在招财猫上面，然而都挺麻烦的，而且不安全、不可控。

本项目旨在做一个可控的、安全的、简便的步数修改器。

[视频DEMO](https://github.com/zhangjingye03/BLE_WeRun_Faker/raw/master/demo.mp4)

## 硬件准备
* `esp32` x 1

这个我想啰嗦几句，淘宝上没见到啥廉价的支持GATT的开发板，之前买了个cc2540和nrf51822，前者因为自带固件太屎搞不成，后者针脚太细，而且两者都因为缺少编程器而无法自己写固件，只好选昂贵一些的esp32，只能大材小用了。。

* `微动开关/按钮` x 1
* `10k电阻` x 1
* `3461AS` x 1
* `7x9 洞洞板` x 1

## 电路图连接
![a](https://github.com/zhangjingye03/BLE_WeRun_Faker/raw/master/circuit.png)

## 原理
参考源码`main.cpp`、`ble.cpp`，BLE的协议文档，以及[微信硬件平台-蓝牙精简协议](http://iot.weixin.qq.com/wiki/new/index.html?page=4-3)
## 绑定微信
创建[微信测试号](https://mp.weixin.qq.com/debug/cgi-bin/sandbox?t=sandbox/login)，打开设备功能，新建具有蓝牙连接能力的设备，然后参考[微信硬件平台-设备授权新接口](http://iot.weixin.qq.com/wiki/new/index.html?page=3-4-6)

## License
MIT