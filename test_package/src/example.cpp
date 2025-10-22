#include "objectivelib.h"
#include <iostream>
#include "example.h"
using namespace objectivelib;
int main() 
{
#ifdef DEVELOP
	main_self();
#else
	//调用初始化接口
	OBJECTIVELIB_Init();
	
	//测试dump
	OBJECTIVELIB_DumpTrigger();
#endif // DEVELOP
	return 0;
}
