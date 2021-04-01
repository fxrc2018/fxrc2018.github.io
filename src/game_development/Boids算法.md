## Boids算法

- Flock
代表一个群体，负责初始化一个群体，并持有各个群体的引用。
- FlockAgent
代表一个物体。
- FlockBehavior
代表了一种群聚行为。
- BehaviorFilter
用于过滤一些物体。

整个流程很简单，首先，Flock初始化一个群体，然后每次迭代，计算每个FlockAgent的受力，更新每个FlockAgent的位置。

github地址：https://github.com/fxrc2018/Demos/tree/master/Unity/Boids