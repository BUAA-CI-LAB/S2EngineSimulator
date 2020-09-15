#include "../../include/component/RePE.hpp"

/** ---------- up -------- **/
const ReformedPE::PortName ReformedPE::  OWInUp(ReformedPE::PortName::_PortName::  OWInUp); /**  O+W+V+Off+E-bit in  **/
const ReformedPE::PortName ReformedPE::OWPushUp(ReformedPE::PortName::_PortName::OWPushUp); /**            1-bit in  **/
const ReformedPE::PortName ReformedPE::OWFullUp(ReformedPE::PortName::_PortName::OWFullUp); /**            1-bit out **/
/** --------- down ------- **/
const ReformedPE::PortName ReformedPE:: OWOutDown(ReformedPE::PortName::_PortName:: OWOutDown); /**  O+W+V+Off+E-bit out **/
const ReformedPE::PortName ReformedPE::OWPushDown(ReformedPE::PortName::_PortName::OWPushDown); /**            1-bit in  **/
const ReformedPE::PortName ReformedPE::OWFullDown(ReformedPE::PortName::_PortName::OWFullDown); /**            1-bit out **/
/** -------- left -------- **/
const ReformedPE::PortName ReformedPE::  XInLeft(ReformedPE::PortName::_PortName::  XInLeft); /**    X+V+Off+E-bit in  **/
const ReformedPE::PortName ReformedPE::XPushLeft(ReformedPE::PortName::_PortName::XPushLeft); /**            1-bit in  **/
const ReformedPE::PortName ReformedPE::XFullLeft(ReformedPE::PortName::_PortName::XFullLeft); /**            1-bit out **/
/** -------- right ------- **/
const ReformedPE::PortName ReformedPE:: XOutRight(ReformedPE::PortName::_PortName:: XOutRight); /**    X+V+Off+E-bit out **/
const ReformedPE::PortName ReformedPE::XPushRight(ReformedPE::PortName::_PortName::XPushRight); /**            1-bit in  **/
const ReformedPE::PortName ReformedPE::XFullRight(ReformedPE::PortName::_PortName::XFullRight); /**            1-bit out **/
/** -------- up-up ------- **/
const ReformedPE::PortName ReformedPE::  SInUUp(ReformedPE::PortName::_PortName::  SInUUp); /**        S+E-bit in  **/
const ReformedPE::PortName ReformedPE::SPushUUp(ReformedPE::PortName::_PortName::SPushUUp); /**          1-bit in  **/
const ReformedPE::PortName ReformedPE::SFullUUp(ReformedPE::PortName::_PortName::SFullUUp); /**          1-bit out **/
/** -------- down-down ------- **/
const ReformedPE::PortName ReformedPE:: SOutDDown(ReformedPE::PortName::_PortName:: SOutDDown); /**        S+E-bit out **/
const ReformedPE::PortName ReformedPE::SPushDDown(ReformedPE::PortName::_PortName::SPushDDown); /**          1-bit in  **/
const ReformedPE::PortName ReformedPE::SFullDDown(ReformedPE::PortName::_PortName::SFullDDown); /**          1-bit out **/
