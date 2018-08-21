// B_TREE.cpp: 定义控制台应用程序的入口点。
//
// 函数注释
// 类单元测试
// 图形显示树
#include "stdafx.h"
//#include <iostream>
//#include <cassert>
#include "TYPE_b_tree_node.h"
#include "TYPE_b_tree.h"
int main()
{
	B_TREE<unsigned long long, unsigned long long, unsigned long long> bt(6);

	int TYPE_key = 10;
	while (TYPE_key > 0)
	{
		bt.Insert(TYPE_key, TYPE_key * 100);
		TYPE_key--;
	}
	TYPE_key = 10;
	while (TYPE_key > 0)
	{
		bt.Delete(TYPE_key);
		TYPE_key--;
	}
	//bt.Insert(1, 111);
	//bt.Search(1);
	TYPE_key = 30;
	while (TYPE_key > 0)
	{
		bt.Insert(TYPE_key, TYPE_key * 100);
		TYPE_key--;
	}
	while (TYPE_key < 30)
	{
		TYPE_key++;
		bt.Delete(TYPE_key);
	}
	getchar();
	return 0;
}