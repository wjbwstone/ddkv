// SPDX-License-Identifier: Proprietary
/*
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 */

#include "Types.h"
#include "Log.h"
#include "ddkv.h"

static ws::ustring __getProcessPath() {
    ws::ustring d(1024, '\0');
	auto bytes = readlink("/proc/self/exe", (ws::i8*)d.data(), 1024);
    if (bytes != -1) {
        auto n = d.find_last_of('/');
        d.resize(n + 1);
    } else {
        d.clear();
    }

    return d;
}

static ddkv g_kv;

static void __init(void) {
    MLOGT("Hello, DDKV {}", "is the most fashion key-value db");
}

static void __testkv(void) {
    g_kv.open(__getProcessPath().append("ddkv.db"));
    g_kv.set(1, 10000);
}

ws::i32 main(ws::i32 argc, ws::i8* argv[]) {
    __init();

    __testkv();

    g_kv.close();
    return 0;
}