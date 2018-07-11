# Lesson2.5 Hello, Lua

标签（空格分隔）： RoboCup rbk lua

Organized by Z.Y.L.
---

##**0.学习目标**

 1. 了解lua脚本语言
 2. 会写一个简单的战术脚本，并学会使用decoda
 3. 了解lua文件之间的关系
 4. 了解C++与lua之间的通信

##**1.了解lua脚本语言**

Lua 是一个小巧的脚本语言，其设计目的是为了嵌入应用程序中，从而为应用程序提供灵活的扩展和定制功能。Lua由标准C编写而成，几乎在所有操作系统和平台上都可以编译，运行。Lua并没有提供强大的库，这是由它的定位决定的。所以Lua不适合作为开发独立应用程序的语言。Lua有一个同时进行的JIT项目，提供在特定平台上的即时编译功能。Lua脚本可以很容易的被C/C++代码调用，也可以反过来调用C/C++的函数，这使得Lua在应用程序中可以被广泛应用。不仅仅作为扩展脚本，也可以作为普通的配置文件，代替XML,ini等文件格式，并且更容易理解和维护。Lua由标准C编写而成，代码简洁优美，几乎在所有操作系统和平台上都可以编译，运行。 一个完整的Lua解释器不过200k，在目前所有脚本引擎中，Lua的速度是最快的。这一切都决定了Lua是作为嵌入式脚本的最佳选择。

```Lua
--注释符号: --单行注释；--[[]]多行注释；sublime快捷键Ctrl + '/'

print("hello world")

local posX = 50

b = 10 --在默认情况下，变量总是认为是全局的
b = nil --delete b

-- false = nil 
-- 0 = 1 = true = ...

local tbl2 = {"apple", "pear", "orange", "grape"} --初始化一个“表”
a = {}
a["key"] = "value"
key = 10
a[key] = 22
a[key] = a[key] + 11
for k, v in pairs(a) do
    print(k .. " : " .. v)
end

optional_function_scope function function_name( argument1, argument2, argument3..., argumentn)
    function_body
    return result_params_comma_separated
end

-- optional_function_scope: 该参数是可选的制定函数是全局函数还是局部函数，未设置该参数默认为全局函数，如果你需要设置函数为局部函数需要使用关键字 local。
-- function_name: 指定函数名称。
-- argument1, argument2, argument3..., argumentn: 函数参数，多个参数以逗号隔开，函数也可以不带参数。
-- function_body: 函数体，函数中需要执行的代码语句块。
-- result_params_comma_separated: 函数返回值，Lua语言函数可以返回多个值，每个值以逗号隔开。

--在函数参数列表中使用三点 ... 表示函数有可变的参数。
function add(...)  
local s = 0  
  for i, v in ipairs{...} do   --> {...} 表示一个由所有变长参数构成的数组  
    s = s + v  
  end  
  return s  
end  
print(add(3,4,5,6,7))  --->25

-- [[泛型for迭代器
泛型 for 在自己内部保存迭代函数，实际上它保存三个值：迭代函数、状态常量、控制变量。

泛型 for 迭代器提供了集合的 key/value 对，语法格式如下：

for k, v in pairs(t) do
    print(k, v)
end
上面代码中，k, v为变量列表；pairs(t)为表达式列表。]]

--Lua模块与包
---创建一个模块就是创建一个table
-- 文件名为 module.lua
-- 定义一个名为 module 的模块
module = {}
module.constant = "这是一个常量" -- 定义一个常量
function module.func1() -- 定义一个函数
    io.write("这是一个公有函数！\n")
end
local function func2()
    print("这是一个私有函数！")
end
function module.func3()
    func2()
end
return module
-- Lua提供了一个名为require的函数用来加载模块。要加载一个模块，只需要简单地调用就可以了。
require("<模块名>")
require "<模块名>"
--执行 require 后会返回一个由模块常量或函数组成的 table，并且还会定义一个包含该 table 的全局变量。
--
```

参考教程：[Lua教程][1]

##**2. 会写一个简单的战术脚本，并学会使用decoda**

详见教学视频！

##**3.了解lua文件之间的关系**

###3.1 rbk、zeus中的lua战术脚本结构与分类

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







  [1]: http://www.runoob.com/lua/lua-tutorial.html

