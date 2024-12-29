#include <iostream>

#include "cvtbase.h"
#include "cvtinterface.h"

using namespace std;

int main(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        cout << argv[i] << " ";
    }
    cout << endl;

	REQUESTINFO requestInfo;

	//必须指定输入图像地址和最终输出图像地址
	requestInfo.ifilename = argv[1];
	requestInfo.ofilename = argv[2];

	//输入图像和输出图像的大小一致？
	//TODO： 是否必须一致？
	requestInfo.iformat.height = atoi(argv[3]);
	requestInfo.iformat.width = atoi(argv[4]);
	requestInfo.oformat.height = atoi(argv[3]);
	requestInfo.oformat.width = atoi(argv[4]);

	//只需要指定输入图像格式，字符串形式的
	//TODO：这意味着底层需要做一个字符串格式到枚举格式的对照表，是要分离的，不需要添加一种新cvt方法时就得替换所有方法
	string type = argv[5];

	RESULT ret = cvt_process(type, requestInfo);

	if (ret != RESULT::SUCCESS)
		cout << "failed" << endl;
	else
		cout << "passed" << endl;

	return 0;
}