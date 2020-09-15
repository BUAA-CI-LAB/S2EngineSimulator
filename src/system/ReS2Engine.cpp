#include "../../include/system/ReS2Engine.hpp"

ReformedS2Engine::SerialType ReformedS2Engine::PortName::serialCount=0;

Object::PortDataType ReformedS2Engine::_high = 1;
Object::PortDataType ReformedS2Engine::_low  = 0;

const Object::Port ReformedS2Engine::_High      (1,Object::PortDirection::OUT,ReformedS2Engine::_high);
const Object::Port ReformedS2Engine::_Low       (1,Object::PortDirection::OUT,ReformedS2Engine::_low );
const Object::Port ReformedS2Engine::_Low_2_X_XL(2+RU::X_BIT_WIDTH+RU::XL_BIT_WIDTH
                                                  ,Object::PortDirection::OUT,ReformedS2Engine::_low );
