#include "../../include/system/ReMixedS2Engine.hpp"

ReMixedS2Engine::SerialType ReMixedS2Engine::PortName::serialCount=0;

Object::PortDataType ReMixedS2Engine::_high = 1;
Object::PortDataType ReMixedS2Engine::_low  = 0;

const Object::Port ReMixedS2Engine::_High      (1,Object::PortDirection::OUT,ReMixedS2Engine::_high);
const Object::Port ReMixedS2Engine::_Low       (1,Object::PortDirection::OUT,ReMixedS2Engine::_low );
const Object::Port ReMixedS2Engine::_Low_2_X_XL(
                      ReMixedPE::  MARK_BIT_WIDTH
                    + ReMixedPE::     X_BIT_WIDTH
                    + ReMixedPE::    XE_BIT_WIDTH
                    + ReMixedPE::OFFSET_BIT_WIDTH,Object::PortDirection::OUT,ReMixedS2Engine::_low );

