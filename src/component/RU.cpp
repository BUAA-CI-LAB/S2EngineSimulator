#include "../../include/component/RU.hpp"

    /** ---------- up -------- **/
const RU::PortName RU::BufPushUp  (RU::PortName::_PortName::BufPushUp);  /**    1-bit  in  **/
const RU::PortName RU::BufInUp    (RU::PortName::_PortName::BufInUp  );  /** 1+WL_BIT  in  **/
const RU::PortName RU::BufFullUp  (RU::PortName::_PortName::BufFullUp);  /**    1-bit  out **/
    /** --------- down ------- **/
const RU::PortName RU::BufPushDown(RU::PortName::_PortName::BufPushDown);/**    1-bit  out **/
const RU::PortName RU::BufInDown  (RU::PortName::_PortName::BufInDown  );/** 1+WL_BIT  out **/
const RU::PortName RU::BufFullDown(RU::PortName::_PortName::BufFullDown);/**    1-bit  in  **/
    /** --------- left ------- **/
const RU::PortName RU::XELIn  (RU::PortName::_PortName::XELIn  ); /** 2+X_BIT+XL_BIT  in  **/
const RU::PortName RU::XELPush(RU::PortName::_PortName::XELPush); /** 1-bit           in  **/
const RU::PortName RU::XELFull(RU::PortName::_PortName::XELFull); /** 1-bit           out **/
    /** -------- right ------- **/
const RU::PortName RU::OOut  (RU::PortName::_PortName::OOut  ); /** X_BIT   out **/
const RU::PortName RU::OPop  (RU::PortName::_PortName::OPop  ); /** 1-bit   in  **/
const RU::PortName RU::OEmpty(RU::PortName::_PortName::OEmpty); /** 1-bit   out **/
