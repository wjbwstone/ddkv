#ifndef _HELPER_TYPES_2020_01_15_12_26_41_H
#define _HELPER_TYPES_2020_01_15_12_26_41_H

/*
						_ooOoo_
					   o8888888o
					   88" . "88
					   (| -_- |)
						O\ = /O
					____/`---'\____
				  .   ' \\| |// `.
				   / \\||| : |||// \
				 / _||||| -:- |||||- \
				   | | \\\ - /// | |
				 | \_| ''\---/'' | |
				  \ .-\__ `-` ___/-. /
			   ___`. .' /--.--\ `. . __
			."" '< `.___\_<|>_/___.' >'"".
		   | | : `- \`.;`\ _ /`;.`/ - ` : | |
			 \ \ `-. \_ __\ /__ _/ .-` / /
	 ======`-.____`-.___\_____/___.-`____.-'======
						`=---='

	 .............................................
			  God save me            Never BUG
*/

#include <string.h>
#include <assert.h>

#include <map>
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include <memory>
#include <thread>
#include <functional>
#include <condition_variable>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>

#include "Uncopyable.h"

namespace ws {
	typedef char i8;
	typedef unsigned char byte;
	typedef wchar_t wchar;
	typedef short i16;
	typedef unsigned short u16;
	typedef int i32;
	typedef unsigned int u32;
	typedef long long i64;
	typedef unsigned long long u64;
	typedef std::string ustring;
	typedef std::wstring wstring;
	typedef std::string bstring;
	const static ustring __emptyString;
}

#endif //_HELPER_TYPES_2020_01_15_12_26_41_H