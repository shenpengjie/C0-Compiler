#include<iostream>
#include"c0.h"

#define stacksize 500
int main()
{
	cout << "Input c0 file?" << endl;
	cin >> fname;
	
	fin.open(fname, ios::in|ios::binary);
	if (!fin)
	{
		cout << "Fail to open this file!" << endl;
		return 0;
	}
	else
	{
		cout << "List object code?(Y/N)" << endl;
		cin >> fname;

	}
}
