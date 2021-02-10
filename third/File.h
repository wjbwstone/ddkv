#ifndef _LIBCOMMON_FILE_2020_04_30_11_20_05_H
#define _LIBCOMMON_FILE_2020_04_30_11_20_05_H

#include <Types.h>
#include <Uncopyable.h>

namespace ws {

class File {
	kDisableCopyAssign(File);

public:
	enum CreateMode {
		kCreateNew = 1, //新建，存在返回失败
		kOpenExisting = 2, //打开已存在的, 只能读取
	};

	enum MoveType {
		kMoveBegin = 0,
		kMoveCurrnt = 1,
		kMoveEnd = 2,
	};

public:
	File();
	File(const i8* path, CreateMode mode = kOpenExisting);
	~File();

public:
	operator i32() const { return _file; }
	operator bool() const { return -1 != _file; }
	const i8* getPath() const { return _path.c_str(); }

public:
	void close();
	bool open(const i8* path, CreateMode mode = kOpenExisting);

public:
	i64 size() const;
	bool flush() const;
	i64 createTime() const;
	i64 lastModifyTime() const;
	bool write(const i8* data);
	bool write(const bstring& data);
	bool read(i8* data, u32& readBytes);
	bool write(const i8* data, u32 bytes);
	bool read(bstring& data, u32& bytes);
	bool move(i64 indexBytes, u32 method = kMoveBegin);

public:
	static i64 size(const i8* path);
	static bool del(const i8* path);
	static i64 createTime(const i8* path);
	static i64 lastModifyTime(const i8* path);
	static bool load(const i8* path, bstring& data);
	static bool rename(const i8* from, const i8* to);
	static bool save(const i8* path, const bstring& data,
		bool createDirectory = false, bool destroyIfExist = false);

private:
	i32 _file;
	ustring _path;
};

}

#endif //_LIBCOMMON_FILE_2020_04_30_11_20_05_H