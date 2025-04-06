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

* 启动程序

```c++
cvt_schedul_init()			// 加载动态库
MainWindow::MainWindow()	// 自定义主窗口类默认构造函数
--->autocomplete_load()		// 自动加载.nyuv.config文件中根目录路径
    MainWindow::loadRawSet()			// 加载liblist.xml中不同lib的source port支持的raw格式
    MainWindow::initMainWindow()		// 实例化主窗口中的各组件，初始化图像格式信息栏和目录栏组件布局
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
    		ImageInformationBar::initBox()				// 初始化图像格式信息栏布局
    		--->ImageInformationBar::initFormatBox()		// 初始化图像格式信息栏Format窗口布局，返回Format窗口布局管理器。
    			--->ImageInformationBar::createTypeComboBox()	// 创建Format窗口支持的图像格式下拉列表
    			--->new QSpinBox()			// 整数输入组件，确定图像宽高等信息
    			--->new QPushButton("提交")	// 提交图像并显示
    			--->ImageInformationBar::initFormatBox()::connect()		// 连接QButton、QSpinBox对应的信号和槽函数
    		--->ImageInformationBar::initCheckBox()		// 初始化check栏窗口布局，设置功能
    			--->ImageInformationBar::setBayerShow()	// 显示bayer格式或者rgb格式
    			--->ImageInformationBar::setFit()		// 自适应窗口大小
    			--->ImageInformationBar::setHF()			// 更准确推导格式
    			--->ImageInformationBar::setAL()			// 使用自动亮度
    			--->ImageInformationBar::setStats()		// 计算图像stats信息
    		--->ImageInformationBar::initSliderBox()		// 初始化slider栏窗口布局，确定图像的缩放大小
    			--->ImageInformationBar::setZoomValue()
    		--->ImageInformationBar::initStatsBox()		// 初始化数据统计stats栏布局
    			--->ImageInformationBar::calStats()
    		--->QWidget::setLayout()// 设置当前窗口的布局管理器为m_layout
    		ImageInformationBar::initFormat()			//  初始化并自动推导图片格式
    		--->autocomplete_init()
    	new LocalFileSystemViewer()		// 实例化文件树视图QTreeView派生类
		--->QWidget::setStyleSheet()	// 设置文件目录栏格式
    	--->QTreeView::header()			// 返回QTreeView的表头对象
    	--->new QFileSystemModel()		// 数据模型，用于管理文件系统，供视图类使用
    	--->QFileSystemModel::setNameFilterDisables()
    	--->QFileSystemModel::setNameFilters()
  		--->QFileSystemModel::setIconProvider()
    	--->LocalFileSystemViewer::initPathBox()::connect()	// 连接信号和槽函数，需要区别文件夹、文件还是展开按钮，以及单击还是双击
    	--->LocalFileSystemViewer::changeRootPath()
    		--->QFileSystemModel::setRootPath()	// 设置文件树视图根目录
    		--->MainWindow::filetreeShowAction() // 更新主窗口状态栏
    		--->QFileSystemModel::setRootIndex() // 根据rootPath设置rootIndex
    		--->QFileSystemModel::hideColumn()
    		--->LocalFileSystemViewer::updatePathEdit()		// 更新文件树目录编辑栏内容
	MainWindow::initUiComponent()
    --->MainWindow::setWindowComponet()		// 为菜单栏和工具栏添加QAction，并连接上对应的槽函数
    	--->QTabBar::tabCloseRequested()	// 选项卡关闭按钮点击时触发
    	--->QTabBar::currentChanged()		// 当标签栏的当前标签发生变化时，会发出此信号
    	--->QTabBar::tabMoved()				// 移动标签栏时触发
   	--->MainWindow::setQImageViewerWidget()	// 初始化显示图像的标签组件QLabel，添加滚动视图框架
   	MainWindow::initImageResource()
    --->QLabel::clear()		// 清空显示图像标签组件的内容
    --->QMainWindow::setWindowTitle	// 设置主窗口标题
    new QImageViewer()		// 用于保存图像buffer和格式信息
QApplication::exec()
cvt_schedul_uninit()		// 释放动态库
```

* 执行程序：通过菜单栏或工具栏打开一张图片

```c++
// 通过拖拽一张图片并显示
MainWindow::setAcceptDrops(true); 				// 首先设置主窗口接受拖放事件
void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;	// 通过重写QWidget的mousePressEvent、mouseMoveEvent和mouseReleaseEvent等方法来实现拖放功能，并结合使用QMimeData和QDrag类。
void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;

// 通过菜单栏或工具栏选择一张图片打开并显示
MainWindow::openActionTriggered()	// mp_action_open::triggered()触发打开图片槽函数
--->QFileDialog::getOpenFileName()
--->MainWindow::fileTreeTriggered()
    --->QImageViewer::openImageFile()			// 加载图片文件信息到fileInfoList中
    --->MainWindow::loadImageResource(true)					// 加载图像buffer并显示
    	--->ImageInformationBar::parse()		// 解析文件格式
    		--->ImageInformationBar::set()		// 保存文件格式到图像格式信息栏的成员变量中
    	--->MainWindow::settingTriggered()
    		--->QImageViewer::setRaw(, true)	// 记录图像格式，并加载图像buffer
    			--->QImageViewer::upgradeFileInfo()
    				--->QImageViewer::loadNormalImg()			// 通过QImage类load接口加载普通图，自动分配内存。
    				--->QImageViewer::loadRawImg()			// 加载raw图
    					--->QImageViewer::cvt_process()		// 调用动态库做格式转换，接口内手动分配内存
    		--->MainWindow::loadImageResource(false)		// 直接显示图片
```

* 输入图像格式并提交显示

```c++
// 选择图像格式
QComboBox m_type;				// 直接选择下拉列表即可，通过currentText()获得所选内容

// 输入图像宽高和stride
QSpinBox m_height;				// 通过value()接口获得图像宽高和行跨距
QSpinBox m_width
QSpinBox m_stride;

// 提交按钮按下
QPushButton::clicked()
--->OnProcessBtnClicked()		// 自定义槽函数，发送信号processBtnClicked()
    --->signal::void processBtnClicked(const IMAGEINFO &imageInfo)
```



#### 1.2 注解

* `ImageInformationBar::initFormatBox()`初始化“提交”按钮，并连接槽函数`OnProcessBtnClicked()`，该槽函数发送信号`void processBtnClicked(const IMAGEINFO)`，由主窗口MainWindow接收并触发主窗口槽函数`settingTriggered(const IMAGEINFO)`。

* 图像大小信息中width、height、stride和align的作用

  * width、height表示图像宽高
  * stride表示跨距，图像每一行数据在内存中占用的字节数，stride = width × bytes_per_pixel + padding。
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
    --->SCHEDULE::linkTest()	// 判断各动态算法库的输入输出link是否正常（还比较呆）
    --->SCHEDULE::loadLibs()	// dlopen加载动态库，然后通过dlsym找到对应的入口函数
    --->SCHEDULE::initLibs()	// 算法库实例化
```

* 自动关闭动态库

```c++
// 程序执行结束
cvt_schedul_uninit()
--->SCHEDULE::~SCHEDULE()
    --->SCHEDULE::uninitLibs()	// 算法库析构
```

* 执行算法

```c++
QImageViewer::loadRawImg()
--->QImageViewer::cvt_process()
    --->SCHEDULE::process()
        --->SCHEDULE::loadInputFile()
            --->CVT::testIBuffer()		// 测试输入图宽高是否为偶数
            --->CVT::getIBufferInfo()	// 计算输入图需要的buffer size
            --->REQUESTINFO::BUFFER::resize()	// 分配输入图需要的buffer
            --->ifstream::read()		// 以二进制形式读取图片数据
        --->SCHEDULE::iterator()		// 包含一个递归逻辑
            --->CVT::setOBufferInfo()	// 计算输出图需要的buffer size，一般输出都是RGB888格式
            --->REQUESTINFO::BUFFER::resize()	// 分配输出图需要的buffer
            --->CVT::cvt()				// 开始转换格式
--->QImageViewer::setAngleAndScale()	// 旋转缩放并更新pixmap供显示
```

### 4. CMake编译

* 顶层CMakeLists.txt

  * **添加宏定义**

  ```cmake
  add_definitions(-DINSTALL) # 向编译器传递预处理宏定义INSTALL
  ```

  ```c++
  // 源代码使用示例：根据是否启用安装模式选择配置文件路径
  #ifdef INSTALL
      const std::string config_path = "/etc/nyuv/config.conf";  // 系统安装路径
  #else
      const std::string config_path = "./config.conf";          // 本地构建路径
  #endif
  ```

  最佳实践：

  ​	这里写在顶层CMakeLists.txt中，则宏定义为全局，如果希望便面直接使用`add_definitions`，则可以通过`target_compile_definitions`更加精确地控制宏定义，这种方式更符合现代CMake实践，能避免全局宏定义污染其他目标。
  
  ```cmake
  if (${INSTALL})
      target_compile_definitions(my_target PUBLIC INSTALL)  # 为指定目标添加宏
  endif()
  ```

  * **添加链接库路径**

  ```cmake
  link_directories(${LIB_DIR})
  ```
  
  ​	在CMake中，`link_directories()` 命令用于 ‌**向链接器（Linker）添加库文件的搜索路径**‌，使得链接器能够在指定目录中查找需要链接的共享库（`.so`、`.dll`）或静态库（`.a`、`.lib`）。该命令**全局生效**，后续所有的`target_link_libraries()`命令都会自动在这些路径中查找库文件。
  
  ```cmake
  set(LIB_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}) # 设置库目录为构建输出路径
  link_directories(${LIB_DIR})      # 链接目录
  ```
  
  最佳实践：
  
   1. 作用域更明确，仅对指定目标生效
  
  ```cmake
  target_link_directories(my_app PRIVATE ${LIB_DIR})  # 仅对 my_app 生效
  ```
  
    2. 自动查找
  
  ```cmake
  # 推荐方式：结合 find_library 自动查找
  find_library(NYUV_LIB nyuv_lib PATHS ${LIB_DIR} REQUIRED)
  target_link_libraries(my_app ${NYUV_LIB})
  ```
  
  *  **设置编译选项**
  
  ```cmake
  SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")     # 设置编译选项，-O0：关闭优化，-Wall：启用所有警告，-g2 -ggdb：生成GDB专用调试信息，级别2
  ```
  
  ‌	CMAKE_CXX_FLAGS_DEBUG变量**定义Debug模式的编译选项**‌，当项目以 `Debug` 配置构建时（例如 `cmake -DCMAKE_BUILD_TYPE=Debug`），这些选项会被附加到C++编译器的命令行。`$ENV{CXXFLAGS}`继承环境变量 `CXXFLAGS` 的值（允许用户通过环境变量传递额外选项）。
  
  *  **向编译器添加头文件的搜索路径**
  
  ```cmake
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common/base)
  ```
  
  ​	这条CMake命令的作用是 ‌**向编译器添加头文件的搜索路径**‌，使得项目中的源代码可以方便地引用指定目录下的头文件，该命令会影响 ‌**后续所有目标**‌（如通过 `add_executable` 或 `add_library` 创建的目标），所有目标的编译都会包含此路径。
  
  最佳实践：
  
  ```cmake
  # 创建目标（如可执行文件或库）
  add_executable(my_app main.cpp)
  
  # 仅为此目标添加头文件路径
  target_include_directories(my_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common/base)
  ```
  
  *  **配置安装包**
  
  ```cmake
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5gui5")
  ```
  
  ​	声明该Debian包的运行时依赖，确保安装时会自动安装以下库：`libqt5core5a`（Qt5核心）、`libqt5gui5`（Qt5图形界面库），如果用户系统中未安装这些依赖，包管理器（如`apt`）会提示自动安装。
  
  * **自定义安装规则**
  
  ```cmake
  set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
      OWNER_READ
      OWNER_WRITE
      OWNER_EXECUTE
      GROUP_READ
  )
  ```
  
  ​	设置安装目录的默认权限，变量名`CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS`用于定义通过CMake `install(DIRECTORY ...)` 命令安装目录时的默认权限。
  
  ​	值得注意的是，该命令仅影响通过 `install(DIRECTORY ...)` 安装的目录，不影响单个文件的权限（文件权限由 `CMAKE_INSTALL_DEFAULT_FILE_PERMISSIONS` 控制）。**CMake中若没有显示指定安装目录，CMake不会自动推断或创建目录层级**‌，则CMake不会安装任何目录内容，需通过`install(DIRECTORY ...)`明确声明目录及其目标路径，以确保完整部署‌。
  
  ```cmake
  file(GLOB GLOB_BIN ${EXECUTABLE_OUTPUT_PATH}/nyuv)      # 收集构建生成的可执行文件nyuv，路径由EXECUTABLE_OUTPUT_PATH指定
  ```
  
  ​	`GLOB`用于匹配规则在指定的目录内找到所需的文件，并将这些文件的路径存储在变量中。
  
  ```cmake
  INSTALL(FILES ${GLOB_BIN} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUT)    # 安装可执行文件到目录 /usr/bin
  INSTALL(FILES ${GLOB_LIB} DESTINATION lib/nyuv)                         # 安装库文件到 /usr/lib/nyuv
  ```
  
  ​	安装可执行文件`{GLOB_BIN}`和库文件`{GLOB_LIB}`，路径为 `/usr/bin`和`/usr/lib/nyuv`，由 `CMAKE_INSTALL_PREFIX=/usr` 和 `DESTINATION bin` 决定）。可执行文件权限为所有者可执行。
  
  ```cmake
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/nyuv.desktop DESTINATION share/applications/) # 安装桌面启动程序
  ```
  
  ​	安装路径为`/usr/share/applications/nyuv.desktop`，使应用程序出现在系统菜单或启动器中（需 `.desktop` 文件符合规范）。
  
  ```cmake
  INSTALL(FILES ${EXECUTABLE_OUTPUT_PATH}/.nyuv.config DESTINATION etc/nyuv/ PERMISSIONS WORLD_WRITE WORLD_READ) # 安装配置文件
  ```
  
  ​	安装路径为 `/usr/etc/nyuv/.nyuv.config`，权限为全局可读可写（`rw-rw-rw-`，即 `666`）。
  
  *  **启用打包程序**
  
  ```cmake
  include(CPack)
  ```
  
  ​	在 `CMakeLists.txt` 的末尾添加 `include(CPack)`，在定义所有安装规则（`install()`）后调用 `include(CPack)`，以确保打包内容完整。在build目录下执行`cpack`即可开始打包，打包之前无需执行安装程序，只需完成配置和编译命令。

> 值得注意的是，安装文件时设置全局可写权限（如`WORLD_WRITE`），可能触发CPack的安全检查机制，导致打包失败‌。



* gui目录下的CMakeLists.txt

  ```cmake
  set(CMAKE_AUTOMOC ON)               # 启动自动元对象编译器MOC
  set(CMAKE_AUTOUIC ON)               # 启动自动用户界面编译器UIC
  set(CMAKE_AUTORCC ON)               # 启动自动资源编译器RCC
  set(CMAKE_INCLUDE_CURRENT_DIR ON)   # 将当前构建目录和源代码路径加入头文件搜索路径
  ```

  以上代码用于简化Qt项目的构建配置，自动化处理Qt特有的元对象、界面和资源编译过程。

  1. `set(CMAKE_AUTOMOC ON) `

     ​	这句命令启动自动MOC（Meta-Object Compiler），作用是自动扫面头文件（如含有Q_OBJECT的类），生成`moc_*.cpp`文件以支持Qt信号槽机制，开发者无需手动运行moc命令。

     **什么是MOC：**

     MOC是Qt框架的预处理器，在Qt开发中，MOC（元对象编译器）是一个核心工具，它通过扩展C++语言特性，支撑了Qt特有的功能，他会扫描代码中带有`Q_OBJECT`宏的类头文件（如`*.h`），并生成对应的`moc_*.cpp`文件，这些生成的代码实现Qt的元对象系统，包括：

     * 信号与槽：实现对象间通信
     * 动态属性：运行运行时修改/添加属性
     * 运行时类型信息RTTI：支持`qobject_cast`等动态类型转换

     举个例子，在nyuv代码中，MOC会生成这6个文件。

     ```c++
     moc_mainwindow.cpp
     moc_myFileTree.cpp
     moc_myInfoBar.cpp
     moc_myPrefer.cpp
     moc_myQImageviewer.cpp
     moc_update.cpp
     ```

     若禁用自动MOC，开发者需要手动处理MOC生成步骤，例如：

     ```cmake
     # 手动指定MOC处理的头文件
     qt5_wrap_cpp(MOC_FILES MyWidget.h)
     add_executable(MyApp main.cpp MyWidget.cpp ${MOC_FILES})
     ```

     **推荐始终启用AUTOMOC。**

  2. `set(CMAKE_AUTOUIC ON)`

     ​	这句命令启动自动UIC（User Interface Compiler），UIC是用于自动化处理Qt Designer创建的界面文件（.ui）的工具。输入是由Qt Designer生成的XML格式界面文件，例如`mydialog.ui`，描述了窗口、按钮布局等UI元素属性，输出文件为`ui_*.h`，例如`ui_mydialog.ui`，其中包好一个c++类，例如（`Ui::MyDialog`），封装了界面元素的创建和布局逻辑。

     若禁用自动UIC，开发者需要手动处理UIC生成，例如：

     ```cmake
     # 手动指定需要处理的.ui文件
     qt5_wrap_ui(UI_HEADERS mydialog.ui)
     
     # 将生成的ui_*.h文件添加到目标
     add_executable(MyApp main.cpp MyDialog.cpp ${UI_HEADERS})
     ```

     ​	在nyuv中未使用

  3. `set(CMAKE_AUTORCC ON) `

     ​	这句命令启动Qt的自动RCC（Resource Compiler）功能，RCC是用于自动化处理Qt资源文件（`.qrc`）的工具。它允许开发者将图片、图标、翻译文件等静态资源**直接嵌入到可执行文件中**，避免运行时依赖外部路径的问题。nyuv中通过`icon.qrc`嵌入图标。`icon.qrc`是**资源文件**，是一个XML格式的文件，用于声明需要嵌入到程序中的资源路径，自动RCC会调用RCC工具将`.qrc`文件编译为c++代码，如`qrc_icon.cpp`，其中包含资源的二进制数据（图片会被转为二进制数据），自动链接，无需开发者手动管理。

     若禁用自动RCC，开发者需要手动处理RCC生成，例如：

     ```cmake
     # 手动指定.qrc文件
     qt5_add_resources(RCC_SOURCES resources.qrc)
     
     # 将生成的代码加入目标
     add_executable(MyApp main.cpp ${RCC_SOURCES})
     ```

  4. `set(CMAKE_INCLUDE_CURRENT_DIR ON) `

     这句代码的作用是**自动将每个源文件所在的目录添加到编译器的头文件搜索路径中**‌。若未显式设置此选项，CMake不会自动添加当前目录到包含路径，此时头文件引用需通过一下方式之一：

     * 使用完整的相对路径

       ```c++
       #include "../src/MyClass.h"  // 假设源文件在 build/ 目录下编译
       ```

     * 手动添加包含路径：在CMake中显示指定目录

       ```cmake
       include_directories(${CMAKE_SOURCE_DIR}/src)  # 手动添加头文件目录
       ```

     在Qt项目中通常涉及**自动生成的代码**（如 `ui_*.h`, `moc_*.cpp`），这些文件默认生成在构建目录（如 `build/`）中。`CMAKE_INCLUDE_CURRENT_DIR ON` 对它们的处理至关重要：

     * 由自动UIC生成的UI头文件

       若由`ui_mydialog.ui`自动生成的`ui_*.h`位于构建目录如`build/`下，在代码 `MainWindow.cpp` 中需包含此头文件，若未启用 `CMAKE_INCLUDE_CURRENT_DIR`，需手动添加构建目录路径：

       ```cmake
       include_directories(${CMAKE_BINARY_DIR})  # 否则编译失败
       ```

     * **与自动MOC/UIC/RCC的协同**

       自动MOC/UIC/RCC生成的代码通常位于构建目录的子文件夹（如 `build/gui/mywidgets/mywidgets_autogen/include/moc_*.cpp`和`build/gui/qrc_icon.cpp`）,`CMAKE_INCLUDE_CURRENT_DIR ON` 会确保这些路径被自动包含，避免手动配置。

  * **查找Qt库**

    ```cmake
    find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)  # 查找并验证项目所需的Qt5库及其组件‌
    ```

    该代码的作用为在系统中搜索安装的Qt5开发环境，并加载其CMake配置。

    - ‌**依赖条件**‌：需确保Qt5已正确安装，且其路径已通过环境变量 `Qt5_DIR` 或在CMake中显式设置（如 `set(Qt5_DIR "/path/to/Qt5/lib/cmake/Qt5")`）。

    - 在系统中搜索安装的Qt5开发环境，并加载其CMake配置。

    `REQUIRED`表示严格模式，若任一组建未找到，CMake配置阶段会立即终止并报错，避免后续编译或链接出现错误。

  * **处理Qt资源文件**

    ```cmake
    file(GLOB_RECURSE QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)   # 递归搜索当前目录下的.qrc文件，并保存它们的路径到 QRC_SOURCE_FILES 中
    set(QRC_SOURCE_FILES icon.qrc)                                          # 将变量 QRC_SOURCE_FILES 的值覆盖为 icon.qrc
    qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})                        # 调用rcc工具生成c++代码，并将生成的文件路径保存到 QRC_FILES 
    ```

    上面第1句代码和第2句代码存在逻辑冲突，保存一句即可。`qt5_add_resources`生成的 `qrc_icon.cpp` 包含所有在 `icon.qrc` 中声明的资源（图片、样式表等）的二进制数据。

