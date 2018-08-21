// B_TREE.cpp: 定义控制台应用程序的入口点。
//
// 函数注释
// 类单元测试
// 图形显示树
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
	void splitNode(TYPE_b_tree_node_pointer parentNode, TYPE_index childIndex)
	{
		std::cout << "split node" << std::endl;
		// right child node
		auto rightChildNode = this->createNode();
		auto leftChildNode = parentNode->children[childIndex];
		this->copyFullLeftNodeToEmptyRightNode(leftChildNode, rightChildNode);
		// parent node, enlarge parent node by inserting one "key value child"
		auto leftChildMiddleKey = leftChildNode->keys[this->minimalNumberOfKeys()];
		auto leftChildMiddleValue = leftChildNode->values[this->minimalNumberOfKeys()];
		this->insertKeyValueChild(parentNode, childIndex, leftChildMiddleKey, leftChildMiddleValue, rightChildNode);
		// left child node, shrink left child node
		leftChildNode->numberOfKeys = this->minimalNumberOfKeys();
	}
	void insertNonFullNode(TYPE_b_tree_node_pointer node, TYPE_key key, TYPE_value value)
	{
		std::cout << "insert non full node" << std::endl;
		if (node->isLeaf)
		{
			this->insertKeyValueChild(node, this->findEqualOrGreaterKeyIndex(node, key), key, value, nullptr);
		}
		else
		{
			auto middleIndex = this->findEqualOrGreaterKeyIndex(node, key);
			if (this->isFullNode(node->children[middleIndex]))
			{
				this->splitNode(node, middleIndex);
				if (key > node->keys[middleIndex])
				{
					middleIndex++;
				}
			}
			this->insertNonFullNode(node->children[middleIndex], key, value);
		}
	}
	// v#1, void mergeNode(B_TREE_NODE<key, value> *node, TYPE_index mid)
	// v#2, void mergeNode(B_TREE_NODE<key, value> *&node, TYPE_index mid)
	// be careful! node is referenced here, that means it will be changed in this funtion, it is not a good coding style.
	// v#3, B_TREE_NODE<key, value> *mergeNode(B_TREE_NODE<key, value> *node, TYPE_index mid)
	TYPE_b_tree_node_pointer mergeNode(TYPE_b_tree_node_pointer node, TYPE_index middleIndex)
	{
		auto leftChild = node->children[middleIndex], rightChild = node->children[middleIndex + 1];
		auto nLeft = node->children[middleIndex]->numberOfKeys;
		// move mid key in parent to left child
		leftChild->CopyKeyValue(node, middleIndex, nLeft);
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
		auto nParent = middleIndex;
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
	void deleteKey(TYPE_b_tree_node_pointer node, TYPE_key key)
	{
		//std::cout << "delete key " << k << std::endl;
		auto middleIndex = this->findEqualOrGreaterKeyIndex(node, key);		
		if (this->isEqualKeyIndex(node, key, middleIndex))
		{
			if (node->isLeaf)
			{
				// max i = n - 2, so the last assignment is node->keys[n - 2] = node->keys[n - 1];
				while (middleIndex < node->numberOfKeys - 1)
				{
					node->CopyKeyValue(node, middleIndex + 1, middleIndex);
					middleIndex++;
				}
				node->numberOfKeys--;
				// fix bug, it should return or will not go into following code blocks. so add "return;" and "else" before the follwing first "if".
				return;
			}
			else if (node->children[middleIndex]->numberOfKeys >= this->minimalDegree)
			{
				// lift left max key
				node->CopyKeyValue(node->children[middleIndex], node->children[middleIndex]->numberOfKeys - 1, middleIndex);
				return this->deleteKey(node->children[middleIndex], node->children[middleIndex]->keys[node->children[middleIndex]->numberOfKeys - 1]);
			}
			else if (node->children[middleIndex + 1]->numberOfKeys >= this->minimalDegree)
			{
				// lift right min key
				node->CopyKeyValue(node->children[middleIndex + 1], 0, middleIndex);
				return this->deleteKey(node->children[middleIndex + 1], node->children[middleIndex + 1]->keys[0]);
			}
			else
			{
				node = this->mergeNode(node, middleIndex);
				return this->deleteKey(node, key);
			}
		}
		else if (node->children[middleIndex]->numberOfKeys >= this->minimalDegree)
		{
			return this->deleteKey(node->children[middleIndex], key);
		}
		else
		{
			// borrow 1 key from left node
			if (middleIndex > 0 && node->children[middleIndex - 1]->numberOfKeys >= this->minimalDegree)
			{
				auto mid = middleIndex - 1;
				// add 1 key to right node
				this->moveRightOneStep(node->children[mid + 1]);
				node->children[mid + 1]->CopyKeyValue(node, mid, 0);
				node->children[mid + 1]->children[0] = node->children[mid]->children[node->children[mid]->numberOfKeys];
				node->children[mid + 1]->numberOfKeys++;
				// change parent node
				node->CopyKeyValue(node->children[mid], node->children[mid]->numberOfKeys - 1, mid);
				// delete 1 key from left node
				node->children[mid]->numberOfKeys--;
				return this->deleteKey(node->children[middleIndex], key);
			}
			// borrow 1 key from right node
			if (middleIndex < node->numberOfKeys && node->children[middleIndex + 1]->numberOfKeys >= this->minimalDegree)
			{
				// add 1 key to left node
				node->children[middleIndex]->CopyKeyValue(node, middleIndex, node->children[middleIndex]->numberOfKeys);
				node->children[middleIndex]->children[node->children[middleIndex]->numberOfKeys + 1] = node->children[middleIndex + 1]->children[0];
				node->children[middleIndex]->numberOfKeys++;
				// change parent node
				node->CopyKeyValue(node->children[middleIndex + 1], 0, middleIndex);
				// delete 1 key from right node
				this->moveLeftOneStep(node->children[middleIndex + 1]);
				node->children[middleIndex + 1]->numberOfKeys--;
				return this->deleteKey(node->children[middleIndex], key);
			}
			auto mid = middleIndex;
			if (middleIndex > 0)
			{
				mid = middleIndex - 1;
			}
			else if (middleIndex < node->numberOfKeys)
			{
				mid = middleIndex;
			}
			node = this->mergeNode(node, mid);
			// fix bug, because mergeNode will destroy "node" it could be invalid memory, so change mergeNode signarture, and redo delete key on the "node".
			return this->deleteKey(node, key);
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
	TYPE_index minimalNumberOfKeys()
	{
		return this->minimalDegree - 1;
	}
	TYPE_index maximalNumberOfKeys()
	{
		return this->minimalDegree * 2 - 1;
	}
	TYPE_index findEqualOrGreaterKeyIndex(TYPE_b_tree_node_pointer node, TYPE_key key)
	{
		TYPE_index middleIndex = 0;
		while (middleIndex < node->numberOfKeys && key > node->keys[middleIndex])
		{
			middleIndex++;
		}
		return middleIndex;
	}
	bool isEqualKeyIndex(TYPE_b_tree_node_pointer node, TYPE_key key, TYPE_index index)
	{
		return index < node->numberOfKeys && key == node->keys[index];
	}
	void copyFullLeftNodeToEmptyRightNode(TYPE_b_tree_node_pointer fullLeftNode, TYPE_b_tree_node_pointer emptyRightNode)
	{
		assert(nullptr != fullLeftNode);
		assert(fullLeftNode->numberOfKeys == this->maximalNumberOfKeys());
		// copy isLeaf
		emptyRightNode->isLeaf = fullLeftNode->isLeaf;
		// copy key value
		for (TYPE_index i = 0; i < this->minimalNumberOfKeys(); i++)
		{
			emptyRightNode->CopyKeyValue(fullLeftNode, this->minimalDegree + i, i);
		}
		// copy child
		if (false == fullLeftNode->isLeaf)
		{
			for (TYPE_index i = 0; i <= this->minimalNumberOfKeys(); i++)
			{
				emptyRightNode->children[i] = fullLeftNode->children[this->minimalDegree + i];
			}
		}
		emptyRightNode->numberOfKeys = this->minimalNumberOfKeys();
	}
	void insertKeyValueChild(TYPE_b_tree_node_pointer node, TYPE_index index, TYPE_key key, TYPE_value value, TYPE_b_tree_node_pointer child)
	{
		for (TYPE_index i = node->numberOfKeys; i > index; i--)
		{
			node->CopyKeyValue(node, i - 1, i);
		}
		node->SetKeyValue(index, key, value);
		if (false == node->isLeaf)
		{
			for (TYPE_index i = node->numberOfKeys + 1; i > index + 1; i--)
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
		TYPE_index numberOfKeys = node->numberOfKeys;		
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
		for (TYPE_index k = 0; k < node->numberOfKeys - 1; k++)
		{
			node->CopyKeyValue(node, k + 1, k);
			node->children[k] = node->children[k + 1];
		}
		node->children[node->numberOfKeys - 1] = node->children[node->numberOfKeys];
	}	
	void printTree(TYPE_b_tree_node_pointer node)
	{
		std::cout << node << " node contains " << node->numberOfKeys << " keys." << std::endl;
		if (node->numberOfKeys > 0)
		{
			std::cout << "keys: " << std::endl;
			for (TYPE_index i = 0; i < node->numberOfKeys; i++)
			{
				std::cout << "(" << node->keys[i] << ", " << node->values[i] << ") ";
			}
			std::cout << std::endl;
		}
		if (node->isLeaf)
		{
			std::cout << "it is leaf node." << std::endl;
		}
		else
		{
			std::cout << "it is internal node. it contains following childrens: " << std::endl;
			for (TYPE_index i = 0; i <= node->numberOfKeys; i++)
			{
				printTree(node->children[i]);
			}
		}
		//std::cout << std::endl;
	}
	void printTree()
	{
		std::cout << "Print tree: " << std::endl;
		this->printTree(this->root);
		std::cout << std::endl;
	}
};

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