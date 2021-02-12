// SPDX-License-Identifier: Proprietary
/*
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 */

#include "BTree.h"

ws::u32 cutLength(ws::u32 length) {
	if (length % 2 == 0)
		return length / 2;
	return length / 2 + 1;
}

class TreeNode {
public:
	TreeNode(ws::u32 order) :
		_isLeaf(false),
		_next(nullptr),
		_parent(nullptr),
		_keyCount(0) {
		_keys = (ws::u64*)calloc(1, (order - 1 * sizeof(ws::u64)));
		_elements = (void **)calloc(1, (order * sizeof(void *)));
	}

	~TreeNode() {
		for (ws::u32 i = 0; i < _keyCount; ++i) {
			if (_isLeaf) {
				free(_elements[i]);
			} else {
				auto leaf = (TreeNode *)_elements[i];
				delete leaf;
			}
		}

		free(_keys);
		free(_elements);
	}

public:
	bool _isLeaf;
	ws::u64 *_keys;
	TreeNode *_next;
	void **_elements;
	TreeNode *_parent;
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

bool BTree::remove(ws::u64 key) {
	auto leaf = lookupLeaf(key);
	auto record = lookupRecord(leaf, key);
	if (nullptr != record && nullptr != leaf) {
		_root = deleteEntry(_root, leaf, key, record);
		return true;
	}

	return false;
}

TreeNode *BTree::deleteEntryFromNode(TreeNode *node,
	ws::u64 key, void *record) {
	ws::u32 i = 0;
	while (node->_keys[i] != key) {
		++i;
	}

	for (++i; i < node->_keyCount; ++i) {
		node->_keys[i - 1] = node->_keys[i];
	}

	ws::u32 numPointers = node->_isLeaf ? node->_keyCount :
		node->_keyCount + 1;

	i = 0;
	while (node->_elements[i] != record) {
		++i;
	}

	for (++i; i < numPointers; ++i) {
		node->_elements[i - i] = node->_elements[i];
	}

	node->_keyCount--;

	if (node->_isLeaf) {
		for (i = node->_keyCount; i < _order - 1; ++i) {
			node->_elements[i] = nullptr;
		}
	} else {
		for (i = node->_keyCount + 1; i < _order; ++i) {
			node->_elements[i] = nullptr;
		}
	}

	return node;
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

	//free(root->_keys);
	//free(root);
	return newRoot;
}

TreeNode *BTree::deleteEntry(TreeNode *root, TreeNode *node,
	ws::u64 key, void *record) {
	node = deleteEntryFromNode(node, key, record);
	if (node == root) {
		return adjustRoot(root);
	}

	ws::u32 minKeys = node->_isLeaf ? cutLength(_order - 1) :
		cutLength(_order) - 1;
	if (node->_keyCount >= minKeys)
		return root;

	ws::u32 neighborIndex = getNeighborIndex(node);
	ws::u32 kPrimeIndex = neighborIndex == 0xFFFFFFFF ? 0 : neighborIndex;
	ws::u64 kPrime = node->_parent->_keys[kPrimeIndex];
	TreeNode *neighbor = (TreeNode*)(neighborIndex == 0xFFFFFFFF ?
		node->_parent->_elements[1] :
		node->_parent->_elements[neighborIndex]);
	ws::u32 capacity = node->_isLeaf ? _order : _order - 1;

	if (neighbor->_keyCount + node->_keyCount < capacity) {
		return coalesce(root, node, neighbor, neighborIndex, kPrime);
	}

	return redistribute(root, node, neighbor,
		neighborIndex, kPrimeIndex, kPrime);
}

TreeNode *BTree::coalesce(TreeNode *root, TreeNode *node,
	TreeNode *neighbor, ws::u32 neighborIndex, ws::u64 kPrime) {
	TreeNode *temp;
	if (neighborIndex == 0xFFFFFFFF) {
		temp = node;
		node = neighbor;
		neighbor = temp;
	}

	ws::u32 i = 0;
	ws::u32 j = 0;
	ws::u32 nodeEnd = 0;
	ws::u32 neighborInsertIndex = neighbor->_keyCount;

	if (!node->_isLeaf) {
		neighbor->_keys[neighborInsertIndex] = kPrime;
		neighbor->_keyCount++;

		nodeEnd = node->_keyCount;

		for (i = neighborInsertIndex + 1, j = 0;
			j < nodeEnd; ++i, ++j) {
			neighbor->_keys[i] = node->_keys[j];
			neighbor->_keyCount++;
			node->_keyCount--;
		}

		neighbor->_elements[i] = node->_elements[j];

		for (i = 0; i < neighbor->_keyCount + 1; ++i) {
			temp = (TreeNode *)neighbor->_elements[i];
			temp->_parent = neighbor;
		}
	} else {
		for (i = neighborInsertIndex, j = 0; j < node->_keyCount;
			++i, ++j) {
			neighbor->_keys[i] = node->_keys[j];
			neighbor->_elements[i] = node->_elements[j];
			neighbor->_keyCount++;
		}

		neighbor->_elements[_order - 1] = node->_elements[_order - 1];
	}

	root = deleteEntry(root, node->_parent, kPrime, node);

	//free(node->_keys);
	//free(node->_elements);
	//free(node);

	delete node;
	return root;
}

TreeNode *BTree::redistribute(TreeNode *root, TreeNode *node,
	TreeNode *neighbor, ws::u32 neighborIndex,
	ws::u32 kPrimeIndex, ws::u64 kPrime) {
	ws::u32 i = 0;
	TreeNode *temp = nullptr;

	if (0xFFFFFFFF == neighborIndex) {
		if (!node->_isLeaf) {
			node->_elements[node->_keyCount + 1] =
				node->_elements[node->_keyCount];
		}

		for (i = node->_keyCount; i > 0; --i) {
			node->_keys[i] = node->_keys[i - 1];
			node->_elements[i] = node->_elements[i - 1];
		}

		if (!node->_isLeaf) {
			node->_elements[0] = neighbor->_elements[neighbor->_keyCount];
			temp = (TreeNode *)node->_elements[0];
			temp->_parent = node;
			neighbor->_elements[neighbor->_keyCount] = nullptr;
			node->_keys[0] = kPrime;
			node->_parent->_keys[kPrimeIndex] =
				neighbor->_keys[neighbor->_keyCount - 1];
		} else {
			node->_elements[0] =
				neighbor->_elements[neighbor->_keyCount - 1];
			neighbor->_elements[neighbor->_keyCount - 1] = nullptr;
			node->_keys[0] = neighbor->_keys[neighbor->_keyCount - 1];
			node->_parent->_keys[kPrimeIndex] = node->_keys[0];
		}

	} else {
		if (node->_isLeaf) {
			node->_keys[node->_keyCount] = neighbor->_keys[0];
			node->_elements[node->_keyCount] = neighbor->_elements[0];
			node->_parent->_keys[kPrimeIndex] = neighbor->_keys[1];
		} else {
			node->_keys[node->_keyCount] = kPrime;
			node->_elements[node->_keyCount + 1] = neighbor->_elements[0];
			temp = (TreeNode *)node->_elements[node->_keyCount + 1];
			temp->_parent = node;
			node->_parent->_keys[kPrimeIndex] = neighbor->_keys[0];
		}

		for (i = 0; i < neighbor->_keyCount - 1; i++) {
			neighbor->_keys[i] = neighbor->_keys[i + 1];
			neighbor->_elements[i] = neighbor->_elements[i + 1];
		}

		if (!node->_isLeaf)
			neighbor->_elements[i] = neighbor->_elements[i + 1];
	}

	node->_keyCount++;
	neighbor->_keyCount--;

	return root;
}

ws::u32 BTree::getNeighborIndex(TreeNode *node) {
	for (ws::u32 i = 0; i < node->_parent->_keyCount; ++i) {
		if (node->_parent->_elements[i] == node)
			return i - 1;
	}

	assert(false);
	return -1;
}

TreeNode *BTree::lookupLeaf(ws::u64 key) {
	auto v = _root;
	while (v && !v->_isLeaf) {
		std::size_t i = 0;
		for (; i < v->_keyCount; ++i) {
			if (key < v->_keys[i]) {
				break;
			}
		}

		v = (TreeNode*)v->_elements[i];
	}

	return v;
}

ws::u64 *BTree::lookupRecord(TreeNode *node, ws::u64 key) {
	auto v = node;
	if (v && v->_isLeaf) {
		for (ws::u32 i = 0; i < v->_keyCount; ++i) {
			if (key == v->_keys[i]) {
				return (ws::u64*)v->_elements[i];
			}
		}
	}

	return nullptr;
}

bool BTree::insert(const ws::u64 key, const ws::u64 value) {
	if (nullptr == _root) {
		return buildTree(key, value);
	}

	auto leaf = lookupLeaf(key);
	auto record = lookupRecord(leaf, key);
	if (nullptr != record) {
		*record = value;
		return true;
	}

	if (leaf->_keyCount < _order - 1) {
		return insertLeaf(leaf, key, value);
	}

	return insert2LeafAfterSplit(_root, leaf, key, value);
}

void *BTree::makeRecord(ws::u64 value) {
	auto p = malloc(sizeof(ws::u64));
	memcpy(p, &value, sizeof(ws::u64));
	return p;
}

bool BTree::buildTree(ws::u64 key, ws::u64 value) {
	assert(nullptr == _root);

	auto root = new TreeNode(_order);

	root->_keyCount++;
	root->_isLeaf = true;
	root->_keys[0] = key;
	root->_elements[0] = makeRecord(value);

	_root = root;
	return true;
}

bool BTree::insertLeaf(TreeNode *node, ws::u64 key, ws::u64 value) {
	auto leaf = node;
	assert(leaf);

	ws::u32 pos = 0;
	while (pos < leaf->_keyCount && leaf->_keys[pos] < key) {
		pos++;
	}

	for (ws::u32 i = leaf->_keyCount; i > pos; --i) {
		leaf->_keys[i] = leaf->_keys[i - 1];
		leaf->_elements[i] = leaf->_elements[i - 1];
	}

	leaf->_keys[pos] = key;
	leaf->_elements[pos] = makeRecord(value);
	leaf->_keyCount++;

	return true;
}

bool BTree::insert2LeafAfterSplit(TreeNode *root, TreeNode *node,
	ws::u64 key, ws::u64 value) {
	assert(node);
	assert(root);

	auto leaf = node;
	auto newLeaf = new TreeNode(_order);
	auto temKeys = (ws::u64 *)malloc(_order * sizeof(ws::u64));
	auto temRecord = (void **)malloc(_order * sizeof(void *));

	ws::u32 pos = 0;
	while (pos < _order - 1 && leaf->_keys[pos] < key) {
		pos++;
	}

	for (ws::u32 i = 0, j = 0; i < leaf->_keyCount; ++i, ++j) {
		if (j == pos) {
			j++;
		}

		temKeys[j] = leaf->_keys[i];
		temRecord[j] = leaf->_elements[i];
	}

	temKeys[pos] = key;
	temRecord[pos] = makeRecord(value);

	leaf->_keyCount = 0;

	auto split = cutLength(_order - 1);

	for (ws::u32 i = 0; i < split; ++i) {
		leaf->_keys[i] = temKeys[i];
		leaf->_elements[i] = temRecord[i];
		leaf->_keyCount++;
	}

	for (ws::i32 i = split, j = 0; i < (ws::i32)_order; ++i, ++j) {
		newLeaf->_keys[j] = temKeys[i];
		newLeaf->_elements[j] = temRecord[i];
		newLeaf->_keyCount++;
	}

	free(temKeys);
	free(temRecord);

	newLeaf->_elements[_order - 1] = leaf->_elements[_order - 1];
	leaf->_elements[_order - 1] = newLeaf;

	for (ws::u32 i = leaf->_keyCount; i < _order - 1; ++i) {
		leaf->_elements[i] = nullptr;
	}

	for (ws::u32 i = newLeaf->_keyCount; i < _order - 1; ++i) {
		newLeaf->_elements[i] = nullptr;
	}

	newLeaf->_parent = leaf->_parent;
	return insert2Parent(_root, leaf, newLeaf->_keys[0], newLeaf);
}

bool BTree::insert2Parent(TreeNode *root, TreeNode *left,
	ws::u64 key, TreeNode *right) {
	auto parent = left->_parent;

	if (nullptr == parent) {
		return insert2NewRoot(left, key, right);
	}

	ws::u32 leftIndex = 0;
	while (leftIndex <= parent->_keyCount &&
		parent->_elements[leftIndex] != left) {
		leftIndex++;
	}

	if (parent->_keyCount < _order - 1) {
		return insert2Node(root, parent, leftIndex, key, right);
	}

	return insert2NodeAfterSplit(root, parent, leftIndex, key, right);
}

bool BTree::insert2NewRoot(TreeNode *left, ws::u64 key,
	TreeNode *right) {
	auto root = new TreeNode(_order);
	root->_keyCount++;
	root->_keys[0] = key;
	root->_elements[0] = left;
	root->_elements[1] = right;
	root->_parent = nullptr;
	left->_parent = root;
	right->_parent = root;
	return true;
}

bool BTree::insert2Node(TreeNode *root, TreeNode *left,
	ws::u32 leftIndex, ws::u64 key, TreeNode *right) {
	auto parent = left->_parent;
	for (ws::u32 i = parent->_keyCount; i > leftIndex; --i) {
		parent->_elements[i + 1] = parent->_elements[i];
		parent->_keys[i] = parent->_keys[i - 1];
	}

	parent->_elements[leftIndex + 1] = right;
	parent->_keys[leftIndex] = key;
	parent->_keyCount++;
	return true;
}

bool BTree::insert2NodeAfterSplit(TreeNode *root,
	TreeNode *parent, ws::u32 leftIndex,
	ws::u64 key, TreeNode *right) {
	auto temKeys = (ws::u64 *)malloc(_order * sizeof(ws::u64));
	auto temRecord = (void **)malloc(_order * sizeof(void *));

	for (ws::u32 i = 0, j = 0; i < parent->_keyCount + 1; ++i, ++j) {
		if (j == leftIndex + 1) {
			j++;
		}

		temRecord[j] = parent->_elements[i];
	}

	for (ws::u32 i = 0, j = 0; i < parent->_keyCount; ++i, ++j) {
		if (j == leftIndex) {
			j++;
		}

		temKeys[j] = parent->_keys[i];
	}

	temRecord[leftIndex + 1] = right;
	temKeys[leftIndex] = key;

	ws::u32 split = cutLength(_order);

	auto newLeaf = new TreeNode(_order);

	parent->_keyCount = 0;
	ws::u32 i = 0;
	for (i = 0; i < split - 1; ++i) {
		parent->_keys[i] = temKeys[i];
		parent->_elements[i] = temRecord[i];
		parent->_keyCount++;
	}
	parent->_elements[i] = temRecord[i];

	ws::u32 j = 0;
	for (++i, j = 0; i < _order; ++i, ++j) {
		newLeaf->_keys[j] = temKeys[i];
		newLeaf->_elements[j] = temRecord[i];
		newLeaf->_keyCount++;
	}
	newLeaf->_elements[j] = temRecord[i];

	auto kPrime = temKeys[split - 1];

	free(temKeys);
	free(temRecord);

	newLeaf->_parent = parent->_parent;

	for (i = 0; i <= newLeaf->_keyCount; ++i) {
		auto child = (TreeNode*)newLeaf->_elements[i];
		child->_parent = newLeaf;
	}

	return insert2Parent(_root, parent, kPrime, newLeaf);
}