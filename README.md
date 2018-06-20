# ihealth
## 介绍
ihealth是一个康复机器人的工程，实验室内部合作开发。没钱买private，所以就放在这里公开了。（反正没人看>_<）
这是一个windows vs的工程，但是是用cmake来管理的，希望尽量让安装更加方便简洁。
因为wke在当前的项目中只存在32位的动态库，而且我们没有wke的源码，所以当前的工程统一只能选择Win32平台。

## 下载第三方库

### OpenCV
下载并安装windows 版本的[OpenCV](https://opencv.org/releases.html)，最新版本的并不会直接提供x86的库，所以如果工程需要x86的OpenCV，则需要用cmake
自行编译。
### Eigen
下载最新版本的[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)，并解压。
### Boost
下载windows版本的[Boost](http://www.boost.org/users/history/version_1_65_1.html)，并解压。
### TBB
下载windows版本的[TBB](https://github.com/01org/tbb/releases)，并解压。

## 编译
上述第三方库下载解压完成之后，就可以编译ihealth了
* 打开git bash 运行 `git clone git@github.com:hust-IR2/ihealth.git`克隆本工程到本地目录；
* 下载windows版本的cmake gui，[下载地址](https://cmake.org/download/)；
* 利用cmake gui编译ihealth，这个流程在这里就不详细介绍，只提几个关键位置
  * 需要OpenCV、Eigen、Boost、TBB的路径，如果报错，自己多尝试一下，可能路径的层次不对；
  * 此外还有几个库，像NI、WKE，也要手动添加，放在ihealth文件夹里了（这个后面或许会抽出来)
  * 注意需要把动态库dll所在的文件夹设置到path环境变量中。
