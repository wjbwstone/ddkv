// SPDX-License-Identifier: Proprietary
/*
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 */

#include "ddkv.h"

ddkv::ddkv() : _tree(3) {

}

ddkv::~ddkv() {

}

bool ddkv::open(const ws::ustring &file) {
	return false;
}

void ddkv::close() {

}

bool ddkv::get(const ws::u64 key, ws::u64 &value) {
	auto v = _tree.lookupRecord(_tree.lookupLeaf(key), key);
	if (nullptr != v) {
		value = *v;
	}

	return (nullptr != v);
}

bool ddkv::set(const ws::u64  key, const ws::u64 value) {
	return _tree.insert(key, value);
}
