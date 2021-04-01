##游戏管理者
通常，游戏需要一个游戏管理者，游戏管理者知道游戏所有的状态，其他的游戏组件可以通过游戏管理者知道其他组件的情况。
```
public class GameManager {
    //这种写法，相当声明了一个接口，然后再创建了一个该接口的数组
    public event System.Action<Player> onLocalPlayerJoined;

    [HideInInspector]
    public GameObject gameObject;
    private static GameManager mInstance;

    public static string rolename = "rolename";
    public static int level = 1;
    public static int health = 100;
    public static int ammo = 120;

    public static bool isPause = false;

    public static GameManager Instance {
        get {
            if (mInstance == null) {
                mInstance = new GameManager();
                mInstance.gameObject = new GameObject("GameManager");

            }
            return mInstance;
        }
    }

    //这种写法，在java中相当于一个private变量的set和get方法
    private Player mLocalPlayer;
    public Player LocalPlayer {
        get {
            return mLocalPlayer;
        }
        set {
            mLocalPlayer = value;
            if (onLocalPlayerJoined != null) {
                onLocalPlayerJoined(mLocalPlayer); //调用注册在上面的回调函数

            }

        }
    }
}
```

##人物移动、旋转和跳跃
```
//人物的移动和跳跃
if (characterController.isGrounded) { //在地面的时候才能进行移动，空中不能
    moveDirection = new Vector3(Input.GetAxis("Horizontal"), 0.0f, Input.GetAxis("Vertical"));
    moveDirection = transform.TransformDirection(moveDirection); //相当于以自身的坐标轴前进
    moveDirection *= speed;
    if (Input.GetButton("Jump")) {
        moveDirection.y = jumpSpeed; //跳跃相当于加了一个向上的速度
    }
}
moveDirection.y -= gravity * Time.deltaTime; //向上的速度受到重力的影响，每次会下降
characterController.Move(moveDirection * Time.deltaTime);

//人物绕y轴旋转
mouseInput.x = Mathf.Lerp(mouseInput.x, playerInput.MouseInput.x, 1 / MouseControl.Damping.x);
transform.Rotate(Vector3.up * mouseInput.x * MouseControl.Sensitivity.x);
```
##第三人称相机

相机紧跟着玩家，但和玩家保持一定的距离。

##瞄准

准心是固定在屏幕某个位置的UI，当射击的时候，用光线检测从相机的中心看出去有没有发现物体，如果发现了，射击的目标就是该物体，如果没有，射击的目标是距离相机位置的最大射程处。射击的时候，初始化子弹，子弹的方式是炮口的位置，指向要射击的位置。

```
//计算准星的位置
Vector3 rayOrigin = Camera.main.ViewportToWorldPoint(new Vector3(0.5f, 0.5f, 0));
Vector3 targetPosition = Camera.main.transform.position + Camera.main.transform.forward * 500;
RaycastHit hit;
if (Physics.Raycast(rayOrigin, Camera.main.transform.forward, out hit, 500)) {
    targetPosition = hit.point;
}
```

##子弹
子弹初始化的时候，有方向和速度，然后按照一定的速度进行前进，前进中进行检测，检测是否碰到了可以造成伤害的物体，有就造成伤害。当一定时间后，子弹会自动消失，即达到了最大的射程。

```
void Start() {
    Destroy(gameObject, timeToLive);
}

void Update() {
    //由于速度太快，碰撞检测可能出错
    transform.Translate(Vector3.forward * speed * Time.deltaTime);
    RaycastHit hit;
    if(Physics.Raycast(transform.position,transform.forward,out hit, 5.0f)) {
        CheckDestructable(hit.transform);
    }
}

void CheckDestructable(Transform other) {
    var destructable = other.GetComponent<Destructable>();
    if (destructable == null) {
        return;
    }
    destructable.TakeDamage(transform.position,Random.Range(minDamage,maxDamage));
    Destroy(gameObject);
}
```

##换弹夹

子弹打完了就需要换弹夹，换弹夹期间不能射击，使用延时操作来完成。

##敌人AI

敌人比较简单，没有发现玩家的时候，会进行巡逻。巡逻的逻辑就是走到某个点，到了之后又走下一个点，走到最后一个点时，又回到第一个点。发现玩家的逻辑是，用一个球碰撞体进行检测，任何进入到球里面的玩家，检测视线，如果在视线范围内，就发现了玩家，然后进行射击。扫描玩家的代码如下。
```
public class Scanner : MonoBehaviour {
    [SerializeField] float scanSpeed;
    [SerializeField] float fieldOfView;

    SphereCollider rangeTrigger;
    List<Player> targets;

    Player mSelectedTarget;
    Player selectedTarget {
        get {
            return mSelectedTarget;
        }
        set {
            mSelectedTarget = value;
            if (mSelectedTarget == null) {
                return;
            }
            if (OnTargetSelected != null) {
                OnTargetSelected(mSelectedTarget.transform.position);
            }
        }
    }

    [SerializeField] LayerMask mask;

    public event System.Action<Vector3> OnTargetSelected;


    void Start() {
        rangeTrigger = GetComponent<SphereCollider>();
        targets = new List<Player>();
    }


    Vector3 GetViewAngle(float angle) {
        float radian = (angle + transform.eulerAngles.y) * Mathf.Deg2Rad;
        return new Vector3(Mathf.Sin(radian), 0, Mathf.Cos(radian));
    }

    public Player ScanForTarget() {
        selectedTarget = null;
        targets.Clear();
        Collider[] results = Physics.OverlapSphere(transform.position, rangeTrigger.radius);
        for (int i = 0; i < results.Length; i++) {
            Player player = results[i].GetComponent<Player>();
            if (player == null || !player.gameObject.activeSelf) {
                continue;
            }
            if (!IsInLineOfSight(Vector3.up, player.transform.position)) {
                continue;
            }
            targets.Add(player);
        }
        if (targets.Count >= 1) {
            float closestTarget = rangeTrigger.radius;
            foreach (var possibleTarget in targets) {
                if (Vector3.Distance(transform.position, possibleTarget.transform.position) <= closestTarget) {
                    selectedTarget = possibleTarget;
                }
            }
        }
        return selectedTarget;
    }

    bool IsInLineOfSight(Vector3 eyeHeight, Vector3 targetPosition) {
        Vector3 direction = targetPosition - transform.position;
        if (Vector3.Angle(transform.forward, direction.normalized) < fieldOfView / 2) {
            float distanceToTarget = Vector3.Distance(transform.position, targetPosition);
            //有遮挡物挡住了
            if (Physics.Raycast(transform.position + eyeHeight, direction.normalized, distanceToTarget, mask)) {
                Debug.Log("hide");
                return false;
            }
            return true;
        }
        return false;
    }
}
```
##总结
这个Demo一共花了10天左右，主要是看了一个视频教程。做完这个Demo，最大的感受是，游戏开发就是一个导演，主要要做的工作就是同步。
