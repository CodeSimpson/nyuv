## NYUV

### 1. 代码逻辑

Qt程序主函数基本代码：

```c++
int main(int argc, char ** argv)
{
    QApplication app(argc, argv);		// 定义并创建应用程序
    MainWindow mainWindow;				// 定义并创建窗口
    mainWindow.show();					// 显示窗口
    int ret = app.exec();				// 运行应用程序
    return ret;
}
```

​	这里main函数在运行程序后，开始应用程序的消息循环和事件处理。



**完整的代码执行逻辑：**

> 只注释自定义函数功能

* 启动程序

```c++
cvt_schedul_init()
MainWindow::MainWindow()	// 自定义主窗口类默认构造函数
--->autocomplete_load()		// 自动加载.nyuv.config文件中根目录路径
    loadRawSet()			// 加载liblist.xml中不同lib的source port支持的raw格式
    initMainWindow()		// 实例化主窗口中的各组件，并设置主窗口所属的菜单栏、工具栏和状态栏
    --->QWidget::setAcceptDrops()
    	QMainWindow::setMenuBar()
    	QMainWindow::addTooBar()
    	QMainWindow::setStatusBar()
    	QWidget::setStyleSheet()	// QToolBar、QStatusBar调用，设置工具栏和状态栏格式
    	new QSplitter()		// 实例化分割器组件，可以动态调整分割器内窗口的大小
    	QMainWindow::SetCentralWidget()
    	new ImageInformationBar()	// 实例化QFrame派生类，图像格式信息类
    	--->QFrame::setFrameShape()
    		QFrame::setFrameShadow()
    		initBox()				// 
    		--->QWidget::setLayout()// 设置当前窗口的布局管理器为m_layout
    		initFormat()			// 
    	new LocalFileSystemViewer()		// 实例化文件树视图QTreeView派生类
		--->QWidget::setStyleSheet()	// 设置文件目录栏格式
    		
	initUiComponent()
   	initImageResource()
    new QImageViewer()
cvt_schedul_uninit()
```



### 2. Qt类

* QMainWindow类：主窗口类
  * setAcceptDrops()：基类QWidget成员函数，设置当前窗口组件是否支持拖放事件。
  * setMenuBar()：设置主窗口所属的菜单栏。
  * addToolBar()：设置主窗口的工具栏，默认在窗口上部，可以添加在窗口四周，一个窗口可以添加多个工具栏。
  * setStatusBar()：设置主窗口的状态栏，此时QMainWindow会拥有该状态栏的所有权。
  * setStyleSheet()：基类QWidget成员函数，设置当前组件（工具栏、状态栏）样式表。
  * setCentralWidget()：设置主窗口的中央组件，此时QMainWindow会拥有该组件的所有权。
* QSplitter类：分离器组件
  * addWidget()：添加小组件到分离器中。
  * setStretchFactor()：按索引设置分离器内各组件的拉伸因子。
* QFrame类：框架类
  * setFrameShape()：设置框架形状。
  * setFrameShadow()：设置框架阴影样式。
  * setLayout()：基类QWidget成员函数，设置框架类的布局管理器
* QGridLayout类：布局管理器类
  * setSpacing()：设置水平和垂直方向的间距。
  * setContentsMargins()：基类QLayout成员函数，设置布局与其父容器间的内容边距。