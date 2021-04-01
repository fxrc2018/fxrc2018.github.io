原地址[https://www.redblobgames.com/pathfinding/a-star/implementation.html](https://www.redblobgames.com/pathfinding/a-star/implementation.html)。

A\*算法是游戏中常用的寻路算法。A\*算法在Dijkstra算法的基础上进行了一些改进，Dijkstra算法只按照离起点的值作为搜索的顺序，而实际上，离起点近并不意味着离终点近，这样的搜索顺序可能并不是最优的。但问题是，我们并不能准确知道当前点离终点的距离，因此，只能用一种估算的方式来计算。A\*算法的有一个“估价函数”，该函数以任意“状态”为输入，计算出从该状态到目标状态所需代价的估计值。带有估价函数的优先队列BFS就称为A\*算法。

A\*算法作为游戏中常用的寻路算法是因为游戏地图通常是基于网格的，使用A\*算法有更快的运行效率。

先来看图的数据结构。因为我最近在学Python，所以这里值引用了Python的写法。在原地址中，有C++和C#的版本。

```
class SimpleGraph:
    def __init__(self):
        self.edges = {}

    def neighbors(self,id):
        return self.edges[id]

class SquareGrid:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.walls = []

    def in_bounds(self, id):
        (x, y) = id
        return 0 <= x < self.width and 0 <= y < self.height

    def passable(self, id):
        return id not in self.walls

    def neighbors(self, id):
        (x, y) = id
        results = [(x + 1, y), (x, y - 1), (x - 1, y), (x, y + 1)]
        if (x + y) % 2 == 0: results.reverse()  # aesthetics
        results = filter(self.in_bounds, results)
        results = filter(self.passable, results)
        return results

class GridWithWeights(SquareGrid):
    def __init__(self, width, height):
        super(SquareGrid).__init__(width,height)
        self.weights = {}

    def cost(self, from_node, to_node):
        # 走到相邻单位的消耗为1，这里相当于返回一个常量
        return self.weights.get(to_node, 1)
```
这里的写法确实让我印象深刻，可能是因为我才学Python的缘故吧。我觉得他写得接口比较好，图的结构写得相当简洁。

##BFS

为了更好的理解A\*算法，作者首先介绍了BFS的搜索方式。

```
def breadth_first_search(graph, start, goal):
    frontier = collections.deque()
    came_from = {}
    came_from[start] = None
    frontier.append(start)
    while len(frontier) > 0:
        current = frontier.popleft()
        if current == goal:
            break
        for next in graph.neighbors(current):
            if next not in came_from:
                came_from[next] = current
                frontier.append(next)
    return came_from
```
##Dijkstra
```
def dijkstra_search(graph, start, goal):
    frontier = []
    heapq.heappush(frontier,(0,start))
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0

    while len(frontier) > 0:
        current = heapq.heappop(frontier)[1]
        if current == goal:
            break
        for next in graph.neighbors(current):
            new_cost = cost_so_far[current] + graph.cost(current, next)
            if next not in cost_so_far or new_cost < cost_so_far[next]:
                cost_so_far[next] = new_cost
                priority = new_cost
                heapq.heappush(frontier,(priority,next))
                came_from[next] = current
                
    return came_from, cost_so_far
```
##A\*
```
def heuristic(a, b):
    (x1, y1) = a
    (x2, y2) = b
    return abs(x1 - x2) + abs(y1 - y2)


def a_star_search(graph, start, goal):
    frontier = []
    heapq.heappush(frontier,(0,start))
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0

    while len(frontier) > 0:
        current = heapq.heappop(frontier)[1]
        if current == goal:
            break

        for next in graph.neighbors(current):
            new_cost = cost_so_far[current] + graph.cost(current, next)
            if next not in cost_so_far or new_cost < cost_so_far[next]:
                cost_so_far[next] = new_cost
                priority = new_cost + heuristic(goal, next)
                heapq.heappush(frontier,(priority,next))
                came_from[next] = current

    return came_from, cost_so_far
```
可以看出，A\*在Dijkstra的基础上改动并不大，就是改变了搜索的顺序。因此，A\*算法提高搜索效率的关键，就在于能否设计出一个优秀的估价函数。估价函数的估值不能大于未来实际代价，并且还应该尽可能反映未来实际代价的变化趋势和相对大小关系，这样搜索才会较快地逼近最优解。

在寻路算法中，通常使用的估计函数有：

* 曼哈顿距离$D(p,q)=\sum_{i=1}^n(px_i + qx_i)$，在二维平面就是$D=|x_1 - x_2| + |y_1 - y_2|$。上面的例子就是用的曼哈顿距离。
* 欧氏几何平面距离$D(p,q)=\sqrt{\sum_{i=1}^n(px_i-qx_i)^2}$，在二维平面是$D=\sqrt{(x_1 - x _2)^2 + (y_1-y_2)^2}$。
* 切比雪夫距离$D=max(|p_1-q_i|)$，在二维平面就是$D=max(|x_1-x_2|,|y_1-y_2|)$。
