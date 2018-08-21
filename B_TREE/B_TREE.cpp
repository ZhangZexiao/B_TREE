// B_TREE.cpp: 定义控制台应用程序的入口点。
//
// 函数注释
// 类单元测试
// 
#include "stdafx.h"
#include <iostream>
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
	void CopyKeyValue(TYPE_b_tree_node_pointer nodeFrom, TYPE_index indexFrom, TYPE_index indexTo)
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
	void SetKeyValue(TYPE_index indexTo, TYPE_key k, TYPE_value v)
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
	B_TREE_NODE(TYPE_index minimalDegree) :isLeaf(false), numberOfKeys(0)
	{
		keys = new TYPE_key[2 * minimalDegree - 1];
		values = new TYPE_value[2 * minimalDegree - 1];
		children = new TYPE_b_tree_node_pointer[2 * minimalDegree];
	}
	/// 析构函数
	~B_TREE_NODE()
	{
		delete[] keys;
		delete[] values;
		delete[] children;
	}
private:
	/// 禁止缺省的构造函数。
	B_TREE_NODE()
	{
	}
};

template <typename TYPE_key, typename TYPE_value, typename TYPE_index>
class B_TREE
{
	using TYPE_b_tree_node_pointer = B_TREE_NODE<TYPE_key, TYPE_value, TYPE_index> *;
	TYPE_b_tree_node_pointer root;
	TYPE_index minimalDegree;
public:
	B_TREE(TYPE_index minimalDegree) :minimalDegree(minimalDegree)
	{
		this->createRootNode();
		// leaf!!!
		this->root->isLeaf = true;
	}
	void Insert(TYPE_key key, TYPE_value value)
	{
		std::cout << "insert key " << key << std::endl;
		// main logic
		if (this->isFullRootNode())
		{
			this->splitFullRootNode();
		}
		this->insertNonFullNode(this->root, key, value);
		// print the tree after adding.
		this->printTree();
	}
	auto Search(TYPE_key key)
	{
		std::cout << "search key " << key << std::endl;
		// main logic
		auto result = this->searchKey(this->root, key);
		// print result
		std::cout << "result is " << result << std::endl;
		return result;
	}
	void Delete(TYPE_key key)
	{
		std::cout << "delete key " << key << std::endl;
		// main logic
		this->deleteKey(this->root, key);
		// print the tree after removing.
		this->printTree();
	}
private:
	TYPE_value searchKey(TYPE_b_tree_node_pointer node, TYPE_key key)
	{
		std::cout << "search key " << key << " in node " << node << std::endl;
		auto middleIndex = this->findEqualOrGreaterKeyIndex(node, key);
		// key in node
		if (this->isEqualKeyIndex(node, key, middleIndex))
		{
			return node->values[middleIndex];
		}
		// key NOT in node
		else
		{
			// no key
			if (node->isLeaf)
			{				
				return TYPE_value();
			}
			// key MAY in child node
			else
			{				
				return this->searchKey(node->children[middleIndex], key);
			}
		}
	}
	void splitNode(TYPE_b_tree_node_pointer node, int index)
	{
		std::cout << "split node" << std::endl;
		auto leftChildNode = node->children[index];
		// right child node
		auto rightChildNode = this->createNode();
		this->copyFullLeftNodeToEmptyRightNode(leftChildNode, rightChildNode);
		// parent node
		this->insertKeyValueChild(node, index, leftChildNode->keys[this->minimalNumberOfKeys()], leftChildNode->values[this->minimalNumberOfKeys()], rightChildNode);
		// left child node
		leftChildNode->numberOfKeys = this->minimalNumberOfKeys();
	}
	void insertNonFullNode(TYPE_b_tree_node_pointer node, TYPE_key k, TYPE_value v)
	{
		std::cout << "insert non full node" << std::endl;
		if (node->isLeaf)
		{
			this->insertKeyValueChild(node, this->findEqualOrGreaterKeyIndex(node, k), k, v, nullptr);
		}
		else
		{
			auto i = this->findEqualOrGreaterKeyIndex(node, k);
			if (this->isFullNode(node->children[i]))
			{
				this->splitNode(node, i);
				if (k > node->keys[i])
				{
					i++;
				}
			}
			this->insertNonFullNode(node->children[i], k, v);
		}
	}
	// v#1, void mergeNode(B_TREE_NODE<key, value> *node, int mid)
	// v#2, void mergeNode(B_TREE_NODE<key, value> *&node, int mid)
	// be careful! node is referenced here, that means it will be changed in this funtion, it is not a good coding style.
	// v#3, B_TREE_NODE<key, value> *mergeNode(B_TREE_NODE<key, value> *node, int mid)
	TYPE_b_tree_node_pointer mergeNode(TYPE_b_tree_node_pointer node, int mid)
	{
		auto leftChild = node->children[mid], rightChild = node->children[mid + 1];
		auto nLeft = node->children[mid]->numberOfKeys;
		// move mid key in parent to left child
		leftChild->CopyKeyValue(node, mid, nLeft);
		nLeft++;
		auto nRight = 0;
		// move right child to left child
		while (nRight < rightChild->numberOfKeys)
		{
			leftChild->CopyKeyValue(rightChild, nRight, nLeft);
			leftChild->children[nLeft] = rightChild->children[nRight];
			nRight++;
			nLeft++;
		}
		leftChild->children[nLeft] = rightChild->children[nRight];
		leftChild->numberOfKeys += (1 + rightChild->numberOfKeys);
		delete rightChild;
		// shrink parent
		auto nParent = mid;
		while (nParent < node->numberOfKeys - 1)
		{
			node->CopyKeyValue(node, nParent + 1, nParent);
			node->children[nParent + 1] = node->children[nParent + 2];
			nParent++;
		}
		node->numberOfKeys--;
		if (0 == node->numberOfKeys && false == node->isLeaf)
		{
			delete this->root;
			this->root = leftChild;
			node = leftChild;
		}
		return node;
	}
	void deleteKey(TYPE_b_tree_node_pointer node, TYPE_key k)
	{
		//std::cout << "delete key " << k << std::endl;
		auto i = this->findEqualOrGreaterKeyIndex(node, k);
		if (i < node->numberOfKeys && node->keys[i] == k)
		{
			if (node->isLeaf)
			{
				// max i = n - 2, so the last assignment is node->keys[n - 2] = node->keys[n - 1];
				while (i < node->numberOfKeys - 1)
				{
					node->CopyKeyValue(node, i + 1, i);
					i++;
				}
				node->numberOfKeys--;
				// fix bug, it should return or will not go into following code blocks. so add "return;" and "else" before the follwing first "if".
				return;
			}
			else if (node->children[i]->numberOfKeys >= this->minimalDegree)
			{
				// lift left max key
				node->CopyKeyValue(node->children[i], node->children[i]->numberOfKeys - 1, i);
				return this->deleteKey(node->children[i], node->children[i]->keys[node->children[i]->numberOfKeys - 1]);
			}
			else if (node->children[i + 1]->numberOfKeys >= this->minimalDegree)
			{
				// lift right min key
				node->CopyKeyValue(node->children[i + 1], 0, i);
				return this->deleteKey(node->children[i + 1], node->children[i + 1]->keys[0]);
			}
			else
			{
				node = this->mergeNode(node, i);
				return this->deleteKey(node, k);
			}
		}
		else if (node->children[i]->numberOfKeys >= this->minimalDegree)
		{
			return this->deleteKey(node->children[i], k);
		}
		else
		{
			// borrow 1 key from left node
			if (i > 0 && node->children[i - 1]->numberOfKeys >= this->minimalDegree)
			{
				auto mid = i - 1;
				// add 1 key to right node
				this->moveRightOneStep(node->children[mid + 1]);
				node->children[mid + 1]->CopyKeyValue(node, mid, 0);
				node->children[mid + 1]->children[0] = node->children[mid]->children[node->children[mid]->numberOfKeys];
				node->children[mid + 1]->numberOfKeys++;
				// change parent node
				node->CopyKeyValue(node->children[mid], node->children[mid]->numberOfKeys - 1, mid);
				// delete 1 key from left node
				node->children[mid]->numberOfKeys--;
				return this->deleteKey(node->children[i], k);
			}
			// borrow 1 key from right node
			if (i < node->numberOfKeys && node->children[i + 1]->numberOfKeys >= this->minimalDegree)
			{
				// add 1 key to left node
				node->children[i]->CopyKeyValue(node, i, node->children[i]->numberOfKeys);
				node->children[i]->children[node->children[i]->numberOfKeys + 1] = node->children[i + 1]->children[0];
				node->children[i]->numberOfKeys++;
				// change parent node
				node->CopyKeyValue(node->children[i + 1], 0, i);
				// delete 1 key from right node
				this->moveLeftOneStep(node->children[i + 1]);
				node->children[i + 1]->numberOfKeys--;
				return this->deleteKey(node->children[i], k);
			}
			auto mid = 0;
			if (i > 0)
			{
				mid = i - 1;
			}
			else if (i < node->numberOfKeys)
			{
				mid = i;
			}
			node = this->mergeNode(node, mid);
			// fix bug, because mergeNode will destroy "node" it could be invalid memory, so change mergeNode signarture, and redo delete key on the "node".
			return this->deleteKey(node, k);
		}
	}
private:
	bool isFullNode(TYPE_b_tree_node_pointer node)
	{
		return node->numberOfKeys == this->minimalDegree * 2 - 1;
	}
	bool isFullRootNode()
	{
		return this->isFullNode(this->root);
	}
	auto createNode()
	{
		return new B_TREE_NODE<TYPE_key, TYPE_value, TYPE_index>(this->minimalDegree);
	}
	void createRootNode()
	{
		this->root = this->createNode();
		this->root->numberOfKeys = 0;
	}
	void splitFullRootNode()
	{
		auto tmp = this->root;
		this->createRootNode();
		// leaf!!!
		this->root->isLeaf = false;
		this->root->children[0] = tmp;
		this->splitNode(this->root, 0);
	}
	int minimalNumberOfKeys()
	{
		return this->minimalDegree - 1;
	}
	int maximalNumberOfKeys()
	{
		return this->minimalDegree * 2 - 1;
	}
	int findEqualOrGreaterKeyIndex(TYPE_b_tree_node_pointer node, TYPE_key k)
	{
		int i = 0;
		while (i < node->numberOfKeys && k > node->keys[i])
		{
			i++;
		}
		return i;
	}
	bool isEqualKeyIndex(TYPE_b_tree_node_pointer node, TYPE_key k, int index)
	{
		return index < node->numberOfKeys && k == node->keys[index];
	}
	void copyFullLeftNodeToEmptyRightNode(TYPE_b_tree_node_pointer fullLeftNode, TYPE_b_tree_node_pointer emptyRightNode)
	{
		assert(nullptr != fullLeftNode);
		assert(fullLeftNode->numberOfKeys == this->maximalNumberOfKeys());
		// copy isLeaf
		emptyRightNode->isLeaf = fullLeftNode->isLeaf;
		// copy key value
		for (int i = 0; i < this->minimalNumberOfKeys(); i++)
		{
			emptyRightNode->CopyKeyValue(fullLeftNode, this->minimalDegree + i, i);
		}
		// copy child
		if (false == fullLeftNode->isLeaf)
		{
			for (int i = 0; i <= this->minimalNumberOfKeys(); i++)
			{
				emptyRightNode->children[i] = fullLeftNode->children[this->minimalDegree + i];
			}
		}
		// set n
		emptyRightNode->numberOfKeys = this->minimalNumberOfKeys();
	}
	void insertKeyValueChild(TYPE_b_tree_node_pointer node, int index, TYPE_key k, TYPE_value v, TYPE_b_tree_node_pointer child)
	{
		for (int i = node->numberOfKeys; i > index; i--)
		{
			node->CopyKeyValue(node, i - 1, i);
		}
		node->SetKeyValue(index, k, v);
		if (false == node->isLeaf)
		{
			for (int i = node->numberOfKeys + 1; i > index + 1; i--)
			{
				node->children[i] = node->children[i - 1];
			}
			node->children[index + 1] = child;
		}
		node->numberOfKeys++;
	}
	void moveRightOneStep(TYPE_b_tree_node_pointer node)
	{
		assert(nullptr != node);
		assert(node->numberOfKeys < this->maximalNumberOfKeys());
		int numberOfKeys = node->numberOfKeys;		
		while (numberOfKeys > 0)
		{
			node->CopyKeyValue(node, numberOfKeys - 1, numberOfKeys);
			node->children[numberOfKeys + 1] = node->children[numberOfKeys];
			numberOfKeys--;
		}
		node->children[numberOfKeys + 1] = node->children[numberOfKeys];
	}
	void moveLeftOneStep(TYPE_b_tree_node_pointer node)
	{
		assert(nullptr != node);
		assert(node->numberOfKeys > 0);
		for (int k = 0; k < node->numberOfKeys - 1; k++)
		{
			node->CopyKeyValue(node, k + 1, k);
			node->children[k] = node->children[k + 1];
		}
		node->children[node->numberOfKeys - 1] = node->children[node->numberOfKeys];
	}	
	void printTree(TYPE_b_tree_node_pointer node)
	{
		std::cout << node << " node contains " << node->numberOfKeys << " keys." << std::endl;
		std::cout << "keys: " << std::endl;
		for (int i = 0; i < node->numberOfKeys; i++)
		{
			std::cout << "(" << node->keys[i] << ", " << node->values[i] << ") ";
		}
		std::cout << std::endl;
		if (node->isLeaf)
		{
			std::cout << "it is leaf node." << std::endl;
		}
		else
		{
			std::cout << "it is internal node. it contains following childrens: " << std::endl;
			for (int i = 0; i <= node->numberOfKeys; i++)
			{
				printTree(node->children[i]);
			}
		}
		std::cout << std::endl;
	}
	void printTree()
	{
		std::cout << "Print tree: " << std::endl;
		this->printTree(this->root);
	}
};

int main()
{
	B_TREE<int, int, int> bt(6);

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
	TYPE_key = 100;
	while (TYPE_key > 0)
	{
		bt.Insert(TYPE_key, TYPE_key * 100);
		TYPE_key--;
	}
	while (TYPE_key < 100)
	{
		TYPE_key++;
		bt.Delete(TYPE_key);
	}
	getchar();
	return 0;
}