#include "../../include/system/S2Engine.hpp"

S2Engine::SerialType S2Engine::PortName::serialCount=0;

Object::PortDataType S2Engine::_low  = 0;

const Object::Port S2Engine::_Low  (1,Object::PortDirection::OUT,S2Engine::_low );
const Object::Port S2Engine::_Low_S(SynPE::SUM_BIT_WIDTH
                                     ,Object::PortDirection::OUT,S2Engine::_low );
