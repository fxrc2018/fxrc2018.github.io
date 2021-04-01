## Lua和C++

### 环境搭建

从[http://www.lua.org/ftp/](http://www.lua.org/ftp/)下载lua源码，本文使用的lua版本是5.1.4。下载完成之后，解压，进入到src目录，修改Makefile，指定`PLAT`，本文使用的是`mingw`，然后执行`make all`，得到`liblua.a`文件。得到之后，将src中的头文件拷贝到`include`目录中，将`liblua.a`拷贝到`lib`目录中。然后设置C++的头文件目录和库目录，以及使用到的库。

环境搭建好了之后，就可以输入下面的代码进行测试。

```cpp
#include <iostream>
using namespace std;

extern "C"{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

int main(int argc, char const *argv[])
{
    int error;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    //在这里注册c函数
    error = luaL_loadstring(L,"dofile(\"D:/Project/VSCode/LuaCpp/main.lua\")") 
        || lua_pcall(L,0,0,0);
    if(error){
        cout<<lua_tostring(L,-1)<<endl;
        lua_pop(L,1);
    }
    //在这里调用Lua函数
    lua_close(L);
    system("pause");
    return 0;
}
```

`main.lua`的代码如下。

```lua
print("hello world")
```
使用如下的命令进行编译。

```shell
g++ -g main.cpp -I include -L lib -l lua -o main.exe
```

运行`main.exe`，可以成功的打印出`hello world`。

### C++调用Lua函数

Lua严格按照后进先出的规则来操作栈。在调用Lua时，只有栈顶的部分会发生改变；而C语言代码则有更大的自由度。更具体地说，C语言可以检视栈中的任何一个元素，甚至可以在栈的任意位置插入或删除元素。

调用Lua函数的API规范很简单：首先，将待调用的函数压栈；然后，压入函数的参数；接着用`lua_pcal`进行实际的调用；最后，从栈中取出结果。

现在有一个`M.lua`模块。

```lua
module(...,package.seeall)

function add(a,b)
    return a + b
end
```

在`main.lua`中执行`require "M"`，此时，如果要在C++中调用`add`函数，可以用下面的方式进行调用。

```cpp
int callLuaFunctionAdd(lua_State *L, int x, int y){
    lua_getglobal(L,"M"); //查找M 会自动压栈
    lua_getfield(L,-1,"add"); //在M中查找add
    lua_pushinteger(L,x);
    lua_pushinteger(L,y);
    lua_pcall(L,2,1,0); //两个参数，一个结果
    int res = lua_tointeger(L,-1);
    lua_pop(L,1);
    return res;
}
```

### Lua调用C++函数

Lua调用调用C函数时，也使用了一个与C语言调用Lua函数时相同类型的栈，C函数从栈中获取参数，并将结果压入栈中。

此处的重点在于，这个栈不是一个全局结构；每个函数都有其私有的局部栈。当Lua调用一个C函数时，第一个参数总是位于这个局部栈中索引为1的位置。即使一个C函数调用了Lua代码，而且Lua代码又再次调用了同一个或其他的C函数，这些调用每一次都只会看到本次调用自己的私有栈，其中索引为1的位置上的参数就是第一个参数。

我们必须先注册C函数，然后才能在Lua用使用这个函数。所有在Lua中注册的函数都必须使用一个相同的原型，该原型如下。

```c
typedef int (*lua_CFunction)(lua_State *L)
```

返回值为一个整数，代表压入栈中返回值的个数。因此，该函数在压入结果前无须清空栈。在该函数返回之后，Lua会自动保存返回值并清空整个栈。

假设在`main.cpp`有这样一个函数。

```cpp
static int cppAdd(lua_State *L){
    int a = luaL_checkinteger(L,1);
    int b = luaL_checkinteger(L,2);
    cout<<"cppAdd"<<" "<<a<<" "<<b<<endl;
    lua_pushinteger(L,a + b);
    return 1;
}
```

然后在`main.cpp`中注册这个函数。

```cpp
lua_pushcfunction(L,cppAdd);
lua_setglobal(L,"cppAdd");
```

然后就可以在`main.lua`中直接调用`cppAdd`这个函数`print("lua",cppAdd(1,2))`。执行程序，发现`cppAdd`可以被正常调用。

可以看出，这个方法不太方便，因此，Lua提供了C模块的方法来注册函数。我们可以写一个C模块，然后在Lua中`require`这个模块。现在有一个`mylib.cpp`的模块。

```cpp
#include <iostream>
using namespace std;

extern "C"{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

extern "C"{
static int cppAdd(lua_State *L){
    int a = luaL_checkinteger(L,1);
    int b = luaL_checkinteger(L,2);
    cout<<"cppAdd"<<" "<<a<<" "<<b<<endl;
    lua_pushinteger(L,a + b);
    return 1;
}

static const struct luaL_Reg mylib[] ={
    {"cppAdd", cppAdd},
    {NULL,NULL}
};

int luaopen_mylib(lua_State *L){
    luaL_register(L,"mylib",mylib);
    return 1; //表示将这个表返回给lua
}
}
```

可以用下面的命令生成DLL文件。

```shell
g++ -shared mylib.cpp -I include -L lib -l lua -o mylib.dll
```

然后在Lua中调用这个模块。

```lua
require "mylib"
print("mulib.cppAdd",mylib.cppAdd(1,2))
```

这里有两点需要注意，首先是C++中导出的函数一定要使用`extern "C"`，不然会C++会应用函数重载的机制，更改函数名，导致链接失效。其次是通过Lua和C++通过名称进行绑定，一定要检查名字是否正确。

### 导出C++类到Lua

仔细想想，类方法和普通方法的主要区别在于类方法多了一个对象，所以，将这个参数显式表示出来，就可以将类方法转化为普通方法。这样的调用看上去有点怪，但想想Lua中的类，使用`:`调用会将第前面的对象当做第一个参数传入。

C数据结构，在Lua中用userdata表示，我们可以为userdata设置元表，然后使用`:`来调用元表中的方法。

到这里，导出C++类的核心思想就说完了，下面看看具体的代码。

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

extern "C"{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

class Person{
public:
    void add(string name, int age){
        arr.push_back(make_pair(name,age));
    }

    void sortByAge(){
        sort(arr.begin(),arr.end(),
            [](const pair<string,int> &p1, const pair<string,int> &p2)->bool{
                if(p1.second < p2.second){
                    return true;
                }else if(p1.second > p2.second){
                    return false;
                }else{
                    return p1.first < p2.first;
                }
            }
        );
    }

    pair<string,int> geti(int idx){
        if(idx > arr.size()){
            return make_pair("",0);
        }
        return arr[idx];
    }

    int getSize(){
        return arr.size();
    }

private:
    vector<pair<string,int>> arr;
};

extern "C"{

static int newperson(lua_State *L){
    Person *person;
    void* mem = lua_newuserdata(L,sizeof(Person));
    person = new (mem)Person(); //调用初始化函数
    luaL_getmetatable(L,"mylib.person"); //设置元表
    lua_setmetatable(L,-2);
    return 1; //userdata已经在栈中了
}

static int person_add(lua_State *L){
    Person *person = (Person *)lua_touserdata(L,1);
    const char *name = luaL_checkstring(L,2);
    int age = luaL_checkinteger(L,3);
    person->add(string(name),age);
    return 0;
}

static int person_sortByAge(lua_State *L){
    Person *person = (Person *)lua_touserdata(L,1);
    person->sortByAge();
    return 0;
}

static int person_getSize(lua_State *L){
    Person *person = (Person *)lua_touserdata(L,1);
    int size = person->getSize();
    lua_pushinteger(L,size);
    return 1;
}

static int person_geti(lua_State *L){
    Person *person = (Person *)lua_touserdata(L,1);
    int idx = luaL_checkinteger(L,2);
    pair<string,int> p = person->geti(idx);
    lua_pushstring(L,p.first.c_str());
    lua_pushinteger(L,p.second);
    return 2;
}

static const struct luaL_Reg mylib_f[] ={
    {"newperson", newperson},
    {NULL,NULL}
};

static const struct luaL_Reg mylib_m[] ={
    {"add", person_add},
    {"geti",person_geti},
    {"size",person_getSize},
    {"sortByAge",person_sortByAge},
    {NULL,NULL}
};


int luaopen_mylib(lua_State *L){
    luaL_newmetatable(L,"mylib.person"); //存储在注册表区域，该区域只有C能够访问
    lua_pushvalue(L,-1); //复制元表
    lua_setfield(L,-2,"__index"); //mt.__index = mt
    luaL_register(L,NULL,mylib_m); //设置元方法
    luaL_register(L,"mylib",mylib_f);
    return 1; //表示将这个表返回给lua
}
}
```

这里的`Person`类比较简单，使用了STL库来实现。可以看出，为了导出这个类，要将类中的每个方法都写一遍，代码重复度比较高，在实际的开发，通常使用一些工具来自动生成这个类。在Lua中可以用下面的代码调用。

```lua
require "mylib"
p = mylib.newperson()
m = getmetatable(p)
if not m then
    print("error:metatable is nil")
else
    p:add("a",3)
    p:add("b",2)
    p:add("c",1)
    p:add("da",1)
    p:add("db",1)
    p:sortByAge()
    local len = p:size()
    for i = 0,len-1 do
        local name,age = p:geti(i)
        print(name,age)
    end
end
```