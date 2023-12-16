#pragma once

#include <queue>
#include <iostream>
#include <string>

using std::string;

class NodeAVL
{
public:
	NodeAVL* _left, * _right;
	NodeAVL() : _key(""), _type(-1), _left(nullptr), _right(nullptr) {}
	NodeAVL(string x, int type) : _key(x), _type(type), _left(nullptr), _right(nullptr) {}
	string _key;
	int _type;
};

class AVLTree
{
private:

	int height(const NodeAVL*) const;

	void small_left_rotate(NodeAVL*&);
	void small_right_rotate(NodeAVL*&);
	void big_left_rotate(NodeAVL*&);
	void big_right_rotate(NodeAVL*&);

	void balance(NodeAVL*&);

	void insert(NodeAVL*&, string, int);

	void erase(NodeAVL*&, string);

	void bfs_out(NodeAVL*) const;

	int count(NodeAVL*, string) const;

	int get_type(NodeAVL*, string) const;

private:
	NodeAVL* _tree;

public:
	AVLTree() : _tree(nullptr) {}
	AVLTree(NodeAVL* other) : _tree(other) {}

	void insert(string, int);
	void erase(string);
	int count(string) const;
	int get_type(string) const;

	void bfs_out() const;
};