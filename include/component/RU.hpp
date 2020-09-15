#ifndef __COMPONENT_RU_HPP
#define __COMPONENT_RU_HPP

#include "RePE.hpp"
#include "../resource/BaseSequLogic.hpp"
#include "../base/Object.hpp"

class RU:public ClockedObject{
public:
    static constexpr const uint8_t
          X_BIT_WIDTH = ReformedPE::X_BIT_WIDTH,
         XL_BIT_WIDTH = ReformedPE::OFFSET_BIT_WIDTH;///group number == 64
    static constexpr const uint8_t
         WL_BIT_WIDTH = ReformedPE::OFFSET_BIT_WIDTH;

    /** Compress methods **/
    static inline PortDataType CompressEWL(
            PortDataType wl,
            PortDataType eow
        ){
        assert(eow == 0 || eow == 1);
        assert((wl & ((Object::extend[WL_BIT_WIDTH])<<1))==0);
        return (wl<<1)+eow;
    }
    static inline PortDataType CompressXEXL(
            PortDataType x,
            PortDataType eox,
            PortDataType xl,
            PortDataType valid
        ){
        assert(( eox  == 0 ||  eox  == 1)
             &&(valid == 0 || valid == 1));
        assert((x  & (Object::extend[ X_BIT_WIDTH]))==0/// >=0
            || (x  & (Object::extend[ X_BIT_WIDTH]))/// <0
                 ==  (Object::extend[ X_BIT_WIDTH]));
        assert((xl & (Object::extend[XL_BIT_WIDTH]))==0);
        return ((xl<<(X_BIT_WIDTH+2))+((x&Object::mask[X_BIT_WIDTH])<<2)+(eox<<1)+valid);
    }
    static inline PortDataType CompressXEXL(bool valid){
        assert(valid == false);
        return 0;
    }
    /** Decompress methods **/
    /**
     *
     *  XL
     *  X
     *  XE
     *  V
     *
     **/
    static inline PortDataType DecompressXL   (PortDataType xexl){
        return  (xexl>>(X_BIT_WIDTH+2));
    }
    static inline PortDataType DecompressX    (PortDataType xexl){
        return ((xexl>> 2)&Object::mask[X_BIT_WIDTH]);
    }
    static inline PortDataType DecompressEOX  (PortDataType xexl){
        return  ((xexl & 2)>>1);
    }
    static inline PortDataType DecompressValid(PortDataType xexl){
        return  (xexl & 1);
    }
    static inline PortDataType DecompressWL (PortDataType ewl){
        return  ( ewl>> 1);
    }
    static inline PortDataType DecompressEOW(PortDataType ewl){
        return  ( ewl & 1);
    }

    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
        /** ---------- up -------- **/
            BufPushUp,  /**    1-bit  in  **/
            BufInUp,    /** 1+WL_BIT  in  **/
            BufFullUp,  /**    1-bit  out **/
        /** --------- down ------- **/
            BufPushDown,/**    1-bit  out **/
            BufInDown,  /** 1+WL_BIT  out **/
            BufFullDown,/**    1-bit  in  **/
        /** --------- left ------- **/
            XELIn,    /** 2+X_BIT+XL_BIT  in  **/
            XELPush,  /** 1-bit           in  **/
            XELFull,  /** 1-bit           out **/
        /** -------- right ------- **/
            OOut,     /** X_BIT   out **/
            OPop,     /** 1-bit   in  **/
            OEmpty,   /** 1-bit   out **/
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

private:
    static constexpr const int
         BUF_FIFO_LEN =  2,
         EWL_FIFO_LEN = 16,
        XEXL_FIFO_LEN = 16,
        XOUT_FIFO_LEN = 16;

    enum class RU_STATE{
        WL_LESS_XL,
        WL_EQUAL_XL,
        WL_BIGGER_XL,
        WL_END,    /// wl reach the end of group
        WL_OX_END, /// wl reach the end of group and output x
        WL_OZ_END, /// wl reach the end of group and output 0
        XL_END,    /// xl reach the end of group
        XL_OX_END, /// xl reach the end of group and output x
        XL_OZ_END, /// xl reach the end of group and output 0
        WX_END,    /// both xl and wl reach the end of group
        WX_OX_END, /// both xl and wl reach the end of group and output x
        WX_OZ_END  /// both xl and wl reach the end of group and output 0
    };

    enum RU_STATE state;

    FIFO   oFIFO, ///          X_BIT   x
         bufFIFO, ///       1+WL_BIT       end-of-w w-location
         ewlFIFO, ///       1+WL_BIT       end-of-w w-location
        xexlFIFO; /// 2+X_BIT+XL_BIT   x   end-of-x x-location

    Reg  ewlReg0, ///       1+WL_BIT
         ewlReg1, ///       1+WL_BIT
        xexlReg0, /// 2+X_BIT+XL_BIT
        xexlReg1; /// 2+X_BIT+XL_BIT

    PortDataType
        bufPushDown;/** BufPushDown    1-bit  out **/

    Port bufFIFOEmpty, ///   bufFIFO             1-bit  IN
         ewlFIFOFull,  ///   ewlFIFO             1-bit  IN
         ewlFIFOEmpty, ///   ewlFIFO             1-bit  IN
        xexlFIFOFull,  ///  xexlFIFO             1-bit  IN
        xexlFIFOEmpty, ///  xexlFIFO             1-bit  IN
           oFIFOFull,  ///     oFIFO             1-bit  IN
         ewlReg0Out,   ///   ewlReg0          1+WL_BIT  IN
         ewlReg1Out,   ///   ewlReg1          1+WL_BIT  IN
        xexlReg0Out,   ///  xexlReg0    2+X_BIT+XL_BIT  IN
        xexlReg1Out;   ///  xexlReg1    2+X_BIT+XL_BIT  IN

    ///inner output
    Port ///inner output
        pBufFIFOPop, /**  bufFIFO    1-bit **/
          pOFIFOPush,/**    oFIFO    1-bit **/
        pEWLFIFOPop, /**  ewlFIFO    1-bit **/
        pEWLFIFOPush,/**  ewlFIFO    1-bit **/
       pXEXLFIFOPop, /** xexlFIFO    1-bit **/
         pEWLReg0En, /**  ewlReg0    1-bit **/
         pEWLReg1En, /**  ewlReg1    1-bit **/
        pXEXLReg0En, /** xexlReg0    1-bit **/
        pXEXLReg1En, /** xexlReg1    1-bit **/
           pXReg1Out;/** xexlReg1    X_BIT  X_BIT of xexlout **/

    PortDataType ///inner output
        bufFIFOPop ,/**  bufFIFO    1-bit **/
          oFIFOPush,/**    oFIFO    1-bit **/
        ewlFIFOPop ,/**  ewlFIFO    1-bit **/
        ewlFIFOPush,/**  ewlFIFO    1-bit **/
       xexlFIFOPop, /** xexlFIFO    1-bit **/
         ewlReg0En, /**  ewlReg0    1-bit **/
         ewlReg1En, /**  ewlReg1    1-bit **/
        xexlReg0En, /** xexlReg0    1-bit **/
        xexlReg1En, /** xexlReg1    1-bit **/
           xReg1Out;/** xexlReg1    X_BIT  X_BIT of xexlout **/

    void InitializePorts() override{
        this->ports.clear();
        /** ---------- up -------- **/
        this->ports.push_back(Port(1,PortDirection::IN ));/** BufPushUp    1-bit  in  **/
        this->ports.push_back(Port(1
                  + RU::WL_BIT_WIDTH,PortDirection::IN ));/** BufInUp   1+WL_BIT  in  **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** BufFullUp    1-bit out **/
        /** --------- down ------- **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** BufPushDown  1-bit  out **/
        this->ports.push_back(Port(1
                  + RU::WL_BIT_WIDTH,PortDirection::OUT));/** BufInDown 1+WL_BIT  out **/
        this->ports.push_back(Port(1,PortDirection::IN ));/** BufFullDown  1-bit  in  **/
        /** --------- left ------- **/
        this->ports.push_back(Port(2
                  + RU:: X_BIT_WIDTH
                  + RU::XL_BIT_WIDTH,PortDirection::IN ));/** XELIn    2+X_BIT+XL_BIT in  **/
        this->ports.push_back(Port(1,PortDirection::IN ));/** XELPush  1-bit          in  **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** XELFull  1-bit          out **/
        /** -------- right ------- **/
        this->ports.push_back(Port(
                    RU:: X_BIT_WIDTH,PortDirection::OUT));/** OOut   X_BIT   out **/
        this->ports.push_back(Port(1,PortDirection::IN ));/** OPop   1-bit   in  **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** OEmpty 1-bit   out **/
        assert(this->ports.size()==PortName::PortNum());
        return;
    }

    void OutputConnect() override{
        /** ---------- up -------- **/
        this->bufFIFO.Clone(
            this->ports[PORT_INDEX(BufFullUp )],
            FIFO::Full);
        /** --------- down ------- **/
        this->ports[PORT_INDEX(BufPushDown)].Clone(this->bufPushDown);
        this->bufFIFO.Clone(
            this->ports[PORT_INDEX(BufInDown)],
            FIFO::Out);
        /** --------- left ------- **/
        this->xexlFIFO.Clone(
            this->ports[PORT_INDEX(XELFull)],
            FIFO::Full);
        /** -------- right ------- **/
        this->oFIFO.Clone(
            this->ports[PORT_INDEX(OOut)],
            FIFO::Out);
        this->oFIFO.Clone(
            this->ports[PORT_INDEX(OEmpty)],
            FIFO::Empty);
        /** -------- inner ------- **/
        this-> pBufFIFOPop .Clone(this-> bufFIFOPop ); /**  bufFIFO    1-bit **/
        this->   pOFIFOPush.Clone(this->   oFIFOPush); /**    oFIFO    1-bit **/
        this-> pEWLFIFOPop .Clone(this-> ewlFIFOPop ); /**  ewlFIFO    1-bit **/
        this-> pEWLFIFOPush.Clone(this-> ewlFIFOPush); /**  ewlFIFO    1-bit **/
        this->pXEXLFIFOPop .Clone(this->xexlFIFOPop ); /** xexlFIFO    1-bit **/
        this->  pEWLReg0En .Clone(this-> ewlReg0En  ); /**  ewlReg0    1-bit **/
        this->  pEWLReg1En .Clone(this-> ewlReg1En  ); /**  ewlReg1    1-bit **/
        this-> pXEXLReg0En .Clone(this->xexlReg0En  ); /** xexlReg0    1-bit **/
        this-> pXEXLReg1En .Clone(this->xexlReg1En  ); /** xexlReg1    1-bit **/
        this->    pXReg1Out.Clone(this->   xReg1Out ); /** xexlReg1    X_BIT  X_BIT of xexlout **/
        assert(this->OutputConnected());
        assert(this->OutputConnectedCorrect());
        return;
    }
    /**   ewl register 0  **/
    inline PortDataType wlReg0() const{
        return  RU::DecompressWL (this->ewlReg0Out.Data());
    }
    inline bool eowReg0() const{
        return  RU::DecompressEOW(this->ewlReg0Out.Data());
    }
    /**   ewl register 1  **/
    inline PortDataType wlReg1() const{
        return  RU::DecompressWL (this->ewlReg1Out.Data());
    }
    inline bool eowReg1() const{
        return  RU::DecompressEOW(this->ewlReg1Out.Data());
    }
    /**  xexl register 0  **/
    inline PortDataType xReg0() const{
        return  RU::DecompressX  (this->xexlReg0Out.Data());
    }
    inline PortDataType xlReg0() const{
        return  RU::DecompressXL (this->xexlReg0Out.Data());
    }
    inline bool eoxReg0() const{
        return  RU::DecompressEOX(this->xexlReg0Out.Data());
    }
    /**  xexl register 1  **/
    inline PortDataType xReg1() const{
        return  RU::DecompressX  (this->xexlReg1Out.Data());
    }
    inline PortDataType xlReg1() const{
        return  RU::DecompressXL (this->xexlReg1Out.Data());
    }
    inline bool eoxReg1() const{
        return  RU::DecompressEOX(this->xexlReg1Out.Data());
    }
    inline bool validReg1() const{
        return  (RU::DecompressValid(this->xexlReg1Out.Data())==1);
    }
    /** action handles **/
    inline void Stuck(){
        this-> ewlFIFOPop= 0; /**  ewlFIFO    1-bit **/
        this-> ewlReg0En = 0; /**  ewlReg0    1-bit **/
        this-> ewlReg1En = 0; /**  ewlReg1    1-bit **/
        this->xexlFIFOPop= 0; /** xexlFIFO    1-bit **/
        this->xexlReg0En = 0; /**  ewlReg0    1-bit **/
        this->xexlReg1En = 0; /**  ewlReg1    1-bit **/
        this->oFIFOPush  = 0;
        this->xReg1Out   = 0;
        return;
    }
    inline void WPush(){
        this-> ewlFIFOPop= 1; /**  ewlFIFO    1-bit **/
        this-> ewlReg0En = 1; /**  ewlReg0    1-bit **/
        this-> ewlReg1En = 1; /**  ewlReg1    1-bit **/
        return;
    }
    inline void WNotPush(){
        this-> ewlFIFOPop= 0; /**  ewlFIFO    1-bit **/
        this-> ewlReg0En = 0; /**  ewlReg0    1-bit **/
        this-> ewlReg1En = 0; /**  ewlReg1    1-bit **/
        return;
    }
    inline void XPush(){
        this->xexlFIFOPop= 1; /** xexlFIFO    1-bit **/
        this->xexlReg0En = 1; /**  ewlReg0    1-bit **/
        this->xexlReg1En = 1; /**  ewlReg1    1-bit **/
        return;
    }
    inline void XNotPush(){
        this->xexlFIFOPop= 0; /** xexlFIFO    1-bit **/
        this->xexlReg0En = 0; /**  ewlReg0    1-bit **/
        this->xexlReg1En = 0; /**  ewlReg1    1-bit **/
        return;
    }
    inline void OutputZero(){
        this->oFIFOPush  = 1;
        this->xReg1Out   = 0;
        return;
    }
    inline void OutputX(){
        this->oFIFOPush  = this->validReg1();
        this->xReg1Out   = this->xReg1();
        return;
    }
    inline void NotOutput(){
        this->oFIFOPush  = 0;
        this->xReg1Out   = 0;
        return;
    }
    /** for state transfer **/
    inline void SetNextState(
        PortDataType wl,bool eow,
        PortDataType xl,bool eox){
        if (this->state == RU_STATE::WL_OX_END
         || this->state == RU_STATE::WL_OZ_END
         || this->state == RU_STATE::XL_OX_END
         || this->state == RU_STATE::XL_OZ_END
         || this->state == RU_STATE::WL_END
         || this->state == RU_STATE::XL_END){
            if(eow && eox){
                if (this->state == RU_STATE::XL_OZ_END
                 || this->state == RU_STATE::XL_OX_END
                 || this->state == RU_STATE::XL_END){
                    if (wl == xl){
                        this->state = RU_STATE::WX_OX_END;
                    }
                    else if (wl > xl){
                        this->state = RU_STATE::WX_OZ_END;
                    }
                    else{
                        this->state = RU_STATE::WX_END;
                    }
                }
                else
                    this->state  = RU_STATE::WX_END;
            }
            else
            if (this->state == RU_STATE::WL_OX_END
             || this->state == RU_STATE::WL_OZ_END
             || this->state == RU_STATE::WL_END){
                this->state  = RU_STATE::WL_END;
            }
            else
                this->state  = RU_STATE::XL_END;
        }
        else{
            if (wl == xl){
                if (!eow &&!eox)
                    this->state = RU_STATE::WL_EQUAL_XL;
                else if (!eow && eox)
                    this->state = RU_STATE::XL_OX_END;
                else if ( eow &&!eox)
                    this->state = RU_STATE::WL_OX_END;
                else
                    this->state = RU_STATE::WX_OX_END;
            }
            else if (wl < xl){
                if (!eow)
                    this->state = RU_STATE::WL_LESS_XL;
                else if ( eow &&!eox)
                    this->state = RU_STATE::WL_OZ_END;
                else
                    this->state = RU_STATE::WX_OZ_END;
            }
            else{///wl > xl
                if (!eox)
                    this->state = RU_STATE::WL_BIGGER_XL;
                else if (!eow && eox)
                    this->state = RU_STATE::XL_OZ_END;
                else
                    this->state = RU_STATE::WX_OZ_END;
            }
        }
        return;
    }
    /** current states handles **/
    inline bool bufFIFOIsEmpty(){
        return (this->bufFIFOEmpty.Data()==1);
    }
    inline bool ewlFIFOIsFull(){
        return (this->ewlFIFOFull. Data()==1);
    }
    inline bool ewlFIFOIsEmpty(){
        return (this->ewlFIFOEmpty.Data()==1);
    }
    inline bool xexlFIFOIsFull(){
        return (this->xexlFIFOFull. Data()==1);
    }
    inline bool xexlFIFOIsEmpty(){
        return (this->xexlFIFOEmpty.Data()==1);
    }
    inline bool oFIFOIsFull(){
        return (this->oFIFOFull.Data()==1);
    }
    inline bool DownBufFull(){
        return (this->ports[PORT_INDEX(BufFullDown)].Data()==1);
    }
    inline bool ThisBuffEmpty(){
        return this->bufFIFOEmpty.Data()==1;
    }

public:
    /** ---------- up -------- **/
    static const RU::PortName BufPushUp;  /**    1-bit  in  **/
    static const RU::PortName BufInUp;    /** 1+WL_BIT  in  **/
    static const RU::PortName BufFullUp;  /**    1-bit  out **/
    /** --------- down ------- **/
    static const RU::PortName BufPushDown;/**    1-bit  out **/
    static const RU::PortName BufInDown;  /** 1+WL_BIT  out **/
    static const RU::PortName BufFullDown;/**    1-bit  in  **/
    /** --------- left ------- **/
    static const RU::PortName XELIn;    /** 2+X_BIT+XL_BIT  in  **/
    static const RU::PortName XELPush;  /** 1-bit           in  **/
    static const RU::PortName XELFull;  /** 1-bit           out **/
    /** -------- right ------- **/
    static const RU::PortName OOut;     /** X_BIT   out **/
    static const RU::PortName OPop;     /** 1-bit   in  **/
    static const RU::PortName OEmpty;   /** 1-bit   out **/

    RU( PortDataType  bufFIFOPop  = 0, /**  bufFIFO    1-bit **/
        PortDataType    oFIFOPush = 0, /**    oFIFO    1-bit **/
        PortDataType  ewlFIFOPop  = 0, /**  ewlFIFO    1-bit **/
        PortDataType  ewlFIFOPush = 0, /**  ewlFIFO    1-bit **/
        PortDataType xexlFIFOPop  = 0, /** xexlFIFO    1-bit **/
        PortDataType   ewlReg0En  = 0, /**  ewlReg0    1-bit **/
        PortDataType   ewlReg1En  = 0, /**  ewlReg1    1-bit **/
        PortDataType  xexlReg0En  = 0, /** xexlReg0    1-bit **/
        PortDataType  xexlReg1En  = 0, /** xexlReg1    1-bit **/
        PortDataType     xReg1Out = 0, /** xexlReg1    X_BIT  X_BIT of xexlout **/
        PortDataType  bufPushDown = 0, /** BufPushDown    1-bit  out **/
        enum RU_STATE state = RU_STATE::WL_EQUAL_XL
       ):  oFIFO(   X_BIT_WIDTH,XOUT_FIFO_LEN,ClockEdge::POS_EDGE),
         bufFIFO(1+WL_BIT_WIDTH, BUF_FIFO_LEN,ClockEdge::POS_EDGE),
         ewlFIFO(1+WL_BIT_WIDTH, EWL_FIFO_LEN,ClockEdge::POS_EDGE),
        xexlFIFO(2+XL_BIT_WIDTH
                  + X_BIT_WIDTH,XEXL_FIFO_LEN,ClockEdge::POS_EDGE),
         ewlReg0(1+WL_BIT_WIDTH,ClockEdge::POS_EDGE), ///       1+WL_BIT
         ewlReg1(1+WL_BIT_WIDTH,ClockEdge::POS_EDGE), ///       1+WL_BIT
        xexlReg0(2+ X_BIT_WIDTH
                  +XL_BIT_WIDTH,ClockEdge::POS_EDGE), /// 2+X_BIT+XL_BIT
        xexlReg1(2+ X_BIT_WIDTH
                  +XL_BIT_WIDTH,ClockEdge::POS_EDGE), /// 2+X_BIT+XL_BIT
         bufFIFOEmpty(1,PortDirection::IN),
         ewlFIFOFull (1,PortDirection::IN),
         ewlFIFOEmpty(1,PortDirection::IN),
        xexlFIFOFull (1,PortDirection::IN),
        xexlFIFOEmpty(1,PortDirection::IN),
           oFIFOFull (1,PortDirection::IN),
         ewlReg0Out  (1
          +WL_BIT_WIDTH,PortDirection::IN),
         ewlReg1Out  (1
          +WL_BIT_WIDTH,PortDirection::IN),
        xexlReg0Out  (2
          + X_BIT_WIDTH
          +XL_BIT_WIDTH,PortDirection::IN),
        xexlReg1Out  (2
          + X_BIT_WIDTH
          +XL_BIT_WIDTH,PortDirection::IN),
         pBufFIFOPop (1,PortDirection::OUT), /**  bufFIFO    1-bit **/
           pOFIFOPush(1,PortDirection::OUT), /**    oFIFO    1-bit **/
         pEWLFIFOPop (1,PortDirection::OUT), /**  ewlFIFO    1-bit **/
         pEWLFIFOPush(1,PortDirection::OUT), /**  ewlFIFO    1-bit **/
        pXEXLFIFOPop (1,PortDirection::OUT), /** xexlFIFO    1-bit **/
          pEWLReg0En (1,PortDirection::OUT), /**  ewlReg0    1-bit **/
          pEWLReg1En (1,PortDirection::OUT), /**  ewlReg1    1-bit **/
         pXEXLReg0En (1,PortDirection::OUT), /** xexlReg0    1-bit **/
         pXEXLReg1En (1,PortDirection::OUT), /** xexlReg1    1-bit **/
            pXReg1Out(
            X_BIT_WIDTH,PortDirection::OUT)  /** xexlReg1    X_BIT  X_BIT of xexlout **/
        {
        this-> bufFIFOPop  =  bufFIFOPop ; /**  bufFIFO    1-bit **/
        this->   oFIFOPush =    oFIFOPush; /**    oFIFO    1-bit **/
        this-> ewlFIFOPop  =  ewlFIFOPop ; /**  ewlFIFO    1-bit **/
        this-> ewlFIFOPush =  ewlFIFOPush; /**  ewlFIFO    1-bit **/
        this->xexlFIFOPop  = xexlFIFOPop ; /** xexlFIFO    1-bit **/
        this-> ewlReg0En   =  ewlReg0En  ; /**  ewlReg0    1-bit **/
        this-> ewlReg1En   =  ewlReg1En  ; /**  ewlReg1    1-bit **/
        this->xexlReg0En   = xexlReg0En  ; /** xexlReg0    1-bit **/
        this->xexlReg1En   = xexlReg1En  ; /** xexlReg1    1-bit **/
        this->   xReg1Out  =    xReg1Out ; /** xexlReg1    X_BIT  X_BIT of xexlout **/
        this->bufPushDown  = bufPushDown ; /** BufPushDown    1-bit  out **/
        this->      state  = state;
        this->InitializePorts();
        this->OutputConnect();
        return;
    }
    RU(const RU& ru):
           oFIFO(   X_BIT_WIDTH,XOUT_FIFO_LEN,ClockEdge::POS_EDGE),
         bufFIFO(1+WL_BIT_WIDTH, BUF_FIFO_LEN,ClockEdge::POS_EDGE),
         ewlFIFO(1+WL_BIT_WIDTH, EWL_FIFO_LEN,ClockEdge::POS_EDGE),
        xexlFIFO(2+XL_BIT_WIDTH
                  + X_BIT_WIDTH,XEXL_FIFO_LEN,ClockEdge::POS_EDGE),
         ewlReg0(1+WL_BIT_WIDTH,ClockEdge::POS_EDGE), ///       1+WL_BIT
         ewlReg1(1+WL_BIT_WIDTH,ClockEdge::POS_EDGE), ///       1+WL_BIT
        xexlReg0(2+ X_BIT_WIDTH
                  +XL_BIT_WIDTH,ClockEdge::POS_EDGE), /// 2+X_BIT+XL_BIT
        xexlReg1(2+ X_BIT_WIDTH
                  +XL_BIT_WIDTH,ClockEdge::POS_EDGE), /// 2+X_BIT+XL_BIT
         bufFIFOEmpty(1,PortDirection::IN),
         ewlFIFOFull (1,PortDirection::IN),
         ewlFIFOEmpty(1,PortDirection::IN),
        xexlFIFOFull (1,PortDirection::IN),
        xexlFIFOEmpty(1,PortDirection::IN),
           oFIFOFull (1,PortDirection::IN),
         ewlReg0Out  (1
          +WL_BIT_WIDTH,PortDirection::IN),
         ewlReg1Out  (1
          +WL_BIT_WIDTH,PortDirection::IN),
        xexlReg0Out  (2
          + X_BIT_WIDTH
          +XL_BIT_WIDTH,PortDirection::IN),
        xexlReg1Out  (2
          + X_BIT_WIDTH
          +XL_BIT_WIDTH,PortDirection::IN),
         pBufFIFOPop (1,PortDirection::OUT), /**  bufFIFO    1-bit **/
           pOFIFOPush(1,PortDirection::OUT), /**    oFIFO    1-bit **/
         pEWLFIFOPop (1,PortDirection::OUT), /**  ewlFIFO    1-bit **/
         pEWLFIFOPush(1,PortDirection::OUT), /**  ewlFIFO    1-bit **/
        pXEXLFIFOPop (1,PortDirection::OUT), /** xexlFIFO    1-bit **/
          pEWLReg0En (1,PortDirection::OUT), /**  ewlReg0    1-bit **/
          pEWLReg1En (1,PortDirection::OUT), /**  ewlReg1    1-bit **/
         pXEXLReg0En (1,PortDirection::OUT), /** xexlReg0    1-bit **/
         pXEXLReg1En (1,PortDirection::OUT), /** xexlReg1    1-bit **/
            pXReg1Out(
            X_BIT_WIDTH,PortDirection::OUT)  /** xexlReg1    X_BIT  X_BIT of xexlout **/{
        this-> bufFIFOPop  = ru. bufFIFOPop ; /**  bufFIFO    1-bit **/
        this->   oFIFOPush = ru.   oFIFOPush; /**    oFIFO    1-bit **/
        this-> ewlFIFOPop  = ru. ewlFIFOPop ; /**  ewlFIFO    1-bit **/
        this-> ewlFIFOPush = ru. ewlFIFOPush; /**  ewlFIFO    1-bit **/
        this->xexlFIFOPop  = ru.xexlFIFOPop ; /** xexlFIFO    1-bit **/
        this-> ewlReg0En   = ru. ewlReg0En  ; /**  ewlReg0    1-bit **/
        this-> ewlReg1En   = ru. ewlReg1En  ; /**  ewlReg1    1-bit **/
        this->xexlReg0En   = ru.xexlReg0En  ; /** xexlReg0    1-bit **/
        this->xexlReg1En   = ru.xexlReg1En  ; /** xexlReg1    1-bit **/
        this->   xReg1Out  = ru.   xReg1Out ; /** xexlReg1    X_BIT  X_BIT of xexlout **/
        this->bufPushDown  = ru.bufPushDown ; /** BufPushDown    1-bit  out **/
        this->      state  = ru.state;
        this->InitializePorts();
        this->OutputConnect();
        return;
    }
    bool OutputConnectedCorrect() const override{
        return
            /** ---------- up -------- **/
            this->bufFIFO.SameTo(
                this->ports[PORT_INDEX(BufFullUp )],
                FIFO::Full)
            /** --------- down ------- **/
         && this->ports[PORT_INDEX(BufPushDown)].SameTo(this->bufPushDown)
         && this->bufFIFO.SameTo(
                this->ports[PORT_INDEX(BufInDown)],
                FIFO::Out)
            /** --------- left ------- **/
         && this->xexlFIFO.SameTo(
                this->ports[PORT_INDEX(XELFull)],
                FIFO::Full)
            /** -------- right ------- **/
         && this->oFIFO.SameTo(
                this->ports[PORT_INDEX(OOut)],
                FIFO::Out)
         && this->oFIFO.SameTo(
                this->ports[PORT_INDEX(OEmpty)],
                FIFO::Empty)
         && this->pBufFIFOPop.SameTo(
            this-> bufFIFOPop) /**  bufFIFO    1-bit **/
         && this->pOFIFOPush.SameTo(
            this-> oFIFOPush)  /**    oFIFO    1-bit **/
         && this->pEWLFIFOPop.SameTo(
            this-> ewlFIFOPop) /**  ewlFIFO    1-bit **/
         && this->pEWLFIFOPush.SameTo(
            this-> ewlFIFOPush)/**  ewlFIFO    1-bit **/
         && this->pXEXLFIFOPop.SameTo(
            this-> xexlFIFOPop)/** xexlFIFO    1-bit **/
         && this->pEWLReg0En.SameTo(
            this-> ewlReg0En)  /**  ewlReg0    1-bit **/
         && this->pEWLReg1En.SameTo(
            this-> ewlReg1En)  /**  ewlReg1    1-bit **/
         && this->pXEXLReg0En.SameTo(
            this-> xexlReg0En) /** xexlReg0    1-bit **/
         && this->pXEXLReg1En.SameTo(
            this-> xexlReg1En) /** xexlReg1    1-bit **/
         && this->pXReg1Out.SameTo(
            this-> xReg1Out);  /** xexlReg1    X_BIT  X_BIT of xexlout **/
    }
    inline void InnerConnect() override{
        assert(this->OuterConnected());
        /******************************************
         ** connect input ports with inner logic **
         ******************************************/
        /*************************
         ** internal connection **
         *************************/
        /** ---- bufFIFO ---- **/
        this->bufFIFO.Clone(
            FIFO::In,
            this->ports[PORT_INDEX(BufInUp)]);
        this->bufFIFO.Clone(
            FIFO::Push,
            this->ports[PORT_INDEX(BufPushUp)]);
        this->bufFIFO.ConnectTo(
            FIFO::Pop,
            this->pBufFIFOPop);
        /** ---- ewlFIFO ---- **/
        this->ewlFIFO.ConnectTo(
            FIFO::In,
            this->bufFIFO,
            FIFO::Out);
        this->ewlFIFO.ConnectTo(
            FIFO::Push,
            this->pEWLFIFOPush);
        this->ewlFIFO.ConnectTo(
            FIFO::Pop,
            this->pEWLFIFOPop);
        /** ---- xexlFIFO ---- **/
        this->xexlFIFO.Clone(
            FIFO::In,
            this->ports[PORT_INDEX(XELIn)]);
        this->xexlFIFO.Clone(
            FIFO::Push,
            this->ports[PORT_INDEX(XELPush)]);
        this->xexlFIFO.ConnectTo(
            FIFO::Pop,
            this->pXEXLFIFOPop);
        /** ---- oFIFO ---- **/
        this->oFIFO.ConnectTo(
            FIFO::In,
            this->pXReg1Out);
        this->oFIFO.ConnectTo(
            FIFO::Push,
            this->pOFIFOPush);
        this->oFIFO.Clone(
            FIFO::Pop,
            this->ports[PORT_INDEX(OPop)]);
        /** ---- ewlReg0 ---- **/
        this->ewlReg0.ConnectTo(
            Reg::in,
            this->ewlFIFO,
            FIFO::Out);
        this->ewlReg0.ConnectTo(
            Reg::en,
            this->pEWLReg0En);
        /** ---- ewlReg1 ---- **/
        this->ewlReg1.ConnectTo(
            Reg::in,
            this->ewlReg0,
            Reg::out);
        this->ewlReg1.ConnectTo(
            Reg::en,
            this->pEWLReg1En);
        /** ---- xexlReg0 ---- **/
        this->xexlReg0.ConnectTo(
            Reg::in,
            this->xexlFIFO,
            FIFO::Out);
        this->xexlReg0.ConnectTo(
            Reg::en,
            this->pXEXLReg0En);
        /** ---- xexlReg1 ---- **/
        this->xexlReg1.ConnectTo(
            Reg::in,
            this->xexlReg0,
            Reg::out);
        this->xexlReg1.ConnectTo(
            Reg::en,
            this->pXEXLReg1En);
        /*****************
         ** stray ports **
         *****************/
        this->bufFIFO.ConnectTo(
            this->bufFIFOEmpty, ///   bufFIFO             1-bit  IN
            FIFO::Empty);
        this->ewlFIFO.ConnectTo(
            this->ewlFIFOFull,  ///   ewlFIFO             1-bit  IN
            FIFO::Full);
        this->ewlFIFO.ConnectTo(
            this->ewlFIFOEmpty, ///   ewlFIFO             1-bit  IN
            FIFO::Empty);
        this->xexlFIFO.ConnectTo(
            this->xexlFIFOFull, ///  xexlFIFO             1-bit  IN
            FIFO::Full);
        this->xexlFIFO.ConnectTo(
            this->xexlFIFOEmpty,///  xexlFIFO             1-bit  IN
            FIFO::Empty);
        this->oFIFO.ConnectTo(
            this->oFIFOFull,    ///  xexlFIFO             1-bit  IN
            FIFO::Full);
        this->ewlReg0.ConnectTo(
            this->ewlReg0Out,   ///   ewlReg0          1+WL_BIT  IN
            Reg::out);
        this->ewlReg1.ConnectTo(
            this->ewlReg1Out,   ///   ewlReg1          1+WL_BIT  IN
            Reg::out);
        this->xexlReg0.ConnectTo(
            this->xexlReg0Out,   ///  xexlReg0    2+X_BIT+XL_BIT  IN
            Reg::out);
        this->xexlReg1.ConnectTo(
            this->xexlReg1Out,   ///  xexlReg1    2+X_BIT+XL_BIT  IN
            Reg::out);
        assert(this->InnerConnectedCorrect());
        return;
    }
    inline bool InnerConnected() const{
        return  this->   oFIFO.Connected() ///          X_BIT   x
             && this-> bufFIFO.Connected() ///       1+WL_BIT       end-of-w w-location
             && this-> ewlFIFO.Connected() ///       1+WL_BIT       end-of-w w-location
             && this->xexlFIFO.Connected() /// 2+X_BIT+XL_BIT   x   end-of-x x-location
             && this-> ewlReg0.Connected() ///       1+WL_BIT
             && this-> ewlReg1.Connected() ///       1+WL_BIT
             && this->xexlReg0.Connected() /// 2+X_BIT+XL_BIT
             && this->xexlReg1.Connected();/// 2+X_BIT+XL_BIT
    }
    inline bool InnerConnectedCorrect() const{
        return  /*************************
                 ** internal connection **
                 *************************/
                /** ---- bufFIFO ---- **/
                this->bufFIFO.SameTo(
                    FIFO::In,
                    this->ports[PORT_INDEX(BufInUp)])
             && this->bufFIFO.SameTo(
                    FIFO::Push,
                    this->ports[PORT_INDEX(BufPushUp)])
             && this->bufFIFO.ConnectedTo(
                    FIFO::Pop,
                    this->pBufFIFOPop)
                /** ---- ewlFIFO ---- **/
             && this->ewlFIFO.ConnectedTo(
                    FIFO::In,
                    this->bufFIFO,
                    FIFO::Out)
             && this->ewlFIFO.ConnectedTo(
                    FIFO::Push,
                    this->pEWLFIFOPush)
             && this->ewlFIFO.ConnectedTo(
                    FIFO::Pop,
                    this->pEWLFIFOPop)
                /** ---- xexlFIFO ---- **/
             && this->xexlFIFO.SameTo(
                    FIFO::In,
                    this->ports[PORT_INDEX(XELIn)])
             && this->xexlFIFO.SameTo(
                    FIFO::Push,
                    this->ports[PORT_INDEX(XELPush)])
             && this->xexlFIFO.ConnectedTo(
                    FIFO::Pop,
                    this->pXEXLFIFOPop)
                /** ---- oFIFO ---- **/
             && this->oFIFO.ConnectedTo(
                    FIFO::In,
                    this->pXReg1Out)
             && this->oFIFO.ConnectedTo(
                    FIFO::Push,
                    this->pOFIFOPush)
             && this->oFIFO.SameTo(
                    FIFO::Pop,
                    this->ports[PORT_INDEX(OPop)])
                /** ---- ewlReg0 ---- **/
             && this->ewlReg0.ConnectedTo(
                    Reg::in,
                    this->ewlFIFO,
                    FIFO::Out)
             && this->ewlReg0.ConnectedTo(
                    Reg::en,
                    this->pEWLReg0En)
                /** ---- ewlReg1 ---- **/
             && this->ewlReg1.ConnectedTo(
                    Reg::in,
                    this->ewlReg0,
                    Reg::out)
             && this->ewlReg1.ConnectedTo(
                    Reg::en,
                    this->pEWLReg1En)
                /** ---- xexlReg0 ---- **/
             && this->xexlReg0.ConnectedTo(
                    Reg::in,
                    this->xexlFIFO,
                    FIFO::Out)
             && this->xexlReg0.ConnectedTo(
                    Reg::en,
                    this->pXEXLReg0En)
                /** ---- xexlReg1 ---- **/
             && this->xexlReg1.ConnectedTo(
                    Reg::in,
                    this->xexlReg0,
                    Reg::out)
             && this->xexlReg1.ConnectedTo(
                    Reg::en,
                    this->pXEXLReg1En)
                /*****************
                 ** stray ports **
                 *****************/
             && this->bufFIFO.ConnectedTo(
                    this->bufFIFOEmpty, ///   bufFIFO             1-bit  IN
                     FIFO::Empty)
             && this->ewlFIFO.ConnectedTo(
                    this->ewlFIFOFull,  ///   ewlFIFO             1-bit  IN
                     FIFO::Full)
             && this->ewlFIFO.ConnectedTo(
                    this->ewlFIFOEmpty, ///   ewlFIFO             1-bit  IN
                     FIFO::Empty)
             && this->xexlFIFO.ConnectedTo(
                    this->xexlFIFOFull, ///  xexlFIFO             1-bit  IN
                     FIFO::Full)
             && this->xexlFIFO.ConnectedTo(
                    this->xexlFIFOEmpty,///  xexlFIFO             1-bit  IN
                     FIFO::Empty)
             && this->oFIFO.ConnectedTo(
                    this->oFIFOFull,    ///  xexlFIFO             1-bit  IN
                     FIFO::Full)
             && this->ewlReg0.ConnectedTo(
                    this->ewlReg0Out,   ///   ewlReg0          1+WL_BIT  IN
                    Reg::out)
             && this->ewlReg1.ConnectedTo(
                    this->ewlReg1Out,   ///   ewlReg1          1+WL_BIT  IN
                    Reg::out)
             && this->xexlReg0.ConnectedTo(
                    this->xexlReg0Out,   ///  xexlReg0    2+X_BIT+XL_BIT  IN
                    Reg::out)
             && this->xexlReg1.ConnectedTo(
                    this->xexlReg1Out,   ///  xexlReg1    2+X_BIT+XL_BIT  IN
                    Reg::out);
    }
    inline bool atPosEdge() override{
        bool hasChange = false;
        hasChange = this->   oFIFO.atPosEdge() || hasChange; ///          X_BIT   x
        hasChange = this-> bufFIFO.atPosEdge() || hasChange; ///       1+WL_BIT       end-of-w w-location
        hasChange = this-> ewlFIFO.atPosEdge() || hasChange; ///       1+WL_BIT       end-of-w w-location
        hasChange = this->xexlFIFO.atPosEdge() || hasChange; /// 2+X_BIT+XL_BIT   x   end-of-x x-location
        hasChange = this-> ewlReg0.atPosEdge() || hasChange; ///       1+WL_BIT
        hasChange = this-> ewlReg1.atPosEdge() || hasChange; ///       1+WL_BIT
        hasChange = this->xexlReg0.atPosEdge() || hasChange; /// 2+X_BIT+XL_BIT
        hasChange = this->xexlReg1.atPosEdge() || hasChange; /// 2+X_BIT+XL_BIT
        return hasChange;
    }
    inline bool atNegEdge() override{
        assert(false);
        return false;
    }

    inline bool Run() override{
        this->   oFIFO.Run(); ///          X_BIT   x
        this-> bufFIFO.Run(); ///       1+WL_BIT       end-of-w w-location
        this-> ewlFIFO.Run(); ///       1+WL_BIT       end-of-w w-location
        this->xexlFIFO.Run(); /// 2+X_BIT+XL_BIT   x   end-of-x x-location
        this-> ewlReg0.Run(); ///       1+WL_BIT
        this-> ewlReg1.Run(); ///       1+WL_BIT
        this->xexlReg0.Run(); /// 2+X_BIT+XL_BIT
        this->xexlReg1.Run(); /// 2+X_BIT+XL_BIT
        /*****************************
         *   combinational logic 1   *
         *****************************/
        ///the forward strategy still has space for optimization
        if(!this->ThisBuffEmpty()
         &&!this->ewlFIFOIsFull()
         &&!this->  DownBufFull()){
            this->bufFIFOPop  = 1;
            this->bufPushDown = 1;
            this->ewlFIFOPush = 1;
        }
        else{
            this->bufFIFOPop  = 0;
            this->bufPushDown = 0;
            this->ewlFIFOPush = 0;
        }
        /*****************************
         *   combinational logic 0   *
         *****************************/
        switch(this->state){
        case RU_STATE::WX_END:
            if(!this-> ewlFIFOIsEmpty()
            && !this->xexlFIFOIsEmpty()
            && !this->   oFIFOIsFull ()){
                this->    WPush();
                this->    XPush();
                this->NotOutput();
                this->SetNextState(
                    this-> wlReg0(),
                    this->eowReg0(),
                    this-> xlReg0(),
                    this->eoxReg0()
                );
            }
            else
                 this->Stuck();
            break;
        case RU_STATE::WX_OX_END:
            if(!this-> ewlFIFOIsEmpty()
            && !this->xexlFIFOIsEmpty()
            && !this->   oFIFOIsFull ()){
                this->  WPush();
                this->  XPush();
                this->OutputX();
                this->SetNextState(
                    this-> wlReg0(),
                    this->eowReg0(),
                    this-> xlReg0(),
                    this->eoxReg0()
                );
            }
            else
                 this->Stuck();
            break;
        case RU_STATE::WX_OZ_END:
            if(!this-> ewlFIFOIsEmpty()
            && !this->xexlFIFOIsEmpty()
            && !this->   oFIFOIsFull ()){
                this->     WPush();
                this->     XPush();
                this->OutputZero();
                this->SetNextState(
                    this-> wlReg0(),
                    this->eowReg0(),
                    this-> xlReg0(),
                    this->eoxReg0()
                );
            }
            else
                 this->Stuck();
            break;
        case RU_STATE::WL_LESS_XL:
            if(!this-> ewlFIFOIsEmpty()
            && !this->   oFIFOIsFull ()){
                this->     WPush();
                this->  XNotPush();
                this->OutputZero();
                this->SetNextState(
                    this-> wlReg0(),
                    this->eowReg0(),
                    this-> xlReg1(),
                    this->eoxReg1()
                );
            }
            else
                 this->Stuck();
            break;
        case RU_STATE::WL_EQUAL_XL:
            if(!this-> ewlFIFOIsEmpty()
            && !this->xexlFIFOIsEmpty()
            && !this->   oFIFOIsFull ()){
                this->  WPush();
                this->  XPush();
                this->OutputX();
                this->SetNextState(
                    this-> wlReg0(),
                    this->eowReg0(),
                    this-> xlReg0(),
                    this->eoxReg0()
                );
            }
            else
                this->Stuck();
            break;
        case RU_STATE::WL_BIGGER_XL:
            if(!this->xexlFIFOIsEmpty()){
                this-> WNotPush();
                this->    XPush();
                this->NotOutput();
                this->SetNextState(
                    this-> wlReg1(),
                    this->eowReg1(),
                    this-> xlReg0(),
                    this->eoxReg0()
                );
            }
            else
                this->Stuck();
            break;
        case RU_STATE::WL_END:
            if(!this->xexlFIFOIsEmpty()){
                this-> WNotPush();
                this->    XPush();
                this->NotOutput();
                this->SetNextState(
                    this-> wlReg1(),
                    this->eowReg1(),
                    this-> xlReg0(),
                    this->eoxReg0()
                );
            }
            else
                 this->Stuck();
            break;
        case RU_STATE::WL_OX_END:
            if(!this->xexlFIFOIsEmpty()
            && !this->   oFIFOIsFull ()){
                this->WNotPush();
                this->   XPush();
                this-> OutputX();
                this->SetNextState(
                    this-> wlReg1(),
                    this->eowReg1(),
                    this-> xlReg0(),
                    this->eoxReg0()
                );
            }
            else
                 this->Stuck();
            break;
        case RU_STATE::WL_OZ_END:
            if(!this->xexlFIFOIsEmpty()
            && !this->   oFIFOIsFull ()){
                this->  WNotPush();
                this->     XPush();
                this->OutputZero();
                this->SetNextState(
                    this-> wlReg1(),
                    this->eowReg1(),
                    this-> xlReg0(),
                    this->eoxReg0()
                );
            }
            else
                 this->Stuck();
            break;
        case RU_STATE::XL_END:/// x reach the end of group
            if(!this->ewlFIFOIsEmpty()
            && !this->  oFIFOIsFull ()){
                this->       WPush();
                this->    XNotPush();
                this->  OutputZero();
                this->SetNextState(
                    this-> wlReg0(),
                    this->eowReg0(),
                    this-> xlReg1(),
                    this->eoxReg1()
                );
            }
            else
                this->Stuck();
            break;
        case RU_STATE::XL_OZ_END:/// x reach the end of group
            if(!this->ewlFIFOIsEmpty()
            && !this->  oFIFOIsFull ()){
                this->       WPush();
                this->    XNotPush();
                this->  OutputZero();
                this->SetNextState(
                    this-> wlReg0(),
                    this->eowReg0(),
                    this-> xlReg1(),
                    this->eoxReg1()
                );
            }
            else
                this->Stuck();
            break;
        case RU_STATE::XL_OX_END:/// x reach the end of group
            if(!this->ewlFIFOIsEmpty()
            && !this->   oFIFOIsFull ()){
                this->    WPush();
                this-> XNotPush();
                this->  OutputX();
                this->SetNextState(
                    this-> wlReg0(),
                    this->eowReg0(),
                    this-> xlReg1(),
                    this->eoxReg1()
                );
            }
            else
                this->Stuck();
            break;
        default:
            assert(false);
        }
        return true;
    }
    inline void Print(){
        std::cout<<"output FIFO:"<<std::endl;
        this->   oFIFO.Print(); ///          X_BIT   x
        std::cout<<"buffer FIFO:"<<std::endl;
        this-> bufFIFO.Print(); ///       1+WL_BIT       end-of-w w-location
        std::cout<<"ewl FIFO:"   <<std::endl;
        this-> ewlFIFO.Print(); ///       1+WL_BIT       end-of-w w-location
        std::cout<<"xexl FIFO:"  <<std::endl;
        this->xexlFIFO.Print(); /// 2+X_BIT+XL_BIT   x   end-of-x x-location
        /**   ewl register 0  **/
        std::cout<<" ewl Reg0:"<<std::endl;
        std::cout<<"[ wl]: "<<this-> wlReg0()<<std::endl;
        std::cout<<"[eow]: "<<this->eowReg0()<<std::endl;
//        this->ewlReg0.Print();
        /**   ewl register 1  **/
        std::cout<<" ewl Reg1:"<<std::endl;
        std::cout<<"[ wl]: "<<this-> wlReg1()<<std::endl;
        std::cout<<"[eow]: "<<this->eowReg1()<<std::endl;
//        this->ewlReg1.Print();
        /**  xexl register 0  **/
        std::cout<<"xexl Reg0:"<<std::endl;
        std::cout<<"[  x]: "<<this->  xReg0()<<std::endl;
        std::cout<<"[ xl]: "<<this-> xlReg0()<<std::endl;
        std::cout<<"[eox]: "<<this->eoxReg0()<<std::endl;
//        this->xexlReg0.Print();
        /**  xexl register 1  **/
        std::cout<<"xexl Reg1:"<<std::endl;
        std::cout<<"[  x]: "<<this->  xReg1()<<std::endl;
        std::cout<<"[ xl]: "<<this-> xlReg1()<<std::endl;
        std::cout<<"[eox]: "<<this->eoxReg1()<<std::endl;
//        this->xexlReg1.Print();
        if (this->validReg1())
            std::cout<<"x valid"<<std::endl;
        else
            std::cout<<"x not valid"<<std::endl;
        std::cout<<"RU state: ";
        switch(this->state){
        case RU_STATE:: WL_LESS_XL:
            std::cout<<"WL_LESS_XL"<<std::endl;
            break;
        case RU_STATE:: WL_EQUAL_XL:
            std::cout<<"WL_EQUAL_XL"<<std::endl;
            break;
        case RU_STATE:: WL_BIGGER_XL:
            std::cout<<"WL_BIGGER_XL"<<std::endl;
            break;
        case RU_STATE:: WL_END:
            std::cout<<"WL_END"<<std::endl;
            break;
        case RU_STATE:: WL_OX_END:
            std::cout<<"WL_OX_END"<<std::endl;
            break;
        case RU_STATE:: WL_OZ_END:
            std::cout<<"WL_OZ_END"<<std::endl;
            break;
        case RU_STATE:: XL_END:
            std::cout<<"XL_END"<<std::endl;
            break;
        case RU_STATE:: XL_OX_END:
            std::cout<<"XL_OX_END"<<std::endl;
            break;
        case RU_STATE:: XL_OZ_END:
            std::cout<<"XL_OZ_END"<<std::endl;
            break;
        case RU_STATE:: WX_END:
            std::cout<<"WX_END"<<std::endl;
            break;
        case RU_STATE:: WX_OX_END:
            std::cout<<"WX_OX_END"<<std::endl;
            break;
        case RU_STATE:: WX_OZ_END:
            std::cout<<"WX_OZ_END"<<std::endl;
            break;
        default:
            assert(false);
        }
        std::cout<<std::endl;
        return;
    }
};
#endif //__COMPONENT_RU_HPP
