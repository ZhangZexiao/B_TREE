#pragma once
#include <cassert>
template <typename TYPE_key, typename TYPE_value, typename TYPE_index>
struct B_TREE_NODE
{
	TYPE_key *keys;
	TYPE_value *values;
	using TYPE_b_tree_node_pointer = B_TREE_NODE<TYPE_key, TYPE_value, TYPE_index> *;
	TYPE_b_tree_node_pointer *children;
	TYPE_index numberOfKeys;
	bool isLeaf;
	/// 复制“指定结点”（nodeFrom）的“指定位置”（indexFrom）的“关键字”(key)和“值”(value)
	/// 到“本结点”(this)的“指定位置”(indexTo)。
	/// 假定：
	/// nodeFrom是非空指针，指向一个结点。
	/// indexFrom和indexTo是合法下标，大于等于零，小于等于最大关键字数。
	void CopyKeyValue(TYPE_b_tree_node_pointer, TYPE_index, TYPE_index);
	/// 复制“关键字”(key)和“值”(value)到“本结点”(this)的“指定位置”(indexTo)。
	/// 假定：
	/// indexTo是合法下标，大于等于零，小于等于最大关键字数。
	void SetKeyValue(TYPE_index, TYPE_key, TYPE_value);
	/// 构造函数
	/// isLeaf缺省值为false，因为除了“第一个创建的根结点的isLeaf是true”外，此后创建的根结点都是内部结点。
	/// 按照多数原则，isLeaf应该初始化为false。
	/// 在使用时，最佳实践是在构造函数外，显示地初始化它。
	/// numberOfKeys缺省值为0。
	B_TREE_NODE(TYPE_index);
	/// 析构函数
	~B_TREE_NODE();
private:
	/// 禁止缺省的构造函数。
	B_TREE_NODE();
};

/// 复制“指定结点”（nodeFrom）的“指定位置”（indexFrom）的“关键字”(key)和“值”(value)
/// 到“本结点”(this)的“指定位置”(indexTo)。
/// 假定：
/// nodeFrom是非空指针，指向一个结点。
/// indexFrom和indexTo是合法下标，大于等于零，小于等于最大关键字数。
template <typename TYPE_key, typename TYPE_value, typename TYPE_index>
void B_TREE_NODE<TYPE_key, TYPE_value, TYPE_index>::CopyKeyValue(TYPE_b_tree_node_pointer nodeFrom, TYPE_index indexFrom, TYPE_index indexTo)
{
	assert(nullptr != nodeFrom);
	assert(0 <= indexFrom);
	assert(0 <= indexTo);
	this->keys[indexTo] = nodeFrom->keys[indexFrom];
	this->values[indexTo] = nodeFrom->values[indexFrom];
}
/// 复制“关键字”(key)和“值”(value)到“本结点”(this)的“指定位置”(indexTo)。
/// 假定：
/// indexTo是合法下标，大于等于零，小于等于最大关键字数。
template <typename TYPE_key, typename TYPE_value, typename TYPE_index>
void B_TREE_NODE<TYPE_key, TYPE_value, TYPE_index>::SetKeyValue(TYPE_index indexTo, TYPE_key k, TYPE_value v)
{
	assert(0 <= indexTo);
	this->keys[indexTo] = k;
	this->values[indexTo] = v;
}
/// 构造函数
/// isLeaf缺省值为false，因为除了“第一个创建的根结点的isLeaf是true”外，此后创建的根结点都是内部结点。
/// 按照多数原则，isLeaf应该初始化为false。
/// 在使用时，最佳实践是在构造函数外，显示地初始化它。
/// numberOfKeys缺省值为0。
template <typename TYPE_key, typename TYPE_value, typename TYPE_index>
B_TREE_NODE<TYPE_key, TYPE_value, TYPE_index>::B_TREE_NODE(TYPE_index minimalDegree) :isLeaf(false), numberOfKeys(0)
{
	keys = new TYPE_key[2 * minimalDegree - 1];
	values = new TYPE_value[2 * minimalDegree - 1];
	children = new TYPE_b_tree_node_pointer[2 * minimalDegree];
}
/// 析构函数
template <typename TYPE_key, typename TYPE_value, typename TYPE_index>
B_TREE_NODE<TYPE_key, TYPE_value, TYPE_index>::~B_TREE_NODE()
{
	delete[] keys;
	delete[] values;
	delete[] children;
}
/// 禁止缺省的构造函数。
template <typename TYPE_key, typename TYPE_value, typename TYPE_index>
B_TREE_NODE<TYPE_key, TYPE_value, TYPE_index>::B_TREE_NODE()
{
}
