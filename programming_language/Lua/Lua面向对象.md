## Lua面向对象

### 类

Lua并没有提供class关键字，Lua只能模拟类。Lua模拟类参考了基于原型的语言的一些做法，在基于原型的语言中，对象不属于类。相反，每个对象可以有一个原型。原型也是一种普通的对象，当对象遇到一个未知操作时会首先在原型中查找。要在这种语言中表示一个类，我们只需要创建一个专门被用作其他对象的原型对象即可。类和原型都是一种组织多个对象间共享行为的方式。

在Lua语言中，要让对象B成为对象A的一个原型，只需要：

```lua
setmetatable(A,{__index = B})
```

在此之后，A就会在B中查找所有它没有的操作。如果把B看做对象A的类，则只不过是术语的一个变化。

### 继承

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

值得注意的是，使用这种方法实现继承，如果把子类的属性设置为`nil`，仍然会访问到父类的属性。

```lua
c.a = nil
print(c.a) -- 仍然会打印出a
```

因为按照查找规则，在c中a为`nil`时，就会向上查找。这里是需要注意的地方。但一般继承体系中不会出现这种需求，但如果出现了，需要注意一下。

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

要实现多重继承，其实就是一个对象中有多个原型，当遇到一个未知的操作时，会在多个原型中进行查找。

```lua
local function search(k, plist)
    for i = 1, #plist do
        local v = plist[i][k] -- 尝试i个超类
        if v then
            return v
        end
    end
end

function createClass(...)
    local c = {} -- 新类
    local parents = {...} -- 父类列表

    -- 在父类列表中查找类缺失的方法
    setmetatable(c,{__index = function (t, k)
        return search(k, parents)
    end})

    -- 将c作为其实例的元表
    c.__index = c

    -- 为新类定义一个新的构造函数
    function c:new(o)
        o = o or {}
        setmetatable(o,c)
        return o
    end

    return c
end

A = {a = 10}
B = {b = 20}
AB = createClass(A,B)
ab = AB:new()
print(ab.a) -- 对a的访问经过了两次查找 c c.__index
print(ab.b)
```

