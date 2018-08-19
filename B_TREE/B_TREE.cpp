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
	//B_TREE_NODE<key, value> &operator=(const B_TREE_NODE<key, value> &)
	//{
	//}
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
		this->printTree(this->root);
	}
	value Search(key k)
	{
		return this->searchKey(this->root, k);
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
	bt.Insert(1, 111);
	bt.Search(1);
	getchar();
    return 0;
}