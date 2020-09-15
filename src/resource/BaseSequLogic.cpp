#include "../../include/resource/BaseSequLogic.hpp"

 const Reg::PortName  Reg::out(Reg::PortName::_PortName::out);
 const Reg::PortName  Reg::in (Reg::PortName::_PortName::in );
 const Reg::PortName  Reg::en (Reg::PortName::_PortName::en );
vector<Reg::RegParam> Reg::regParam;
 bool  Reg::hasReserved = false;

const MarkReg::PortName MarkReg::out(MarkReg::PortName::_PortName::out);
const MarkReg::PortName MarkReg::Clr(MarkReg::PortName::_PortName::Clr);
const MarkReg::PortName MarkReg::Set(MarkReg::PortName::_PortName::Set);
MarkReg::MarkRegParam MarkReg::markRegParam;

//const MultiStateMarkReg::PortName MultiStateMarkReg::out(MarkReg::PortName::_PortName::out);
//const MultiStateMarkReg::PortName MultiStateMarkReg::sub(MarkReg::PortName::_PortName::sub);
//const MultiStateMarkReg::PortName MultiStateMarkReg::add(MarkReg::PortName::_PortName::add);

const  FIFO::PortName FIFO::In   (FIFO::PortName::_PortName::In   ); /** n-bit  in **/
const  FIFO::PortName FIFO::Out  (FIFO::PortName::_PortName::Out  ); /** n-bit out **/
const  FIFO::PortName FIFO::Push (FIFO::PortName::_PortName::Push ); /** 1-bit  in **/
const  FIFO::PortName FIFO::Pop  (FIFO::PortName::_PortName::Pop  ); /** 1-bit  in **/
const  FIFO::PortName FIFO::Full (FIFO::PortName::_PortName::Full ); /** 1-bit out **/
const  FIFO::PortName FIFO::Empty(FIFO::PortName::_PortName::Empty); /** 1-bit out **/
vector<FIFO::FIFOParam> FIFO::fifoParam;
 bool  FIFO::hasReserved = false;


