// B_TREE.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <cassert>
template <class key, class value>
struct B_TREE_NODE
{
	key *keys;
	value *values;
	B_TREE_NODE<key, value> **children;
	bool isLeaf;
	int n;
	void CopyKeyValue(B_TREE_NODE<key, value> *nodeFrom, int iFrom, int iTo)
	{
		assert(nullptr != nodeFrom);
		assert(iFrom >= 0);
		assert(iTo >= 0);
		this->keys[iTo] = nodeFrom->keys[iFrom];
		this->values[iTo] = nodeFrom->values[iFrom];
	}
	void SetKeyValue(int iTo, key k, value v)
	{
		assert(iTo >= 0);
		this->keys[iTo] = k;
		this->values[iTo] = v;
	}
	B_TREE_NODE(int minimalDegree) :isLeaf(false), n(0)
	{
		keys = new key[2 * minimalDegree - 1];
		values = new value[2 * minimalDegree - 1];
		children = new B_TREE_NODE<key, value> *[2 * minimalDegree];
	}
	~B_TREE_NODE()
	{
		delete[] keys;
		delete[] values;
		delete[] children;
	}
private:
	B_TREE_NODE()
	{
	}
};

template <class key, class value>
class B_TREE
{
	B_TREE_NODE<key, value> *root;
	int minimalDegree;
public:
	B_TREE(int minimalDegree) :minimalDegree(minimalDegree)
	{
		this->createRootNode();
		// leaf!!!
		this->root->isLeaf = true;
	}
	void Insert(key k, value v)
	{
		std::cout << "insert key " << k << std::endl;
		// main logic
		if (this->isFullRootNode())
		{
			this->splitFullRootNode();
		}
		this->insertNonFullNode(this->root, k, v);
		// print the tree after adding.
		this->printTree();
	}
	value Search(key k)
	{
		std::cout << "search key " << k << std::endl;
		// main logic
		auto result = this->searchKey(this->root, k);
		// print result
		std::cout << "result is " << result << std::endl;
		return result;
	}
	void Delete(key k)
	{
		std::cout << "delete key " << k << std::endl;
		// main logic
		this->deleteKey(this->root, k);
		// print the tree after removing.
		this->printTree();
	}
private:
	value searchKey(B_TREE_NODE<key, value> *node, key k)
	{
		std::cout << "search key " << k << " in node " << node << std::endl;
		int i = this->findEqualOrGreaterKeyIndex(node, k);
		// key in node
		if (this->isEqualKeyIndex(node, k, i))
		{
			return node->values[i];
		}
		// key NOT in node
		else
		{
			// no key
			if (node->isLeaf)
			{
				
				return value();
			}
			// key MAY in child node
			else
			{				
				return this->searchKey(node->children[i], k);
			}
		}
	}
	void splitNode(B_TREE_NODE<key, value> *node, int index)
	{
		std::cout << "split node" << std::endl;
		auto leftChildNode = node->children[index];
		// right child node
		auto rightChildNode = this->createNode();
		this->copyFullLeftNodeToEmptyRightNode(leftChildNode, rightChildNode);
		// parent node
		this->insertKeyValueChild(node, index, leftChildNode->keys[this->minimalNumberOfKeys()], leftChildNode->values[this->minimalNumberOfKeys()], rightChildNode);
		// left child node
		leftChildNode->n = this->minimalNumberOfKeys();
	}
	void insertNonFullNode(B_TREE_NODE<key, value> *node, key k, value v)
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
	B_TREE_NODE<key, value> *mergeNode(B_TREE_NODE<key, value> *node, int mid)
	{
		auto leftChild = node->children[mid], rightChild = node->children[mid + 1];
		auto nLeft = node->children[mid]->n;
		// move mid key in parent to left child
		leftChild->CopyKeyValue(node, mid, nLeft);
		nLeft++;
		auto nRight = 0;
		// move right child to left child
		while (nRight < rightChild->n)
		{
			leftChild->CopyKeyValue(rightChild, nRight, nLeft);
			leftChild->children[nLeft] = rightChild->children[nRight];
			nRight++;
			nLeft++;
		}
		leftChild->children[nLeft] = rightChild->children[nRight];
		leftChild->n += (1 + rightChild->n);
		delete rightChild;
		// shrink parent
		auto nParent = mid;
		while (nParent < node->n - 1)
		{
			node->CopyKeyValue(node, nParent + 1, nParent);
			node->children[nParent + 1] = node->children[nParent + 2];
			nParent++;
		}
		node->n--;
		if (0 == node->n && false == node->isLeaf)
		{
			delete this->root;
			this->root = leftChild;
			node = leftChild;
		}
		return node;
	}
	void deleteKey(B_TREE_NODE<key, value> *node, key k)
	{
		//std::cout << "delete key " << k << std::endl;
		auto i = this->findEqualOrGreaterKeyIndex(node, k);
		if (i < node->n && node->keys[i] == k)
		{
			if (node->isLeaf)
			{
				// max i = n - 2, so the last assignment is node->keys[n - 2] = node->keys[n - 1];
				while (i < node->n - 1)
				{
					node->CopyKeyValue(node, i + 1, i);
					i++;
				}
				node->n--;
				// fix bug, it should return or will not go into following code blocks. so add "return;" and "else" before the follwing first "if".
				return;
			}
			else if (node->children[i]->n >= this->minimalDegree)
			{
				// lift left max key
				node->CopyKeyValue(node->children[i], node->children[i]->n - 1, i);
				return this->deleteKey(node->children[i], node->children[i]->keys[node->children[i]->n - 1]);
			}
			else if (node->children[i + 1]->n >= this->minimalDegree)
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
		else if (node->children[i]->n >= this->minimalDegree)
		{
			return this->deleteKey(node->children[i], k);
		}
		else
		{
			// borrow 1 key from left node
			if (i > 0 && node->children[i - 1]->n >= this->minimalDegree)
			{
				auto mid = i - 1;
				// add 1 key to right node
				this->moveRightOneStep(node->children[mid + 1]);
				node->children[mid + 1]->CopyKeyValue(node, mid, 0);
				node->children[mid + 1]->children[0] = node->children[mid]->children[node->children[mid]->n];
				node->children[mid + 1]->n++;
				// change parent node
				node->CopyKeyValue(node->children[mid], node->children[mid]->n - 1, mid);
				// delete 1 key from left node
				node->children[mid]->n--;
				return this->deleteKey(node->children[i], k);
			}
			// borrow 1 key from right node
			if (i < node->n && node->children[i + 1]->n >= this->minimalDegree)
			{
				// add 1 key to left node
				node->children[i]->CopyKeyValue(node, i, node->children[i]->n);
				node->children[i]->children[node->children[i]->n + 1] = node->children[i + 1]->children[0];
				node->children[i]->n++;
				// change parent node
				node->CopyKeyValue(node->children[i + 1], 0, i);
				// delete 1 key from right node
				this->moveLeftOneStep(node->children[i + 1]);
				node->children[i + 1]->n--;
				return this->deleteKey(node->children[i], k);
			}
			auto mid = 0;
			if (i > 0)
			{
				mid = i - 1;
			}
			else if (i < node->n)
			{
				mid = i;
			}
			node = this->mergeNode(node, mid);
			// fix bug, because mergeNode will destroy "node" it could be invalid memory, so change mergeNode signarture, and redo delete key on the "node".
			return this->deleteKey(node, k);
		}
	}
private:
	bool isFullNode(B_TREE_NODE<key, value> *node)
	{
		return node->n == this->minimalDegree * 2 - 1;
	}
	bool isFullRootNode()
	{
		return this->isFullNode(this->root);
	}
	auto createNode()
	{
		return new B_TREE_NODE<key, value>(this->minimalDegree);
	}
	void createRootNode()
	{
		this->root = this->createNode();
		this->root->n = 0;
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
	int findEqualOrGreaterKeyIndex(B_TREE_NODE<key, value> *node, key k)
	{
		int i = 0;
		while (i < node->n && k > node->keys[i])
		{
			i++;
		}
		return i;
	}
	bool isEqualKeyIndex(B_TREE_NODE<key, value> *node, key k, int index)
	{
		return index < node->n && k == node->keys[index];
	}
	void copyFullLeftNodeToEmptyRightNode(B_TREE_NODE<key, value> *fullLeftNode, B_TREE_NODE<key, value> *emptyRightNode)
	{
		assert(nullptr != fullLeftNode);
		assert(fullLeftNode->n == this->maximalNumberOfKeys());
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
		emptyRightNode->n = this->minimalNumberOfKeys();
	}
	void insertKeyValueChild(B_TREE_NODE<key, value> *node, int index, key k, value v, B_TREE_NODE<key, value> *child)
	{
		for (int i = node->n; i > index; i--)
		{
			node->CopyKeyValue(node, i - 1, i);
		}
		node->SetKeyValue(index, k, v);
		if (false == node->isLeaf)
		{
			for (int i = node->n + 1; i > index + 1; i--)
			{
				node->children[i] = node->children[i - 1];
			}
			node->children[index + 1] = child;
		}
		node->n++;
	}
	void moveRightOneStep(B_TREE_NODE<key, value> *node)
	{
		assert(nullptr != node);
		assert(node->n < this->maximalNumberOfKeys());
		int n = node->n;		
		while (n > 0)
		{
			node->CopyKeyValue(node, n - 1, n);
			node->children[n + 1] = node->children[n];
			n--;
		}
		node->children[n + 1] = node->children[n];
	}
	void moveLeftOneStep(B_TREE_NODE<key, value> *node)
	{
		assert(nullptr != node);
		assert(node->n > 0);
		for (int k = 0; k < node->n - 1; k++)
		{
			node->CopyKeyValue(node, k + 1, k);
			node->children[k] = node->children[k + 1];
		}
		node->children[node->n - 1] = node->children[node->n];
	}	
	void printTree(B_TREE_NODE<key, value> *node)
	{
		std::cout << node << " node contains " << node->n << " keys." << std::endl;
		std::cout << "keys: " << std::endl;
		for (int i = 0; i < node->n; i++)
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
			for (int i = 0; i <= node->n; i++)
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
	B_TREE<int, int> bt(6);

	int key = 10;
	while (key > 0)
	{
		bt.Insert(key, key * 100);
		key--;
	}
	key = 10;
	while (key > 0)
	{
		bt.Delete(key);
		key--;
	}
	//bt.Insert(1, 111);
	//bt.Search(1);
	key = 100;
	while (key > 0)
	{
		bt.Insert(key, key * 100);
		key--;
	}
	while (key < 100)
	{
		key++;
		bt.Delete(key);
	}
	getchar();
	return 0;
}