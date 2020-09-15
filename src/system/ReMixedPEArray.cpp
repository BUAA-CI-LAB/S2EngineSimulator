#include "../../include/system/ReMixedPEArray.hpp"

ReMixedPEArray::SerialType ReMixedPEArray::PortName::serialCount=0;

FILE* ReMixedPEArray::peStatefp = fopen(REFORMED_PE_ARRAY_STATE_FILE_PATH,"w+");

Object::PortDataType ReMixedPEArray::_low  = 0;

const Object::Port ReMixedPEArray::_Low_1  (1,Object::PortDirection::OUT,ReMixedPEArray::_low );
const Object::Port ReMixedPEArray::_Low_S_E(ReMixedPE:: S_BIT_WIDTH
                                           +ReMixedPE::SE_BIT_WIDTH
                                              ,Object::PortDirection::OUT,ReMixedPEArray::_low );
