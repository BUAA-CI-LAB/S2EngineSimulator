#include "../../include/system/PEArray.hpp"


FILE* SynPEArray::peStatefp = fopen(PE_ARRAY_STATE_FILE_PATH,"w+");

SynPEArray::SerialType SynPEArray::PortName::serialCount=0;
