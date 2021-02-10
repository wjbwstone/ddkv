#include "ConsoleSink.h"

namespace ws {

ConsoleSink::ConsoleSink() {

}

ConsoleSink::~ConsoleSink() {

}

void ConsoleSink::flush() {

}

void ConsoleSink::write(LogDataPtr data) {
	FastMutex::ScopedLock lock(_fmConsole);

	if(kLoggerLevelError == data->level()) {
		fwrite("\033[31m", 1, 5, stdout);
	} else if(kLoggerLevelWarn == data->level()) {
		fwrite("\033[33m", 1, 5, stdout);
	} else if(kLoggerLevelTrace == data->level()) {
		fwrite("\033[32m", 1, 5, stdout);
	}

	fwrite(data->data(), 1, data->size(), stdout);
	fwrite("\033[0m", 1, 4, stdout);
}

}