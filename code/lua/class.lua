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

B = A:new()
B.b = "b"

C = B:new()
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

--Lua中的面向对象  
--[[  
  复制表方式面向对象  
  参数为一张表，通过遍历这张表取值，赋给一张空表，最后返回新建的表，来达到克隆表  
]]  
function cloneTab(tab)  
    local ins = {}  
    for key, var in pairs(tab) do  
        ins[key] = var  
    end  
    return ins  
end  

Object = {class_id = 1}

function Object.new()
　　local o = cloneTab(Object)
　　return o
end


-- 使用这个类
local p = Object.new()

