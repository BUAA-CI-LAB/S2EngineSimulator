#include "../../include/component/ReMixedPE.hpp"

/** ---------- up -------- **/
const ReMixedPE::PortName ReMixedPE::  OWInUp(ReMixedPE::PortName::_PortName::  OWInUp); /**  O+W+V+Off+E-bit in  **/
const ReMixedPE::PortName ReMixedPE::OWPushUp(ReMixedPE::PortName::_PortName::OWPushUp); /**            1-bit in  **/
const ReMixedPE::PortName ReMixedPE::OWFullUp(ReMixedPE::PortName::_PortName::OWFullUp); /**            1-bit out **/
/** --------- down ------- **/
const ReMixedPE::PortName ReMixedPE:: OWOutDown(ReMixedPE::PortName::_PortName:: OWOutDown); /**  O+W+V+Off+E-bit out **/
const ReMixedPE::PortName ReMixedPE::OWPushDown(ReMixedPE::PortName::_PortName::OWPushDown); /**            1-bit in  **/
const ReMixedPE::PortName ReMixedPE::OWFullDown(ReMixedPE::PortName::_PortName::OWFullDown); /**            1-bit out **/
/** -------- left -------- **/
const ReMixedPE::PortName ReMixedPE::  XInLeft(ReMixedPE::PortName::_PortName::  XInLeft); /**    X+V+Off+E-bit in  **/
const ReMixedPE::PortName ReMixedPE::XPushLeft(ReMixedPE::PortName::_PortName::XPushLeft); /**            1-bit in  **/
const ReMixedPE::PortName ReMixedPE::XFullLeft(ReMixedPE::PortName::_PortName::XFullLeft); /**            1-bit out **/
/** -------- right ------- **/
const ReMixedPE::PortName ReMixedPE:: XOutRight(ReMixedPE::PortName::_PortName:: XOutRight); /**    X+V+Off+E-bit out **/
const ReMixedPE::PortName ReMixedPE::XPushRight(ReMixedPE::PortName::_PortName::XPushRight); /**            1-bit in  **/
const ReMixedPE::PortName ReMixedPE::XFullRight(ReMixedPE::PortName::_PortName::XFullRight); /**            1-bit out **/
/** -------- up-up ------- **/
const ReMixedPE::PortName ReMixedPE::  SInUUp(ReMixedPE::PortName::_PortName::  SInUUp); /**        S+E-bit in  **/
const ReMixedPE::PortName ReMixedPE::SPushUUp(ReMixedPE::PortName::_PortName::SPushUUp); /**          1-bit in  **/
const ReMixedPE::PortName ReMixedPE::SFullUUp(ReMixedPE::PortName::_PortName::SFullUUp); /**          1-bit out **/
/** -------- down-down ------- **/
const ReMixedPE::PortName ReMixedPE:: SOutDDown(ReMixedPE::PortName::_PortName:: SOutDDown); /**        S+E-bit out **/
const ReMixedPE::PortName ReMixedPE::SPushDDown(ReMixedPE::PortName::_PortName::SPushDDown); /**          1-bit in  **/
const ReMixedPE::PortName ReMixedPE::SFullDDown(ReMixedPE::PortName::_PortName::SFullDDown); /**          1-bit out **/
