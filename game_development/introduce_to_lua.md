## Lua入门

### 面向对象

Lua的面向对象有两种面向对象的写法

```lua
A = {}
A.a = "a"
function A:new()
    local res = {}
    self.__index = self
    setmetatable(res,self)
    return res
end

B = A:new()
B.b = "b"

C = B:new()
C.c = "c"

c = C:new()
c1 = C:new()
print(c.a)
print(c.b)
print(c.c)
c.c = "d" --当对c赋值时，会在表中插入一个新的值
print(c1.c)
print(c.c)
```

### 环境和模块

