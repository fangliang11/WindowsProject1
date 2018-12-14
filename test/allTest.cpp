#include<iostream>
#include<vector>
using namespace std;
int main()
{
	vector<int> a;
	a.push_back(1);
	a.push_back(2);
	a.push_back(3);
	vector<int> b;
	b.push_back(4);
	b.push_back(5);

	for (int i = 0; i < a.size(); i++)
	{
		cout << "vector a 中的元素： " <<a.at(i) << endl;
	}
	for (int i = 0; i < b.size(); i++)
	{
		cout << "vector b 中的元素： " << b.at(i) << endl;
	}
	b = a;
	for (int i = 0; i < b.size(); i++)
	{
		cout << "赋值后vector b 中的元素： " << b.at(i) << endl;
	}
	a.at(0) = 666;
	for (int i = 0; i < b.size(); i++)
	{
		cout << "用666赋值后vector b 中的元素： " << b.at(i) << endl;
	}

	system("pause");
	return 1;
}
