#include "objectivelib.h"
#include <iostream>
#include "example.h"
using namespace objectivelib;
int main() 
{
#ifdef DEVELOP
	main_self();
#else
	//���ó�ʼ���ӿ�
	OBJECTIVELIB_Init();
	
	//����dump
	OBJECTIVELIB_DumpTrigger();
#endif // DEVELOP
	return 0;
}
