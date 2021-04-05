## UI

### 触摸事件

触摸事件主要有3个，按下、移动和抬起。

### ScrollView

ScrollView可以看做是由两个部分组成：Panel和Container。其中，Panel用于控制显示的区域和接受触摸事件，Container用于放置内容。当Panel接受到触摸事件后，根据dx和dy移动Container的位置。

如果Container中有可以点击的控件，Panel需要将点击事件传递给Container中的控件。

稍微对ScrollView进行改造，就可以变成ListView，其核心在于，每次增加一个Item，动态的创建一个Item，将其加入到列表的最后，动态改变Container的大小。

