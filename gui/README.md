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



#### 1.1 完整的代码执行逻辑

> 只注释自定义函数功能

* 启动程序

```c++
cvt_schedul_init()			// 加载动态库
MainWindow::MainWindow()	// 自定义主窗口类默认构造函数
--->autocomplete_load()		// 自动加载.nyuv.config文件中根目录路径
    loadRawSet()			// 加载liblist.xml中不同lib的source port支持的raw格式
    initMainWindow()		// 实例化主窗口中的各组件，初始化图像格式信息栏和目录栏组件布局
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
    		--->initSliderBox()		// 初始化slider栏窗口布局，确定图像的缩放大小
    			--->setZoomValue()
    		--->initStatsBox()		// 初始化数据统计stats栏布局
    			--->calStats()
    		--->QWidget::setLayout()// 设置当前窗口的布局管理器为m_layout
    		initFormat()			//  初始化并自动推导图片格式
    		--->autocomplete_init()
    	new LocalFileSystemViewer()		// 实例化文件树视图QTreeView派生类
		--->QWidget::setStyleSheet()	// 设置文件目录栏格式
    	--->QTreeView::header()			// 返回QTreeView的表头对象
    	--->new QFileSystemModel()		// 数据模型，用于管理文件系统，供视图类使用
    	--->QFileSystemModel::setNameFilterDisables()
    	--->QFileSystemModel::setNameFilters()
  		--->QFileSystemModel::setIconProvider()
    	--->connet()	// 连接信号和槽函数，需要区别文件夹、文件还是展开按钮，以及单击还是双击
    	--->changeRootPath()
    		--->QFileSystemModel::setRootPath()	// 设置文件树视图根目录
    		--->MainWindow::filetreeShowAction() // 更新主窗口状态栏
    		--->QFileSystemModel::setRootIndex() // 根据rootPath设置rootIndex
    		--->QFileSystemModel::hideColumn()
    		--->updatePathEdit()		// 更新文件树目录编辑栏内容
	initUiComponent()
    --->setWindowComponet()		// 为菜单栏和工具栏添加QAction，并连接上对应的槽函数
    	--->QTabBar::tabCloseRequested()	// 选项卡关闭按钮点击时触发
    	--->QTabBar::currentChanged()		// 当标签栏的当前标签发生变化时，会发出此信号
    	--->QTabBar::tabMoved()				// 移动标签栏时触发
   	--->setQImageViewerWidget()	// 初始化显示图像的标签组件QLabel，添加滚动视图框架
   	initImageResource()
    --->QLabel::clear()		// 清空显示图像标签组件的内容
    --->QMainWindow::setWindowTitle	// 设置主窗口标题
    new QImageViewer()		// 用于保存图像buffer和格式信息
cvt_schedul_uninit()		// 释放动态库
```

* 执行程序：通过菜单栏或工具栏打开一张图片

```c++
// 通过拖拽一张图片并显示
MainWindow::setAcceptDrops(true); 				// 首先设置主窗口接受拖放事件
void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;	// 通过重写QWidget的mousePressEvent、mouseMoveEvent和mouseReleaseEvent等方法来实现拖放功能，并结合使用QMimeData和QDrag类。
void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;

// 通过菜单栏或工具栏选择一张图片打开并显示
openActionTriggered()	// mp_action_open::triggered()触发打开图片槽函数
--->QFileDialog::getOpenFileName()
--->fileTreeTriggered()
    --->QImageViewer::openImageFile()			// 加载图片文件信息到fileInfoList中
    --->loadImageResource(true)					// 加载图像buffer并显示
    	--->ImageInformationBar::parse()		// 解析文件格式
    		--->ImageInformationBar::set()		// 保存文件格式到图像格式信息栏的成员变量中
    	--->settingTriggered()
    		--->QImageViewer::setRaw(, true)	// 记录图像格式，并加载图像buffer
    			--->QImageViewer::upgradeFileInfo()
    				--->loadNormalImg()			// 加载普通图
    				--->loadRawImg()			// 加载raw图
    					--->cvt_process()		// 调用动态库做格式转换
    		--->loadImageResource(false)		// 直接显示图片  
```

* 输入图像格式并提交显示

```c++
```



#### 1.2 注解

* `ImageInformationBar::initFormatBox()`初始化“提交”按钮，并连接槽函数`OnProcessBtnClicked()`，该槽函数发送信号`void processBtnClicked(const IMAGEINFO)`，由主窗口MainWindow接收并触发主窗口槽函数`settingTriggered(const IMAGEINFO)`。

* 图像大小信息中width、height、stride和align的作用

  * width、height表示图像宽高
  * stride表示跨距，是图像一行数据的实际字节数，通常大于或等于width
  * align表示内存对齐的字节数

  假设有一张raw图，width为5个像素，height为4个像素，每个像素占2个字节，align要求4字节对齐，即硬件要求每行数据按4个字节对齐，此时每行的理论字节数为10字节，不是4的倍数，因此需要填充2字节，stride为12。



### 2. Qt类

* QMainWindow类：主窗口类
  * setAcceptDrops()：基类QWidget成员函数，设置当前窗口组件是否支持拖放事件【指直接拖拽一张图片到主窗口显示】。
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
  * addWidget(QWidget* widget, int row, int column, int rowSpan, int columnSpan)：添加布局单元到布局管理器的row行和column列，行高为rawSpan，列长为columnSpan，==不转移所有权==。
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
* QCheckBox类：复选框类，允许用户选择一个或多个选项
  * setCheckState()：设置复选框状态
* QTreeView：树视图类，用于显示文件路径
  * header()：返回QTreeView的表头对象
  * setModel()：设置当前视图类的数据模型
  * setRootIndex()：显示该索引对应的目录或数据项作为根节点。
  * hideColumn(i)：隐藏当前视图第i列
* QFileSystemModel：文件系统数据模型，用于在Qt应用程序中展示和管理文件系统的数据
  * setFilter()：根据mask设置文件系统的过滤条件
  * setNameFilterDisables()：设置未通过名称过滤器的文件是否被过滤或隐藏
  * setNameFilters()：根据文件名设置过滤条件
  * setIconProvider()：设置文件模型图标
  * setRootPath()：设置根目录
* QTabBar：选项卡栏，主要提供选项卡功能，使得用户可以通过不同的选项卡切换不同的内容或功能区域
  * tabCloseRequested()：选项卡关闭按钮点击时触发
  * currentChanged()：当标签栏的当前标签发生变化时，会发出此信号
  * tabMoved()：移动标签栏时触发
  * setTabsClosable()：设置选项卡是否可以关闭
  * setAutoHide()：选项卡是否可以自动隐藏
  * setMovable()：是否可移动
  * setChangeCurrentOnDrag()：如果为真，则拖动标签栏时当前选项卡会自动更改
  * setIconSize()：设置图标大小
  * setElideMode()：如何隐藏过长的标签栏文本
  * setShape()：设置标签栏形状
* QScrollArea：滚动视图框架类
  * setWidget()：设置widget成为滚动区域的子窗口小部件
* QFileDialog：qt中用于选择文件或目录的对话框类
  * getOpenFileName()：静态函数，可以打开一个对话框，让用户选择一个文件并返回选中的文件路径。



### 3. 动态库的加载和关闭

​	我们定义一个类`SCHEDULE`类保存不同功能动态库对应的句柄，并在程序启动时自动加载动态库，在程序关闭时自动关闭动态库。在源文件`cvtinterface.cpp`中，我们定义了一个静态全局变量`p_cvt_instance`来保存类`SCHEDULE`的类实例。

* 自动加载动态库

```c++
// 启动程序
cvt_schedul_init()
--->SCHEDULE::SCHEDULE()
    --->SCHEDULE::getLibList()	// 解析libxml.xml，保存动态库名称、输入图和输出图格式
    --->SCHEDULE::linkTest()
    --->SCHEDULE::loadLibs()
    --->SCHEDULE::initLibs()
```

* 自动关闭动态库

```c++
// 程序执行结束
cvt_schedul_uninit()
    
```



### 4. CMake编译



