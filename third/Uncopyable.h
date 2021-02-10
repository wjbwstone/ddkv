#ifndef _HELPER_UNCOPYABLE_2020_01_10_14_52_21_H
#define _HELPER_UNCOPYABLE_2020_01_10_14_52_21_H

namespace ws {

namespace UnCopyable {

#define kDisableAssign(classname) \
private: \
	classname& operator=(const classname&)

#define kDisableCopy(classname) \
private: \
	classname(const classname&)

#define kDisableCopyAssign(classname) \
private: \
	classname(const classname&); \
	classname& operator=(const classname&)
}

}

#endif //_HELPER_UNCOPYABLE_2020_01_10_14_52_21_H