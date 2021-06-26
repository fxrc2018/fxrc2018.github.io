## UI

### 触摸事件

触摸事件主要有3个，按下、移动和抬起。

### ScrollView

ScrollView 可以看做是由两个部分组成：Panel 和 Container。其中，Panel 用于控制显示的区域和接受触摸事件，Container 用于放置内容。Panel 的大小一般小于 Container 的大小，但超过 Panel 范围的内容是不可见的。


当 Panel 接受到触摸事件后，根据 dx 和 dy 移动 Container 的位置。

如果 Container 中有可以点击的控件，Panel 需要将点击事件传递给 Container 中的控件。

稍微对 ScrollView 进行改造，就可以变成 ListView，其核心在于，每次增加一个 Item，动态的创建一个 Item，将其加入到列表的最后，动态改变 Container 的大小。

