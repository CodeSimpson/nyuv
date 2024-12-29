# nyuv

## 简介
nyuv是一个用于看raw图的软件。
已支持的功能见Release CHANGELOG栏目。

## 目前支持的格式
- RGB16
- Bayer16
- MIPI10/12/14
- YUV

## 运行环境要求
- cmake >= 3.10.2
- gcc 和 g++ >= 7.5
- qt5

```
sudo apt install make cmake gcc build-essential qt5-default
```

## 代码结构
- common: 存放公共文件
- cvts: 存放各种格式转换的方法
- libs: 存放项目生成的共享库
- interface: 上层和底层的接口
- gui: ui
- test: 测试文件

## build
```
mkdir ~/bin
mkdir ~/bin/nyuv
git clone git@git.n.xiaomi.com:caibingcheng/nyuv.git ~/bin/nyuv
cd ~/bin/nyuv

cd ./build
cmake ..
make -j2        #会在~/bin/nyuv/下生成release目录
```

## install

### 用我们打包好的deb
在这里可以下载我们发布的应用 [release](https://git.n.xiaomi.com/caibingcheng/nyuv/-/releases)  
下载之后点击安装就可以了, 也可以使用dpkg安装.  
`sudo dpkg -i nyuv*.deb`

### 自己编译
```
#会将可执行文件安装到/usr/bin/nyuv，动态库安装到/usr/lib/
#如果需要安装应用，则cmake需要带上-DINSTALL=ON选项，否则应用将启动失败
cd ./build; cmake -DINSTALL=ON ..; sudo make install -j8; sudo chmod a+x /usr/bin/nyuv 

#调用
nyuv
#如果遇到权限问题，则
sudo chmod a+x /usr/bin/nyuv
```
以上，如果遇到权限问题则需要加sudo权限

## 参与贡献
这一部分会介绍, 如何参与贡献, 使得nyuv可以支持更多的格式, 更多的贡献部分将在以后整理.  
关注cvts部分, cvts中的每一个目录下都代表着支持的一种或者一类格式, 比如Bayer16, 则代表支持16bits的bayer格式, 有效位数可以是10/12/14/16bits.  
如果添加新的格式支持, 则需要:  
- 在cvts/下面添加对应格式的目录
- 并且在`cvts/CMakeLists.txt`中, 添加对应的路径
- 在`cvts/liblist.xml`"描述"其功能
接下来可以专心编写对应格式的代码了.  

你实现的格式的代码, 需要继承自`common/base/cvtbase.h`, 所以有些通用方法你是必须实现的.  
会给你传入什么参数呢? 参数的定义在`common/base/defines.h`, 当然, 最好的方式是参考cvts下的其他案例.  

如果你的算法是保密/私有的, 则直接编译成动态库, 并将这个库复制到系统的对应路径中, 再替换liblist.xml, 如此, nyuv就能使用你的库了.

## 发布(deb包)
```
cd ./build
cmake -DINSTALL=ON ..
make -j8
sudo make -j8 package
```

## 配置文件 (.nyuv.conf)
- 配置说明，[看这里](./gui/autocomplete/README.md)
- 配置文件的作用是用来保存用户的一些配置。在当前版本中，配置文件用来存放图像的format数据，可以在输入format信息时，自动补全。
- 如果没有安装（make install）则会在项目根目录的release或者debug的bin路径下生成一个配置文件。
- 如果执行了make install，配置文件则会保存在/etc/nyuv/

## 为什么引入docker？
- 用于发布ubuntu16/18可用的deb包，并且不污染本机环境。

- Dockerfile 包含了Ubuntu16 和 Ubuntu18的镜像及其编译环境
```
＃构建虚拟镜像
docker build -t nyuv_u16 --target U16 .
docker build -t nyuv_u18 --target U18 .

＃挂载本地目录到容器，并启动
docker run -it --name u16 -v /home/xx/bin/nyuv/:/nyuv/ nyuv_u16
docker run -it --name u18 -v /home/xx/bin/nyuv/:/nyuv/ nyuv_u18

# 次后，再次编译只需要运行一下容器，即可编译对应版本
docker start -i u16
docker start -i u18
```

- snap安装包还在调试中，进度见snap分支，未来的目的是只发布snap安装包。