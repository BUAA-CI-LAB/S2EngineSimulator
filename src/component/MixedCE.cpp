#include "../../include/component/MixedCE.hpp"

const enum MixedCE::CE_CTRL MixedCE::InsTable[]={
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

const uint8_t MixedCE::CtrlNum = 9;

/** ---------- up -------- **/
const MixedCE::PortName MixedCE::BufPopUp    (MixedCE::PortName::_PortName::BufPopUp    ); /**          1-bit  in  **/
const MixedCE::PortName MixedCE::BufOutUp    (MixedCE::PortName::_PortName::BufOutUp    ); /** 2+X_BIT+XL_BIT  out **/
const MixedCE::PortName MixedCE::BufEmptyUp  (MixedCE::PortName::_PortName::BufEmptyUp  ); /**          1-bit  out **/
/** --------- down ------- **/
const MixedCE::PortName MixedCE::BufPopDown  (MixedCE::PortName::_PortName::BufPopDown  ); /**          1-bit  out **/
const MixedCE::PortName MixedCE::BufInDown   (MixedCE::PortName::_PortName::BufInDown   ); /** 2+X_BIT+XL_BIT  in  **/
const MixedCE::PortName MixedCE::BufEmptyDown(MixedCE::PortName::_PortName::BufEmptyDown); /**          1-bit  in  **/
/** --------- left ------- **/
const MixedCE::PortName MixedCE::XELIn       (MixedCE::PortName::_PortName::XELIn       ); /** 2+X_BIT+XL_BIT  in  **/
const MixedCE::PortName
MixedCE::XELPop      (MixedCE::PortName::_PortName::XELPop      ); /**          1-bit  out **/
const MixedCE::PortName MixedCE::XELEmpty    (MixedCE::PortName::_PortName::XELEmpty    ); /**          1-bit  in  **/
/** -------- right ------- **/
const MixedCE::PortName MixedCE::OOut        (MixedCE::PortName::_PortName::OOut        ); /** 2+X_BIT+XL_BIT  out **/
const MixedCE::PortName MixedCE::OPush       (MixedCE::PortName::_PortName::OPush       ); /**          1-bit  out **/
const MixedCE::PortName MixedCE::OFull       (MixedCE::PortName::_PortName::OFull       ); /**          1-bit  in  **/
