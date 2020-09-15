#include "../../include/system/RUArray.hpp"

RUArray::SerialType RUArray::PortName::serialCount=0;

Object::PortDataType RUArray::_high = 1;
Object::PortDataType RUArray::_low  = 0;

const Object::Port RUArray::_High      (1,Object::PortDirection::OUT,RUArray::_high);
const Object::Port RUArray::_Low       (1,Object::PortDirection::OUT,RUArray::_low );
const Object::Port RUArray::_Low_2_X_XL(2+RU::X_BIT_WIDTH+RU::XL_BIT_WIDTH
                                         ,Object::PortDirection::OUT,RUArray::_low );
