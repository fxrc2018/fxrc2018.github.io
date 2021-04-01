##下载源码
去官网http://ffmpeg.org/download.html手动下载源码，或者使用git命令下载：
```
git clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
```
修改编译文件，注释的是以前的，没有注释的是修改后的：
```
#SLIBNAME_WITH_MAJOR='$(SLIBNAME).$(LIBMAJOR)'
#LIB_INSTALL_EXTRA_CMD='$$(RANLIB) "$(LIBDIR)/$(LIBNAME)"'
#SLIB_INSTALL_NAME='$(SLIBNAME_WITH_VERSION)'
#SLIB_INSTALL_LINKS='$(SLIBNAME_WITH_MAJOR) $(SLIBNAME)'
SLIBNAME_WITH_MAJOR='$(SLIBPREF)$(FULLNAME)-$(LIBMAJOR)$(SLIBSUF)'
LIB_INSTALL_EXTRA_CMD='$$(RANLIB) "$(LIBDIR)/$(LIBNAME)"'
SLIB_INSTALL_NAME='$(SLIBNAME_WITH_MAJOR)'
SLIB_INSTALL_LINKS='$(SLIBNAME)'
```
改变configure的权限：
```
chmod 777 configure
```
执行configure，我在编译的时候出现了这样的错误：
```
nasm/yasm not found or too old. Use --disable-x86asm for a crippled build.
```
按照提示，加入--disable-x86asm命令进行编译。

###添加文件
* 把include目录复制到cpp目录下。
* 在main下面创建jniLibs目录，复制arm的.so库到jniLibs目录。

###配置文件
```
android->externalNativeBuild->cmake->addFilters'armeabi-v7a'
```
```
android->so
```
###配置cmakelist
1. 导入include路径
2. 设置动态库路径
3. 添加动态库
4. 链接动态库


