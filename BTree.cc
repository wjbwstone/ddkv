// SPDX-License-Identifier: Proprietary
/*
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 */

#include "BTree.h"

ws::u32 ceiling(ws::u32 order) {
	if (order % 2 == 0)
		return order / 2;
	return order / 2 + 1;
}

class TreeNode {
public:
	TreeNode(ws::u32 order, bool leaf = false) :
		_isLeaf(leaf),
		_parent(nullptr),
		_keyCount(0) {
		_sibling = nullptr;
		_keys = (ws::u64*)calloc(1, (order * sizeof(ws::u64)));
		_elements = (void **)calloc(1, (order * sizeof(void *)));
	}

	~TreeNode() {
		free (_keys);
		free (_elements);
	}

public:
	void insert(ws::u64 key, void *element, ws::u32 order) {
		for (ws::u32 i = 0; i < _keyCount; ++i) {
			if (key == _keys[i]) {
				_elements[i] = element;
				return;
			}
		}

		assert(_keyCount < order);

		ws::u32 pos = 0;
		while (pos < _keyCount && _keys[pos] < key) {
			pos++;
		}

		for (ws::u32 i = _keyCount; i > pos; --i) {
			_keys[i] = _keys[i - 1];
			_elements[i] = _elements[i - 1];
		}

		_keys[pos] = key;
		_elements[pos] = element;

		_keyCount++;
	}

	TreeNode *split(bool& insertLeft, ws::u64 insertKey, ws::u64 order) {
		insertLeft = false;

		ws::u32 pos = ceiling(order);
		if ((0 != (order % 2)) && _keys[pos - 1] > insertKey) {
			pos = pos - 1;
			insertLeft = true;
		}

		auto node = new TreeNode(order);

		node->_isLeaf = _isLeaf;
		node->_sibling = _sibling;
		_sibling = node;

		ws::u32 count = 0;
		for (ws::u32 i = pos; i < _keyCount; ++i) {
			count++;
			node->insert(_keys[i], _elements[i], order);
			this->_keys[i] = 0;
			this->_elements[i] = nullptr;
		}

		if (0 != count) {
			this->_keyCount -= count;
		}

		if (!node->_isLeaf) {
			for (ws::u32 i = 0; i < node->_keyCount; ++i) {
				auto ch = (TreeNode *)node->_elements[i];
				if (ch) {
					ch->_parent = node;
				}
			}
		}

		node->_parent = this->_parent;
	}

	bool getInParentKey(ws::u64& pkey) {
		if (!_parent) {
			return false;
		}

		ws::u32 pos = 0;
		while (pos < _parent->_keyCount &&
			_parent->_elements[pos] != this) {
			pos++;
		}

		if (pos == _parent->_keyCount) {
			return false;
		}

		return _parent->_keys[pos];
	}

	void remove(ws::u64 key) {
		ws::u32 pos = 0;
		while (pos < _keyCount && _keys[pos] < key) {
			pos++;
		}

		if (pos == _keyCount) {
			return;
		}

		for (ws::u32 i = pos; i < (_keyCount - 1); ++i) {
			_keys[i] = _keys[i + 1];
			_elements[i] = _elements[i + 1];
		}

		_keyCount--;
		_keys[_keyCount] = 0;
		_elements[_keyCount] = nullptr;
	}

	void remove(void *element) {
		ws::u32 pos = 0;
		while (pos < _keyCount && _elements[pos] != element) {
			pos++;
		}

		if (pos == _keyCount) {
			return;
		}

		for (ws::u32 i = pos; i < (_keyCount - 1); ++i) {
			_keys[i] = _keys[i + 1];
			_elements[i] = _elements[i + 1];
		}

		_keyCount--;
		_keys[_keyCount] = 0;
		_elements[_keyCount] = nullptr;
	}

	void print(ws::u32 level = 0) {
		printf("\n");

		for (auto i = 0; i < level; ++i) {
			printf(" ");
		}

		if (!_isLeaf) {
			for (ws::u32 i = 0; i < _keyCount; ++i) {
				printf("key %llu ", _keys[i]);
			}

			for (auto i = 0; i < level; ++i) {
				printf(" ");
			}

			for (ws::u32 i = 0; i < _keyCount; ++i) {
				auto v = (TreeNode *)_elements[i];
				v->print(level + 1);
			}

		} else {
			for (ws::u32 i = 0; i < _keyCount; ++i) {
				printf("leaf %llu ", _keys[i]);
			}
		}
	}

	inline ws::u64 maxKey() const {
		return _keys[_keyCount - 1];
	}

public:
	bool _isLeaf;
	ws::u64* _keys;
	void **_elements;
	TreeNode *_parent;
	TreeNode *_sibling;
	ws::u32 _keyCount;
};

BTree::BTree(ws::u32 order) :
	_order(order),
	_root(nullptr) {
	assert(_order >= 2);

}

BTree::~BTree() {
	if (nullptr != _root) {
		delete _root;
	}
}

void BTree::print() {
	fflush(stdout);
	if (!_root) {
		printf("empty tree.\n");
		return;
	}

	_root->print();
	fflush(stdout);
}

bool BTree::earse(ws::u64 key) {
	auto leaf = lookupLeaf(key);
	auto record = lookupRecord(leaf, key);
	if (nullptr != record && nullptr != leaf) {
		_root = deleteEntry(_root, leaf, key);
		delete record;
		return true;
	}

	return false;
}

TreeNode *BTree::adjustRoot(TreeNode *root) {
	TreeNode *newRoot = nullptr;

	if (root->_keyCount > 0) {
		return root;
	}

	if (!root->_isLeaf) {
		newRoot = (TreeNode*)root->_elements[0];
		newRoot->_parent = nullptr;
	}

	delete root;
	return newRoot;
}

TreeNode *BTree::getSibling(TreeNode *node) {
	ws::u32 pos = 0;
	while(pos < node->_parent->_keyCount &&
		node->_parent->_elements[pos] != node) {
		pos++;
	}

	assert(2 <= node->_parent->_keyCount);

	if(0 == pos) {
		return (TreeNode*)node->_parent->_elements[1];
	}

	auto left = (TreeNode *)node->_parent->_elements[pos - 1];
	if((node->_parent->_keyCount) == (pos + 1)) {
		return left;
	}

	if((left->_keyCount + node->_keyCount) < _order) {
		return left;
	}

	return (TreeNode *)node->_parent->_elements[pos + 1];
}

TreeNode *BTree::deleteEntry(TreeNode *root, TreeNode *node, ws::u64 key) {
	node->remove(key);
	if (node == root) {
		return adjustRoot(root);
	}

	if (node->_keyCount >= ceiling(_order))
		return root;

	TreeNode *neighbor = getSibling(node);

	if ((neighbor->_keyCount + node->_keyCount) < _order) {
		return merge(root, node, neighbor);
	}

	return redistribute(root, node, neighbor);
}

TreeNode *BTree::merge(TreeNode *root, TreeNode *node, TreeNode *neighbor) {
	for (ws::u32 i = 0; i < node->_keyCount; i++) {
		neighbor->insert(node->_keys[i], node->_elements[i], _order);
	}

	neighbor->_sibling = node->_sibling;

	ws::u64 key = 0;
	for (ws::u32 i = 0; i < node->_parent->_keyCount; i++) {
		if(node == node->_parent->_elements[i]) {
			key = node->_parent->_keys[i];
			break;
		}
	}

	root = deleteEntry(root, node->_parent, key);

	delete node;
	return root;
}

TreeNode *BTree::redistribute(TreeNode *root, TreeNode *node, TreeNode *neighbor) {
	TreeNode *parent = node->_parent;

	ws::u64 key = 0;
	void *element = nullptr;
	if(node->maxKey() > neighbor->maxKey()) {
		key = neighbor->maxKey();
		element = neighbor->_elements[neighbor->_keyCount - 1];
	} else {
		key = neighbor->_keys[0];
		element = neighbor->_elements[0];
	}

	node->insert(key, element, _order);
	neighbor->remove(key);

	return root;
}

TreeNode *BTree::lookupLeaf(ws::u64 key) {
	auto v = _root;
	while (v && !v->_isLeaf) {
		ws::u32 i = 0;
		for (; i < v->_keyCount; ++i) {
			if (key <= v->_keys[i]) {
				break;
			}
		}

		if (i == v->_keyCount) {
			v = (TreeNode *)v->_elements[i - 1];
		} else {
			v = (TreeNode*)v->_elements[i];
		}
	}

	assert(nullptr != v);
	return v;
}

ws::u64 *BTree::lookupRecord(TreeNode *leaf, ws::u64 key) {
	auto v = leaf;
	if (nullptr == leaf) {
		return nullptr;
	}

	if (v && v->_isLeaf) {
		for (ws::u32 i = 0; i < v->_keyCount; ++i) {
			if (key == v->_keys[i]) {
				return (ws::u64*)v->_elements[i];
			}
		}
	}

	return nullptr;
}

std::vector<ws::u64> BTree::gets(ws::u64 minKey, ws::u64 maxKey) {
	std::vector<ws::u64> values;

	auto v = lookupLeaf(minKey);
	if (nullptr == v) {
		return values;
	}

	assert(v->_isLeaf);

	while (v && v->_isLeaf) {
		for (ws::u32 i = 0; i < v->_keyCount; ++i) {
			if (maxKey < v->_keys[i]) {
				return values;
			}

			if (minKey <= v->_keys[i]) {
				values.push_back(v->_keys[i]);
			}
		}

		v = (TreeNode*)v->_sibling;
	}

	return values;
}

void BTree::insert(const ws::u64 key, const ws::u64 value) {
	if (nullptr == _root) {
		buildRoot(key, value);
		return;
	}

	auto leaf = lookupLeaf(key);
	auto element = lookupRecord(leaf, key);
	if (nullptr != element) {
		*element = value;
		return;
	}

	if (leaf->_keyCount < _order) {
		leaf->insert(key, makeElements(value), _order);
		updateNodeParentKey(leaf);
		return;
	}

	_root = insert2LeafAfterSplit(_root, leaf, key, value);
}

void BTree::updateNodeParentKey(TreeNode *node) {
	auto parent = node->_parent;
	if (!parent) {
		return;
	}

	if (parent->maxKey() >= node->maxKey()) {
		return;
	}

	parent->_keys[parent->_keyCount - 1] = node->maxKey();

	updateNodeParentKey(parent);
}

void *BTree::makeElements(ws::u64 value) {
	auto p = malloc(sizeof(ws::u64));
	memcpy(p, &value, sizeof(ws::u64));
	return p;
}

void BTree::buildRoot(ws::u64 key, ws::u64 value) {
	assert(nullptr == _root);
	_root = new TreeNode(_order, true);
	_root->insert(key, makeElements(value), _order);
}

TreeNode *BTree::insert2LeafAfterSplit(TreeNode *root,
	TreeNode *leaf, ws::u64 key, ws::u64 value) {
	bool insertLeft = false;
	auto newLeaf = leaf->split(insertLeft, key, _order);

	if (insertLeft) {
		leaf->insert(key, makeElements(value), _order);
	} else {
		newLeaf->insert(key, makeElements(value), _order);
	}

	return insert2Parent(root, leaf, newLeaf, key);
}

TreeNode * BTree::insert2Parent(TreeNode *root, TreeNode *oldNode,
	TreeNode *newNode, ws::u64 insertKey) {
	auto parent = oldNode->_parent;
	if (nullptr == parent) {
		return insert2NewRoot(oldNode, newNode);
	}

	if (parent->_keyCount < _order) {
		insert2Node(parent, oldNode);
		insert2Node(parent, newNode);
		updateNodeParentKey(parent);
		return root;
	}

	return insert2NodeAfterSplit(root, parent, oldNode, newNode, insertKey);
}

TreeNode *BTree::insert2NewRoot(TreeNode *left, TreeNode *right) {
	auto root = new TreeNode(_order);
	root->insert(left->maxKey(), left, _order);
	root->insert(right->maxKey(), right, _order);
	left->_parent = root;
	right->_parent = root;
	return root;
}

void BTree::insert2Node(TreeNode *node, TreeNode *sub) {
	node->remove(sub);
	node->insert(sub->maxKey(), sub, _order);
	sub->_parent = node;
}

TreeNode *BTree::insert2NodeAfterSplit(TreeNode *root,
	TreeNode *node, TreeNode *left, TreeNode *right, ws::u64 insertKey) {
	bool insertLeft = false;
	auto newNode = node->split(insertLeft, insertKey, _order);

	if (insertLeft) {
		insert2Node(node, left);
		if (right->maxKey() < newNode->_keys[0]) {
			insert2Node(node, right);
		} else {
			insert2Node(newNode, right);
		}

	} else {
		insert2Node(newNode, left);
		insert2Node(newNode, right);
	}

	return insert2Parent(root, node, newNode, insertKey);
}