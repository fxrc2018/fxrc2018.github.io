## Lua基础

本文使用的Lua版本5.1.4。

### 基本语法

#### 类型与值

Lua语言是一种动态类型语言，在这种语言中没有类型定义，每个值都带有其自身的类型信息。

Lua语言中一共有8种基本类型：

- nil
空，只有一个nil值的类型，它的主要作用就是与其他所有值进行区分。
- boolean
布尔值，只有两个值，true和false。在Lua中，只有false和nil表示假，其他情况都为真，数字0也为真。
- number
实数，不论浮点数还是整型数都是number类型。
- string
字符串。
- userdata
用户数据。
- function
函数。
- thread
线程。
- table
表。

可以使用type函数获得一个值对应的类型名称。

#### 表达式

- 算术运算符
算术运算符有+（加）、-（减）、*（乘）、/（除）、^（指数）、%（取模）、-（负号）。
- 关系运算符
关系运算符有<（小于）、>（大于）、<=（小于等于）、>=（大于等于）、~=（不等于）、==（等于）。
- 逻辑操作符
逻辑操作符有and（与）、or（或）和not（非）。
    - 对于and，当第一个操作数为假时，返回第一个操作数，否则返回第二个操作数。
    - 对于or，当一个操作数为真时，返回第一个操作数，否则返回第二个操作数。
- 字符串连接符
字符串连接操作符“..”用来连接两个字符串，当后一个为其他类型时会转化为字符串。
- table构造式
构造式用来创建和初始化table的表达式，是Lua中特有的一种表达式。
```lua
t = {} --空表
t = {
    1, --未设置key的项，key会自动冲1开始编号 t[1] = 1
    x = 1, --t[x] = 1
    y = 2, --t[]
    2 --t[2] = 2
}
```

#### 语句

- 赋值语句
Lua中的赋值，直接使用赋值操作符号。当有多个参数赋值时，可以同时给多个参数赋值。
```lua
a = 10
a, b = 10, 20
a, b = b, a --交换两个变量的值
```

#### 函数


### 面向对象编程

#### 元表

Lua可以有两种方式可以实现继承——设置元表和复制。

#### 通过设置元表实现继承

```lua
A = {}
A.a = "a"
function A:new()
    local res = {}
    self.__index = self
    -- 默认的设置函数大概是这样
    -- self.__newindex = function(t,k,v)
    --     rawset(t,k,v)
    -- end
    setmetatable(res,self)
    return res
end

B = A:new() --B继承自A
B.b = "b"

C = B:new() --C继承自B
C.c = "c"

c = C:new()
--当访问c.a时，c中没有，会去元表中查找，因为元表有__index，所以返回__index返回的值
--因为__index是一个元表而不是一个函数，并且该元表也有元表，因此形成了递归查找，
--所以能打印出A类的a变量
print(c.a)

c1 = C:new()
--当对c赋值时，会在表中插入一个新的值，这样就实现了每个类的属性都是不同的
--但查找时，如果没有，会访问到类中的值
c.a = c.a .. "1" 
c1.a = c1.a .. "2"
print(c.a)
print(c1.a)

--通过元表去访问时得到的值还是原来的值
print(c.__index.a) 
print(c1.__index.a)
```

#### 通过复制实现继承

```lua
A = {}
A.val = 1

function A:new(o)
    o = o or {}
    for k,v in pairs(self) do
        o[k] = v
    end
    return o
end

a1 = A:new()
a2 = A:new()
print(a1.val)
print(a2.val)
a1.val = 2
a2.val = 3
print(a1.val)
print(a2.val)
```

#### 多重继承

### 环境和模块

### 热更新

#### 重载一个模块

当我们加载一个模块的时候，会先判断是否在package.loaded中已存在，若存在则返回改模块，不存在才会加载(loadfile)，防止重复加载。

最简单粗暴的热更新就是将package.loaded[modelname]的值置为nil，强制重新加载：

这样做虽然能完成热更，但问题是已经引用了该模块的地方不会得到更新， 因此我们需要将引用该模块的地方的值也做对应的更新。

```lua
function reload_module(module_name)
    local old_module = _G[module_name]
    package.loaded[module_name] = nil
    require (module_name)
    local new_module = _G[module_name]
    for k, v in pairs(new_module) do
        old_module[k] = v
    end
    package.loaded[module_name] = old_module
end
```

#### 使用loadstring动态更新一个函数

### 高级特性

#### 协程

Lua语言中协程相关的所有函数都被放在表coroutine中。函数create用于创建新协程，该函数只有一个参数，即协程要执行的代码的函数。函数create返回一个“thread”类型的值，即新协程。

一个协程有以下四种状态，即挂起、运行、正常和死亡。函数status来检查协程的状态。刚创建的协程的状态为挂起。函数resume用于启动或再次启动一个协程的执行，并将其状态由挂起改为运行。函数yeild可以让一个运行中的协程挂起自己，直到协程被唤醒，然后继续执行直到遇到下一个yeild或执行结束。函数resume有返回值，第一个返回为true表示没有错误，之后的返回值对应函数yeild的参数。

```lua
co = coroutine.create( function()
    for i = 1, 3 do 
        coroutine.yield(i)
    end
end
)

print(coroutine.status(co)) --协程的状态挂起
for i = 1,5 do
    local flag,res = coroutine.resume(co)
    if flag then
        print(res) --i=4时执行到最后，没有res
    else
        --在一个已死亡的协程上调用resume会返回false，并返回一个提示字符串
        print(coroutine.status(co))
        print(res) 
    end
end
```

可以用协程来实现生产者-消费者模式。

```lua
function producer()
    while true do 
        local x = io.read()
        send(x)
    end
end

pco = coroutine.create( producer )

function consumer()
    while true do 
        local x = receive()
        io.write(x, "\n")
    end
end

function receive()
    local status, value = coroutine.resume(pco)
    return value
end

function send(x)
    coroutine.yield( x )
end

consumer()
```


