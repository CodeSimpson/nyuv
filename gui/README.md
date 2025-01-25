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



完整的代码执行逻辑：

```c++
```



### 2. Qt类