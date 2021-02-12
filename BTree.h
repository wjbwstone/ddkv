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
	bool remove(ws::u64 key);
	TreeNode *lookupLeaf(ws::u64 key);
	bool insert(ws::u64 key, ws::u64 value);
	ws::u64 *lookupRecord(TreeNode *leaf, ws::u64 key);

protected:
	void *makeRecord(ws::u64 value);
	bool buildTree(ws::u64 key, ws::u64 value);
	bool insertLeaf(TreeNode *leaf, ws::u64 key, ws::u64 value);
	bool insert2LeafAfterSplit(TreeNode *root, TreeNode *leaf,
		ws::u64 key, ws::u64 value);
	bool insert2Parent(TreeNode *root, TreeNode *left,
		ws::u64 key, TreeNode *right);
	bool insert2Node(TreeNode *root, TreeNode *left,
		ws::u32 leftIndex, ws::u64 key, TreeNode *right);
	bool insert2NewRoot(TreeNode *left, ws::u64 key, TreeNode *right);
	bool insert2NodeAfterSplit(TreeNode *root, TreeNode *parent,
		ws::u32 leftIndex, ws::u64 key, TreeNode *right);

	ws::u32 getNeighborIndex(TreeNode *node);
	TreeNode *deleteEntry(TreeNode *root, TreeNode *node,
		ws::u64 key, void *pointer);
	TreeNode *deleteEntryFromNode(TreeNode *node,
		ws::u64 key, void *record);
	TreeNode *adjustRoot(TreeNode *root);
	TreeNode *coalesce(TreeNode *root, TreeNode *node,
		TreeNode *neighbor, ws::u32 neighborIndex, ws::u64 kPrime);
	TreeNode *redistribute(TreeNode *root, TreeNode *node,
		TreeNode *neighbor, ws::u32 neighborIndex,
		ws::u32 kPrimeIndex, ws::u64 kPrime);

private:
	ws::u32 _order;
	TreeNode *_root;
};

#endif//_H_DDKV_BTREE_2021_02_09_10_52_38
