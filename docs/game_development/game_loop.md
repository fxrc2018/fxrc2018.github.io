## 游戏程序三要素

### 游戏循环

整个游戏程序的核心流程控制称为游戏循环。一个传统的游戏循环可以分成3部分：处理输入、更新游戏世界、生成输出。

```c
while game is running 
    process inputs
    update game world
    generate outputs
loop
```

### 游戏时间

早期的游戏经常以特定处理器的速度来处理逻辑。这样不能兼容不同的处理器速度，因此，游戏中引入增量时间的概念：从上一帧起流逝的时间。

```c
targetFrameTime  = 33.3f
while game is running 
    realDeltaTime = time since last frame
    gameDeltaTime = realDeltaTime * gameTimeFactor

    //处理输入
    ...

    update game world with gameDeltaTime

    //渲染输出
    ...

    while (time spent this frame < targetFrameTime>)
        // 做一些事情将多出的时间用完
    
    loop
loop
```

### 游戏对象

广义上的游戏对象是每一帧都需要更新或者绘制的对象。因此，游戏对象可以分为三种：

- 更新和绘制都需要的对象。
- 只绘制不更新的对象，称为静态对象。
- 只更新但不绘制的对象。

因此，游戏对象可以设计为以下的接口：

```cpp
class GameObject
    interface Drawable
        function Draw()
    end

    interface Updateable
        function Update (float deltaTime)
    end
end
```

在游戏世界中需要管理这些游戏对象。

```cpp
class GameWorld
    List updateableObjects
    list drawableObjects
end
```

游戏世界的更新可以写成下面的伪代码。

```cpp
while game is running 
    realDeltaTime = time since last frame
    gameDeltaTime = realDeltaTime * gameTimeFactor

    //处理输入
    ...

    //更新游戏世界
    foreach Updateable o in GameWorld.updateableObjects
        o.update(gameDeltaTime)
    loop

    //渲染输出
    ...

    //限制帧数代码
    ...
loop
```