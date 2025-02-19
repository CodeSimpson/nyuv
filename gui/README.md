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
    		initBox()				// 初始化图像格式信息栏布局
    		--->initFormatBox()		// 初始化图像格式信息栏Format窗口布局，返回Format窗口布局管理器。
    			--->createTypeComboBox()	// 创建Format窗口支持的图像格式下拉列表
    			--->new QSpinBox()			// 整数输入组件，确定图像宽高等信息
    			--->new QPushButton("提交")	// 提交图像并显示
    			--->connect()		// 连接QButton、QSpinBox对应的信号和槽函数
    		--->initCheckBox()		// 初始化check栏窗口布局，设置功能
    			--->setBayerShow()	// 显示bayer格式或者rgb格式
    			--->setFit()		// 自适应窗口大小
    			--->setHF()			// 更准确推导格式
    			--->setAL()			// 使用自动亮度
    			--->setStats()		// 计算图像stats信息
    		--->initSliderBox()
    
    
    
    		--->QWidget::setLayout()// 设置当前窗口的布局管理器为m_layout
    		initFormat()			// 
    	new LocalFileSystemViewer()		// 实例化文件树视图QTreeView派生类
		--->QWidget::setStyleSheet()	// 设置文件目录栏格式
    		
	initUiComponent()
   	initImageResource()
    new QImageViewer()
cvt_schedul_uninit()
```

**注解：**

* `ImageInformationBar::initFormatBox()`初始化“提交”按钮，并连接槽函数`OnProcessBtnClicked()`，该槽函数发送信号`void processBtnClicked(const IMAGEINFO)`，由主窗口MainWindow接收并触发主窗口槽函数`settingTriggered(const IMAGEINFO)`。

* 图像大小信息中width、height、stride和align的作用

  * width、height表示图像宽高
  * stride表示跨距，是图像一行数据的实际字节数，通常大于或等于width
  * align表示内存对齐的字节数

  假设有一张raw图，width为5个像素，height为4个像素，每个像素占2个字节，align要求4字节对齐，即硬件要求每行数据按4个字节对齐，此时每行的理论字节数为10字节，不是4的倍数，因此需要填充2字节，stride为12。

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
  * addLayout()：布局管理器内部直接再添加一个布局管理器
  * addWidget(QWidget* widget, int row, int column, int rowSpan, int columnSpan)：添加布局单元到布局管理器的row行和column列，行高为rawSpan，列长为columnSpan。
* QComboBox类：下拉列表类
  * addItems()：添加下拉列表的列表内容
  * setMaximumSize()：基类QWidget成员函数，设置最大宽高
  * setFont()：基类QWidget成员函数，设置当前组件格式
* QSpinBox类：微调框类，带有按钮的输入框，可以直接输入或通过按钮输入整数值
  * setSingleStep()：设置按钮的调整步长
  * setMinimum()：设置输入框的最小值
  * setMaximum()：设置输入框的最大值
  * setValue()：设置输入框的值，若值与旧值不同，则会触发一个signal信号
  * setSuffix()：设置输入框的后缀Qstring
  * setDisabled()：基类QWidget成员函数，为true时关闭当前组件的输入功能
* QLabel类：标签类，显示静态文本或者图像并提供一系列格式化和交互功能。
  * setAlignment()：设置对齐样式
  * setFont()：设置文本格式
* QPushButton类：按钮类
  * setIcon()：设置Icon符号
  * setToolTip()：基类QLayout成员函数，设置工具提示内容
* QCheckBox()类：复选框类，允许用户选择一个或多个选项
  * setCheckState()：设置复选框状态