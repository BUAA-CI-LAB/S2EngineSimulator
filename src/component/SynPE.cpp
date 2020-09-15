#include "../../include/component/SynPE.hpp"

/** ---------- up -------- **/
const SynPE::PortName SynPE::WIn (SynPE::PortName::_PortName::WIn ); /**  8-bit in  **/
const SynPE::PortName SynPE::OIn (SynPE::PortName::_PortName::OIn ); /**  1-bit in  **/
/** --------- down ------- **/
const SynPE::PortName SynPE::OOut(SynPE::PortName::_PortName::OOut); /**  1-bit out **/
const SynPE::PortName SynPE::WOut(SynPE::PortName::_PortName::WOut); /**  8-bit out **/
/** -------- left -------- **/
const SynPE::PortName SynPE::XIn (SynPE::PortName::_PortName::XIn ); /**  8-bit in  **/
/** -------- right ------- **/
const SynPE::PortName SynPE::XOut(SynPE::PortName::_PortName::XOut); /**  8-bit out **/
/** -------- up-up ------- **/
const SynPE::PortName SynPE::SIn (SynPE::PortName::_PortName::SIn ); /** 24-bit in  **/
const SynPE::PortName SynPE::SEn (SynPE::PortName::_PortName::SEn ); /**  1-bit in  **/
const SynPE::PortName SynPE::FIn (SynPE::PortName::_PortName::FIn ); /**  1-bit in  **/
/** -------- up-up ------- **/
const SynPE::PortName SynPE::SOut(SynPE::PortName::_PortName::SOut); /** 24-bit out **/
const SynPE::PortName SynPE::SEnO(SynPE::PortName::_PortName::SEnO); /**  1-bit out **/
const SynPE::PortName SynPE::FOut(SynPE::PortName::_PortName::FOut); /**  1-bit out **/

Object::PortDataType SynPE::_high = 1;
Object::PortDataType SynPE::_low  = 0;

const Object::Port SynPE::_High(1,Object::PortDirection::OUT,SynPE::_high);
const Object::Port SynPE::_Low (1,Object::PortDirection::OUT,SynPE::_low );

const Object::PortBitWidthType SynPE::SUM_BIT_WIDTH = 24;
