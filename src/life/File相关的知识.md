##读写文件的标准操作
文件读写是一个基本的操作，不难，但是我觉得对于java来说，异常的处理比较麻烦。下面的代码来自《java编程思想》，个人感觉这种写法确实不错。
```
public static Object readObject(String path){
    Object object = null;
    try {
        ObjectInputStream inputStream = new ObjectInputStream(new FileInputStream(new File(path)));
        try {
            object = inputStream.readObject();
        }catch (Exception e){
            e.printStackTrace();
        }finally {
            inputStream.close();
        }
    }catch (Exception e){
        Log.e(TAG,"open input stream failed");
    }
    return object;
}
```
