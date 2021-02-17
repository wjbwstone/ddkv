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
    fflush(stdout);
}

static void __testkv(void) {
    g_kv.open(__getProcessPath().append("ddkv.db"));
    g_kv.set(1, 10001);
    g_kv.set(7, 10007);
    g_kv.set(2, 10002);
    g_kv.set(20, 10020);
    g_kv.printTree();
    g_kv.set(5, 10005);
    g_kv.printTree();
    g_kv.set(100, 200);
    g_kv.printTree();
   // g_kv.set(3, 3);
    g_kv.printTree();
  //  g_kv.set(4, 5);
    g_kv.printTree();
   // g_kv.set(6, 5);
    g_kv.printTree();
  //  g_kv.set(8, 5);
    g_kv.printTree();
  //  g_kv.set(9, 5);
    g_kv.printTree();
    g_kv.set(800, 200);
    g_kv.printTree();
    g_kv.set(33, 200);
    g_kv.printTree();
    g_kv.set(300, 2300);
    g_kv.printTree();
    g_kv.set(440, 900);
    g_kv.printTree();
    g_kv.set(540, 900);
    g_kv.printTree();
    g_kv.set(545, 900);
    g_kv.printTree();
    g_kv.set(123, 12455);
    g_kv.printTree();
    g_kv.set(10, 12455);
    g_kv.printTree();
    g_kv.set(183, 12455);
    g_kv.printTree();
    g_kv.set(64000, 900);
    g_kv.printTree();

    for (auto i = 1; i < 9; ++i) {
        g_kv.set(i, i);
        g_kv.printTree();
    }

    printf("\n");
    g_kv.printTree();

    auto keys = g_kv.gets(0, (ws::u64)-1);
    for (const auto it : keys) {
        printf("%llu ", it);
    }
    printf("\n");

    ws::u64 value = 0;
    g_kv.get(2, value);
    g_kv.get(300, value);
    g_kv.get(123, value);
}

ws::i32 main(ws::i32 argc, ws::i8* argv[]) {
    __init();

    __testkv();

    g_kv.close();
    return 0;
}