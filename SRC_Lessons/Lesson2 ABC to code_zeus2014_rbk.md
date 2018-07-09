# Lesson2 初识代码——zeus2014+、rbk

标签（空格分隔）： RoboCup SRC 代码视频教学 rbk zeus2014+
Z.Y.L.
---

##**0.学习目标**

**1.获取代码，编译并仿真成功
2.获取教学视频，观看教学视频，做笔记
3.完成习题**

---

##**1.获取代码，编译并仿真成功**

通过Lesson1，大家应该对RoboCup有了一个初步的了解。大家可以发现，SRC战队的比赛用车已经非常成熟和精巧，我们在硬件方面已经不会做出太大的改动。而且，其他国家战队的参赛用车的设计也是相仿的。队伍的强大与否，取决于代码的优劣。同时，赛场上的情况多而复杂，一份简单却相对可靠的代码将是走向胜利的必要条件。

由于历史原因，现在手头上有两套代码，分别为rbk（2017）和zeus2014+（2018），后多使用rbk

在SRC的绝大多数工作，将会围绕着这些代码展开。

###**1.1获取代码**

| 名称 | 用途简介 | 
|:----:  |:----:   | :----:  |
|**rbk部分：**||
|crazy| 内含连接实车的程序|
|Decoda|调试lua脚本的工具|
|VMediator| 内含连接图像机的程序 |
|rbk.7z|源代码|
|InstallPackage|可能需要的软件的安装包|
| ZJUNlictClient|内含仿真模拟器的客户端|
|**zeus部分：**||
|zeus2014+|zeus2014+源代码|

**【注意】一定要将源代码做备份！**

###**1.2编译代码**

现在拿到手的代码是未经过编译的，无法进行下一步的学习。本节将介绍编译代码的方法。
编译代码，需要两个软件：
1. cmake
2. visual studio 2010

编译代码的过程：

```flow
st1=>start: 开始编译rbk
op1=>operation: 打开Cmake，在Where is the source code栏用
Browse Sourse...打开rbk/robokit_ssl2017文件的目录
op2=>operation: 同理，在Where to build the binaries栏填写
rbk/robokit_ssl2017文件的目录并加上“/build” 
op3=>operation: 点击左下角Configure并选择编译器为visual studio 2010，
点击finish，等待出现error
op4=>operation: 替换两个文件 RBK_BOOST_DIR栏用rbk/boost文件代替；
RBK_THIRDPARTY_LIB_DIR栏用rbk/rbk_thirdparty_lib文件代替
op5=>operation: 点击configure，再点击generate，再点击open project，
在跳出visual studio内点击生成-重新生成解决方案，并等候
op6=>operation: 询问学长
op7=>operation: 在rbk文件夹内搜索“smallsim”并用管理员身份运行；
在rbk文件夹内搜索“test.bat”并用管理员身份运行
op8=>operation: 打开ZJUNlictClient文件夹，打开ZjuNlictClient
（仿真可视化程序，建议固定到任务栏）；
打开rbk/robokit_ssl2017/bin/leftblue.bat；在ZjuNlictClient中点击simulator
op=>operation: Your Operation
cond1=>condition: 是否生成成功？
cond2=>condition: 是否看到机器人自动射门？
sub=>subroutine: Your Subroutine
e=>end: 编译及仿真成功！

st1->op1->op2->op3->op4->op5->cond1
cond1(yes)->op7->op8->cond2
cond1(no)->op6
cond2(yes)->e
cond2(no)->op6
```

##**2.获取教学视频，观看教学视频，做笔记**

该视频是浙大小型组2015年的代码讲解视频，他们的代码是叫做zeus2015

> 视频获取通道
> http://i.youku.com/i/UMzM5ODA1NzQ3Ng==?spm=a2h0j.8191423.subscription_wrap.DD~A

建议：在观看视频的同时要记一些笔记，便于更好地理解代码

##**3.完成习题**

目标：写一个自己的lua脚本

> 习题通道
> https://src-ssl.github.io/2017/10/24/tasks.html
















