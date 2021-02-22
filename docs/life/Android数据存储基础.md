Android的数据存储主要分为以下几类：SharedPreferences、文件存储、SQLite 数据库、网络存储、ContentProvider。下面就上面这几个方面写写我个人的一些理解。

##SharedPreferences

```
private static final String SHARED_PREFERENCES_NAME = "shared_preferences_name";
private static SharedPreferences sharedPreferences;
public static int getInt(Context context, String key, int defValue) {
    if (sharedPreferences == null) {
        sharedPreferences = context.getSharedPreferences(SHARED_PREFERENCES_NAME, Context.MODE_PRIVATE);
    }
    return sharedPreferences.getInt(key, defValue);
}
public static void putInt(Context context, String key, int value) {
    if (sharedPreferences == null) {
        sharedPreferences = context.getSharedPreferences(SHARED_PREFERENCES_NAME, Context.MODE_PRIVATE);
    }
    sharedPreferences.edit().putInt(key, value).commit();
}
```

##文件存储

##SQLite 数据库

##网络存储

##ContentProvider
