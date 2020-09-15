#include "../../include/component/CE.hpp"

const enum CE::CE_CTRL CE::InsTable[]={
/**0**/ CE_CTRL::RAB  ,/// read from asyn FIFO, broadcast
/**1**/ CE_CTRL::RAW  ,/// read from asyn FIFO, write to sync FIFO,
/**2**/ CE_CTRL::RAWB ,/// read from asyn FIFO, write to sync FIFO, broadcast
/**3**/ CE_CTRL::RAWBZ,/// read from asyn FIFO, write to sync FIFO, broadcast zero
/**4**/ CE_CTRL::RSB  ,/// read from sync FIFO, broadcast
/**5**/ CE_CTRL::RSBZ ,/// read from sync FIFO, broadcast zero
/**6**/ CE_CTRL::RSWB ,/// read from sync FIFO, write to sync FIFO, broadcast
/**7**/ CE_CTRL::RSWBZ,/// read from sync FIFO, write to sync FIFO, broadcast zero
/**8**/ CE_CTRL::BZ    /// broadcast zero
};

const uint8_t CE::CtrlNum = 9;

/** ---------- up -------- **/
const CE::PortName CE::BufPopUp    (CE::PortName::_PortName::BufPopUp    ); /**          1-bit  in  **/
const CE::PortName CE::BufOutUp    (CE::PortName::_PortName::BufOutUp    ); /** 2+X_BIT+XL_BIT  out **/
const CE::PortName CE::BufEmptyUp  (CE::PortName::_PortName::BufEmptyUp  ); /**          1-bit  out **/
/** --------- down ------- **/
const CE::PortName CE::BufPopDown  (CE::PortName::_PortName::BufPopDown  ); /**          1-bit  out **/
const CE::PortName CE::BufInDown   (CE::PortName::_PortName::BufInDown   ); /** 2+X_BIT+XL_BIT  in  **/
const CE::PortName CE::BufEmptyDown(CE::PortName::_PortName::BufEmptyDown); /**          1-bit  in  **/
/** --------- left ------- **/
const CE::PortName CE::XELIn       (CE::PortName::_PortName::XELIn       ); /** 2+X_BIT+XL_BIT  in  **/
const CE::PortName CE::XELPop      (CE::PortName::_PortName::XELPop      ); /**          1-bit  out **/
const CE::PortName CE::XELEmpty    (CE::PortName::_PortName::XELEmpty    ); /**          1-bit  in  **/
/** -------- right ------- **/
const CE::PortName CE::OOut        (CE::PortName::_PortName::OOut        ); /** 2+X_BIT+XL_BIT  out **/
const CE::PortName CE::OPush       (CE::PortName::_PortName::OPush       ); /**          1-bit  out **/
const CE::PortName CE::OFull       (CE::PortName::_PortName::OFull       ); /**          1-bit  in  **/
