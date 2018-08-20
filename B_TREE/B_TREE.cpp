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
	void CopyKeyValue(B_TREE_NODE<key, value> *from, int iFrom, int iTo)
	{
		assert(nullptr != from);
		this->keys[iTo] = from->keys[iFrom];
		this->values[iTo] = from->values[iFrom];
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
		if (this->isFullRootNode())
		{
			this->splitFullRootNode();
		}
		this->insertNonFullNode(this->root, k, v);
	}
	value Search(key k)
	{
		std::cout << "search key " << k << std::endl;
		return this->searchKey(this->root, k);
	}
	void Delete(key k)
	{
		std::cout << "delete key " << k << std::endl;
		return this->deleteKey(this->root, k);
	}
private:
	value searchKey(B_TREE_NODE<key, value> *node, key k)
	{
		std::cout << "search key " << k << " in node " << node << std::endl;
		int i = 0;
		while (i < node->n && k > node->keys[i])
		{
			i++;
		}
		if (i < node->n && k == node->keys[i])
		{
			std::cout << "result is " << node->values[i] << std::endl;
			return node->values[i];
		}
		else if (node->isLeaf)
		{
			return value();
		}
		else
		{
			return this->searchKey(node->children[i], k);
		}
	}
	void splitNode(B_TREE_NODE<key, value> *node, int index)
	{
		std::cout << "split node" << std::endl;
		auto leftChildNode = node->children[index];
		// right child node
		auto rightChildNode = this->createNode();
		// leaf!!!
		rightChildNode->isLeaf = leftChildNode->isLeaf;
		rightChildNode->n = this->minimalDegree - 1;
		for (int i = 0; i < rightChildNode->n; i++)
		{
			rightChildNode->CopyKeyValue(leftChildNode, this->minimalDegree + i, i);
		}
		if (leftChildNode->isLeaf == false)
		{
			for (int i = 0; i <= rightChildNode->n; i++)
			{
				rightChildNode->children[i] = leftChildNode->children[this->minimalDegree + i];
			}
		}
		// left child node
		leftChildNode->n = this->minimalDegree - 1;
		// parent node
		for (int i = node->n + 1; i > index + 1; i--)
		{
			node->children[i] = node->children[i - 1];
		}
		node->children[index + 1] = rightChildNode;
		for (int i = node->n; i > index; i--)
		{
			node->CopyKeyValue(node, i - 1, i);
		}
		node->CopyKeyValue(leftChildNode, leftChildNode->n, index);
		node->n++;
	}
	void insertNonFullNode(B_TREE_NODE<key, value> *node, key k, value v)
	{
		std::cout << "insert non full node" << std::endl;
		if (node->isLeaf)
		{
			int i = node->n - 1;
			while (i >= 0 && node->keys[i] > k)
			{
				node->CopyKeyValue(node, i, i + 1);
				std::cout << "move key " << node->keys[i] << " from position " << i << " to position " << i + 1 << std::endl;
				i--;
			}
			node->keys[i + 1] = k;
			node->values[i + 1] = v;
			std::cout << "assign key " << k << " to position " << i + 1 << std::endl;
			node->n++;
		}
		else
		{
			int i = node->n - 1;
			while (i >= 0 && node->keys[i] > k)
			{
				i--;
			}
			i++;
			if (node->children[i]->n == this->minimalDegree * 2 - 1)
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
		// parent to left child
		leftChild->CopyKeyValue(node, mid, nLeft);
		nLeft++;
		auto nRight = 0;
		// right child to left child
		while (nRight < rightChild->n)
		{
			leftChild->CopyKeyValue(rightChild, nRight, nLeft);
			// fix bug, it should be "nLeft + 1", not "nLeft".
			// revert fixing, it should be "nLeft".
			leftChild->children[nLeft] = rightChild->children[nRight];
			nRight++;
			nLeft++;
		}
		// fix bug, it should be "nLeft + 1", not "nLeft".
		// revert fixing, it should be "nLeft".
		leftChild->children[nLeft] = rightChild->children[nRight];
		leftChild->n += (1 + rightChild->n);
		delete rightChild;
		// move parent
		auto nParent = mid;
		while (nParent < node->n - 1)
		{
			node->CopyKeyValue(node, nParent + 1, nParent);
			node->children[nParent + 1] = node->children[nParent + 2];
		}
		node->n--;
		if (node->n == 0 && !node->isLeaf)
		{
			delete this->root;
			this->root = leftChild;
			node = leftChild;
		}
		return node;
	}
	void deleteKey(B_TREE_NODE<key, value> *node, key k)
	{
		std::cout << "delete key " << k << std::endl;
		int i = 0;
		// i stop at keys[i] = k or keys[i] > k or i == node->n
		while (i < node->n && node->keys[i] < k)
		{
			i++;
		}
		if (i < node->n && node->keys[i] == k)
		{
			if (node->isLeaf)
			{
				// max i = n - 2, so the last assignment is node->keys[n - 2] = node->keys[n - 1];
				while (i < node->n - 1)
				{
					node->CopyKeyValue(node, i + 1, i);
				}
				node->n--;
				// fix bug, it should return or will not go into following code blocks. so add "return;" and "else" before the follwing first "if".
				return;
			}
			else if (node->children[i]->n >= this->minimalDegree)
			{
				node->CopyKeyValue(node->children[i], node->children[i]->n - 1, i);
				return this->deleteKey(node->children[i], node->children[i]->keys[node->children[i]->n - 1]);
			}
			else if (node->children[i + 1]->n >= this->minimalDegree)
			{
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
			// left node
			if (i > 0 && node->children[i - 1]->n >= this->minimalDegree)
			{
				auto mid = i - 1;
				// change i node
				int nRight = node->children[mid + 1]->n;
				while (nRight > 0)
				{
					node->children[mid + 1]->CopyKeyValue(node->children[mid + 1], nRight - 1, nRight);
					node->children[mid + 1]->children[nRight + 1] = node->children[mid + 1]->children[nRight];
					nRight--;
				}
				node->children[mid + 1]->children[nRight + 1] = node->children[mid + 1]->children[nRight];
				node->children[mid + 1]->CopyKeyValue(node, mid, 0);
				node->children[mid + 1]->children[0] = node->children[mid]->children[node->children[mid]->n];
				node->children[mid + 1]->n++;
				// change node
				node->CopyKeyValue(node->children[mid], node->children[mid]->n - 1, mid);
				// change i - 1 node internal
				node->children[mid]->n--;
				return this->deleteKey(node->children[i], k);
			}
			if (i < node->n && node->children[i + 1]->n >= this->minimalDegree)
			{
				auto mid = i;
				node->children[i]->CopyKeyValue(node, i, node->children[i]->n);
				node->children[i]->children[node->children[i]->n + 1] = node->children[i + 1]->children[0];
				node->children[i]->n++;
				node->keys[i] = node->children[i + 1]->keys[0];
				node->values[i] = node->children[i + 1]->values[0];
				for (int k = 0; k < node->children[i + 1]->n - 1; k++)
				{
					node->children[i + 1]->CopyKeyValue(node->children[i + 1], k + 1, k);
					node->children[i + 1]->children[k] = node->children[i + 1]->children[k + 1];
				}
				node->children[i + 1]->children[node->children[i + 1]->n - 1] = node->children[i + 1]->children[node->children[i + 1]->n];
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
	void printTree(B_TREE_NODE<key, value> *node)
	{
		std::cout << "This node contains " << node->n << " keys." << std::endl;
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
	B_TREE<int, int> bt(2);

	int i = 10;
	while (i > 0)
	{
		bt.Insert(i, i * 100);
		i--;
	}
	i = 10;
	while (i > 0)
	{
		bt.Delete(i);
		i--;
	}
	bt.Insert(1, 111);
	bt.Search(1);
	getchar();
	return 0;
}