#include "../../include/resource/BaseCombiLogic.hpp"


const  Adder::PortName Adder::in0(Adder::PortName::_PortName::input0);
const  Adder::PortName Adder::in1(Adder::PortName::_PortName::input1);
const  Adder::PortName Adder::out(Adder::PortName::_PortName::output);
vector<Adder::AdderParam> Adder::adderParam;
 bool  Adder::hasReserved = false;

const  Multer::PortName Multer::in0(Multer::PortName::_PortName::input0);
const  Multer::PortName Multer::in1(Multer::PortName::_PortName::input1);
const  Multer::PortName Multer::out(Multer::PortName::_PortName::output);
vector<Multer::MulterParam> Multer::multerParam;
 bool  Multer::hasReserved = false;

const  Comparer::PortName Comparer::in0(Comparer::PortName::_PortName::in0);
const  Comparer::PortName Comparer::in1(Comparer::PortName::_PortName::in1);
const  Comparer::PortName Comparer::out(Comparer::PortName::_PortName::out);
vector<Comparer::CompParam> Comparer::compParam;
 bool  Comparer::hasReserved = false;

const  ShiftAdder::PortName ShiftAdder::in0  (ShiftAdder::PortName::_PortName::input0);
const  ShiftAdder::PortName ShiftAdder::in1  (ShiftAdder::PortName::_PortName::input1);
const  ShiftAdder::PortName ShiftAdder::out  (ShiftAdder::PortName::_PortName::output);
const  ShiftAdder::PortName ShiftAdder::shift(ShiftAdder::PortName::_PortName::shift );
const  ShiftAdder::PortName ShiftAdder::sign0(ShiftAdder::PortName::_PortName::sign0 );
vector<ShiftAdder::ShiftAdderParam> ShiftAdder::shiftAdderParam;
 bool  ShiftAdder::hasReserved = false;

const  SignCtrlMulter::PortName SignCtrlMulter::in0  (SignCtrlMulter::PortName::_PortName::input0);
const  SignCtrlMulter::PortName SignCtrlMulter::in1  (SignCtrlMulter::PortName::_PortName::input1);
const  SignCtrlMulter::PortName SignCtrlMulter::out  (SignCtrlMulter::PortName::_PortName::output);
const  SignCtrlMulter::PortName SignCtrlMulter::sign0(SignCtrlMulter::PortName::_PortName::sign0 );
const  SignCtrlMulter::PortName SignCtrlMulter::sign1(SignCtrlMulter::PortName::_PortName::sign1 );
vector<SignCtrlMulter::SignCtrlMulterParam> SignCtrlMulter::signCtrlMulterParam;
 bool  SignCtrlMulter::hasReserved = false;
