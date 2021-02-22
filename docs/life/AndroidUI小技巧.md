##使用XML创建Drawable
用XML创建的Drawable本身是就是可缩放的。如果将其设为背景，它会自动填充视图的边界。


##ImageView
使ImageView充满父容器：
```
android:scaleType="fitXY"
```

```
mImageView=(ImageView)findViewById(R.id.image);
LayoutParams lp=(LayoutParams) mImageView.getLayoutParams();
//将宽度设置为屏幕的1/3
lp.width=mWidth;
mImageView.setLayoutParams(lp);
```
##创建资源文件夹
res->new->Android Resource Directory->要创建的资源文件夹

##退出app

