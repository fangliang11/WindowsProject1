#include <Windows.h>
#include<iostream>
#include<vector>
#include <string>
#include <stdio.h>
#include "allTest.h"

using namespace std;


Stu getStu(int x, int y) {
	Stu result;
	result.a = x;
	result.b = y;
	return result;
}

int main()
{
	int a = 2, b = 3;
	Stu test = getStu(a, b);
	printf("%d %d\n", test.a, test.b);



	system("pause");
	return 0;
}
