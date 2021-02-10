#ifndef _LIBCOMMON_FILESINK_2020_02_27_13_18_58_H
#define _LIBCOMMON_FILESINK_2020_02_27_13_18_58_H

#include "LogSink.h"

namespace ws {

class FileSink : public LogSink {
public:
	FileSink(const ustring& filename, bool encrypt);
	~FileSink();

public:
	void flush() override;
	void write(LogDataPtr data) override;

protected:
	void closeLogFile();
	void createLogFile();
	void createLogFileName();

protected:
	File _file;
	bool _encrypt;
	u32 _writeBytes;
	ustring _fileName;
	FastMutex _fmFile;
};

}

#endif //_LIBCOMMON_FILESINK_2020_02_27_13_18_58_H