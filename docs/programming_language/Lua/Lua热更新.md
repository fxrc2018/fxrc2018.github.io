## Lua热更新


### 执行动态生成的代码

Lua是一种解释型语言，解释型语言的一大特征是有能力执行动态生成的代码。Lua热更新就依赖于这个特性。

lua提供了一些函数用于从字符串中加载内容。常用的函数有以下两个。

- `loadstring(string)`从字符串中加载一串代码。
- `dofile(filename)`加载并执行一个lua文件。

```lua
val = 10
str = io.read()
loadstring(str)() -- 相当于在这里插入了一段代码
print(val)

>lua main.lua
val = 20
20 -- 可以动态的更改val的值
```

Lua将全部变量都存储在一个表中，因此，如果我们要热更一个全局的函数或全局的变量，只需要更改这个表中的值。

```lua
-- A.lua
module(...,package.seeall)

val = 10

function f()
    print("f()")
end

-- hotfix.lua
require "A"

A.val = 20

A.f = function ()
    print("g()")
end

-- main.lua
require "A"

print(A.val)
print(A.f())

dofile("hotfix.lua")

print(A.val)
print(A.f())
```

热更新的基本原理就是执行一段额外的代码来修改原始的值，这样后续访问到的值就是修改之后的值。

### 访问非局部local变量

如果要访问或修改非局部local变量的值，需要用到Lua的debug库。

- `debug.getupval(func,up)`获取一个函数的第up个上值。
- `debug.setupval(func,up,val)`设置一个函数的第up个上值。

```lua
-- A.lua
module(...,package.seeall)

local val = 10

function printVal()
    print(val)
end

-- hotfix.lua
require "A"

function getUpValByName(func,name)
    for i =1,math.huge do
        local n,v = debug.getupvalue(func,i)
        if not n then
            return nil
        end
        if n == name then
            return v
        end
    end
end

function setUpValByName(func,name,val)
    for i =1,math.huge do
        local n,v = debug.getupvalue(func,i)
        if not n then
            return
        end
        if n == name then
            debug.setupvalue(func,i,val)
        end
    end
end

setUpValByName(A.printVal,"val",20)

-- main.lua
require "A"

A.printVal()
dofile("hotfix.lua")
A.printVal()
```

### 重新加载一个模块

如果一个函数中访问了过多的非局部的local变量，要重写这个函数比较麻烦，因为找热更文件中无法直接访问这些非局部的local变量。如果文件比较小，我们可以通过重新加载一个模块来实现热更。

当我们加载一个模块的时候，会先判断是否在package.loaded中已存在，若存在则返回改模块，不存在才会加载(loadfile)，防止重复加载。

最简单粗暴的热更新就是将package.loaded[modelname]的值置为nil，强制重新加载：

这样做虽然能完成热更，但问题是已经引用了该模块的地方不会得到更新， 因此我们需要将引用该模块的地方的值也做对应的更新。

```lua
function reload_module(module_name)
    local old_module = _G[module_name]
    package.loaded[module_name] = nil
    -- 暂时不考虑a.b的形式，如果不设置为nil，再次加载会在同一个表
    _G[module_name] = nil 
    require (module_name)
    local new_module = _G[module_name]
    for k, v in pairs(new_module) do
        old_module[k] = v
    end
    package.loaded[module_name] = old_module
    _G[module_name] = old_module
end 
```