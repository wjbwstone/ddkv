/* SPDX-License-Identifier: Proprietary
 *
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 * B+ tree
 */

#ifndef _H_DDKV_BTREE_2021_02_09_10_52_38
#define _H_DDKV_BTREE_2021_02_09_10_52_38

#include "Types.h"

class TreeNode;

class BTree {
public:
	BTree(ws::u32 order);
	~BTree();

public:
	BTree(const BTree &other) = delete;
	BTree &operator=(const BTree &other) = delete;

public:
	void print();
	bool earse(ws::u64 key);
	TreeNode *lookupLeaf(ws::u64 key);
	void insert(ws::u64 key, ws::u64 value);
	ws::u64 *lookupRecord(TreeNode *leaf, ws::u64 key);
	std::vector<ws::u64> gets(ws::u64 minKey, ws::u64 maxKey);

protected:
	void *makeElements(ws::u64 value);
	void buildRoot(ws::u64 key, ws::u64 value);
	void updateNodeParentKey(TreeNode *node);
	TreeNode *insert2LeafAfterSplit(TreeNode *root, TreeNode *leaf,
		ws::u64 key, ws::u64 value);
	void insert2Node(TreeNode *node, TreeNode *left);
	TreeNode *insert2NewRoot(TreeNode *left, TreeNode *right);
	TreeNode *insert2Parent(TreeNode *root, TreeNode *oldNode,
		TreeNode *newNode, ws::u64 insertKey);
	TreeNode *insert2NodeAfterSplit(TreeNode *root, TreeNode *parent,
		TreeNode *left, TreeNode *right, ws::u64 insertKey);

	TreeNode *getSibling(TreeNode *node);
	TreeNode *adjustRoot(TreeNode *root);
	TreeNode *deleteEntry(TreeNode *root, TreeNode *node, ws::u64 key);
	TreeNode *merge(TreeNode *root, TreeNode *node, TreeNode *neighbor);
	TreeNode *redistribute(TreeNode *root, TreeNode *node, TreeNode *neighbor);

private:
	ws::u32 _order;
	TreeNode *_root;
};

#endif//_H_DDKV_BTREE_2021_02_09_10_52_38
