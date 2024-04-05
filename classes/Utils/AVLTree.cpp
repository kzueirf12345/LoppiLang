#include "AVLTree.h"


int AVLTree::height(const NodeAVL* t) const
{
	if (t == nullptr) return 0;
	int h1 = height(t->_left), h2 = height(t->_right);
	return (h1 > h2 ? h1 : h2) + 1;
}

void AVLTree::small_left_rotate(NodeAVL*& t)
{
	if (t == nullptr) return;
	NodeAVL* p = t->_right;
	t->_right = p->_left;
	p->_left = t;
	t = p;
	if (this->_tree == t->_left) this->_tree = t;
}

void AVLTree::small_right_rotate(NodeAVL*& t)
{
	if (t == nullptr) return;
	NodeAVL* p = t->_left;
	t->_left = p->_right;
	p->_right = t;
	t = p;
	if (this->_tree == t->_right) this->_tree = t;
}

void AVLTree::big_left_rotate(NodeAVL*& t)
{
	if (t == nullptr) return;
	NodeAVL* p = t->_right, * q = p->_left;
	t->_right = q->_left;
	p->_left = q->_right;
	q->_left = t;
	q->_right = p;
	t = q;
	if (this->_tree == t->_left) this->_tree = t;
}

void AVLTree::big_right_rotate(NodeAVL*& t)
{
	if (t == nullptr) return;
	NodeAVL* p = t->_left, * q = p->_right;
	t->_left = q->_right;
	p->_right = q->_left;
	q->_left = p;
	q->_right = t;
	t = q;
	if (this->_tree == t->_right) this->_tree = t;
}

void AVLTree::balance(NodeAVL*& t)
{
	if (t == nullptr) return;

	int hl = height(t->_left), hr = height(t->_right);
	if (-1 <= hl - hr && hl - hr <= 1) return;

	if (hr - hl == 2) {
		NodeAVL* p = t->_right;
		if (height(p->_left) - height(p->_right) == 1) {
			big_left_rotate(t);
		}
		else {
			small_left_rotate(t);
		}
	}
	else if (hl - hr == 2) {
		NodeAVL* p = t->_left;
		if (height(p->_right) - height(p->_left) == 1) {
			big_right_rotate(t);
		}
		else {
			small_right_rotate(t);
		}
	}
}

void AVLTree::insert(NodeAVL*& t, string x, int type)
{
	if (t == nullptr) {
		t = new NodeAVL(x, type);
		return;
	}
	if (x < t->_key) insert(t->_left, x, type);
	else if (x > t->_key) insert(t->_right, x, type);
	else return;

	balance(t);
}

void AVLTree::insert(string x, int type)
{
	insert(this->_tree, x, type);
}

void AVLTree::erase(NodeAVL*& t, string x)
{
	if (t == nullptr) return;
	if (x < t->_key) {
		erase(t->_left, x);
		balance(t);
		return;
	}
	else if (x > t->_key) {
		erase(t->_right, x);
		balance(t);
		return;
	}

	NodeAVL* p = t->_right;
	if (p == nullptr) {
		NodeAVL* q = t;
		t = t->_left;
		delete q;
		return;
	}

	while (p->_left != nullptr) p = p->_left;
	string tmp = p->_key;
	p->_key = t->_key;
	t->_key = tmp;
	erase(t->_right, x);
	balance(t);
}

void AVLTree::erase(string x)
{
	erase(this->_tree, x);
}

void AVLTree::bfs_out(NodeAVL* t) const
{
	if (t == nullptr) return;

	std::queue<NodeAVL*> q;
	q.push(t);

	while (!q.empty()) {
		NodeAVL* tmp = q.front();
		q.pop();
		std::cout << tmp->_key << " " << tmp->_type << "\n";
		if (tmp->_left != nullptr)
			q.push(tmp->_left);
		if (tmp->_right != nullptr)
			q.push(tmp->_right);
	}
}

int AVLTree::count(NodeAVL* t, string x) const
{
	if (t == nullptr) return 0;
	if (t->_key == x) return 1;
	if (t->_key < x) return count(t->_right, x);
	else return count(t->_left, x);
}

int AVLTree::get_type(NodeAVL* t, string x) const
{
	if (t == nullptr) return -1;
	if (t->_key == x) return t->_type;
	if (t->_key < x) return get_type(t->_right, x);
	else return get_type(t->_left, x);
}

int AVLTree::count(string x) const
{
	return count(_tree, x);
}

int AVLTree::get_type(string x) const
{
	return get_type(_tree, x);
}

void AVLTree::bfs_out() const
{
	bfs_out(this->_tree);
}