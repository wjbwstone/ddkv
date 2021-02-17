// SPDX-License-Identifier: Proprietary
/*
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 */

#include "ddkv.h"

#include "Log.h"

ddkv::ddkv() : _tree(3) {

}

ddkv::~ddkv() {

}

bool ddkv::open(const ws::ustring &file) {
	if (!_dbfile.open(file.c_str(), ws::File::kOpenAlway)) {
		MLOGE("Open the file \"{}\" failed, errno is {}",
			file, errno);
		return false;
	}

	return true;
}

void ddkv::close() {
	_dbfile.close();
}

std::vector<ws::u64> ddkv::gets(ws::u64 minKey, ws::u64 maxKey) {
	return _tree.gets(minKey, maxKey);
}

bool ddkv::get(const ws::u64 key, ws::u64 &value) {
	auto v = _tree.lookupRecord(_tree.lookupLeaf(key), key);
	if (nullptr != v) {
		value = *v;
	}

	return (nullptr != v);
}

bool ddkv::set(const ws::u64  key, const ws::u64 value) {
	_tree.insert(key, value);
	return true;
}

void ddkv::printTree() {
	printf("new tree~~~~~~~~~~~~~~~~~~~~~~\n");
	_tree.print();
}
