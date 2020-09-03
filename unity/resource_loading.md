## 资源加载

### unity加载资源的方式

unity的资源可以简单分为以下几类：

- Pure Asset
- Prefab
- Scene
- ScriptableObject
- Textasset
- Built-in

纯资源主要有mat、tex、mesh、animator、shader等。预制体和纯资源的区别是预制体拖动到场景就是一个GameObject。从资源中加载的GameObject不能直接显示在场景中，需要在代码中进行初始化。

- 在编辑器中管理
- resources
- assetbundle


unity主要由以下几种资源加载的方式：

- AssetDatabase接口
编辑器内加载、运行速度快。可以动态地加载和卸载资源，并且可以动态地创建和修改资源。
- 