#ifndef __COMPONENT_PE_HPP
#define __COMPONENT_PE_HPP

#include "../base/Object.hpp"
#include "../interface/Port.hpp"
#include "../resource/BaseSequLogic.hpp"

#define      REG_INDEX(a) (    RegName::ToIndex(    RegName::    _RegName::a))
#define MARK_REG_INDEX(a) (MarkRegName::ToIndex(MarkRegName::_MarkRegName::a))

class PE_CL0: public ClockedObject{
    /**********************************************
     *  a part of combinational logical for PE    *
     *  contains the update and the forwarding of *
     *  weight, output mark and feature           *
     *                                            *
     *  Remark:                                   *
     *      not purely combinational logical      *
     *      it implied state transition inside    *
     *      since the transformation of the       *
     *      state dependence on conditions but    *
     *      not cycle, the run method can be      *
     *      called several times in one cycle and *
     *      won't bring wrong result              *
     **********************************************/
private:
    enum class IS:uint8_t{
        /// implicitly state
        /// do not offer extra information
        /// only to make coding more easily
        X__USED_OW__USED,
        X__USED_OW_READY,
        X__USED_OW_FOWED,
        X_FOWED_OW__USED,
        X_FOWED_OW_READY,
        X_FOWED_OW_FOWED,
        X_READY_OW__USED,
        X_READY_OW_READY,
        X_READY_OW_FOWED
    };

    enum IS is;
    /// the implicitly state of PE after the clock edge come

    enum IS nextIS;
    /// the implicitly state which PE going to change

    enum class ValueIS:uint8_t{
        /// do not offer extra information
        /// only to make coding more easily
         USED,
        READY,
        FOWED
    };
    enum ValueIS xIS;
    /// the implicitly state of x after the clock edge come

    enum ValueIS nextXIS;
    /// the implicitly state which x going to change

    enum ValueIS owIS;
    /// the implicitly state of ow after the clock edge come

    enum ValueIS nextOWIS;
    /// the implicitly state which ow going to change

    enum class PE_STATE:uint8_t{
        BUSY,
        STALL
    }state,nextState;
    enum class VALUE_STATE:uint8_t{
        UNUSED,
        USED
    }xState,nextXState,owState,nextOWState;

    PortDataType
        fUpClr,     /** 1-bit out   reg UpFull **/
        fDownSet,   /** 1-bit out   output-port FdownSet **/
        owOutEn,    /** 1-bit out   output-port OWoutEn  **/
        fLeftClr,   /** 1-bit out   reg LeftFull **/
        fRightSet,  /** 1-bit out   output-port FrightOut **/
        xOutEn,     /** 1-bit out   output-port XoutEn    **/
        countEn,    /** 1-bit out   counter **/
        countRst,   /** 1-bit out   counter **/
        owEn,       /** 1-bit out   reg  output-buffer **/
        xEn;        /** 1-bit out   reg  output-buffer **/

    inline bool  XISChanged(){
        return this-> xIS !=  this->nextXIS;
    }
    inline bool OWISChanged(){
        return this->owIS != this->nextOWIS;
    }
    inline void NextIS(enum IS is){
        this->nextIS = is;
        return;
    }
    inline void NextXIS(enum ValueIS valueIS){
        this->nextXIS = valueIS;
        return;
    }
    inline void NextOWIS(enum ValueIS valueIS){
        this->nextOWIS = valueIS;
        return;
    }
    inline void SetXStateNext(){
        if (!this->XISChanged())
            return;
        switch (this->nextXIS){
        case ValueIS:: USED:
            assert( this->ports[PORT_INDEX(FupIn)].Data()==1
                 && this->fUpClr==1);
            this->xState = VALUE_STATE::  USED;
            return;
        case ValueIS::FOWED:
            /// the state has not changed
            assert(this->ports[PORT_INDEX(FupIn)].Data()==1
                && this->fUpClr == 0
                && this->xState == VALUE_STATE::USED);
            this->xState = VALUE_STATE::UNUSED;
            return;
        default:
            assert(false);
        }
        assert(false);
        return;
    }
    inline void SetOWStateNext(){
        if (!this->OWISChanged())
            return;
        switch (this->nextOWIS){
        case ValueIS:: USED:
            assert( this->ports[PORT_INDEX(FleftIn)].Data()==1
                 && this->fLeftClr==1);
            this->owState = VALUE_STATE::USED;
            return;
        case ValueIS::FOWED:
            /// the state has not changed
            assert(this->ports[PORT_INDEX(FleftIn)].Data()==1
                && this->fLeftClr == 0
                && this->owState == VALUE_STATE::USED);
            this->owState = VALUE_STATE::UNUSED;
            return;
        default:
            assert(false);
        }
        assert(false);
        return;
    }
    inline void HoldXIS(){
        this->nextXIS = this->xIS;
        return;
    }
    inline void HoldOWIS(){
        this->nextOWIS = this->owIS;
        return;
    }
    inline void HoldCurState(){
        this->  nextIS = this->  is;
        this-> nextXIS = this-> xIS;
        this->nextOWIS = this->owIS;
        return;
    }
    inline void    ForwardX(){
        this->fRightSet = 1;  /** 1-bit out   output-port FrightOut **/
        this->xOutEn    = 1;  /** 1-bit out   output-port XoutEn    **/
        return;
    }
    inline void NotForwardX(){
        this->fRightSet = 0;  /** 1-bit out   output-port FrightOut **/
        this->xOutEn    = 0;  /** 1-bit out   output-port XoutEn    **/
        return;
    }
    inline void    ForwardOW(){
        this->fDownSet = 1;  /** 1-bit out   output-port FdownSet **/
        this->owOutEn  = 1;  /** 1-bit out   output-port OWoutEn  **/
        return;
    }
    inline void NotForwardOW(){
        this->fDownSet = 0;  /** 1-bit out   output-port FdownSet **/
        this->owOutEn  = 0;  /** 1-bit out   output-port OWoutEn  **/
        return;
    }
    inline void    Compute(){
        this->fUpClr   = 1;  /** 1-bit out   reg UpFull **/
        this->fLeftClr = 1;  /** 1-bit out   reg LeftFull **/
        this->countEn  = 1;  /** 1-bit out   counter **/
        this->owEn     = 1;  /** 1-bit out   reg  output-buffer **/
        this->xEn      = 1;  /** 1-bit out   reg  output-buffer **/
        this->state    = PE_STATE::BUSY;
        return;
    }
    inline void NotCompute(){
        this->fUpClr   = 0;  /** 1-bit out   reg UpFull **/
        this->fLeftClr = 0;  /** 1-bit out   reg LeftFull **/
        this->countEn  = 0;  /** 1-bit out   counter **/
        this->owEn     = 0;  /** 1-bit out   reg  output-buffer **/
        this->xEn      = 0;  /** 1-bit out   reg  output-buffer **/
        return;
    }

    inline enum ValueIS GetXImplicState() const{
        assert(this->ports[PORT_INDEX(FleftIn )].Data()==0
            || this->ports[PORT_INDEX(FleftSet)].Data()==0);

        if ((this->ports[PORT_INDEX(FleftIn )].Data()==1
          || this->ports[PORT_INDEX(FleftSet)].Data()==1)
          && this->fLeftClr == 0){
            if (this->xState == VALUE_STATE::USED)
                return ValueIS::READY;
            else
                return ValueIS::FOWED;
        }
        else{
            if (this->xState == VALUE_STATE::USED)
                return ValueIS::USED;
        }
        assert(false);
        return ValueIS::USED;
    };
    inline enum ValueIS GetOWImplicState() const{
        assert(this->ports[PORT_INDEX(FupIn )].Data()==0
            || this->ports[PORT_INDEX(FupSet)].Data()==0);

        if ((this->ports[PORT_INDEX(FupIn )].Data()==1
          || this->ports[PORT_INDEX(FupSet)].Data()==1)
          && this->fUpClr == 0){
            if (this->owState == VALUE_STATE::USED)
                return ValueIS::READY;
            else
                return ValueIS::FOWED;
        }
        else{
            if (this->owState == VALUE_STATE::USED)
                return ValueIS::USED;
        }
        assert(false);
        return ValueIS::USED;
    }
    enum IS GetImplicState()const{
        switch (this->GetXImplicState()){
        case ValueIS:: USED:
            switch (this->GetOWImplicState()){
            case ValueIS:: USED: return IS::X__USED_OW__USED;
            case ValueIS::READY: return IS::X__USED_OW_READY;
            case ValueIS::FOWED: return IS::X__USED_OW_FOWED;
            default:
                assert(false);
            }
            break;
        case ValueIS::READY:
            switch (this->GetOWImplicState()){
            case ValueIS:: USED: return IS::X_READY_OW__USED;
            case ValueIS::READY: return IS::X_READY_OW_READY;
            case ValueIS::FOWED: return IS::X_READY_OW_FOWED;
            default:
                assert(false);
            }
            break;
        case ValueIS::FOWED:
            switch (this->GetOWImplicState()){
            case ValueIS:: USED: return IS::X_FOWED_OW__USED;
            case ValueIS::READY: return IS::X_FOWED_OW_READY;
            case ValueIS::FOWED: return IS::X_FOWED_OW_FOWED;
            default:
                assert(false);
            }
            break;
        default:
            assert(false);
        }
        assert(false);
        return IS::X__USED_OW__USED;
    }
    inline bool  XCanForward()const{
        return this->ports[PORT_INDEX(FrightIn)].Data()==0;
    }
    inline bool OWCanForward()const{
        return this->ports[PORT_INDEX( FdownIn)].Data()==0;
    }
    inline bool ComputeBusy()const {
        std::cout<<this->ports[PORT_INDEX(CalcDone)].Data()<<std::endl;
        return (this->state==PE_STATE::BUSY && this->ports[PORT_INDEX(CalcDone)].Data()!=1);
    }

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::Number());
        /** the relevant port of weight and output-mark **/
        ///------------- upper side -------------///
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FupSet  1-bit in     input-port FupSet **/
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FupIn   1-bit in    reg UpFull **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** FupClr  1-bit out   reg UpFull **/
        ///------------- under side -------------///
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FdownIn  1-bit in    input-port FdownIn        **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** FdownSet 1-bit out  output-port FdownSet       **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** OWoutEn  1-bit out  output-port WoutEn, OoutEn **/
        /** the relevant port of feature **/
        ///------------- left  side -------------///
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FleftSet 1-bit in    input-port FleftSet **/
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FleftIn  1-bit in   reg LeftFull **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** FleftClr 1-bit out  reg LeftFull **/
        ///------------- right side -------------///
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FrightIn  1-bit  input-port FrightIn  **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** FrightSet 1-bit output-port FrightSet **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** XoutEn    1-bit output-port XoutEn    **/
        /** the ports connected to CL1 **/
        this->ports.push_back(Port( 1,PortDirection::IN )); /** CalcDone  1-bit in  CL1 **/
        /** the ports connected to counter **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** CountEn   1-bit out counter **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** CountRst  1-bit out counter **/
        /** the ports connected to MAC buffer **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** OWEn 1-bit out   weight-buffer **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /**  XEn 1-bit out  feature-buffer **/
        assert(this->ports.size()==PortName::Number());
        return;
    }
    inline void OutputConnect(){
        this->ports[PORT_INDEX(FupClr   )].Clone(this->fUpClr   );
        this->ports[PORT_INDEX(FdownSet )].Clone(this->fDownSet );
        this->ports[PORT_INDEX(OWoutEn  )].Clone(this->owOutEn  );
        this->ports[PORT_INDEX(FleftClr )].Clone(this->fLeftClr );
        this->ports[PORT_INDEX(FrightSet)].Clone(this->fRightSet);
        this->ports[PORT_INDEX(XoutEn   )].Clone(this->xOutEn   );
        this->ports[PORT_INDEX(CountEn  )].Clone(this->countEn  );
        this->ports[PORT_INDEX(CountRst )].Clone(this->countRst );
        this->ports[PORT_INDEX(OWEn     )].Clone(this->owEn     );
        this->ports[PORT_INDEX(XEn      )].Clone(this->xEn      );
        assert(this->OutputConnected());
        return;
    }
public:
    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
        /** the relevant port of weight and output-mark **/
            ///------------- upper side -------------///
            FupSet    =  0, /** 1-bit in     input-port FupSet   **/
            FupIn     =  1, /** 1-bit in    reg UpFull **/
            FupClr    =  2, /** 1-bit out   reg UpFull **/
            ///------------- under side -------------///
            FdownIn   =  3, /** 1-bit in     input-port FdownIn  **/
            FdownSet  =  4, /** 1-bit out   output-port FdownSet **/
            OWoutEn   =  5, /** 1-bit out   output-port OWoutEn  **/
        /** the relevant port of feature **/
            ///------------- left  side -------------///
            FleftSet  =  6, /** 1-bit in     input-port FleftSet **/
            FleftIn   =  7, /** 1-bit in    reg LeftFull **/
            FleftClr  =  8, /** 1-bit out   reg LeftFull **/
            ///------------- right side -------------///
            FrightIn  =  9, /** 1-bit in     input-port FrightIn  **/
            FrightSet = 10, /** 1-bit out   output-port FrightSet **/
            XoutEn    = 11, /** 1-bit out   output-port XoutEn    **/
        /** the ports connected to CL1 **/
            CalcDone  = 12, /** 1-bit in    combinational logical CL1 **/
        /** the ports connected to counter **/
            CountEn   = 13, /** 1-bit out   counter **/
            CountRst  = 14, /** 1-bit out   counter **/
        /** the ports connected to MAC buffer **/
            OWEn      = 15, /** 1-bit out   reg   weight-buffer **/
            XEn       = 16, /** 1-bit out   reg  feature-buffer **/
          /*************/
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

    PE_CL0(
        PortDataType fUpClr    =0, /** 1-bit out   reg UpFull **/
        PortDataType fDownSet  =0, /** 1-bit out   output-port FdownSet **/
        PortDataType owOutEn   =0, /** 1-bit out   output-port OWoutEn  **/
        PortDataType fLeftClr  =0, /** 1-bit out   reg LeftFull **/
        PortDataType fRightSet =0, /** 1-bit out   output-port FrightOut **/
        PortDataType xOutEn    =0, /** 1-bit out   output-port XoutEn    **/
        PortDataType countEn   =0, /** 1-bit out   counter **/
        PortDataType owEn      =0, /** 1-bit out   reg  output-buffer **/
        PortDataType xEn       =0, /** 1-bit out   reg  output-buffer **/
        enum VALUE_STATE      xState = VALUE_STATE::USED,
        enum VALUE_STATE  nextXState = VALUE_STATE::USED,
        enum VALUE_STATE     owState = VALUE_STATE::USED,
        enum VALUE_STATE nextOWState = VALUE_STATE::USED,
        enum    PE_STATE       state = PE_STATE::STALL,
        enum    PE_STATE   nextState = PE_STATE::STALL
    ){
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->fUpClr    = fUpClr;
        this->fDownSet  = fDownSet;
        this->owOutEn   = owOutEn;
        this->fLeftClr  = fLeftClr;
        this->fRightSet = fRightSet;
        this->xOutEn    = xOutEn;
        this->countEn   = countEn;
        this->owEn      = owEn;
        this->xEn       = xEn;
        this->     xState = xState;
        this-> nextXState = nextXState;
        this->    owState = owState;
        this->nextOWState = nextOWState;
        this->      state = state;
        this->  nextState = nextState;
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

    PE_CL0(const PE_CL0& cl0){
        assert(cl0.AllInputNotConnected());
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->fUpClr      = cl0.fUpClr;
        this->fDownSet    = cl0.fDownSet;
        this->owOutEn     = cl0.owOutEn;
        this->fLeftClr    = cl0.fLeftClr;
        this->fRightSet   = cl0.fRightSet;
        this->xOutEn      = cl0.xOutEn;
        this->countEn     = cl0.countEn;
        this->owEn        = cl0.owEn;
        this->xEn         = cl0.xEn;
        this->     xState = cl0.xState;
        this-> nextXState = cl0.nextXState;
        this->    owState = cl0.owState;
        this->nextOWState = cl0.nextOWState;
        this->      state = cl0.state;
        this->  nextState = cl0.nextState;
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
        return  this->ports[PORT_INDEX(FupClr   )].SameTo(this->fUpClr   )
             && this->ports[PORT_INDEX(FdownSet )].SameTo(this->fDownSet )
             && this->ports[PORT_INDEX(OWoutEn  )].SameTo(this->owOutEn  )
             && this->ports[PORT_INDEX(FleftClr )].SameTo(this->fLeftClr )
             && this->ports[PORT_INDEX(FrightSet)].SameTo(this->fRightSet)
             && this->ports[PORT_INDEX(XoutEn   )].SameTo(this->xOutEn   )
             && this->ports[PORT_INDEX(CountEn  )].SameTo(this->countEn  )
             && this->ports[PORT_INDEX(CountRst )].SameTo(this->countRst )
             && this->ports[PORT_INDEX(OWEn     )].SameTo(this->owEn     )
             && this->ports[PORT_INDEX(XEn      )].SameTo(this->xEn      );
    }
    bool atPosEdge() override{
        this->  is = this->  GetImplicState();
        this-> xIS = this-> GetXImplicState();
        this->owIS = this->GetOWImplicState();

        switch(this->is){
        case IS::X__USED_OW__USED:
        case IS::X__USED_OW_FOWED:
        case IS::X_FOWED_OW__USED:
            this->HoldCurState();
            break;
        case IS::X__USED_OW_READY:
            if (this->OWCanForward()){
                ///this->SetImplicState(X__USED_OW_FORWED);
                this-> HoldXIS();
                this->NextOWIS(ValueIS::FOWED);
                this->NextIS(IS::X__USED_OW_FOWED);
            }
            else
                this->HoldCurState();
            break;
        case IS::X_READY_OW__USED:
            if (this->XCanForward()){
                ///this->SetImplicState(X_FOWED_OW__USED);
                this-> NextXIS(ValueIS::FOWED);
                this->HoldOWIS();
                this->NextIS(IS::X_FOWED_OW__USED);
            }
            else
                this->HoldCurState();
            break;
        case IS::X_FOWED_OW_READY:
            if (this->OWCanForward()){
                if (this->ComputeBusy()){
                    ///this->SetImplicState(X_FOWED_OW_FOWED);
                    this-> HoldXIS();
                    this->NextOWIS(ValueIS::FOWED);
                    this->NextIS(IS::X_FOWED_OW_FOWED);
                }
                else{
                    ///this->SetImplicState(X__USED_OW__USED);
                    this-> NextXIS(ValueIS::USED);
                    this->NextOWIS(ValueIS::USED);
                    this->NextIS(IS::X__USED_OW__USED);
                }
            }
            else
                this->HoldCurState();
            break;
        case IS::X_READY_OW_FOWED:
            if (this->XCanForward()){
                if (this->ComputeBusy()){
                    ///this->SetImplicState(X_FOWED_OW_FOWED);
                    this-> NextXIS(ValueIS::FOWED);
                    this->HoldOWIS();
                    this->NextIS(IS::X_FOWED_OW_FOWED);
                }
                else{
                    ///this->SetImplicState(X__USED_OW__USED);
                    this-> NextXIS(ValueIS::USED);
                    this->NextOWIS(ValueIS::USED);
                    this->NextIS(IS::X__USED_OW__USED);
                }
            }
            else
                this->HoldCurState();
            break;
        case IS::X_FOWED_OW_FOWED:
            if (this->ComputeBusy())
                this->HoldCurState();
            else{
                ///this->SetImplicState(X__USED_OW__USED);
                this-> NextXIS(ValueIS::USED);
                this->NextOWIS(ValueIS::USED);
                this->NextIS(IS::X__USED_OW__USED);
            }
            break;
        case IS::X_READY_OW_READY:
            if ( this->XCanForward()&& this->OWCanForward()){
                if (this->ComputeBusy()){
                    ///this->SetImplicState(X_FOWED_OW_FOWED);
                    this-> NextXIS(ValueIS::FOWED);
                    this->NextOWIS(ValueIS::FOWED);
                    this->NextIS(IS::X_FOWED_OW_FOWED);
                }
                else{
                    ///this->SetImplicState(X__USED_OW__USED);
                    this-> NextXIS(ValueIS::USED);
                    this->NextOWIS(ValueIS::USED);
                    this->NextIS(IS::X__USED_OW__USED);
                }
            }
            else if ( this->XCanForward()&&!this->OWCanForward()){
                 ///this->SetImplicState(X_FOWED_OW_READY);
                 this->  NextXIS(ValueIS::FOWED);
                 this->HoldOWIS();
                 this->NextIS(IS::X_FOWED_OW_READY);
            }
            else if (!this->XCanForward()&& this->OWCanForward()){
                 ///this->SetImplicState(X_READY_OW_FOWED);
                 this->HoldXIS();
                 this->NextOWIS(ValueIS::FOWED);
                 this->NextIS(IS::X_READY_OW_FOWED);
            }
            else if (!this->XCanForward()&&!this->OWCanForward()){
                 this->HoldCurState();
            }
            break;
        default: assert(false);
        }
        return true;
    }
    inline bool atNegEdge() override{
        assert(false);
        return false;
    }
    bool Run() override{
        if (this->ports[PORT_INDEX(CalcDone)].Data()==1)
            this->state = PE_STATE::STALL;
        this->countRst = this->ports[PORT_INDEX(CalcDone)].Data();
        if (!this->XISChanged() && !this->OWISChanged()){
            /** do nothing **/
            this->NotForwardX();
            this->NotForwardOW();
            this->NotCompute();
            return false;
        }


        switch(this->is){
        case IS::X_READY_OW_FOWED:
            switch(this->nextIS){
            case IS::X_FOWED_OW_FOWED:
                this->   ForwardX ();
                this->NotForwardOW();
                this->NotCompute  ();
                break;
            case IS::X__USED_OW__USED:
                this->   ForwardX ();
                this->NotForwardOW();
                this->   Compute  ();
                break;
            default:
                assert(false);
            }
            break;
        case IS::X_FOWED_OW_READY:
            switch(this->nextIS){
            case IS::X_FOWED_OW_FOWED:
                this->NotForwardX ();
                this->   ForwardOW();
                this->NotCompute  ();
                break;
            case IS::X__USED_OW__USED:
                this->NotForwardX ();
                this->   ForwardOW();
                this->   Compute  ();
                break;
            default:
                assert(false);
            }
            break;
        case IS::X_FOWED_OW_FOWED:
            switch(this->nextIS){
            case IS::X__USED_OW__USED:
                this->NotForwardX ();
                this->NotForwardOW();
                this->   Compute  ();
                break;
            default:
                assert(false);
            }
            break;
        case IS::X__USED_OW_READY:
            switch(this->nextIS){
            case IS::X__USED_OW_FOWED:
                this->NotForwardX ();
                this->   ForwardOW();
                this->NotCompute  ();
                break;
            default:
                assert(false);
            }
            break;
        case IS::X_READY_OW__USED:
            switch(this->nextIS){
            case IS::X_FOWED_OW__USED:
                this->   ForwardX ();
                this->NotForwardOW();
                this->NotCompute  ();
                break;
            default:
                assert(false);
            }
            break;
        case IS::X_READY_OW_READY:
            switch(this->nextIS){
            case IS::X_FOWED_OW__USED:
                this->   ForwardX ();
                this->NotForwardOW();
                this->NotCompute  ();
                break;
            case IS::X__USED_OW_FOWED:
                this->NotForwardX ();
                this->   ForwardOW();
                this->NotCompute  ();
                break;
            case IS::X_FOWED_OW_FOWED:
                this->   ForwardX ();
                this->   ForwardOW();
                this->NotCompute  ();
                break;
            case IS::X__USED_OW__USED:
                this->   ForwardX ();
                this->   ForwardOW();
                this->   Compute  ();
                break;
            default:
                assert(false);
            }
            break;
        default:
            assert(false);
        }
        this-> SetXStateNext();
        this->SetOWStateNext();
        return true;
    }
    inline bool IsStall(){
        return (this->state==PE_STATE::STALL);
    }
};

class PE_CL1: public ClockedObject{
    /*********************************************
     *  a part of combinational logical for PE   *
     *  contains the computation and forwarding  *
     *  partial sum                              *
     *********************************************/
private:
    PortDataType
        calcDone,   /**  1-bit out   combinational logical CL0 **/
        fUpUpClr,   /**  1-bit out   two-port-reg UpUpFull **/
        fDDownSet,  /**  1-bit out   output-port FddownSet **/
        innerSEn,   /**  1-bit out   reg ParSum  **/
        innerSOut,  /**  1-bit out   reg ParSum  **/
        sOut,       /** 24-bit out   reg    Sum  **/
        sEn,        /**  1-bit out   reg    Sum  **/
        lastCountDone;///temporary solution

    enum class TransType:uint8_t{
        UNRECEIVED,
          RECEIVED,
         FORWARDED,
            TOSENT
    }transState,nextState;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::Number());
        /** the relevant port of feature, weight, and output-mark **/
            ///--------------- buffer ---------------///
        this->ports.push_back(Port( 1,PortDirection::IN )); /** Obuf  1-bit in  reg  output-buffer **/
        this->ports.push_back(Port( 8,PortDirection::IN )); /** Wbuf  8-bit in  reg  weight-buffer **/
        this->ports.push_back(Port( 8,PortDirection::IN )); /** Xbuf  8-bit in  reg feature-buffer **/
        /** the ports connected to CL0 **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** CalcDone  1-bit out combinational logical CL0 **/
        /** the part connect to counter **/
        this->ports.push_back(Port( 1,PortDirection::IN )); /** CountDone 1-bit in  counter **/
        /** the relevant port of partial sum **/
            ///------------   up of up   ------------///
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FupupSet  1-bit in  input-port   FupupSet **/
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FupupIn   1-bit in  two-port-reg FupupIn  **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** FupupClr  1-bit out two-port-reg FupupClr **/
        this->ports.push_back(Port(24,PortDirection::IN )); /** Sin      24-bit in  input-port Sin **/
            ///------------ down of down ------------///
        this->ports.push_back(Port( 1,PortDirection::IN )); /** FddownIn   1-bit in  reg  input-port FddownIn  **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** FddownSet  1-bit out reg output-port FddownSet **/
            ///------------ partial  reg ------------///
        this->ports.push_back(Port(24,PortDirection::IN )); /** InnerSin  24-bit in  reg inner-partial-sum **/
        this->ports.push_back(Port(24,PortDirection::OUT)); /** InnerSout 24-bit out reg inner-partial-sum **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** InnerSen   1-bit out reg inner-partial-sum **/
            ///------------ summation forward ------------///
        this->ports.push_back(Port(24,PortDirection::OUT)); /** Sout 24-bit out reg ParSum **/
        this->ports.push_back(Port( 1,PortDirection::OUT)); /** Sen   1-bit out reg ParSum **/
        assert(this->ports.size()==PortName::Number());
        return;
    }

    inline void OutputConnect() override{
        this->ports[PORT_INDEX(CalcDone )].Clone(this->calcDone );
        this->ports[PORT_INDEX(FupupClr )].Clone(this->fUpUpClr );
        this->ports[PORT_INDEX(FddownSet)].Clone(this->fDDownSet);
        this->ports[PORT_INDEX(InnerSout)].Clone(this->innerSOut);
        this->ports[PORT_INDEX(InnerSen )].Clone(this->innerSEn );
        this->ports[PORT_INDEX(Sout     )].Clone(this->sOut     );
        this->ports[PORT_INDEX(Sen      )].Clone(this->sEn      );
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
        /** the relevant port of feature, weight, and output-mark **/
            ///--------------- buffer ---------------///
            Obuf      =  0, /**  1-bit in    reg  output-buffer **/
            Wbuf      =  1, /**  8-bit in    reg  weight-buffer **/
            Xbuf      =  2, /**  8-bit in    reg feature-buffer **/
        /** the ports connected to CL0 **/
            CalcDone  =  3, /**  1-bit out   combinational logical CL0 **/
        /** the part connect to counter **/
            CountDone =  4, /**  1-bit in    Counter **/
        /** the relevant port of partial sum **/
            ///------------   up of up   ------------///
            FupupSet  =  5, /**  1-bit in    input-port FupupSet   **/
            FupupIn   =  6, /**  1-bit in    two-port-reg UpUpFull **/
            FupupClr  =  7, /**  1-bit out   two-port-reg UpUpFull **/
            Sin       =  8, /** 24-bit in    input-port Sin **/
            ///------------ down of down ------------///
            FddownIn  =  9, /**  1-bit in     input-port FddownIn  **/
            FddownSet = 10, /**  1-bit out   output-port FddownSet **/
            ///------------ partial reg ------------///
            InnerSreg = 11, /** 24-bit in    reg ParSum **/
            InnerSout = 12, /** 24-bit out   reg ParSum **/
            InnerSen  = 13, /**  1-bit out   reg ParSum **/
            ///------------ summation forward ------------///
            Sout      = 14, /** 24-bit out   reg    Sum **/
            Sen       = 15, /**  1-bit out   reg    Sum **/
            /*************/
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
    PE_CL1(
        PortDataType calcDone=0,   /**  1-bit out   combinational logical CL0 **/
        PortDataType fUpUpClr=0,   /**  1-bit out   two-port-reg UpUpFull **/
        PortDataType fDDownSet=0,  /**  1-bit out   output-port FddownSet **/
        PortDataType innerSEn=0,   /**  1-bit out   reg ParSum **/
        PortDataType innerSOut=0,  /**  1-bit out   reg ParSum **/
        PortDataType sOut=0,       /** 24-bit out   reg    Sum **/
        PortDataType sEn=0,        /**  1-bit out   reg    Sum **/
        PortDataType lastCountDone=0,///temporary solution
        enum TransType transState = TransType::UNRECEIVED,
        enum TransType  nextState = TransType::UNRECEIVED
    ){
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->calcDone  = calcDone;  /**  1-bit out   combinational logical CL0 **/
        this->fUpUpClr  = fUpUpClr;  /**  1-bit out   two-port-reg UpUpFull **/
        this->fDDownSet = fDDownSet; /**  1-bit out   output-port FddownSet **/
        this->innerSEn  = innerSEn;  /**  1-bit out   reg ParSum **/
        this->innerSOut = innerSOut; /**  1-bit out   reg ParSum **/
        this->sOut      = sOut;      /** 24-bit out   reg    Sum  **/
        this->sEn       = sEn;       /**  1-bit out   reg    Sum  **/
        this->lastCountDone = lastCountDone;/// temporary solution to control partial sum register
        this->transState=transState;
        this-> nextState= nextState;
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
    PE_CL1(const PE_CL1& cl1){
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->calcDone  = cl1.calcDone;  /**  1-bit out   combinational logical CL0 **/
        this->fUpUpClr  = cl1.fUpUpClr;  /**  1-bit out   two-port-reg UpUpFull **/
        this->fDDownSet = cl1.fDDownSet; /**  1-bit out   output-port FddownSet **/
        this->innerSEn  = cl1.innerSEn;  /**  1-bit out   reg ParSum **/
        this->innerSOut = cl1.innerSOut; /**  1-bit out   reg ParSum **/
        this->sOut      = cl1.sOut;      /** 24-bit out   reg    Sum  **/
        this->sEn       = cl1.sEn;       /**  1-bit out   reg    Sum  **/
        this->transState= cl1.transState;
        this-> nextState= cl1.nextState;
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
        return  this->ports[PORT_INDEX(CalcDone )].SameTo(this->calcDone )
             && this->ports[PORT_INDEX(FupupClr )].SameTo(this->fUpUpClr )
             && this->ports[PORT_INDEX(FddownSet)].SameTo(this->fDDownSet)
             && this->ports[PORT_INDEX(InnerSout)].SameTo(this->innerSOut)
             && this->ports[PORT_INDEX(InnerSen )].SameTo(this->innerSEn )
             && this->ports[PORT_INDEX(Sout     )].SameTo(this->sOut     )
             && this->ports[PORT_INDEX(Sen      )].SameTo(this->sEn      );
    }
    bool atPosEdge() override{
        switch(this->transState){
        case TransType::UNRECEIVED:
            if (this->ports[PORT_INDEX(FupupIn)].Data()==1)
                this->nextState = TransType::RECEIVED;
            else
                this->nextState = TransType::UNRECEIVED;
            break;
        case TransType::RECEIVED:
            if (this->ports[PORT_INDEX(FddownIn)].Data()==0){
                if (this->ports[PORT_INDEX(Obuf)].Data()==1
                &&  this->ports[PORT_INDEX(CountDone)].Data()==1)
                    this->nextState = TransType::TOSENT;
                else
                    this->nextState = TransType::FORWARDED;
            }
            else
                this->nextState = TransType::RECEIVED;
            break;
        case TransType::FORWARDED:
            if (this->ports[PORT_INDEX(Obuf)].Data()==1
            &&  this->ports[PORT_INDEX(CountDone)].Data()==1)
                this->nextState = TransType::TOSENT;
            else
                this->nextState = TransType::FORWARDED;
            break;
        case TransType::TOSENT:
            if (this->ports[PORT_INDEX(FddownIn)].Data()==0)
                this->nextState = TransType::TOSENT;
            else
                this->nextState = TransType::UNRECEIVED;
            break;
        default:
            assert(false);
        }
        return true;
    }
    inline bool atNegEdge() override{
        assert(false);
        return false;
    }
    inline bool Run() override{
        bool innerCalcDone
         =  this->ports[PORT_INDEX(   Obuf  )].Data()==0
         && this->ports[PORT_INDEX(CountDone)].Data()==1;

        this->sOut =
            this->ports[PORT_INDEX(Wbuf)].Data()
          * this->ports[PORT_INDEX(Xbuf)].Data()
          + this->ports[PORT_INDEX(InnerSreg)].Data();

        if (this->transState == this->nextState){
            /** do nothing **/
            this->fUpUpClr  = 0;
            this->fDDownSet = 0;
            this->innerSEn  = lastCountDone;
            this->innerSOut = this->sOut;
            this->sEn       = 0;
            this->calcDone  = innerCalcDone;
            this->lastCountDone = innerCalcDone;
            return true;
        }
        switch(this->transState){
        case TransType::UNRECEIVED:
            switch(this->nextState){
            case TransType::RECEIVED:
                /** receive summation from up-up cell **/
                this->fUpUpClr  = 1;
                this->fDDownSet = 0;
                this->innerSEn  = lastCountDone;
                this->innerSOut = this->sOut;
                this->sEn       = 1;
                this->calcDone  = innerCalcDone;
                break;
            default:
                assert(false);
            }
            break;
        case TransType::RECEIVED:
            switch(this->nextState){
            case TransType::TOSENT:
                /** send the received summation to ddown cell
                 && write the summation to the summation register
                 && clear the ParSum register
                 **/
                this->fUpUpClr  = 0;
                this->fDDownSet = 1;
                this->innerSEn  = 1;
                this->innerSOut = 0;
                this->sEn       = 1;
                this->calcDone  = 1;
                break;
            case TransType::FORWARDED:
                /** receive summation from up-up cell **/
                this->fUpUpClr  = 0;
                this->fDDownSet = 1;
                this->innerSEn  = lastCountDone;
                this->innerSOut = this->sOut;
                this->sEn       = 0;
                this->calcDone  = innerCalcDone;
                break;
            default:
                assert(false);
            }
            break;
        case TransType::FORWARDED:
            switch(this->nextState){
            case TransType::TOSENT:
                /** write the partial sum to the summation register
                 && clear the ParSum register
                 **/
                this->fUpUpClr  = 0;
                this->fDDownSet = 0;
                this->innerSEn  = 1;
                this->innerSOut = 0;
                this->sEn       = 1;
                this->calcDone  = 1;
                break;
            default:
                assert(false);
            }
            break;
        case TransType::TOSENT:
            switch(this->nextState){
            case TransType::UNRECEIVED:
                /** send the summation to the down of down register
                 **/
                this->fUpUpClr  = 0;
                this->fDDownSet = 1;
                this->innerSEn  = lastCountDone;
                this->innerSOut = this->sOut;
                this->sEn       = 0;
                this->calcDone  = innerCalcDone;
                break;
            default:
                assert(false);
            }
            break;
        default:
            assert(false);
        }
        this->transState = this->nextState;
        this->lastCountDone = innerCalcDone;
        return true;
    }
};

class PE: public ClockedObject{
    /********************************************
     *  the basic version of PE                 *
     * # does not contain extra pipeline stage  *
     *   between adder and multer               *
     * # would not forward before calculate     *
     ********************************************/
private:
    vector<Reg> regs;
    vector<MarkReg> markRegs;
    PE_CL0 cl0;
    PE_CL1 cl1;
    Counter counter;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::Number());
        /** the relevant port of weight and output-mark **/
            ///------------- upper side -------------///
        this->ports.push_back(Port( 1,PortDirection::IN ));/** FupSet     1-bit in     up-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** FupOut     1-bit out    up-cell **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** Oin        1-bit in     up-cell **/
        this->ports.push_back(Port( 8,PortDirection::IN ));/** Win        8-bit in     up-cell **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** OWinEn     1-bit in     up-cell **/
            ///------------- under side -------------///
        this->ports.push_back(Port( 1,PortDirection::IN ));/** FdownIn    1-bit in   down-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** FdownSet   1-bit out  down-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** Oout       1-bit out  down-cell **/
        this->ports.push_back(Port( 8,PortDirection::OUT));/** Wout       8-bit out  down-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** OWoutEn    1-bit out  down-cell **/
        /** the relevant port of feature **/
            ///------------- left  side -------------///
        this->ports.push_back(Port( 1,PortDirection::IN ));/** FleftSet   1-bit in   left-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** FleftOut   1-bit out  left-cell **/
        this->ports.push_back(Port( 8,PortDirection::IN ));/** Xin        8-bit in   left-cell **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** XinEn      8-bit in   left-cell **/
            ///------------- right side -------------///
        this->ports.push_back(Port( 1,PortDirection::IN ));/** FrightIn   1-bit in  right-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** FrightSet  1-bit out right-cell **/
        this->ports.push_back(Port( 8,PortDirection::OUT));/** Xout       8-bit out right-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** XoutEn     1-bit out right-cell **/
        /** the relevant port of partial sum **/
            ///------------   up of up   ------------///
        this->ports.push_back(Port( 1,PortDirection::IN ));/** FupupSet   1-bit in   upup-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** FupupOut   1-bit out  upup-cell **/
        this->ports.push_back(Port(24,PortDirection::IN ));/** Sin       24-bit in   upup-cell **/
            ///------------ down of down ------------///
        this->ports.push_back(Port( 1,PortDirection::IN ));/** FddownIn   1-bit in  ddown-cell **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** FddownSet  1-bit out ddown-cell **/
        this->ports.push_back(Port(24,PortDirection::OUT));/** Sout      24-bit out ddown-cell **/
        assert(this->ports.size()==PortName::Number());
        return;
    }
    inline void OutputConnect() override{
            Reg::PortName regOutPort        (Reg::PortName::_PortName::out);
        MarkReg::PortName markRegOutPort(MarkReg::PortName::_PortName::out);
         PE_CL0::PortName cl0OWoutEn     (PE_CL0::PortName::_PortName::OWoutEn);
         PE_CL0::PortName cl0FdownSet    (PE_CL0::PortName::_PortName::FdownSet);
         PE_CL0::PortName cl0XoutEn      (PE_CL0::PortName::_PortName::XoutEn);
         PE_CL0::PortName cl0FrightSet   (PE_CL0::PortName::_PortName::FrightSet);
         PE_CL1::PortName cl1FddownSet   (PE_CL1::PortName::_PortName::FddownSet);
        /** registers **/
        this->regs[REG_INDEX(OutMark)].Clone(
            this->ports[PORT_INDEX(Oout)],
            regOutPort
        );
        this->regs[REG_INDEX(Weight)].Clone(
            this->ports[PORT_INDEX(Wout)],
            regOutPort
        );
        this->regs[REG_INDEX(Feature)].Clone(
            this->ports[PORT_INDEX(Xout)],
            regOutPort
        );
        this->regs[REG_INDEX(Sum)].Clone(
            this->ports[PORT_INDEX(Sout)],
            regOutPort
        );
        /** mark registers **/
        this->markRegs[MARK_REG_INDEX(  UpFull)].Clone(
            this->ports[PORT_INDEX(FupOut)],
            markRegOutPort
        );
        this->markRegs[MARK_REG_INDEX(UpUpFull)].Clone(
            this->ports[PORT_INDEX(FupupOut)],
            markRegOutPort
        );
        this->markRegs[MARK_REG_INDEX(LeftFull)].Clone(
            this->ports[PORT_INDEX(FleftOut)],
            markRegOutPort
        );
        /** combinatorial logic 0 **/
        this->cl0.Clone(
            this->ports[PORT_INDEX(OWoutEn)],
            cl0OWoutEn
        );
        this->cl0.Clone(
            this->ports[PORT_INDEX(FdownSet)],
            cl0FdownSet
        );
        this->cl0.Clone(
            this->ports[PORT_INDEX(XoutEn)],
            cl0XoutEn
        );
        this->cl0.Clone(
            this->ports[PORT_INDEX(FrightSet)],
            cl0FrightSet
        );
        /** combinatorial logic 1 **/
        this->cl1.Clone(
            this->ports[PORT_INDEX(FddownSet)],
            cl1FddownSet
        );
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
        /** the relevant port of weight and output-mark **/
            ///------------- upper side -------------///
            FupSet    =  0, /** 1-bit in     up-cell **/
            FupOut    =  1, /** 1-bit out    up-cell **/
            Oin       =  2, /** 1-bit in     up-cell **/
            Win       =  3, /** 8-bit in     up-cell **/
            OWinEn    =  4, /** 1-bit in     up-cell **/
            ///------------- under side -------------///
            FdownIn   =  5, /** 1-bit in   down-cell **/
            FdownSet  =  6, /** 1-bit out  down-cell **/
            Oout      =  7, /** 1-bit out  down-cell **/
            Wout      =  8, /** 8-bit out  down-cell **/
            OWoutEn   =  9, /** 1-bit out  down-cell **/
        /** the relevant port of feature **/
            ///------------- left  side -------------///
            FleftSet  = 10, /** 1-bit in   left-cell **/
            FleftOut  = 11, /** 1-bit out  left-cell **/
            Xin       = 12, /** 8-bit in   left-cell **/
            XinEn     = 13, /** 8-bit in   left-cell **/
            ///------------- right side -------------///
            FrightIn  = 14, /** 1-bit in  right-cell **/
            FrightSet = 15, /** 1-bit out right-cell **/
            Xout      = 16, /** 8-bit out right-cell **/
            XoutEn    = 17, /** 1-bit out right-cell **/
        /** the relevant port of partial sum **/
            ///------------   up of up   ------------///
            FupupSet  = 18, /** 1-bit in   upup-cell **/
            FupupOut  = 19, /** 1-bit out  upup-cell **/
            Sin       = 20, /**24-bit in   upup-cell **/
            ///------------ down of down ------------///
            FddownIn  = 21, /** 1-bit in  ddown-cell **/
            FddownSet = 22, /** 1-bit out ddown-cell **/
            Sout      = 23, /**24-bit out ddown-cell **/
        /***** REMARK: *****************************************
         *  upup-cell stand for the up cell of the up cell     *
         * ddown-cell stand for the down cell of the down cell *
         *******************************************************/
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
    class RegName{
    public:
        typedef uint8_t RegNameType;

        enum class _RegName:uint8_t{
        /** the relevant register of weight and output-mark **/
            Weight  = 0, /** 8-bit **/
            OutMark = 1, /** 1-bit **/
        /** the relevant register of feature **/
            Feature = 2, /** 8-bit **/
        /** the relevant register of computation component **/
            WBuffer = 3, /** 8-bit **/
            OBuffer = 4, /** 1-bit **/
            XBuffer = 5, /** 8-bit **/
        /** the relevant register of partial sum **/
            ParSum  = 6, /**24-bit **/
        /** the register for forwarding summation **/
               Sum  = 7, /**24-bit **/
          /************/
            RegNumber
        };
        static inline RegNameType ToIndex(enum _RegName regName){
            assert(regName != _RegName::RegNumber);
            return static_cast<RegNameType>(regName);
        }
        static inline RegNameType Number(){
            return static_cast<RegNameType>(_RegName::RegNumber);
        }
    };
    class MarkRegName{
    public:
        typedef uint8_t MarkRegNameType;
        enum class _MarkRegName:uint8_t{
            UpFull  = 0, /** 1-bit **/
            LeftFull= 1, /** 1-bit **/
            UpUpFull= 2, /** 1-bit **/
          /************/
            MarkRegNumber
        };
        static inline MarkRegNameType ToIndex(enum _MarkRegName regName){
            assert(regName != _MarkRegName::MarkRegNumber);
            return static_cast<MarkRegNameType>(regName);
        }
        static inline MarkRegNameType Number(){
            return static_cast<MarkRegNameType>(_MarkRegName::MarkRegNumber);
        }
    };

    PE():counter(2,ClockEdge::POS_EDGE){
        /**************************
         ** initialize the ports  *
         **************************/
        this->InitializePorts();
        /******************************
         ** initialize the registers **
         ******************************/
        this->regs.reserve(PE::RegName::Number());
        /** the relevant register of weight and output-mark **/
        this->regs.push_back(Reg( 8,ClockedObject::ClockEdge::POS_EDGE));/** Weight   8-bit **/
        this->regs.push_back(Reg( 1,ClockedObject::ClockEdge::POS_EDGE));/** OutMark  1-bit **/
        /** the relevant register of feature **/
        this->regs.push_back(Reg( 8,ClockedObject::ClockEdge::POS_EDGE));/** Feature  8-bit **/
        /** the relevant register of computation component **/
        this->regs.push_back(Reg( 8,ClockedObject::ClockEdge::POS_EDGE));/** WBuffer  8-bit **/
        this->regs.push_back(Reg( 1,ClockedObject::ClockEdge::POS_EDGE));/** OBuffer  1-bit **/
        this->regs.push_back(Reg( 8,ClockedObject::ClockEdge::POS_EDGE));/** XBuffer  8-bit **/
        /** the relevant register of partial sum **/
        this->regs.push_back(Reg(24,ClockedObject::ClockEdge::POS_EDGE));/** ParSum  24-bit **/
        /** the relevant register of summation **/
        this->regs.push_back(Reg(24,ClockedObject::ClockEdge::POS_EDGE));/**    Sum  24-bit **/
        assert(this->regs.size()==PE::RegName::Number());
        /**********************************
         ** initialize mark registers **
         **********************************/
        this->markRegs.reserve(PE::MarkRegName::Number());
        this->markRegs.push_back(MarkReg(ClockedObject::ClockEdge::POS_EDGE));/** UpFull   1-bit **/
        this->markRegs.push_back(MarkReg(ClockedObject::ClockEdge::POS_EDGE));/** LeftFull 1-bit **/
        this->markRegs.push_back(MarkReg(ClockedObject::ClockEdge::POS_EDGE));/** UpUpFull 1-bit **/
        assert(this->markRegs.size()==PE::MarkRegName::Number());

        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }

    PE(const PE& pe):
           regs(pe.regs),
       markRegs(pe.markRegs),
            cl0(pe.cl0),
            cl1(pe.cl1),
        counter(pe.counter){
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

    void InnerConnect() override{
        assert(this->OuterConnected());
        const     Reg::PortName   regIn         (Reg::PortName::_PortName::in );
        const     Reg::PortName   regEn         (Reg::PortName::_PortName::en );
        const     Reg::PortName   regOut        (Reg::PortName::_PortName::out);
        const MarkReg::PortName markRegSet  (MarkReg::PortName::_PortName::Set);
        const MarkReg::PortName markRegClr  (MarkReg::PortName::_PortName::Clr);
        const MarkReg::PortName markRegOut  (MarkReg::PortName::_PortName::out);
        const  PE_CL0::PortName cl0FupSet    (PE_CL0::PortName::_PortName::FupSet  ); /** 1-bit in     input-port FupSet   **/
        const  PE_CL0::PortName cl0FupIn     (PE_CL0::PortName::_PortName::FupIn   ); /** 1-bit in    reg UpFull **/
        const  PE_CL0::PortName cl0FupClr    (PE_CL0::PortName::_PortName::FupClr  ); /** 1-bit out   reg UpFull **/
        const  PE_CL0::PortName cl0FdownIn   (PE_CL0::PortName::_PortName::FdownIn ); /** 1-bit in     input-port FdownIn  **/
        const  PE_CL0::PortName cl0FleftSet  (PE_CL0::PortName::_PortName::FleftSet); /** 1-bit in     input-port FleftSet **/
        const  PE_CL0::PortName cl0FleftIn   (PE_CL0::PortName::_PortName::FleftIn ); /** 1-bit in    reg LeftFull **/
        const  PE_CL0::PortName cl0FleftClr  (PE_CL0::PortName::_PortName::FleftClr); /** 1-bit out   reg LeftFull **/
        const  PE_CL0::PortName cl0FrightIn  (PE_CL0::PortName::_PortName::FrightIn); /** 1-bit in     input-port FrightIn  **/
        const  PE_CL0::PortName cl0CalcDone  (PE_CL0::PortName::_PortName::CalcDone); /** 1-bit in    combinational logical CL1 **/
        const  PE_CL0::PortName cl0CountEn   (PE_CL0::PortName::_PortName::CountEn ); /** 1-bit out   counter **/
        const  PE_CL0::PortName cl0CountRst  (PE_CL0::PortName::_PortName::CountRst); /** 1-bit out   counter **/
        const  PE_CL0::PortName cl0OWEn      (PE_CL0::PortName::_PortName::OWEn    ); /** 1-bit out   reg   weight-buffer **/
        const  PE_CL0::PortName cl0XEn       (PE_CL0::PortName::_PortName::XEn     ); /** 1-bit out   reg  feature-buffer **/
        const  PE_CL1::PortName cl1Obuf      (PE_CL1::PortName::_PortName::Obuf      ); /**  1-bit in    reg  output-buffer **/
        const  PE_CL1::PortName cl1Wbuf      (PE_CL1::PortName::_PortName::Wbuf      ); /**  8-bit in    reg  weight-buffer **/
        const  PE_CL1::PortName cl1Xbuf      (PE_CL1::PortName::_PortName::Xbuf      ); /**  8-bit in    reg feature-buffer **/
        const  PE_CL1::PortName cl1CalcDone  (PE_CL1::PortName::_PortName::CalcDone  ); /**  1-bit out   combinational logical CL0 **/
        const  PE_CL1::PortName cl1CountDone (PE_CL1::PortName::_PortName::CountDone ); /**  1-bit in    Counter **/
        const  PE_CL1::PortName cl1FupupIn   (PE_CL1::PortName::_PortName::FupupIn   ); /**  1-bit in    two-port-reg UpUpFull **/
        const  PE_CL1::PortName cl1FupupClr  (PE_CL1::PortName::_PortName::FupupClr  ); /**  1-bit out   two-port-reg UpUpFull **/
        const  PE_CL1::PortName cl1Sin       (PE_CL1::PortName::_PortName::Sin       ); /** 24-bit in    input-port Sin **/
        const  PE_CL1::PortName cl1FddownIn  (PE_CL1::PortName::_PortName::FddownIn  ); /**  1-bit in     input-port FddownIn  **/
        const  PE_CL1::PortName cl1InnerSreg (PE_CL1::PortName::_PortName::InnerSreg ); /** 24-bit in    reg ParSum **/
        const  PE_CL1::PortName cl1InnerSout (PE_CL1::PortName::_PortName::InnerSout ); /** 24-bit out   reg ParSum **/
        const  PE_CL1::PortName cl1InnerSen  (PE_CL1::PortName::_PortName::InnerSen  ); /**  1-bit out   reg ParSum **/
        const  PE_CL1::PortName cl1Sout      (PE_CL1::PortName::_PortName::Sout      ); /** 24-bit out   reg    Sum **/
        const  PE_CL1::PortName cl1Sen       (PE_CL1::PortName::_PortName::Sen       ); /**  1-bit out   reg    Sum **/
        const Counter::PortName countDone   (Counter::PortName::_PortName::done      ); /**  1-bit out  **/
        const Counter::PortName countEn     (Counter::PortName::_PortName::en        ); /**  1-bit in   **/
        const Counter::PortName countRst    (Counter::PortName::_PortName::rst       ); /**  1-bit in   **/

        /******************************************
         ** connect input ports with inner logic **
         ******************************************/
        /** registers **/
        this->regs[REG_INDEX(OutMark)].Clone(
            regIn,
            this->ports[PORT_INDEX(Oin)]
        );
        this->regs[REG_INDEX(OutMark)].Clone(
            regEn,
            this->ports[PORT_INDEX(OWinEn)]
        );
        this->regs[REG_INDEX(Weight)].Clone(
            regIn,
            this->ports[PORT_INDEX(Win)]
        );
        this->regs[REG_INDEX(Weight)].Clone(
            regEn,
            this->ports[PORT_INDEX(OWinEn)]
        );
        this->regs[REG_INDEX(Feature)].Clone(
            regIn,
            this->ports[PORT_INDEX(Xin)]
        );
        this->regs[REG_INDEX(Feature)].Clone(
            regEn,
            this->ports[PORT_INDEX(XinEn)]
        );
        /** mark registers **/
        this->markRegs[MARK_REG_INDEX(  UpFull)].Clone(
            markRegSet,
            this->ports[PORT_INDEX(  FupSet)]
        );
        this->markRegs[MARK_REG_INDEX(UpUpFull)].Clone(
            markRegSet,
            this->ports[PORT_INDEX(FupupSet)]
        );
        this->markRegs[MARK_REG_INDEX(LeftFull)].Clone(
            markRegSet,
            this->ports[PORT_INDEX(FleftSet)]
        );
        /** combinational logic 0 **/
        this->cl0.Clone(
            cl0FupSet,
            this->ports[PORT_INDEX(FupSet)]
        );
        this->cl0.Clone(
            cl0FleftSet,
            this->ports[PORT_INDEX(FleftSet)]
        );
        this->cl0.Clone(
            cl0FrightIn,
            this->ports[PORT_INDEX(FrightIn)]
        );
        this->cl0.Clone(
            cl0FdownIn,
            this->ports[PORT_INDEX(FdownIn)]
        );
        /** combinational logic 1 **/
        this->cl1.Clone(
            cl1Sin,
            this->ports[PORT_INDEX(Sin)]
        );
        this->cl1.Clone(
            cl1FddownIn,
            this->ports[PORT_INDEX(FddownIn)]
        );

        /*************************
         ** internal connection **
         *************************/
        /** register OBuffer **/
        this->regs[REG_INDEX(OBuffer)].ConnectTo(
            regIn,
            this->regs[REG_INDEX(OutMark)],
            regOut
        );
        this->regs[REG_INDEX(OBuffer)].ConnectTo(
            regEn,
            this->cl0,
            cl0OWEn
        );
        /** register WBuffer **/
        this->regs[REG_INDEX(WBuffer)].ConnectTo(
            regIn,
            this->regs[REG_INDEX(Weight)],
            regOut
        );
        this->regs[REG_INDEX(WBuffer)].ConnectTo(
            regEn,
            this->cl0,
            cl0OWEn
        );
        /** register XBuffer **/
        this->regs[REG_INDEX(XBuffer)].ConnectTo(
            regIn,
            this->regs[REG_INDEX(Feature)],
            regOut
        );
        this->regs[REG_INDEX(XBuffer)].ConnectTo(
            regEn,
            this->cl0,
            cl0XEn
        );
        /** register ParSum **/
        this->regs[REG_INDEX(ParSum)].ConnectTo(
            regIn,
            this->cl1,
            cl1InnerSout
        );
        this->regs[REG_INDEX(ParSum)].ConnectTo(
            regEn,
            this->cl1,
            cl1InnerSen
        );
        /** register Sum **/
        this->regs[REG_INDEX(Sum)].ConnectTo(
            regIn,
            this->cl1,
            cl1Sout
        );
        this->regs[REG_INDEX(Sum)].ConnectTo(
            regEn,
            this->cl1,
            cl1Sen
        );
        /** mark register UpFull **/
        this->markRegs[MARK_REG_INDEX(UpFull)].ConnectTo(
            markRegClr,
            this->cl0,
            cl0FupClr
        );
        /** mark register UpUpFull **/
        this->markRegs[MARK_REG_INDEX(UpUpFull)].ConnectTo(
            markRegClr,
            this->cl1,
            cl1FupupClr
        );
        /** mark register LeftFull **/
        this->markRegs[MARK_REG_INDEX(LeftFull)].ConnectTo(
            markRegClr,
            this->cl0,
            cl0FleftClr
        );
        /** combinational logic 0 **/
        this->cl0.ConnectTo(
            cl0FupIn,
            this->markRegs[MARK_REG_INDEX(UpFull)],
            markRegOut
        );
        this->cl0.ConnectTo(
            cl0FleftIn,
            this->markRegs[MARK_REG_INDEX(LeftFull)],
            markRegOut
        );
        this->cl0.ConnectTo(
            cl0CalcDone,
            this->cl1,
            cl1CalcDone
        );
        /** combinational logic 1 **/
        this->cl1.ConnectTo(
            cl1Obuf,
            this->regs[REG_INDEX(OBuffer)],
            regOut
        );
        this->cl1.ConnectTo(
            cl1Wbuf,
            this->regs[REG_INDEX(WBuffer)],
            regOut
        );
        this->cl1.ConnectTo(
            cl1Xbuf,
            this->regs[REG_INDEX(XBuffer)],
            regOut
        );
        this->cl1.ConnectTo(
            cl1InnerSreg,
            this->regs[REG_INDEX(ParSum )],
            regOut
        );
        this->cl1.ConnectTo(
            cl1FupupIn,
            this->markRegs[MARK_REG_INDEX(UpUpFull)],
            markRegOut
        );
        this->cl1.ConnectTo(
            cl1CountDone,
            this->counter,
            countDone
        );
        /** counter **/
        this->counter.ConnectTo(
            countEn,
            this->cl0,
            cl0CountEn
        );
        this->counter.ConnectTo(
            countRst,
            this->cl0,
            cl0CountRst
        );
        assert(this->InnerConnected());
        return;
    }
    bool InnerConnected() const override{
        bool hasConnected = true;
        hasConnected = this->cl0.Connected() && hasConnected;
        hasConnected = this->cl1.Connected() && hasConnected;
        hasConnected = this->counter.Connected() && hasConnected;
        for (auto& reg : this->regs)
            hasConnected = reg.Connected() && hasConnected;
        for (auto& reg : this->markRegs)
            hasConnected = reg.Connected() && hasConnected;
        return hasConnected;
    }
    bool InnerConnectedCorrect() const override{
        const     Reg::PortName   regIn         (Reg::PortName::_PortName::in );
        const     Reg::PortName   regEn         (Reg::PortName::_PortName::en );
        const     Reg::PortName   regOut        (Reg::PortName::_PortName::out);
        const MarkReg::PortName markRegSet  (MarkReg::PortName::_PortName::Set);
        const MarkReg::PortName markRegClr  (MarkReg::PortName::_PortName::Clr);
        const MarkReg::PortName markRegOut  (MarkReg::PortName::_PortName::out);
        const  PE_CL0::PortName cl0FupSet    (PE_CL0::PortName::_PortName::FupSet  ); /** 1-bit in     input-port FupSet **/
        const  PE_CL0::PortName cl0FupIn     (PE_CL0::PortName::_PortName::FupIn   ); /** 1-bit in    reg UpFull **/
        const  PE_CL0::PortName cl0FupClr    (PE_CL0::PortName::_PortName::FupClr  ); /** 1-bit out   reg UpFull **/
        const  PE_CL0::PortName cl0FdownIn   (PE_CL0::PortName::_PortName::FdownIn ); /** 1-bit in     input-port FdownIn  **/
        const  PE_CL0::PortName cl0FleftSet  (PE_CL0::PortName::_PortName::FleftSet); /** 1-bit in     input-port FleftSet **/
        const  PE_CL0::PortName cl0FleftIn   (PE_CL0::PortName::_PortName::FleftIn ); /** 1-bit in    reg LeftFull **/
        const  PE_CL0::PortName cl0FleftClr  (PE_CL0::PortName::_PortName::FleftClr); /** 1-bit out   reg LeftFull **/
        const  PE_CL0::PortName cl0FrightIn  (PE_CL0::PortName::_PortName::FrightIn); /** 1-bit in     input-port FrightIn  **/
        const  PE_CL0::PortName cl0CalcDone  (PE_CL0::PortName::_PortName::CalcDone); /** 1-bit in    combinational logical CL1 **/
        const  PE_CL0::PortName cl0CountEn   (PE_CL0::PortName::_PortName::CountEn ); /** 1-bit out   counter **/
        const  PE_CL0::PortName cl0CountRst  (PE_CL0::PortName::_PortName::CountRst); /** 1-bit out   counter **/
        const  PE_CL0::PortName cl0OWEn      (PE_CL0::PortName::_PortName::OWEn    ); /** 1-bit out   reg   weight-buffer **/
        const  PE_CL0::PortName cl0XEn       (PE_CL0::PortName::_PortName::XEn     ); /** 1-bit out   reg  feature-buffer **/
        const  PE_CL1::PortName cl1Obuf      (PE_CL1::PortName::_PortName::Obuf      ); /**  1-bit in    reg  output-buffer **/
        const  PE_CL1::PortName cl1Wbuf      (PE_CL1::PortName::_PortName::Wbuf      ); /**  8-bit in    reg  weight-buffer **/
        const  PE_CL1::PortName cl1Xbuf      (PE_CL1::PortName::_PortName::Xbuf      ); /**  8-bit in    reg feature-buffer **/
        const  PE_CL1::PortName cl1CalcDone  (PE_CL1::PortName::_PortName::CalcDone  ); /**  1-bit out   combinational logical CL0 **/
        const  PE_CL1::PortName cl1CountDone (PE_CL1::PortName::_PortName::CountDone ); /**  1-bit in    Counter **/
        const  PE_CL1::PortName cl1FupupIn   (PE_CL1::PortName::_PortName::FupupIn   ); /**  1-bit in    two-port-reg UpUpFull **/
        const  PE_CL1::PortName cl1FupupClr  (PE_CL1::PortName::_PortName::FupupClr  ); /**  1-bit out   two-port-reg UpUpFull **/
        const  PE_CL1::PortName cl1Sin       (PE_CL1::PortName::_PortName::Sin       ); /** 24-bit in    input-port Sin **/
        const  PE_CL1::PortName cl1FddownIn  (PE_CL1::PortName::_PortName::FddownIn  ); /**  1-bit in     input-port FddownIn  **/
        const  PE_CL1::PortName cl1InnerSreg (PE_CL1::PortName::_PortName::InnerSreg ); /** 24-bit in    reg ParSum **/
        const  PE_CL1::PortName cl1InnerSout (PE_CL1::PortName::_PortName::InnerSout ); /** 24-bit out   reg ParSum **/
        const  PE_CL1::PortName cl1InnerSen  (PE_CL1::PortName::_PortName::InnerSen  ); /**  1-bit out   reg ParSum **/
        const  PE_CL1::PortName cl1Sout      (PE_CL1::PortName::_PortName::Sout      ); /** 24-bit out   reg    Sum **/
        const  PE_CL1::PortName cl1Sen       (PE_CL1::PortName::_PortName::Sen       ); /**  1-bit out   reg    Sum **/
        const Counter::PortName countDone   (Counter::PortName::_PortName::done      ); /**  1-bit out  **/
        const Counter::PortName countEn     (Counter::PortName::_PortName::en        ); /**  1-bit in   **/
        const Counter::PortName countRst    (Counter::PortName::_PortName::rst       ); /**  1-bit in   **/

        bool connectedCorrect=true;
        connectedCorrect = this->counter.Connected() && connectedCorrect;
        for (auto& reg : this->regs)
            connectedCorrect = reg.Connected() && connectedCorrect;
        for (auto& reg : this->markRegs)
            connectedCorrect = reg.Connected() && connectedCorrect;
        connectedCorrect = this->cl0.Connected() && connectedCorrect;
        connectedCorrect = this->cl1.Connected() && connectedCorrect;

        return connectedCorrect
            /******************************************
             ** connect input ports with inner logic **
             ******************************************/
            /** registers **/
         && this->regs[REG_INDEX(OutMark)].SameTo(
                regIn,
                this->ports[PORT_INDEX(Oin)]
            )
         && this->regs[REG_INDEX(OutMark)].SameTo(
                regEn,
                this->ports[PORT_INDEX(OWinEn)]
            )
         && this->regs[REG_INDEX(Weight)].SameTo(
                regIn,
                this->ports[PORT_INDEX(Win)]
            )
         && this->regs[REG_INDEX(Weight)].SameTo(
                regEn,
                this->ports[PORT_INDEX(OWinEn)]
            )
         && this->regs[REG_INDEX(Feature)].SameTo(
                regIn,
                this->ports[PORT_INDEX(Xin)]
            )
         && this->regs[REG_INDEX(Feature)].SameTo(
                regEn,
                this->ports[PORT_INDEX(XinEn)]
            )
            /** mark registers **/
         && this->markRegs[MARK_REG_INDEX(  UpFull)].SameTo(
                markRegSet,
                this->ports[PORT_INDEX(  FupSet)]
            )
         && this->markRegs[MARK_REG_INDEX(UpUpFull)].SameTo(
                markRegSet,
                this->ports[PORT_INDEX(FupupSet)]
            )
         && this->markRegs[MARK_REG_INDEX(LeftFull)].SameTo(
                markRegSet,
                this->ports[PORT_INDEX(FleftSet)]
            )
            /** combinational logic 0 **/
         && this->cl0.SameTo(
                cl0FupSet,
                this->ports[PORT_INDEX(FupSet)]
            )
         && this->cl0.SameTo(
                cl0FleftSet,
                this->ports[PORT_INDEX(FleftSet)]
            )
         && this->cl0.SameTo(
                cl0FrightIn,
                this->ports[PORT_INDEX(FrightIn)]
            )
         && this->cl0.SameTo(
                cl0FdownIn,
                this->ports[PORT_INDEX(FdownIn)]
            )
            /** combinational logic 1 **/
         && this->cl1.SameTo(
                cl1Sin,
                this->ports[PORT_INDEX(Sin)]
            )
         && this->cl1.SameTo(
                cl1FddownIn,
                this->ports[PORT_INDEX(FddownIn)]
            )
            /*************************
             ** internal connection **
             *************************/
            /** register OBuffer **/
         && this->regs[REG_INDEX(OBuffer)].ConnectedTo(
                regIn,
                this->regs[REG_INDEX(OutMark)],
                regOut
            )
         && this->regs[REG_INDEX(OBuffer)].ConnectedTo(
                regEn,
                this->cl0,
                cl0OWEn
            )
            /** register WBuffer **/
         && this->regs[REG_INDEX(WBuffer)].ConnectedTo(
                regIn,
                this->regs[REG_INDEX(Weight)],
                regOut
            )
         && this->regs[REG_INDEX(WBuffer)].ConnectedTo(
                regEn,
                this->cl0,
                cl0OWEn
            )
            /** register XBuffer **/
         && this->regs[REG_INDEX(XBuffer)].ConnectedTo(
                regIn,
                this->regs[REG_INDEX(Feature)],
                regOut
            )
         && this->regs[REG_INDEX(XBuffer)].ConnectedTo(
                regEn,
                this->cl0,
                cl0XEn
            )
            /** register ParSum **/
         && this->regs[REG_INDEX(ParSum)].ConnectedTo(
                regIn,
                this->cl1,
                cl1InnerSout
            )
         && this->regs[REG_INDEX(ParSum)].ConnectedTo(
                regEn,
                this->cl1,
                cl1InnerSen
            )
            /** register Sum **/
         && this->regs[REG_INDEX(Sum)].ConnectedTo(
                regIn,
                this->cl1,
                cl1Sout
            )
         && this->regs[REG_INDEX(Sum)].ConnectedTo(
                regEn,
                this->cl1,
                cl1Sen
            )
            /** mark register UpFull **/
         && this->markRegs[MARK_REG_INDEX(UpFull)].ConnectedTo(
                markRegClr,
                this->cl0,
                cl0FupClr
            )
            /** mark register UpUpFull **/
         && this->markRegs[MARK_REG_INDEX(UpUpFull)].ConnectedTo(
                markRegClr,
                this->cl1,
                cl1FupupClr
            )
            /** mark register LeftFull **/
         && this->markRegs[MARK_REG_INDEX(LeftFull)].ConnectedTo(
                markRegClr,
                this->cl0,
                cl0FleftClr
            )
            /** combinational logic 0 **/
         && this->cl0.ConnectedTo(
                cl0FupIn,
                this->markRegs[MARK_REG_INDEX(UpFull)],
                markRegOut
            )
         && this->cl0.ConnectedTo(
                cl0FleftIn,
                this->markRegs[MARK_REG_INDEX(LeftFull)],
                markRegOut
            )
         && this->cl0.ConnectedTo(
                cl0CalcDone,
                this->cl1,
                cl1CalcDone
            )
            /** combinational logic 1 **/
         && this->cl1.ConnectedTo(
                cl1Obuf,
                this->regs[REG_INDEX(OBuffer)],
                regOut
            )
         && this->cl1.ConnectedTo(
                cl1Wbuf,
                this->regs[REG_INDEX(WBuffer)],
                regOut
            )
         && this->cl1.ConnectedTo(
                cl1Xbuf,
                this->regs[REG_INDEX(XBuffer)],
                regOut
            )
         && this->cl1.ConnectedTo(
                cl1InnerSreg,
                this->regs[REG_INDEX(ParSum )],
                regOut
            )
         && this->cl1.ConnectedTo(
                cl1FupupIn,
                this->markRegs[MARK_REG_INDEX(UpUpFull)],
                markRegOut
            )
         && this->cl1.ConnectedTo(
                cl1CountDone,
                this->counter,
                countDone
            )
            /** counter **/
         && this->counter.ConnectedTo(
                countEn,
                this->cl0,
                cl0CountEn
            )
         && this->counter.ConnectedTo(
                countRst,
                this->cl0,
                cl0CountRst
            );
    }
    inline bool OutputConnectedCorrect() const override{
        const     Reg::PortName regOutPort        (Reg::PortName::_PortName::out);
        const MarkReg::PortName markRegOutPort(MarkReg::PortName::_PortName::out);
        const  PE_CL0::PortName cl0OWoutEn     (PE_CL0::PortName::_PortName::OWoutEn);
        const  PE_CL0::PortName cl0FdownSet    (PE_CL0::PortName::_PortName::FdownSet);
        const  PE_CL0::PortName cl0XoutEn      (PE_CL0::PortName::_PortName::XoutEn);
        const  PE_CL0::PortName cl0FrightSet   (PE_CL0::PortName::_PortName::FrightSet);
        const  PE_CL1::PortName cl1FddownSet   (PE_CL1::PortName::_PortName::FddownSet);

        return
        /** registers **/
            this->regs[REG_INDEX(OutMark)].SameTo(
                this->ports[PORT_INDEX(Oout)],
                regOutPort
            )
        &&  this->regs[REG_INDEX(Weight)].SameTo(
                this->ports[PORT_INDEX(Wout)],
                regOutPort
            )
        &&  this->regs[REG_INDEX(Feature)].SameTo(
                this->ports[PORT_INDEX(Xout)],
                regOutPort
            )
        &&  this->regs[REG_INDEX(Sum)].SameTo(
                this->ports[PORT_INDEX(Sout)],
                regOutPort
            )
        /** mark registers **/
        &&  this->markRegs[MARK_REG_INDEX(  UpFull)].SameTo(
                this->ports[PORT_INDEX(FupOut)],
                markRegOutPort
            )
        &&  this->markRegs[MARK_REG_INDEX(UpUpFull)].SameTo(
                this->ports[PORT_INDEX(FupupOut)],
                markRegOutPort
            )
        &&  this->markRegs[MARK_REG_INDEX(LeftFull)].SameTo(
                this->ports[PORT_INDEX(FleftOut)],
                markRegOutPort
            )
        /** combinatorial logic 0 **/
        &&  this->cl0.SameTo(
                this->ports[PORT_INDEX(OWoutEn)],
                cl0OWoutEn
            )
        &&  this->cl0.SameTo(
                this->ports[PORT_INDEX(FdownSet)],
                cl0FdownSet
            )
        &&  this->cl0.SameTo(
                this->ports[PORT_INDEX(XoutEn)],
                cl0XoutEn
            )
        &&  this->cl0.SameTo(
                this->ports[PORT_INDEX(FrightSet)],
                cl0FrightSet
            )
        /** combinatorial logic 1 **/
        &&  this->cl1.SameTo(
                this->ports[PORT_INDEX(FddownSet)],
                cl1FddownSet
            );
    }
    inline bool atPosEdge() override{
        assert(this->Connected());
        bool hasChanged = false;
        for (auto& reg:this->regs)
            hasChanged =       reg.atPosEdge() || hasChanged;
        for (auto& markReg:this->markRegs)
            hasChanged =   markReg.atPosEdge() || hasChanged;
        hasChanged = this->counter.atPosEdge() || hasChanged;
        hasChanged = this->    cl1.atPosEdge() || hasChanged;
        hasChanged = this->    cl0.atPosEdge() || hasChanged;

        return hasChanged;
    }
    inline bool atNegEdge() override{
        return false;
    }
    inline bool Run() override{
        bool hasChanged = false;
        for (auto& reg:this->regs)
            hasChanged  =      reg.Run() || hasChanged;
        for (auto& markReg:this->markRegs)
            hasChanged  =  markReg.Run() || hasChanged;
        hasChanged = this->counter.Run() || hasChanged;
        hasChanged = this->    cl1.Run() || hasChanged;
        hasChanged = this->    cl0.Run() || hasChanged;
        return hasChanged;
    }
    inline bool IsStall(){
        return this->cl0.IsStall();
    }
    inline void Print(){
        /** the relevant port of weight and output-mark **/
            ///------------- upper side -------------///
        Object::Port port1 ( 1,Object::PortDirection::IN );/** FupOut     1-bit out    up-cell **/
            ///------------- under side -------------///
        Object::Port port6 ( 1,Object::PortDirection::IN );/** FdownSet   1-bit out  down-cell **/
        Object::Port port7 ( 1,Object::PortDirection::IN );/** Oout       1-bit out  down-cell **/
        Object::Port port8 ( 8,Object::PortDirection::IN );/** Wout       8-bit out  down-cell **/
        Object::Port port9 ( 1,Object::PortDirection::IN );/** OWoutEn    1-bit out  down-cell **/
        /** the relevant port of feature **/
            ///------------- left  side -------------///
        Object::Port port11( 1,Object::PortDirection::IN );/** FleftOut   1-bit out  left-cell **/
            ///------------- right side -------------///
        Object::Port port15( 1,Object::PortDirection::IN );/** FrightSet  1-bit out right-cell **/
        Object::Port port16( 8,Object::PortDirection::IN );/** Xout       8-bit out right-cell **/
        Object::Port port17( 1,Object::PortDirection::IN );/** XoutEn     1-bit out right-cell **/
        /** the relevant port of partial sum **/
            ///------------   up of up   ------------///
        Object::Port port19( 1,Object::PortDirection::IN );/** FupupOut   1-bit out  upup-cell **/
            ///------------ down of down ------------///
        Object::Port port22( 1,Object::PortDirection::IN );/** FddownSet  1-bit out ddown-cell **/
        Object::Port port23(24,Object::PortDirection::IN );/** Sout      24-bit out ddown-cell **/


        /** the relevant port of weight and output-mark **/
            ///------------- upper side -------------///
        port1.ConnectTo(this->ports[PORT_INDEX(FupOut    )]);/** FupOut     1-bit out    up-cell **/
            ///------------- under side -------------///
        port6.ConnectTo(this->ports[PORT_INDEX(FdownSet  )]);/** FdownSet   1-bit out  down-cell **/
        port7.ConnectTo(this->ports[PORT_INDEX(Oout      )]);/** Oout       1-bit out  down-cell **/
        port8.ConnectTo(this->ports[PORT_INDEX(Wout      )]);/** Wout       8-bit out  down-cell **/
        port9.ConnectTo(this->ports[PORT_INDEX(OWoutEn   )]);/** OWoutEn    1-bit out  down-cell **/
        /** the relevant port of feature **/
            ///------------- left  side -------------///
        port11.ConnectTo(this->ports[PORT_INDEX(FleftOut )]);/** FleftOut   1-bit out  left-cell **/
            ///------------- right side -------------///
        port15.ConnectTo(this->ports[PORT_INDEX(FrightSet)]);/** FrightSet  1-bit out right-cell **/
        port16.ConnectTo(this->ports[PORT_INDEX(Xout     )]);/** Xout       8-bit out right-cell **/
        port17.ConnectTo(this->ports[PORT_INDEX(XoutEn   )]);/** XoutEn     1-bit out right-cell **/
        /** the relevant port of partial sum **/
            ///------------   up of up   ------------///
        port19.ConnectTo(this->ports[PORT_INDEX(FupupOut )]);/** FupupOut   1-bit out  upup-cell **/
            ///------------ down of down ------------///
        port22.ConnectTo(this->ports[PORT_INDEX(FddownSet)]);/** FddownSet  1-bit out ddown-cell **/
        port23.ConnectTo(this->ports[PORT_INDEX(Sout     )]);/** Sout      24-bit out ddown-cell **/

        std::cout<<"#########################"<<std::endl;
//        std::cout<<"#   UpFull state #"<<std::endl;
//        this->markRegs[MARK_REG_INDEX(  UpFull)].Print();
//        std::cout<<"# LeftFull state #"<<std::endl;
//        this->markRegs[MARK_REG_INDEX(LeftFull)].Print();
        std::cout<<"# Outmark reg state #"<<std::endl;
        this->regs[REG_INDEX(OutMark)].Print();
        std::cout<<"# Weight  reg state #"<<std::endl;
        this->regs[REG_INDEX(Weight )].Print();
        std::cout<<"# Feature reg state #"<<std::endl;
        this->regs[REG_INDEX(Feature)].Print();
        std::cout<<"# Obuffer reg state #"<<std::endl;
        this->regs[REG_INDEX(OBuffer)].Print();
        std::cout<<"# Wbuffer reg state #"<<std::endl;
        this->regs[REG_INDEX(WBuffer)].Print();
        std::cout<<"# Xbuffer reg state #"<<std::endl;
        this->regs[REG_INDEX(XBuffer)].Print();
        std::cout<<"# ParSum  reg state #"<<std::endl;
        this->regs[REG_INDEX(ParSum)].Print();
        std::cout<<"#   Counter   state #"<<std::endl;
        this->counter.Print();
        std::cout<<"#########################"<<std::endl;
        /** the relevant port of weight and output-mark **/
            ///------------- upper side -------------///
        std::cout<<"FupOut    "<<port1 .Data()<<std::endl;/** FupOut     1-bit out    up-cell **/
            ///------------- under side -------------///
        std::cout<<"FdownSet  "<<port6 .Data()<<std::endl;/** FdownSet   1-bit out  down-cell **/
        std::cout<<"Oout      "<<port7 .Data()<<std::endl;/** Oout       1-bit out  down-cell **/
        std::cout<<"Wout      "<<port8 .Data()<<std::endl;/** Wout       8-bit out  down-cell **/
        std::cout<<"OWoutEn   "<<port9 .Data()<<std::endl;/** OWoutEn    1-bit out  down-cell **/
        /** the relevant port of feature **/
            ///------------- left  side -------------///
        std::cout<<"FleftOut  "<<port11 .Data()<<std::endl;/** FleftOut   1-bit out  left-cell **/
            ///------------- right side -------------///
        std::cout<<"FrightSet "<<port15 .Data()<<std::endl;/** FrightSet  1-bit out right-cell **/
        std::cout<<"Xout      "<<port16 .Data()<<std::endl;/** Xout       8-bit out right-cell **/
        std::cout<<"XoutEn    "<<port17 .Data()<<std::endl;/** XoutEn     1-bit out right-cell **/
        /** the relevant port of partial sum **/
            ///------------   up of up   ------------///
        std::cout<<"FupupOut  "<<port19 .Data()<<std::endl;/** FupupOut   1-bit out  upup-cell **/
            ///------------ down of down ------------///
        std::cout<<"FddownSet "<<port22 .Data()<<std::endl;/** FddownSet  1-bit out ddown-cell **/
        std::cout<<"Sout      "<<port23 .Data()<<std::endl;/** Sout      24-bit out ddown-cell **/
        std::cout<<"#########################"<<std::endl;
        return;
    }
};


#endif // __COMPONENT_PE_HPP
