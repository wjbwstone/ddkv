#include "RollerFileSink.h"

namespace ws {

RollerFileSink::RollerFileSink(const ustring& filename, bool encrypt) :
	FileSink(filename, encrypt),
	_rollIndex(0) {

}

RollerFileSink::~RollerFileSink() {

}

void RollerFileSink::write(LogDataPtr data) {
	FileSink::write(data);

	if(_writeBytes >= RollerFileSink::s_filemaxsize) {
		closeLogFile();
		auto opath = LogCommon::getLogDir().append(_fileName);
		auto npath = LogCommon::getLogDir().append(constructNextFileName());
		File::rename(opath.c_str(), npath.c_str());
		createLogFile();
	}
}

ustring RollerFileSink::constructNextFileName() {
	auto t = _fileName;
	auto found = t.find_last_of(".");
	if(ustring::npos != found) {
		t.erase(found);
	}

	return fmt::format("{}_{}.{}", t, ++_rollIndex, LogCommon::getLogExt());
}

}