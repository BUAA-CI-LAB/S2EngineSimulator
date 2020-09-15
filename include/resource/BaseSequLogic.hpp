#ifndef __RESOURCE_BASE_SEQ_LOGIC_HPP
#define __RESOURCE_BASE_SEQ_LOGIC_HPP

#include<iomanip>

#include "../../define.hpp"
#include "../base/Object.hpp"

class Reg: public ClockedObject{
    /**  it is the abstract module of the register for simulation
     *   it contents the simulation of the behaviour of register
     *   and it can also count the writing and reading time for
     *   energy assumption
     **/
    typedef PortDataType RegValueType;

    class RegParam{
    private:
        Object::CounterType totalNumbers;
        Object::CounterType    zeroWrite;
        Object::CounterType     oneWrite;
        Object::CounterType    writeTimes;
        Object::CounterType     readTimes;
        Object::CounterType fragReadBits;

        const Object::PortBitWidthType bit;

    private:
        RegParam &operator=(const RegParam&);

    public:
        RegParam(PortBitWidthType bit)
            :bit(bit){
            this->totalNumbers  = 1;
            this->   writeTimes = 0;
            this->    readTimes = 0;
            this->   zeroWrite  = 0;
            this->    oneWrite  = 0;
            this->fragReadBits  = 0;
            return;
        }
        inline void Alloc(){
            this->totalNumbers++;
            return;
        }
        inline bool Equal(Object::PortBitWidthType bit){
            return this->bit == bit;
        }
        inline void ReadBits(PortBitWidthType bit){
            this->fragReadBits += bit;
            return;
        }
        inline void AddCounter(Reg::RegValueType next) {
            this->writeTimes++;
            for (int i=0;i<this->bit;i++)
                if ((next & Object::highest[i])==0)
                    this->zeroWrite++;
                else
                    this-> oneWrite++;
            return;
        }
        inline void CloneCounter(Object::Port& port){
            port.CloneCounter(this->readTimes);
            return;
        }
        inline void PrintPower(FILE* fp){
            fprintf(fp,"\t%d-bit register\n",this->bit);
            fprintf(fp,"\t\ttotal write  bits:\t%s\n",String::NumToString(this->writeTimes * this->bit).c_str());
            fprintf(fp,"\t\ttotal read   bits:\t%s\n",String::NumToString(this-> readTimes * this->bit
                                                                        + this->fragReadBits).c_str());
            fprintf(fp,"\t\twrite zero  times:\t%s\n",String::NumToString(this->zeroWrite   ).c_str());
            fprintf(fp,"\t\twrite  one  times:\t%s\n",String::NumToString(this->oneWrite    ).c_str());
            return;
        }
        inline void PrintArea(FILE* fp){
            fprintf(fp,"\t%d-bit register\n",this->bit);
            fprintf(fp,"\t\ttotal numbers:\t%s\n",String::NumToString(this->totalNumbers).c_str());
            fprintf(fp,"\t\ttotal  bits  :\t%s\n",String::NumToString(this->totalNumbers * this->bit).c_str());
            return;
        }
        inline CounterType GetTotalRegBit() const{
            return this->totalNumbers
                 * this->bit;
        }
        inline CounterType GetZeroWrite() const{
            return this->zeroWrite;
        }
        inline CounterType GetOneWrite() const{
            return this->oneWrite;
        }
        inline CounterType GetReadBits() const{
            return   this->readTimes
                   * this->bit
                   + this->fragReadBits;
        }
    };

private:

    static vector<RegParam> regParam;
    static bool hasReserved;

    int RegIdx;

    ClockedObject::ClockEdge clockEdge;
    /** the type of edge which the register
     **/

    const PortBitWidthType bit;
    /** the total bit of the register
     **/

    RegValueType value;
    /** the current value stored in the register,
     *  i.e. the value on the output port
     *       also the value downstream component will get
     **/

    RegValueType next;
    /** the value on the input port
     *  i.e. the value would write to the register
     *       when the edge of clock comes
     **/


    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::PortNum());
        this->ports.emplace_back(bit,PortDirection::OUT);
        this->ports.emplace_back(bit,PortDirection::IN );
        this->ports.emplace_back(  1,PortDirection::IN );
        assert(this->ports.size()==PortName::PortNum());
        return;
    }
    inline void OutputConnect() override{
        this->ports[out.ToIndex()].Clone(this->value);
        Reg::regParam[this->RegIdx].CloneCounter(this->ports[out.ToIndex()]);
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
          out = 0, /** n-bit out **/
          in  = 1, /** n-bit  in **/
          en  = 2, /** 1-bit  in **/
          /************************/
          PortNumber
        };
    private:
        _PortName portName;
    public:
        PortName(enum _PortName _portName):portName(_portName){
            assert(_portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static PortNumType PortNum(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };

    static const Reg::PortName out;
    static const Reg::PortName in;
    static const Reg::PortName en;

private:
    Reg(const Reg& reg);
    Reg &operator=(const Reg&);
public:

    Reg(PortBitWidthType  bit,
        ClockEdge   clockEdge,
        RegValueType  value=0,
        RegValueType   next=0)
        :bit(bit){
        assert(this->bit<=(sizeof(RegValueType)*8));
        /*****************************
         ** initialize the settings **
         *****************************/
        this->clockEdge = clockEdge;
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->value = value;
        this->next  = next;

        bool hasSame = false;
        if (!Reg::hasReserved){
            ///temporary solution for Param
            Reg::regParam.reserve(MAX_PARAM_SIZE);
            hasReserved=true;
        }
        for (int i=0;i<Reg::regParam.size();i++)
            if (Reg::regParam[i].Equal(bit)){
                this->RegIdx= i;
                Reg::regParam[i].Alloc();
                hasSame = true;
                break;
            }
        if (!hasSame){
            ///temporary solution for Param
            assert(Reg::regParam.size()<=MAX_PARAM_SIZE);
            Reg::regParam.emplace_back(bit);
            this->RegIdx = Reg::regParam.size()-1;
        }

        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }

    inline bool atPosEdge() override{
        if (this->clockEdge == ClockEdge::NEG_EDGE){
            assert(false);
            this->next = this->value;
            return false;
        }

        if (!this->ports[Reg::en.ToIndex()].Data()){
            this->next = this->value;
            return false;
        }
        this->ports[Reg::in.ToIndex()].Read();
        this->next = this->ports[Reg::in.ToIndex()].Data();
        Reg::regParam[this->RegIdx].AddCounter(this->next);
        return true;
    }
    inline bool atNegEdge() override{
        if (this->clockEdge == ClockEdge::POS_EDGE){
            assert(false);
            this->next = this->value;
            return false;
        }
        if (!this->ports[PORT_INDEX(en)].Data()){
            this->next = this->value;
            return false;
        }
        this->ports[Reg::in.ToIndex()].Read();
        this->next = this->ports[PORT_INDEX(in)].Data();
        Reg::regParam[this->RegIdx].AddCounter(this->next);
        return true;
    }
    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return this->ports[PORT_INDEX(out)].SameTo(this->value);
    }
    inline bool Run() override{
        if (this->value!=this->next){
            this->value =this->next;
            return true;
        }
        return false;
    }
    inline void Print() const{
        std::cout<<"en : "<<this->ports[PORT_INDEX(en)].Data()<<std::endl;
        std::cout<<"in : "<<this->ports[PORT_INDEX(in)].Data()<<std::endl;
        std::cout<<"out: "<<this->value                       <<std::endl;
        return;
    }
    inline void PrintAddr() const{
        std::cout<<"en   addr: ";
        this->ports[PORT_INDEX(en)].PrintAddr();
        std::cout<<std::endl;
        std::cout<<"in   addr: ";
        this->ports[PORT_INDEX(in)].PrintAddr();
        std::cout<<std::endl;
        std::cout<<"out  addr: ";
        this->ports[PORT_INDEX(out)].PrintAddr();
        std::cout<<std::endl;
    }

    inline void PrintTo(FILE* fp) const{
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<setw(4)<<this->value<<" ";
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(fp,"%4d ",this->value);
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        return;
    }

    inline void ReadBits(PortBitWidthType bit){
        Reg::regParam[this->RegIdx].ReadBits(bit);
        return;
    }

    static inline void PrintPower(FILE* fp){
        fprintf(fp,"Register activation statistics:\n");
        Object::CounterType
            totalZeroWrite = 0,
             totalOneWrite = 0,
             totalReadBits = 0;
        for (auto& _regParam : regParam){
            totalZeroWrite += _regParam.GetZeroWrite();
             totalOneWrite += _regParam.GetOneWrite();
             totalReadBits += _regParam.GetReadBits();
        }
        fprintf(fp,"\tRegister total zero write bits:\t%s-bit\n",String::NumToString(totalZeroWrite).c_str());
        fprintf(fp,"\tRegister total  one write bits:\t%s-bit\n",String::NumToString(totalOneWrite ).c_str());
        fprintf(fp,"\tRegister total    read    bits:\t%s-bit\n",String::NumToString(totalReadBits ).c_str());
        for (int i=0;i<Reg::regParam.size();i++)
            Reg::regParam[i].PrintPower(fp);
        return;
    }
    static inline void PrintArea (FILE* fp){
        fprintf(fp,"Register area statistics:\n");
        Object::CounterType totalBit = 0;
        for (auto& _regParam : regParam)
            totalBit += _regParam.GetTotalRegBit();
        fprintf(fp,"\tRegister total capacity:\t%s-bit\n",String::NumToString(totalBit).c_str());
        for (int i=0;i<Reg::regParam.size();i++)
            Reg::regParam[i].PrintArea(fp);
        return;
    }
    static inline void ResetStatis(){
        Reg::regParam.clear();
        return;
    }
};

class MarkReg: public ClockedObject{
    /**  it is the abstract module of the mark register for
     *   simulation
     *   it is an 1-bit register and can only be set or clear,
     *   to trans information between neighboring PE
     **/
     typedef PortDataType MarkRegType;

    class MarkRegParam{
    private:
        Object::CounterType totalNumbers;
        Object::CounterType    clrTimes;
        Object::CounterType    setTimes;
        Object::CounterType  writeTimes;
        Object::CounterType   readTimes;

    private:
        MarkRegParam &operator=(const MarkRegParam&);

    public:
        MarkRegParam(){
            this->totalNumbers= 0;
            this->writeTimes  = 0;
            this-> readTimes  = 0;
            this->  setTimes  = 0;
            this->  clrTimes  = 0;
            return;
        }
        inline void Alloc(){
            this->totalNumbers++;
            return;
        }
        inline void Clr(){
            this->clrTimes++;
            this->writeTimes++;
            return;
        }
        inline void Set(){
            this->setTimes++;
            this->writeTimes++;
            return;
        }
        inline void CloneCounter(Object::Port& port){
            port.CloneCounter(this->readTimes);
            return;
        }
        inline void PrintPower(FILE* fp){
            fprintf(fp,"\tmark register (1-bit register)\n");
            fprintf(fp,"\t\ttotal write  bits:\t%s\n",String::NumToString(this->writeTimes).c_str());
            fprintf(fp,"\t\ttotal read   bits:\t%s\n",String::NumToString(this-> readTimes).c_str());
            fprintf(fp,"\t\tclr (write zero)  times:\t%s\n",String::NumToString(this->clrTimes).c_str());
            fprintf(fp,"\t\tset (write  one)  times:\t%s\n",String::NumToString(this->setTimes).c_str());
            return;
        }
        inline void PrintArea(FILE* fp){
            fprintf(fp,"\tmark register (1-bit register)\n");
            fprintf(fp,"\t\ttotal numbers:\t%s\n",String::NumToString(this->totalNumbers).c_str());
            return;
        }
        inline CounterType GetTotalRegBit() const{
            return this->totalNumbers;
        }
        inline CounterType GetClrTimes() const{
            return this->clrTimes;
        }
        inline CounterType GetSetTimes() const{
            return this->setTimes;
        }
        inline CounterType GetReadBits() const{
            return   this->readTimes;
        }
        inline void Reset() {
            this->totalNumbers= 0;
            this->writeTimes  = 0;
            this-> readTimes  = 0;
            this->  setTimes  = 0;
            this->  clrTimes  = 0;
            return;
        }
    };

    static MarkRegParam markRegParam;

private:
    ClockedObject::ClockEdge clockEdge;
    /** the type of edge which the register
     **/

    MarkRegType value;
    /** the current value stored in the register,
     *  i.e. the value on the output port
     *       also the value downstream component will get
     **/

    MarkRegType next;
    /** the value on the input port
     *  i.e. the value would write to the register
     *       when the edge of clock comes
     **/

    inline bool SetNext(){
        if (!(this->ports[PORT_INDEX(Set)].Data()
             |this->ports[PORT_INDEX(Clr)].Data())){
            this->next = this->value;
            return false;
        }
        if (this->ports[PORT_INDEX(Set)].Data()){
            this->next = 1;
            MarkReg::markRegParam.Set();
        }
        else{
            this->next = 0;
            MarkReg::markRegParam.Clr();
        }
        return true;
    }

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::PortNum());
        this->ports.push_back(Port(1,PortDirection::OUT));
        this->ports.push_back(Port(1,PortDirection::IN ));
        this->ports.push_back(Port(1,PortDirection::IN ));
        assert(this->ports.size()==PortName::PortNum());
        return;
    }
    inline void OutputConnect() override{
        this->ports[PORT_INDEX(out)].Clone(this->value);
        MarkReg::markRegParam.CloneCounter(this->ports[PORT_INDEX(out)]);
        assert(this->OutputConnected());
        return;
    }

private:
    MarkReg(const MarkReg& MarkReg);
    MarkReg &operator=(const MarkReg&);

public:
    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
          out = 0, /** 1-bit out **/
          Set = 1, /** 1-bit  in **/
          Clr = 2, /** 1-bit  in **/
          /************************/
          PortNumber
        };
    private:
        enum _PortName portName;
    public:
        PortName(enum _PortName _portName):portName(_portName){
            assert(_portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static inline PortNumType PortNum(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };
    static const MarkReg::PortName out;
    static const MarkReg::PortName Clr;
    static const MarkReg::PortName Set;

    MarkReg(
        ClockEdge       clockEdge,
        MarkRegType     value=0,
        MarkRegType     next =0){
        /*****************************
         ** initialize the settings **
         *****************************/
        this->clockEdge = clockEdge;
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->next = next;
        this->value= value;
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        MarkReg::markRegParam.Alloc();
        return;
    }
    inline bool atPosEdge() override{
        assert(this->InputConnected());
        assert(this->ports[PORT_INDEX(Set)].Data()==0
            || this->ports[PORT_INDEX(Clr)].Data()==0);
        if (this->clockEdge == ClockEdge::NEG_EDGE){
            assert(false);
            this->next = this->value;
            return false;
        }
        return this->SetNext();
    }
    inline bool atNegEdge() override{
        assert(this->InputConnected());
        assert(this->ports[PORT_INDEX(Set)].Data()==0
            || this->ports[PORT_INDEX(Clr)].Data()==0);
        assert(this->ports[PORT_INDEX(Set)].Data()==0
            || this->value==0);
        assert(this->ports[PORT_INDEX(Clr)].Data()==0
            || this->value==1);
        if (this->clockEdge == ClockEdge::POS_EDGE){
            assert(false);
            this->next = this->value;
            return false;
        }
        return this->SetNext();
    }
    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return this->ports[PORT_INDEX(out)].SameTo(this->value);
    }
    inline bool Run() override{
        if (this->value!=this->next){
            this->value =this->next;
            return true;
        }
        return false;
    }
    inline void Print() const{
        std::cout<<"clr  "     <<this->ports[PORT_INDEX(Clr)].Data()<<std::endl;
        std::cout<<"set  "     <<this->ports[PORT_INDEX(Set)].Data()<<std::endl;
        std::cout<<"out  "<<hex<<this->value                        <<std::endl;
        return;
    }

    static inline void PrintPower(FILE* fp){
        fprintf(fp,"Mark Register activation statistics:\n");
        MarkReg::markRegParam.PrintPower(fp);
        return;
    }
    static inline void PrintArea (FILE* fp){
        fprintf(fp,"Mark Register area statistics:\n");
        MarkReg::markRegParam.PrintArea(fp);
        return;
    }
    static inline void ResetStatis(){
        MarkReg::markRegParam.Reset();
        return;
    }
};

class Counter: public ClockedObject{
    /**  it is the abstract module of the counter
     *   the behavior is different to the standard counter
     *   it starts to count when it receive 'en',
     *   and would hold done==1 when the counter reach the target cycle
     *   it would not receive 'en' during the counting process
     **/
     typedef uint32_t CounterValueType;
private:
    PortDataType     output;
    CounterValueType nowValue;
    CounterValueType targeValue;
    ClockEdge        clockEdge;
    bool isCounting;

    inline void InitializePorts() override{
        this->ports.reserve(PortName::Number());
        this->ports.emplace_back(1,PortDirection::IN );/**  en  **/
        this->ports.emplace_back(1,PortDirection::IN );/** rst  **/
        this->ports.emplace_back(1,PortDirection::OUT);/** done **/
        assert(this->ports.size()==PortName::Number());
        return;
    }
    inline void OutputConnect() override{
        this->ports[PORT_INDEX(done)].Clone(this->output);
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
          en  = 0, /** 1-bit  in **/
          rst = 1, /** 1-bit  in **/
          done= 2, /** 1-bit out **/
          /************************/
          PortNumber
        };
    private:
        enum _PortName portName;
    public:
        PortName(enum _PortName _portName):portName(_portName){
            assert(_portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static inline PortNumType Number(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };
    Counter(CounterValueType targeValue,
            ClockEdge        clockEdge,
            CounterValueType initValue=0,
            PortDataType     initOutput=0,
            bool isCounting = false){
        /*****************************
         ** initialize the settings **
         *****************************/
        this->clockEdge  = clockEdge;
        this->targeValue = targeValue;
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->nowValue   = initValue;
        this->output     = initOutput;
        this->isCounting = isCounting;
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }
    Counter(const Counter& counter){
        assert(counter.AllInputNotConnected());
        /*****************************
         ** initialize the settings **
         *****************************/
        this->clockEdge  = counter.clockEdge;
        this->targeValue = counter.targeValue;
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->nowValue   = counter.nowValue;
        this->output     = counter.output;
        this->isCounting = counter.isCounting;
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }
    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return this->ports[PORT_INDEX(done)].SameTo(this->output);
    }
    inline bool atPosEdge() override{
        assert(this->ports[PORT_INDEX(en)].Data()==0
            || this->ports[PORT_INDEX(en)].Data()==1);
        if (this->clockEdge == ClockEdge::NEG_EDGE){
            assert(false);
            this->isCounting = false;
            return false;
        }
        assert(this->ports[PORT_INDEX(rst)].Data()==0);
        if (this->ports[PORT_INDEX(rst)].Data()==1)
            this->nowValue  = 0;
        if (this->ports[PORT_INDEX(en)].Data()){
            assert(!this->isCounting);
            this->isCounting=true;
            this->nowValue  = 1;
        }
        return true;
    }
    inline bool atNegEdge() override{
        assert(this->ports[PORT_INDEX(en)].Data()==0
            || this->ports[PORT_INDEX(en)].Data()==1);
        if (this->clockEdge == ClockEdge::POS_EDGE){
            assert(false);
            this->isCounting = false;
            return false;
        }
        assert(this->ports[PORT_INDEX(rst)].Data()==0);
        if (this->ports[PORT_INDEX(rst)].Data()==1)
            this->nowValue  = 0;
        if (this->ports[PORT_INDEX(en)].Data()){
            assert(!this->isCounting);
            this->isCounting=true;
            this->nowValue  = 0;
        }
        return true;
    }
    inline bool Run() override{
        assert((this->output==0 || this->output==1)
             && this->nowValue  <= this->targeValue);
        if(!this->isCounting){
            assert(this->nowValue==this->targeValue?
                  (this->output==1):
                  (this->output==0/*||this->nowValue==0*/));
            /// can just be reseted
            if (this->output == (this->nowValue==this->targeValue))
                return false;
            this->output = (this->nowValue==this->targeValue);
            return true;
        }
        assert(this->nowValue<this->targeValue
            &&(this->output == 0 || this->nowValue==0));
        this->nowValue++;
        if (this->nowValue == this->targeValue){
            this->output  = 1;
            this->isCounting=false;
        }
        else
            this->output = 0;
        return true;
    }
    inline void Print() const{
        std::cout<<"en   "<<this->ports[PORT_INDEX(en)].Data()<<std::endl;
        std::cout<<"done "<<this->output                      <<std::endl;
        std::cout<<"now  "<<this->nowValue                    <<std::endl;
        return;
    }
};

class FIFO: public ClockedObject{
private:

    /**  it is the abstract module of FIFO
     **/
    typedef PortDataType FIFODataType;
    typedef uint32_t     FIFOSizeType;

    class FIFOParam{
    private:
        const Object::PortBitWidthType width;
        const   FIFO::FIFODataType     depth;

        Object::CounterType pushTime;
        Object::CounterType  popTime;

        Object::CounterType zeroWrite;
        Object::CounterType  oneWrite;
        Object::CounterType writeBit ;

        Object::CounterType readTimes;

        int number;

    public:
        FIFOParam(Object::PortBitWidthType width,
                    FIFO::FIFODataType     depth
            ):width(width),
              depth(depth){
            this->pushTime  = 0;
            this-> popTime  = 0;

            this->zeroWrite = 0;
            this-> oneWrite = 0;
            this->writeBit  = 0;

            this->readTimes = 0;

            this->number    = 1;
            return;
        }
        inline bool Equal(Object::PortBitWidthType width,
                            FIFO::FIFODataType     depth) const{
            if ((this->width == width)
             && (this->depth == depth))
                return true;
            return false;
        }
        inline void Alloc(){
            this->number++;
            return;
        }
        inline void AddCount(bool push,bool pop,PortDataType writeData){
            if (push){
                this->pushTime++;
                this->writeBit+=this->width;
                for (int i=0;i<this->width;i++)
                    if ((writeData & Object::highest[i])==0)
                        this->zeroWrite++;
                    else
                        this->oneWrite++;
            }
            if (pop)
                this->popTime++;
            return;
        }

        inline void CloneCounter(Object::Port& port){
            port.CloneCounter(this->readTimes);
            return;
        }

        inline Object::CounterType GetTotalSize() const{
            return this->number
                 * this->width
                 * this->depth;
        }
        inline Object::CounterType GetTotalWriteBits() const{
            return this->writeBit;
        }
        inline Object::CounterType  GetZeroWriteBits() const{
            return this->zeroWrite;
        }
        inline Object::CounterType   GetOneWriteBits() const{
            return this->oneWrite;
        }
        inline Object::CounterType  GetTotalReadBits() const{
            return this->readTimes
                 * this->width;
        }

        inline void PrintPower(FILE* fp) const{
            fprintf(fp,"\tFIFO parameters: width:%d depth:%d\n",this->width,this->depth);
            fprintf(fp,"\t\ttotal push times:\t%s\n",String::NumToString(this->pushTime).c_str());
            fprintf(fp,"\t\ttotal  pop times:\t%s\n",String::NumToString(this-> popTime).c_str());

            fprintf(fp,"\t\ttotal write bits:\t%s\n",String::NumToString(this-> writeBit).c_str());
            fprintf(fp,"\t\t zero write bits:\t%s\n",String::NumToString(this->zeroWrite).c_str());
            fprintf(fp,"\t\t  one write bits:\t%s\n",String::NumToString(this-> oneWrite).c_str());

            fprintf(fp,"\t\ttotal  read bits:\t%s\n",String::NumToString(this->readTimes
                                                                       * this->width).c_str());
            return;
        }
        inline void PrintArea(FILE* fp) const{
            fprintf(fp,"\tFIFO parameters: width:%d depth:%d\n",this->width,this->depth);
            fprintf(fp,"\t\ttotal number:\t%s\n",String::NumToString(this->number).c_str());
            return;
        }
    };

private:
    static vector<FIFOParam> fifoParam;
    static bool hasReserved;

    int FIFOIdx;

    queue<FIFODataType> fifo;
    FIFODataType _Out;
    PortDataType _Full,_Empty;
    const PortBitWidthType bit;
    FIFOSizeType     fifoSize;
    ClockEdge        clockEdge;

    inline void InitializePorts() override{
        this->ports.reserve(PortName::Number());
        this->ports.emplace_back(bit,PortDirection::IN );/** In    n-bit  in **/
        this->ports.emplace_back(bit,PortDirection::OUT);/** Out   n-bit out **/
        this->ports.emplace_back(  1,PortDirection::IN );/** Push  1-bit  in **/
        this->ports.emplace_back(  1,PortDirection::IN );/** Pop   1-bit  in **/
        this->ports.emplace_back(  1,PortDirection::OUT);/** Full  1-bit out **/
        this->ports.emplace_back(  1,PortDirection::OUT);/** Empty 1-bit out **/
        assert(this->ports.size()==PortName::Number());
        return;
    }

    inline void OutputConnect() override{
        this->ports[FIFO::Out  .ToIndex()].Clone(this->_Out  );
        this->ports[FIFO::Full .ToIndex()].Clone(this->_Full );
        this->ports[FIFO::Empty.ToIndex()].Clone(this->_Empty);
        FIFO::fifoParam[this->FIFOIdx].CloneCounter(
            this->ports[FIFO::Out.ToIndex()]
        );
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
          In   = 0, /** n-bit  in **/
          Out  = 1, /** n-bit out **/
          Push = 2, /** 1-bit  in **/
          Pop  = 3, /** 1-bit  in **/
          Full = 4, /** 1-bit out **/
          Empty= 5, /** 1-bit out **/
          /*************************/
          PortNumber
        };
    private:
        enum _PortName portName;
    public:
        PortName(enum _PortName _portName):portName(_portName){
            assert(_portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static inline PortNumType Number(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };

    static const FIFO::PortName In   ; /** n-bit  in **/
    static const FIFO::PortName Out  ; /** n-bit out **/
    static const FIFO::PortName Push ; /** 1-bit  in **/
    static const FIFO::PortName Pop  ; /** 1-bit  in **/
    static const FIFO::PortName Full ; /** 1-bit out **/
    static const FIFO::PortName Empty; /** 1-bit out **/

    FIFO(PortBitWidthType dataWidth,
         FIFOSizeType fifoSize,
         ClockEdge    clockEdge,
         FILE*        initValue=nullptr,
         FIFODataType _Out=0,
         PortDataType _Full=0,
         PortDataType _Empty=1
        ):bit(dataWidth){
        assert(initValue == nullptr && _Out==0);
        /*****************************
         ** initialize the settings **
         *****************************/
        assert(this->fifo.empty());
        this->fifoSize  = fifoSize;
        this->clockEdge = clockEdge;
        this->_Out   = _Out;
        this->_Full  = _Full;
        this->_Empty = _Empty;
        bool hasSame = false;
        if (!FIFO::hasReserved){
            ///temporary solution for Param
            FIFO::fifoParam.reserve(MAX_PARAM_SIZE);
            hasReserved=true;
        }
        for (int i=0;i<FIFO::fifoParam.size();i++)
            if (FIFO::fifoParam[i].Equal(dataWidth,fifoSize)){
                FIFO::fifoParam[i].Alloc();
                this->FIFOIdx= i;
                hasSame = true;
                break;
            }
        if (!hasSame){
            ///temporary solution for Param
            assert(FIFO::fifoParam.size()<=MAX_PARAM_SIZE);
            this->fifoParam.emplace_back(dataWidth,fifoSize);
            this->FIFOIdx = this->fifoParam.size()-1;
        }
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }

    FIFO(const FIFO& _fifo):bit(_fifo.bit){
        assert(_fifo.AllInputNotConnected());
        /*****************************
         ** initialize the settings **
         *****************************/
        this->_Out      = _fifo._Out;
        this->_Full     = _fifo._Full;
        this->_Empty    = _fifo._Empty;
        this->FIFOIdx   = _fifo.FIFOIdx;
        this->fifoSize  = _fifo.fifoSize;
        this->clockEdge = _fifo.clockEdge;
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }
    inline void InnerConnect() override{
        assert(this->InputConnected());
        return;
    }
    inline bool InnerConnected() const override{
        return true;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }

    inline bool OutputConnectedCorrect() const override{
        return  this->ports[PORT_INDEX(Out  )].SameTo(this->_Out  )
             && this->ports[PORT_INDEX(Full )].SameTo(this->_Full )
             && this->ports[PORT_INDEX(Empty)].SameTo(this->_Empty);
    }
    inline bool atPosEdge() override{
        assert(this->ports[PORT_INDEX(Push)].Data()==0 || !this->_Full );
        assert(this->ports[PORT_INDEX(Pop )].Data()==0 || !this->_Empty);
        if (this->clockEdge == ClockEdge::NEG_EDGE){
            assert(false);
            return false;
        }
        if (this->ports[PORT_INDEX(Push)].Data()==1){
            this->fifo.push(this->ports[PORT_INDEX(In)].Data());
            this->ports[PORT_INDEX(In)].Read();
        }
        if (this->ports[PORT_INDEX(Pop )].Data()==1)
            this->fifo.pop();
        FIFO::fifoParam[this->FIFOIdx].AddCount(
            (this->ports[PORT_INDEX(Push)].Data()==1),
            (this->ports[PORT_INDEX(Pop )].Data()==1),
            (this->ports[PORT_INDEX(In  )].Data())
        );
        return  this->ports[PORT_INDEX(Push)].Data()
             || this->ports[PORT_INDEX(Pop )].Data();
    }
    inline bool atNegEdge() override{
        assert(this->ports[PORT_INDEX(Push)].Data()==0 || !this->_Full );
        assert(this->ports[PORT_INDEX(Pop )].Data()==0 || !this->_Empty);
        if (this->clockEdge == ClockEdge::POS_EDGE){
            assert(false);
            return false;
        }
        if (this->ports[PORT_INDEX(Push)].Data()==1){
            this->fifo.push(this->ports[PORT_INDEX(In)].Data());
            this->ports[PORT_INDEX(In)].Read();
        }
        if (this->ports[PORT_INDEX(Pop )].Data()==1)
            this->fifo.pop();
        FIFO::fifoParam[this->FIFOIdx].AddCount(
            (this->ports[PORT_INDEX(Push)].Data()==1),
            (this->ports[PORT_INDEX(Pop )].Data()==1),
            (this->ports[PORT_INDEX(In  )].Data())
        );
        this->ports[PORT_INDEX(In)].Read();
        return  this->ports[PORT_INDEX(Push)].Data()
             || this->ports[PORT_INDEX(Pop )].Data();
    }
    inline bool Run() override{
        if  (!(this->fifo.size()<=this->fifoSize)){
            std::cout<<this->fifo.size()<<std::endl;
            getchar();
        }
        assert(this->fifo.size()<=this->fifoSize);
//        bool hasChange;
//        hasChange = this->fifo.empty()?
//                   (this->_Empty==1):
//                   (this->_Empty==0);
        this->_Empty= this->fifo.empty();
//        hasChange =
//        hasChange&&(this->fifo.size()==this->fifoSize?
//                                      (this->_Full==1):
//                                      (this->_Full==0));
        this->_Full =(this->fifo.size()==this->fifoSize);
//        if (this->fifo.size()==0)
//            return hasChange;
//        if (this->_Out==this->fifo.front())
//            return hasChange;
        this->_Out  = this->fifo.front();
        return true;
    }
    inline void Print(){
        std::cout<<"[in   ]"<<this->ports[PORT_INDEX(In  )].Data()<<std::endl;
        std::cout<<"[push ]"<<this->ports[PORT_INDEX(Push)].Data()<<std::endl;
        std::cout<<"[pop  ]"<<this->ports[PORT_INDEX(Pop )].Data()<<std::endl;
        std::cout<<"[out  ]"<<this->_Out  <<std::endl;
        std::cout<<"[full ]"<<this->_Full <<std::endl;
        std::cout<<"[empty]"<<this->_Empty<<std::endl;
        std::cout<<"[data ]" <<std::endl;
        int fifoSize = this->fifo.size();
        for (int i=0;i<fifoSize;i++){
            std::cout<<this->fifo.front()<<" ";
            this->fifo.push(this->fifo.front());
            this->fifo.pop ();
        }
        std::cout<<std::endl;
        return;
    }

    static inline void PrintPower(FILE* fp){
        fprintf(fp,"FIFO activation statistics:\n");

        Object::CounterType totalBits = 0;
        for (auto& _fifoParam : fifoParam)
            totalBits += _fifoParam.GetTotalWriteBits();
        fprintf(fp,"\tFIFO total write bits:\t%s-bit\n",String::NumToString(totalBits).c_str());

        Object::CounterType totalZeroWriteBits = 0;
        for (auto& _fifoParam : fifoParam)
            totalZeroWriteBits += _fifoParam.GetZeroWriteBits();
        fprintf(fp,"\tFIFO total zero write:\t%s-bit\n",String::NumToString(totalZeroWriteBits).c_str());

        Object::CounterType totalOneWriteBits = 0;
        for (auto& _fifoParam : fifoParam)
            totalOneWriteBits  += _fifoParam. GetOneWriteBits();
        fprintf(fp,"\tFIFO total  one write:\t%s-bit\n",String::NumToString(totalOneWriteBits).c_str());

        Object::CounterType totalReadBits = 0;
        for (auto& _fifoParam : fifoParam)
            totalReadBits += _fifoParam.GetTotalReadBits();
        fprintf(fp,"\tFIFO total read bits :\t%s-bit\n",String::NumToString(totalReadBits).c_str());

        for (int i=0;i<FIFO::fifoParam.size();i++)
            FIFO::fifoParam[i].PrintPower(fp);
        return;
    }
    static inline void PrintArea (FILE* fp){
        fprintf(fp,"FIFO area statistics:\n");
        Object::CounterType totalBits = 0;
        for (auto& _fifoParam : fifoParam)
            totalBits += _fifoParam.GetTotalSize();
        fprintf(fp,"\tFIFO total capacity:\t%s-bit\n",String::NumToString(totalBits).c_str());
        for (int i=0;i<FIFO::fifoParam.size();i++)
            FIFO::fifoParam[i].PrintArea(fp);
        return;
    }
    static inline void ResetStatis(){
        FIFO::fifoParam.clear();
        return;
    }
};


//class TwoEleContainer: public ClockedObject{
//    /**
//     **/
//     typedef PortDataType MultiStateMarkRegType;
//
//private:
//    ClockedObject::ClockEdge clockEdge;
//    /** the type of edge which the register
//     **/
//
//
//    MultiStateMarkRegType output;
//
//    MultiStateMarkRegType maximum;
//    /** maximum state numbers
//     **/
//
//    MultiStateMarkRegType value;
//    /** the current value stored in the register,
//     *  i.e. the value on the output port
//     *       also the value downstream component will get
//     **/
//
//    inline bool SetNext(){
//        if (this->ports[PORT_INDEX(add)].Data()
//         == this->ports[PORT_INDEX(sub)].Data())
//            return false;
//        if (this->ports[PORT_INDEX(add)].Data()==1)
//            this->value++;
//        else
//            this->value--;
//        return true;
//    }
//
//    inline void InitializePorts() override{
//        this->ports.clear();
//        this->ports.reserve(PortName::PortNum());
//        this->ports.push_back(Port(1,PortDirection::OUT));
//        this->ports.push_back(Port(1,PortDirection::IN ));
//        this->ports.push_back(Port(1,PortDirection::IN ));
//        assert(this->ports.size()==PortName::PortNum());
//        return;
//    }
//    inline void OutputConnect() override{
//        this->ports[PORT_INDEX(out)].Clone(this->output);
//        assert(this->OutputConnected());
//        return;
//    }
//
//public:
//    class PortName:public BasePortName{
//    public:
//        enum class _PortName:uint8_t{
//          out = 0, /** 1-bit out **/
//          add = 1, /** 1-bit  in **/
//          sub = 2, /** 1-bit  in **/
//          /************************/
//          PortNumber
//        };
//    private:
//        enum _PortName portName;
//    public:
//        PortName(enum _PortName _portName):portName(_portName){
//            assert(_portName != _PortName::PortNumber);
//            return;
//        }
//        inline PortNumType ToIndex() const override{
//            return static_cast<PortNumType>(this->portName);
//        }
//        static inline PortNumType PortNum(){
//            return static_cast<PortNumType>(_PortName::PortNumber);
//        }
//        static inline PortNumType ToIndex(enum _PortName portName){
//            assert(portName != _PortName::PortNumber);
//            return static_cast<PortNumType>(portName);
//        }
//    };
//    static const MultiStateMarkReg::PortName out;
//    static const MultiStateMarkReg::PortName add;
//    static const MultiStateMarkReg::PortName sub;
//
//    MultiStateMarkReg(
//        ClockEdge       clockEdge,
//        MultiStateMarkRegType maximum,
//        MultiStateMarkRegType value =0,
//        MultiStateMarkRegType next  =0,
//        MultiStateMarkRegType output=0){
//        /*****************************
//         ** initialize the settings **
//         *****************************/
//        this->clockEdge = clockEdge;
//        this->maximum   = maximum;
//        /********************************************************
//         ** initialize the data in case of restoring the scene **
//         ********************************************************/
//        this->value = value;
//        this->output= output;
//        /**************************
//         ** initialize the ports  *
//         **************************/
//        this->InitializePorts();
//        /*******************************************
//         ** connect output ports with inner logic **
//         *******************************************/
//        this->OutputConnect();
//        return;
//    }
//    MultiStateMarkReg(const MultiStateMarkReg& msMarkReg){
//        assert(msMarkReg.AllInputNotConnected());
//        /*****************************
//         ** initialize the settings **
//         *****************************/
//        this->clockEdge = msMarkReg.clockEdge;
//        this->maximum   = msMarkReg.maximum;
//        /********************************************************
//         ** initialize the data in case of restoring the scene **
//         ********************************************************/
//        this->value = msMarkReg.value;
//        this->output= msMarkReg.output;
//        /**************************
//         ** initialize the ports  *
//         **************************/
//        this->InitializePorts();
//        /*******************************************
//         ** connect output ports with inner logic **
//         *******************************************/
//        this->OutputConnect();
//        return;
//    }
//    inline bool atPosEdge() override{
//        assert(this->InputConnected());
//        assert(this->ports[PORT_INDEX(sub)].Data()==0
//            || this->value==0);
//        assert(this->ports[PORT_INDEX(clr)].Data()==0
//            || this->value==this->maximum);
//        if (this->clockEdge == ClockEdge::NEG_EDGE){
//            assert(false);
//            this->next = this->value;
//            return false;
//        }
//        return this->SetNext();
//    }
//    inline bool atNegEdge() override{
//        assert(this->InputConnected());
//        assert(this->ports[PORT_INDEX(Set)].Data()==0
//            || this->ports[PORT_INDEX(Clr)].Data()==0);
//        assert(this->ports[PORT_INDEX(Set)].Data()==0
//            || this->value==0);
//        assert(this->ports[PORT_INDEX(Clr)].Data()==0
//            || this->value==1);
//        if (this->clockEdge == ClockEdge::POS_EDGE){
//            assert(false);
//            this->next = this->value;
//            return false;
//        }
//        return this->SetNext();
//    }
//    inline void InnerConnect() override{
//        assert(this->InputConnected());
//        return;
//    }
//    inline bool InnerConnected() const override{
//        return true;
//    }
//    inline bool InnerConnectedCorrect() const override{
//        return true;
//    }
//    inline bool OutputConnectedCorrect() const override{
//        return this->ports[PORT_INDEX(out)].SameTo(this->output);
//    }
//    inline bool Run() override{
//        this->output = (this->value==this->maximum);
//        return true;
//    }
//    inline void Print() const{
//        std::cout<<"sub "<<this->ports[PORT_INDEX(sub)].Data()<<std::endl;
//        std::cout<<"add "<<this->ports[PORT_INDEX(add)].Data()<<std::endl;
//        std::cout<<"out "<<this->value                        <<std::endl;
//        return;
//    }
//};
#endif // __RESOURCE_BASE_SEQ_LOGIC_HPP
