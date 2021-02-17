#include "File.h"

namespace ws {

File::File() : _file(-1) {

}

File::File(const i8* path, CreateMode mode /* = kOpenExisting */) :
	_file(-1) {
	open(path, mode);
}

File::~File() {
	close();
}

bool File::open(const i8* path, CreateMode mode /*= kOpenExisting*/) {
	if(-1 != _file) {
		assert(-1 == _file);
		return false;
	}

	_path = path;

	//Don't create the direcotry if not exist.

	int flag = O_RDONLY;
	if (mode == kCreateNew) {
		flag = O_RDWR | O_CREAT | O_EXCL;
	} else if (mode == kOpenAlway) {
		flag = O_RDWR | O_CREAT;
	}

	_file = ::open(_path.c_str(), flag, 0644);

	return operator bool();
}

void File::close() {
	_path.clear();
	if(-1 != _file) {
		::close(_file);
		_file = -1;
	}
}

bool File::flush() const {
	return (0 == ::fsync(_file));
}

i64 File::size() const {
	return File::size(_path.c_str());
}

i64 File::createTime() const {
	return File::createTime(_path.c_str());
}

i64 File::lastModifyTime() const {
	return File::lastModifyTime(_path.c_str());
}

bool File::write(const i8* data) {
	if(nullptr != data) {
		return write(data, (u32)strlen(data));
	}

	return false;
}

bool File::write(const bstring& data) {
	return write(data.c_str(), (u32)data.size());
}

bool File::write(const i8* data, u32 bytes) {
	if(nullptr == data) {
		return false;
	}

	return (-1 != ::write(_file, data, bytes));
}

bool File::read(bstring& data, u32& bytes) {
	data.resize(bytes, '\0');

	auto success = read(&data[0], bytes);
	if(success) {
		data.resize(bytes);
	} else {
		data.clear();
	}

	return success;
}

bool File::read(i8* data, u32& bytes) {
	if(nullptr == data) {
		bytes = 0;
		return false;
	}

	return (-1 != ::read(_file, data, bytes));
}

bool File::move(i64 indexBytes, u32 method /*= FILE_BEGIN*/) {
	return (-1 != lseek(_file, indexBytes, method));
}

i64 File::size(const i8* path) {
	if (path != nullptr) {
		struct stat t;
		if (stat(path, &t) == 0)
			return t.st_size;
	}

	return -1;
}

bool File::del(const i8* path) {
	if (path != nullptr) {
		return (-1 != ::remove(path));
	}

	return false;
}

i64 File::createTime(const i8* path) {
	if (path != nullptr) {
		struct stat t;
		if (stat(path, &t) == 0)
			return t.st_atim.tv_sec;
	}

	return -1;
}

i64 File::lastModifyTime(const i8* path) {
	if (path != nullptr) {
		struct stat t;
		if (stat(path, &t) == 0)
			return t.st_mtim.tv_sec;
	}

	return -1;
}

bool File::rename(const i8* from, const i8* to) {
	return (-1 == ::rename(from, to));
}

bool File::load(const i8* path, bstring& data) {
	File f(path);
	if(f) {
		auto readBytes = (u32)f.size();
		return f.read(data, readBytes);
	}

	return false;
}

bool File::save(const i8* path, const bstring& data,
	bool createDirectory /* = false */,
	bool destroyIfExist /* = false */) {
	if(createDirectory) {
		//Directory::createDirectory(path.parentPath());
	}

	auto mode = File::kCreateNew;
	if(destroyIfExist) {
		File::del(path);
	}

	return File(path, mode).write(data);
}

}