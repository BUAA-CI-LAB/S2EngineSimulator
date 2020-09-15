#include "../../include/base/ExpHandler.hpp"

FILE* ExpHandler::fpInfo   = fopen(INFO_FILE_PATH,"w+");
FILE* ExpHandler::fpError  = fopen(INFO_ERROR_PATH,"w+");
FILE* ExpHandler::fpWarning= fopen(INFO_WARNING_PATH,"w+");
