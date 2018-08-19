// B_TREE.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
template <class key, class value>
struct B_TREE_NODE
{
	key *keys;
	value *values;
	B_TREE_NODE<key, value> **sbs;
	bool isLeaf;
	int n;
	B_TREE_NODE(int minimalDegree):isLeaf(false), n(0)
	{
		keys = new key[2 * minimalDegree - 1];
		values = new value[2 * minimalDegree - 1];
		sbs = new B_TREE_NODE<key, value> *[2 * minimalDegree];
	}
	~B_TREE_NODE()
	{
		delete[] keys;
		delete[] values;
		delete[] sbs;
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
	B_TREE(int minimalDegree):minimalDegree(minimalDegree)
	{
		this->root = new B_TREE_NODE<key, value>(minimalDegree);
		// leaf!!!
		this->root->isLeaf = true;
		this->root->n = 0;
	}
	void Insert(key k, value v)
	{
		std::cout << "insert key " << k << std::endl;
		if (this->root->n == this->minimalDegree * 2 - 1)
		{
			B_TREE_NODE<key, value> *tmp = this->root;
			this->root = new B_TREE_NODE<key, value>(this->minimalDegree);
			// leaf!!!
			this->root->isLeaf = false;
			this->root->n = 0;
			this->root->sbs[0] = tmp;
			this->splitNode(this->root, 0);
		}
		this->insertNonFullNode(this->root, k, v);
		this->printTree();
	}
	value Search(key k)
	{
		this->printTree();
		return this->searchKey(this->root, k);
	}
	void Delete(key k)
	{
		return this->deleteKey(this->root, k);
	}
private:
	value searchKey(B_TREE_NODE<key, value> *node, key k)
	{
		std::cout << "search node for key " << k << " in node " << node << std::endl;
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
			return this->searchKey(node->sbs[i], k);
		}
	}
	void splitNode(B_TREE_NODE<key, value> *node, int index)
	{
		std::cout << "split full node" << std::endl;
		// get the child node which to be splitted, finally becoming left child node. 
		// this child node is full currently, which means it contains (2 * minimal degree - 1) keys, 
		// which equals (minimal degree - 1) + 1 + (minimal degree - 1).
		B_TREE_NODE<key, value> *leftChildNode = node->sbs[index];
		// create the right child node
		B_TREE_NODE<key, value> *rightChildNode = new B_TREE_NODE<key, value>(this->minimalDegree);
		// leaf!!!
		rightChildNode->isLeaf = leftChildNode->isLeaf;
		rightChildNode->n = this->minimalDegree - 1;
		// copy keys and values, how many? minimal degree - 1
		for (int i = 0; i < rightChildNode->n; i++)
		{
			rightChildNode->keys[i] = leftChildNode->keys[this->minimalDegree + i];
			rightChildNode->values[i] = leftChildNode->values[this->minimalDegree + i];
		}
		// copy childs, how many? minimal degree
		if (leftChildNode->isLeaf == false)
		{
			for (int i = 0; i <= rightChildNode->n; i++)
			{
				rightChildNode->sbs[i] = leftChildNode->sbs[this->minimalDegree + i];
			}
		}
		// end of the right child node creation, begin of the left child node
		leftChildNode->n = this->minimalDegree - 1;
		// end of the left child node, begin of the parent node
		// move child nodes before left child node 1 step to right
		for (int i = node->n + 1; i > index + 1; i--)
		{
			node->sbs[i] = node->sbs[i - 1];
		}
		// fill the right child node
		node->sbs[index + 1] = rightChildNode;
		// move keys and values including index node but not before index node
		for (int i = node->n; i > index; i--)
		{
			node->keys[i] = node->keys[i - 1];
			node->values[i] = node->values[i - 1];
		}
		// fill key and value from left child node
		node->keys[index] = leftChildNode->keys[leftChildNode->n];
		node->values[index] = leftChildNode->values[leftChildNode->n];
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
				node->keys[i + 1] = node->keys[i];
				node->values[i + 1] = node->values[i];
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
			if (node->sbs[i]->n == this->minimalDegree * 2 - 1)
			{
				this->splitNode(node, i);
				if (k > node->keys[i])
				{
					i++;
				}
			}
			this->insertNonFullNode(node->sbs[i], k, v);
		}
	}
	void mergeNode(B_TREE_NODE<key, value> *&node, int mid)
	{
		auto leftChild = node->sbs[mid], rightChild = node->sbs[mid + 1];
		auto nLeft = node->sbs[mid]->n;
		// parent to left child
		leftChild->keys[nLeft] = node->keys[mid];
		leftChild->values[nLeft] = node->values[mid];
		nLeft++;
		auto nRight = 0;
		// right child to left child
		while (nRight < rightChild->n)
		{
			leftChild->keys[nLeft] = rightChild->keys[nRight];
			leftChild->values[nLeft] = rightChild->values[nRight];
			// fix bug, it should be "nLeft + 1", not "nLeft".
			// revert fixing, it should be "nLeft".
			leftChild->sbs[nLeft] = rightChild->sbs[nRight];
			nRight++;
			nLeft++;
		}
		// fix bug, it should be "nLeft + 1", not "nLeft".
		// revert fixing, it should be "nLeft".
		leftChild->sbs[nLeft] = rightChild->sbs[nRight];
		leftChild->n += (1 + rightChild->n);
		delete rightChild;
		// move parent
		auto nParent = mid;
		while (nParent < node->n - 1)
		{
			node->keys[nParent] = node->keys[nParent + 1];
			node->values[nParent] = node->values[nParent + 1];
			node->sbs[nParent + 1] = node->sbs[nParent + 2];
		}
		node->n--;
		if (node->n == 0 && !node->isLeaf)
		{
			delete this->root;
			this->root = leftChild;
			node = leftChild;
		}
	}
	void deleteKey(B_TREE_NODE<key, value> *node, key k)
	{
		std::cout << "delete key " << k << std::endl;
		this->printTree();
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
					node->keys[i] = node->keys[i + 1];
					node->values[i] = node->values[i + 1];
				}
				node->n--;
				// fix bug, it should return or will not go into following code blocks. so add "return;" and "else" before the follwing first "if".
				return;
			}
			else if (node->sbs[i]->n >= this->minimalDegree)
			{
				node->keys[i] = node->sbs[i]->keys[node->sbs[i]->n - 1];
				node->values[i] = node->sbs[i]->values[node->sbs[i]->n - 1];
				return this->deleteKey(node->sbs[i], node->sbs[i]->keys[node->sbs[i]->n - 1]);
			}
			else if (node->sbs[i + 1]->n >= this->minimalDegree)
			{
				node->keys[i] = node->sbs[i + 1]->keys[0];
				node->values[i] = node->sbs[i + 1]->values[0];
				return this->deleteKey(node->sbs[i + 1], node->sbs[i + 1]->keys[0]);
			}
			else
			{
				this->mergeNode(node, i);
				return this->deleteKey(node, k);
			}
		}
		else if (node->sbs[i]->n >= this->minimalDegree)
		{
			return this->deleteKey(node->sbs[i], k);
		}
		else
		{
			// left node
			if (i > 0 && node->sbs[i - 1]->n >= this->minimalDegree)
			{
				auto mid = i - 1;
				// change i node
				int nRight = node->sbs[mid + 1]->n;
				while (nRight > 0)
				{
					node->sbs[mid + 1]->keys[nRight] = node->sbs[mid + 1]->keys[nRight - 1];
					node->sbs[mid + 1]->values[nRight] = node->sbs[mid + 1]->values[nRight - 1];
					node->sbs[mid + 1]->sbs[nRight + 1] = node->sbs[mid + 1]->sbs[nRight];
					nRight--;
				}
				node->sbs[mid + 1]->sbs[nRight + 1] = node->sbs[mid + 1]->sbs[nRight];
				node->sbs[mid + 1]->keys[0] = node->keys[mid];
				node->sbs[mid + 1]->values[0] = node->values[mid];
				node->sbs[mid + 1]->sbs[0] = node->sbs[mid]->sbs[node->sbs[mid]->n];
				node->sbs[mid + 1]->n++;
				// change node
				node->keys[mid] = node->sbs[mid]->keys[node->sbs[mid]->n - 1];
				node->values[mid] = node->sbs[mid]->values[node->sbs[mid]->n - 1];
				// change i - 1 node internal
				node->sbs[mid]->n--;				
				return this->deleteKey(node->sbs[i], k);
			}
			if (i < node->n && node->sbs[i + 1]->n >= this->minimalDegree)
			{
				auto mid = i;
				node->sbs[i]->keys[node->sbs[i]->n] = node->keys[i];
				node->sbs[i]->values[node->sbs[i]->n] = node->values[i];
				node->sbs[i]->sbs[node->sbs[i]->n + 1] = node->sbs[i + 1]->sbs[0];
				node->sbs[i]->n++;
				node->keys[i] = node->sbs[i + 1]->keys[0];
				node->values[i] = node->sbs[i + 1]->values[0];
				for (int k = 0; k < node->sbs[i + 1]->n - 1; k++)
				{
					node->sbs[i + 1]->keys[k] = node->sbs[i + 1]->keys[k + 1];
					node->sbs[i + 1]->values[k] = node->sbs[i + 1]->values[k + 1];
					node->sbs[i + 1]->sbs[k] = node->sbs[i + 1]->sbs[k + 1];
				}
				node->sbs[i + 1]->sbs[node->sbs[i + 1]->n - 1] = node->sbs[i + 1]->sbs[node->sbs[i + 1]->n];
				node->sbs[i + 1]->n--;
				return this->deleteKey(node->sbs[i], k);
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
			this->mergeNode(node, mid);
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
				printTree(node->sbs[i]);
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