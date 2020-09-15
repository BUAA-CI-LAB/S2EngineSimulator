#include "../../include/system/RePEArray.hpp"

ReformedPEArray::SerialType ReformedPEArray::PortName::serialCount=0;

FILE* ReformedPEArray::peStatefp = fopen(REFORMED_PE_ARRAY_STATE_FILE_PATH,"w+");

Object::PortDataType ReformedPEArray::_low  = 0;

const Object::Port ReformedPEArray::_Low_1  (1,Object::PortDirection::OUT,ReformedPEArray::_low );
const Object::Port ReformedPEArray::_Low_S_E(ReformedPE:: S_BIT_WIDTH
                                            +ReformedPE::SE_BIT_WIDTH
                                              ,Object::PortDirection::OUT,ReformedPEArray::_low );
