// SPDX-License-Identifier: Proprietary
/*
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 */

#include "Types.h"
#include "Log.h"
#include "ddkv.h"

static void __init(void) {
    MLOGT("Hello, DDKV {}", "is the most fashion key-value db");
}

ws::i32 main(ws::i32 argc, ws::i8* argv[]) {
    __init();

    return 0;
}