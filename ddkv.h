/* SPDX-License-Identifier: Proprietary
 *
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 */

#ifndef _H_DDKV_DDKV_2021_02_10_17_19_24
#define _H_DDKV_DDKV_2021_02_10_17_19_24

#include "BTree.h"

class ddkv {
public:
	ddkv();
	~ddkv();

public:
	bool open(const ws::ustring &file);
	void close();

public:
	bool get(const ws::u64 key, ws::u64 &value);
	bool set(const ws::u64 key, const ws::u64 value);

private:
	BTree _tree;
};


#endif//_H_DDKV_DDKV_2021_02_10_17_19_24