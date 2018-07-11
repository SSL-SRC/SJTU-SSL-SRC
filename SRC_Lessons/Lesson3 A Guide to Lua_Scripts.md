# Lesson3 A Guide to Lua_Scripts

标签（空格分隔）： RoboCup rbk lua_scripts opponent play 

Z.Y.L.
---
##**0.学习目标**

 1. 简单了解lua语言
 2. rbk、zeus中的lua战术脚本结构与分类
 2. 常用语句与函数
 3. Lua与C++的接口和通信
##**1.简单了解Lua语言**
Lua 是一个小巧的脚本语言，其设计目的是为了嵌入应用程序中，从而为应用程序提供灵活的扩展和定制功能。Lua由标准C编写而成，几乎在所有操作系统和平台上都可以编译，运行。Lua并没有提供强大的库，这是由它的定位决定的。所以Lua不适合作为开发独立应用程序的语言。Lua有一个同时进行的JIT项目，提供在特定平台上的即时编译功能。Lua脚本可以很容易的被C/C++代码调用，也可以反过来调用C/C++的函数，这使得Lua在应用程序中可以被广泛应用。不仅仅作为扩展脚本，也可以作为普通的配置文件，代替XML,ini等文件格式，并且更容易理解和维护。Lua由标准C编写而成，代码简洁优美，几乎在所有操作系统和平台上都可以编译，运行。 一个完整的Lua解释器不过200k，在目前所有脚本引擎中，Lua的速度是最快的。这一切都决定了Lua是作为嵌入式脚本的最佳选择。

参考教程：[Lua教程][1]
##**2. rbk、zeus中的lua战术脚本结构与分类**

比赛中最常改动的lua脚本位于目录：rbk\robokit_ssl2017\bin\lua_scripts\ssl之下
|FolderName|Description|
|:---:|:---:|
|opponent|针对世界强队和特殊情况的战术脚本|
|play|定位球（Ref）和普通（Nor）情况下的进攻（Kick）和防守（Def）的战术脚本|
|skill|具体动作的战术脚本|
|utils|工具类脚本|
|worldmodel|物理世界的反馈判断（Judge）|
|ZeusLua|未知|
|Config.lua|整场比赛的配置文件|
|...|...|
###2.1 opponent：
CMDragon、immortal、KIKS、MRL、RoboDragon、Skuba、backup、other、delay
```
----------------------------This is a demo:-------------------------------
gOppoConfig = {
---------------------------------play-------------------------------------
CornerKick = {40},
FrontKick = function()
    if ... then 
        return {13}
    else 
        return {2}
    end,
MiddleKick = {15,20},
BackKick = {"Ref_ImmortalKickV1"},
CornerDef = "Ref_CornerDefV4",
...
}
```
###1.2 play：
play脚本分为3类：Nor，Ref，Test，分别对应gNormalPlay =，gRefPlayTable =，gTestPlayTable =
|paramName|Description|
|:---:|:---:|
|p、pos|点位|
|d、dir|朝向|
|a、acc|加速度|
|f、flag|标志（见flag.lua）|


----------
**NormalPlay**
|PlayName|Description|
|:---:|:---:|
|Nor_MorePass|unknown|
|Nor_PassAndShoot|unknown|
|NormalPlayDefend|unknown
|NormalPlayMark|unknown|
|NormalPlayNew|unknown|
|NormalPlayOneState|unknown|


----------
**RefPlay(核心抢分点）**
|PlayName|Description|
|:---:|:---:|
|BackDef|后场球防守战术|
|BackKick|后场球进攻战术|
|CornerDef|角球防守战术|
|CornerKick|角球进攻战术|
|FrontDef|前场球防守战术|
|FrontKick|前场球进攻战术|
|MiddleDef|中场球防守战术|
|MiddleKick|中场球进攻战术|
|||
|PenaltyDef|点球防守战术|
|PenaltyKick|点球进攻战术|
|GameStop|unknown|
|KickOff|开球进攻战术|
|KickOffDef|开球防守战术|

**BackKick后场定位球一览**

Ref_ImmortalKickV1	学习2011年Immortal的战术
Ref_BackKickV1		使用两传一射进行的后场定位球战术
Ref4_FrontKickV1 	用于场上只有4个车时简单的一传一射

**CornerKick角球定位球一览**

Ref_CornerKickV1    往中场挑传，后卫插上进行射门
Ref_CornerKickV2    2012年3:0胜SKUBA的进球
Ref_CornerKickV3    四个车上前，智能选取Breaker
Ref_CornerKickV4    轨迹学习使用挡拆进入禁区
Ref_CornerKickV5    静态的两车分开，后车进入禁区射门
Ref_CornerKickV6    守门员拉位，三车中场上冲进入禁区，智能选取Breaker
Ref_CornerKickV7    动态的两车分开，后车进入禁区射门
Ref_CornerKickV8	2012练习赛MRL使用的战术，后卫上冲
Ref_CornerKickV9	学习SKUBA两个车不停在禁区前扯动，后面插上的战术
Ref4_CornerKickV1	用于场上只有4个车时简单的一传一射

**FrontKick前场定位球一览**

Ref_FrontKickV1		2012年决赛进SKUBA的战术
Ref_FrontKickV2 	单边拉人，智能判断有没有被盯住
Ref_FrontKickV3 	2012年半决赛进KICKS的战术
Ref_FrontKickV4		两车靠前拉位，使用两传一射，当球靠近中场时使用
Ref4_FrontKickV1	用于场上只有4个车时简单的一传一射

**MiddleKick中场定位球一览**

Ref_MiddleKickV1	学SKUBA的后卫上前简单的一传一射
Ref_MiddleKickV2	一个车假装开球，后面车冲上射门
Ref_MiddleKickV3	两个车绕球转，可传可射
Ref4_FrontKickV1	用于场上只有4个车时简单的一传一射


----------
17年某作者的文档

CornerKickV1 打skuba,一辆车冲侧面 （OK）
CornerKickV2 待定(看不懂)
CornerKickV3 两腿张开，跑位点要改，几个车跑位点贴太近，容易把一堆都搞得有车（不对，要改）
CornerKickV4 跑位线路要优化（可以）
CornerKickV5 三个车一起冲。太过集中。V5据说打自己的防守比较好。（可以）
CornerKickV6 前场交叉换位，后卫冲上。备用。比较稳定。（传球力度不太稳定）
CornerKickV7 把防守队员闪开。一个车后撤，另一个车晃动（不使用）
出现一次所有车后撤的bug。
CornerKickV8 右侧两车瞎晃，中间车冲上去打门**********（OK 挑平都可以，人为指定）
CornerKickV9 和两腿叉开的脚本比较像。但是不用插入的车用挡拆的车
确认射门补偿文件
确认所有脚本中，门将上前的标志位 
开球车一直是蓝车四号，比较稳
确认脚本是否少一个车可用
CornerKickV10	三车挡拆 ****************（目前不可以Touch平传）
CornerKickV11	转身同边，向后平传。向外侧转能够避免被别人防。用4号车测得，力度稍微偏大，机器人可能接不到。(目前OK)
CornerKickV12	模拟KIKS的四车冲禁区。规则改变后没有多大用，可以用来骗乌龙。（不建议用。）
CornerKickV13	同边向后挑。冲挑。有问题。不建议用。（和10、11有点像）
CornerKickV14	打档板
CornerKickV15	一起来所有车都不动
CornerKickV16	三车拉同边。同边一台车和侧面一车交叉换位（打KIKS使用）
CornerKickV17	三车拉同边，一辆车从另一边晃几下打门（和16一样，不过拉得更靠边）
CornerKickV18	门将出来射门（不用）
CornerKickV19	阵型试探，一脚打门，没什么用了（不用）
CornerKickV20	车会冲禁区，有bug（不用）
CornerKickV21	试探，换位射门，没什么用（不用）
CornerKickV22	交叉换位，挑中间射门
CornerKickV23   模拟Immortal,用来打KIKS
CornerKickV30	KIKS
CornerKickV31	打ER-Force
CornerKickV40 	Skuba打强版
恢复一下两车拉边冲的定位球
球的y值靠中间的定位球，要修改。

FrontKickV1	平传(不用)
FrontKickV2	平传(不用)
goAndTurnKick有点问题
FrontKickV3	  打日本两个队非常好进球(跑位不对)	
FrontKickV4	  架炮
FrontKickV5	  挑对角，非常稳定(OK)
FrontKickV6	  挡拆后挑对角。点的位置需要修改。(挑的太近，调位置)
FrontKickV7	  倒钩(不用)
FrontKickV8	  直接往前挑(不用)
FrontKickV9	  吸球转身踢。待调整，不稳定。拖时间用。(吸不住)
FrontKickV10  三车扎堆传侧边(需调传球速度)
FrontKickV11  直接挑门(不用)
FrontKickV12  往前挑，后面车上前接球射门(不用)
FrontKickV40  靠中间位置的两传一射

MiddleKickV1	后面插上打门。把开球车带走，打门。	打自己没问题，自己的车慢一点				
MiddleKickV2	挑球对角(力度过小，需要修改)
MiddleKickV3	平传，容易doubleTouch(不用)
MiddleKickV4	架炮
MiddleKickV5	immortalKick
MiddleKickV6	往前调，截球
MiddleKickV7    两传一射(改冲挑)

immortalKickV1
backKickV1	挑对角(传球力度过小,不用)
backKickV2	直接打门
backKickV3	immortalKick的一种(没原来的好)
backKickV4  跟1差不多(不用)
backKickV5  直接射门(不用)
backKick里要加个两传一射

KickOffV1	往前小挑一脚，挑的力度过小
KickOffV2	扭屁股开球

**常用语句**

```lua
local Pos1  = CGeoPoint:new_local(200,250) --标定一个点Pos1的位置为（200,250）

if player.toTargetDist("Kicker") < 20 then......

Kicker  = task.goCmuRush(TargetPos1, 0),...

if (bufcnt(not player.isMarked("Special"), 45, 120)) then...

Special  = {SmartGoto{pos = TOW_POS1}},

```


----------
**TestPlay（骚操作聚集地）**

TestRun 最简单的跑两个点
TestNormal 不知道
TestInterKick 不知道
TestReceivePass 两个车相互接球踢球
TestKickWithoutBall 没有球也踢
TestChase 不知道
TestChaseNew 不知道
TestTwoTouch 不知道
Cha_Goto 不知道
TestSkillPlay 用来测试skill的脚本
TestPathPlanning 用来测试避障的脚本
TestOnePassShoot 用来测试一传一射还能测球速的脚本
TestCompensate 不知道
RunHeartShape 跑心形线
RecordChipData 记录挑球的力度和距离关系
TestGoAndTurnKick 转身射门
TestPassRecv 两车互传
VisionRecord 不知道
RecordCircleBall 不知道
TestImmortalKick Immortalkick
TestAssPass 据说是用车的边接球
TestRoundPass 不知道
TestAvoidShootLine 不知道
TestWaitTouch 不知道
TestChipPass 不知道
TestCirleAndKick 不知道
TestTwoCtrMethod 不知道
TestPassAndRush 不知道
TestChipPassShoot 不知道
CompensateRecord 不知道
TestShortPassAndShoot 不知道
TestHelpDefenceV1 不知道
TestHelpDefenceV2 不知道
IranOpenChallenge 伊朗公开赛挑战赛
IranOpenChallenge_yellow 伊朗公开赛挑战赛
TestForAngle 不知道
TestDribbleTurnKick 不知道
TestTurnPassAndShoot 不知道
OpenSpeedTest 不知道
TestOnePassShootV1 不知道
TestDefence 不知道


**Task.lua常为机器人的一些动作函数**
|TaskName|Description|
|:---:|:---:|
|goCmuRush(pos, dir, acc, flag)|走到pos点位|
|goPassPos("Role")|走到Role要传球的位置|
|staticGetBall("Role")|Role接球静止|
|slowGetBall(pos or nil)|Role缓慢接球|
|pass("Role")|传球给Role|
|receivePass("Role")|从Role接球并传球给Role|
|stop()|停下死掉|
|shoot()|射门|
|goAndTurnKickQuick(role,power,ktype={flat,chip})|接球转身快速射门|
|advance()|无脑进攻|

**Player.lua & ball.lua 常为场上的一些判断函数**
|PlayerJudgeName|Description|
|:---:|:---:|
|toTargetDist("Role")|Role到点位的距离|
|toBallDist("Role")|Role到球的距离|
|isMarked("Role")|Role被敌方盯防|
|iskickBall("Role")|Role是否踢球了|
|||
|**BallJudgeName**|**Description**|
|isBallPassed("Role1","Role2")|是否传球了|
|toPlayerDist("Role")|Ball到Role的距离|

  [1]: http://www.runoob.com/lua/lua-tutorial.html