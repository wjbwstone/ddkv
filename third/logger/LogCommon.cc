#include "LogCommon.h"

namespace ws {

namespace LogCommon {

ustring getLogDir() {
	return ("/var/log/");
}

const char* getLogExt() {
	return "wlog";
}

}

}
