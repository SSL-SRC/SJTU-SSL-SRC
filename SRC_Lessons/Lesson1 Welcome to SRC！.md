# Lesson1 Welcome to SRC！

标签（空格分隔）： RoboCup SRC 入门
Z.Y.L. & XiaoxingChen
---
欢迎你们加入上海交通大学RoboCup战队，即SRC！

##**0.学习目标**
**1.了解RoboCup与SSL（Small Size League）
2.了解SRC
3.掌握简单的入门知识**

---

##**1.了解RoboCup与SSL**

**RoboCup is a competition domain designed to advance robotics and AI research through a friendly competition.** RoboCup is an annual international robotics competition proposed and founded in 1996 (Pre-RoboCup) by a group of university professors (among which Hiroaki Kitano, Manuela M. Veloso, and Minoru Asada). The aim of such a competition consists of promoting robotics and AI research, by offering a publicly appealing, but formidable challenge.

**Small Size robot soccer is one of the RoboCup league divisions.** Small Size robot soccer, or F180 as it is otherwise known, focuses on the problem of intelligent multi-agent cooperation and control in a highly dynamic environment with a hybrid centralized/distributed system.

> URL：
About Robocup：https://en.wikipedia.org/wiki/RoboCup
About SmallSizeLeague：http://wiki.robocup.org/Small_Size_League
RoboCup official website：http://www.robocup.org/

**RoboCup小型组比赛视频集锦** RoboCup 2014 Small Size League 决赛 [精彩视频][1]

##**2.了解SRC**

SRC（Seer Robotics Club）是上海交通大学RoboCup机器人战队的名称。

2017年机器人世界杯比赛将在日本名古屋举行。在校内及校外各界力量的推动下，上海交通大学组建起RoboCup小型足球机器人的队伍。上海交通大学自2006年起便已开始参加中型足球机器人比赛，是国内最早一批参与RoboCup的高校之一。之后的标准平台组NAO机器人也在国内国外的各大比赛中取得较好的成绩。

正是由于交大的机器人团队在多机器人调度，路径规划，移动机器人运动控制方面的深厚的技术积累和队员们的顽强拼搏和不懈奋斗，以及学校层面的大力支持，2017年7月，SRC获得[RoboCup机器人世界杯小型组冠军][2]。2018年4月，SRC获得[RoboCup China Open（机器人世界杯中国公开赛）Small Size League 冠军][3]。

我们的微信公众号：上交SRC

> URL：
仙知机器人公司主页：http://www.seer-robotics.com/
> SRC网站：https://src-ssl.github.io/

**关于专业**：
自动化、计算机、电子信息、机械类等机器人相关专业优先，但SRC不看重队员的专业，只要大家对机器人有很大兴趣就行。即使是学医，学法律的同学也可以参加战队。
**基本要求**：
 - 对机器人开发有着浓厚的兴趣，后期希望在机器人行业有深入的发展。
 - 有较强的学习能力，信息获取能力。
 - 有较好的团队合作能力和领导能力，能够带领团队里的软件，硬件，机械方向的成员相互合作。
 - 能够合理规划时间节点，安排项目进度。
 - 有较好的英语能力，能够快速阅读各类英文数据手册与文献。比赛时能与裁判和各国比赛选手流利地沟通交流。
**技术要求**：
 - 能熟练使用C++
 - 有移动机器人相关参赛经历者、有移动机器人软硬件项目开发经验者、有Qt框架相关经历者优先
**但是：**
SRC没有面试，没有筛选。因为参加比赛和平时调程序比较辛苦，而这个过程本身可以作为一种筛选。如果热爱，请坚持下来。如果坚持下来，收获颇丰。
**关于时间投入**：
每周一次例会，其余的需要自己学习知识和代码，每周约合2~3小时。比赛前的时间投入和强度不计。
**关于场地**：
思源楼一楼C100
**关于程序**：
不开源
**比赛计划**：
2018年12月亚太公开赛；2019年04月中国公开赛、伊朗公开赛；2019年06月 世界杯
**培养计划**：
2018年7月-8月，由企业和学校共同集训，熟悉并掌握现有的RoboCup小型组的机器人系统。后在学期中会固定展开培训。在一年的历练后，将有机会成为队伍的中坚力量，继续为交大夺取荣耀。
在这里，你将会学到：
 - 多机器人调度，路径规划，运动规划，遍历规划探索规划等移动机器人领域相关定位导航算法
 - 直流无刷电机控制技术，永磁同步电机矢量控制技术
 - 嵌入式系统软件硬件设计与开发
你还有机会接触到：
 - 基于EKF,PF进行激光，里程计，视觉等多信息融合算法开发
 - 构建机器人运动模型，观测模型等概率学模型
 - 移动机器人未知环境中的地图构建算法开发
 - SLAM问题中的信息配准融合、闭环检测等前端算法，贝叶斯滤波、图优化等后端算法
 - 人工智能、机器学习在多机器人系统中的应用
**相关福利**：
 - 比赛获得优异成绩并在团队作出主要贡献的同学，可获得推荐免试硕士、博士研究生加分资格
 - 参与比赛并获得名次的同学可在评比奖学金时获得加分且有素拓加成
 - 参与RoboCup比赛，能够享受Mathworks公司专门为参赛选手提供的Matlab Global License
 - 有机会在全国甚至全球范围内参加比赛，感受世界各地风土人情

有意向报名者，请发送简历至zhongyuliang@sjtu.edu.cn或yubaby2003@163.com
##**3.入门知识**

###**3.1我们如何控制机器人踢球？**
这里有三篇文献，是2012、2013、2014年浙江大学的RoboCup战队在比赛之后写的，对整个比赛的架构和实现方法，硬件设备和软件框架等方面都做了详细的解读。

> 获取通道：https://pan.baidu.com/s/1htxBI6o
> 备用URL：https://pan.baidu.com/s/1ggmi0XP 密码：4b19

###**3.2 Small Size League 技术介绍**

> https://src-ssl.github.io/2017/09/26/SSL-for-laymen.html

###**3.3 Small Size League 策略框架**

> https://src-ssl.github.io/2017/09/26/strategy-framework.html















  


  [1]: https://v.youku.com/v_show/id_XNzU2OTc4NDI0.html?tpa=dW5pb25faWQ9MjAwMDE0XzEwMDAwMV8wMV8wMQ&fromvsogou=1
  [2]: https://mp.weixin.qq.com/s/LtZYQoZUvl36grwewJfwnw
  [3]: https://mp.weixin.qq.com/s/YJYtckGJDfhOAmJk9UTeiQ