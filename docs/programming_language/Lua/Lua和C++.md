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

    error = luaL_loadstring(L,"dofile(\"D:/Project/VSCode/LuaCpp/main.lua\")") || lua_pcall(L,0,0,0);
    if(error){
        cout<<lua_tostring(L,-1)<<endl;
    }

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


### Lua调用C++函数


### 导出C++类到Lua