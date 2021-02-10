#ifndef _LIBLOG_LOGCOMMON_2020_01_15_13_39_58_H
#define _LIBLOG_LOGCOMMON_2020_01_15_13_39_58_H

#include <Types.h>

#include <RC4.h>
#include <File.h>
#include <ThreadPool.h>
#include <FastMutex.h>

namespace ws {

namespace LogCommon {

ustring getLogDir();
const char* getLogExt();

}

}

#endif //_LIBLOG_LOGCOMMON_2020_01_15_13_39_58_H