#ifndef __COMPONENT_CE_HPP
#define __COMPONENT_CE_HPP

#include "../base/Object.hpp"
#include "RU.hpp"

#include "../resource/BaseSequLogic.hpp"

class CE: public ClockedObject{
public:
    enum class CE_CTRL{
        RAB   = 0,  /// read from asyn FIFO, broadcast
        RAW   = 1,  /// read from asyn FIFO, write to sync FIFO,
        RAWB  = 2,  /// read from asyn FIFO, write to sync FIFO, broadcast
        RAWBZ = 3,  /// read from asyn FIFO, write to sync FIFO, broadcast zero
        RSB   = 4,  /// read from sync FIFO, broadcast
        RSBZ  = 5,  /// read from sync FIFO, broadcast zero
        RSWB  = 6,  /// read from sync FIFO, write to sync FIFO, broadcast
        RSWBZ = 7,  /// read from sync FIFO, write to sync FIFO, broadcast zero
        BZ    = 8,  /// broadcast zero
        NONE  = 9   /// idle, initialize state
    };
    static const enum CE::CE_CTRL InsTable[];
    static const uint8_t CtrlNum;

    static constexpr const uint8_t BUF_FIFO_LEN = 16;


    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
        /** ---------- up -------- **/
            BufPopUp,    /**          1-bit  in  **/
            BufOutUp,    /** 2+X_BIT+XL_BIT  out **/
            BufEmptyUp,  /**          1-bit  out **/
        /** --------- down ------- **/
            BufPopDown,  /**          1-bit  out **/
            BufInDown,   /** 2+X_BIT+XL_BIT  in  **/
            BufEmptyDown,/**          1-bit  in  **/
        /** --------- left ------- **/
            XELIn,      /** 2+X_BIT+XL_BIT  in  **/
            XELPop,     /**          1-bit  out **/
            XELEmpty,   /**          1-bit  in  **/
        /** -------- right ------- **/
            OOut,       /** 2+X_BIT+XL_BIT  out **/
            OPush,      /**          1-bit  out **/
            OFull,      /**          1-bit  in  **/
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
    enum CE_CTRL state;

    bool hasBZ;

    bool hasNotForward;

    FIFO bufFIFO;/// X_BIT + XL_BIT + 2

    /**  data field for outer ports  **/
    PortDataType
      downFIFOPop,
      leftFIFOPop,
      xexlFIFOOut,
      xexlFIFOPush;

    /**  data field for inner component  **/
    Port pBufFIFOFull;/**          1-bit bufFIFO in  **/
    Port pBufFIFOIn  ;/** 2+X_BIT+XL_BIT bufFIFO out **/
    Port pBufFIFOPush;/**          1-bit bufFIFO out **/

    PortDataType
       bufFIFOIn,
       bufFIFOPush;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(CE::PortName::PortNum());
        /** ---------- up -------- **/
        this->ports.push_back(Port(1,PortDirection::IN ));/** BufPopUp            1-bit in   **/
        this->ports.push_back(Port(2
                  + RU:: X_BIT_WIDTH
                  + RU::XL_BIT_WIDTH,PortDirection::OUT));/** BufOutUp   2+X_BIT+XL_BIT out  **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** BufEmptyUp          1-bit out  **/
        /** --------- down ------- **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** BufPopDown          1-bit out  **/
        this->ports.push_back(Port(2
                  + RU:: X_BIT_WIDTH
                  + RU::XL_BIT_WIDTH,PortDirection::IN ));/** BufInDown  2+X_BIT+XL_BIT in   **/
        this->ports.push_back(Port(1,PortDirection::IN ));/** BufEmptyDown        1-bit in   **/
        /** --------- left ------- **/
        this->ports.push_back(Port(2
                  + RU:: X_BIT_WIDTH
                  + RU::XL_BIT_WIDTH,PortDirection::IN ));/** XELIn    2+X_BIT+XL_BIT  in  **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** XELPop            1-bit  out **/
        this->ports.push_back(Port(1,PortDirection::IN ));/** XELEmpty          1-bit  in  **/
        /** -------- right ------- **/
        this->ports.push_back(Port(2
                  + RU:: X_BIT_WIDTH
                  + RU::XL_BIT_WIDTH,PortDirection::OUT));/** OOut  2+X_BIT+XL_BIT   out **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** OPush          1-bit   out **/
        this->ports.push_back(Port(1,PortDirection::IN ));/** OFull          1-bit   in  **/
        assert(this->ports.size()==CE::PortName::PortNum());
        return;
    }

    inline void OutputConnect(){
        /** --------- up ------- **/
        this->bufFIFO.Clone(
            this->ports[PORT_INDEX(BufOutUp)],
            FIFO::Out
        );
        this->bufFIFO.Clone(
            this->ports[PORT_INDEX(BufEmptyUp)],
            FIFO::Empty
        );
        /** --------- down ------- **/
        this->ports[PORT_INDEX(BufPopDown)].Clone(
            this->downFIFOPop
        );
        /** --------- left ------- **/
        this->ports[PORT_INDEX(XELPop)].Clone(
            this->leftFIFOPop
        );
        /** --------- right ------- **/
        this->ports[PORT_INDEX(OPush)].Clone(
            this->xexlFIFOPush
        );
        this->ports[PORT_INDEX(OOut)].Clone(
            this->xexlFIFOOut
        );
        assert(this->OutputConnected());
        return;
    }

    inline static enum CE_CTRL Decode(PortDataType portData){
        assert((portData>>1) < CE::CtrlNum
            &&((portData& 1) == 0));
        return CE::InsTable[portData>>1];
    }
    /** current states handles **/
    inline bool UpBufFull(){
        return (this->pBufFIFOFull.Data());
    }
    inline bool         LeftBufEmpty(){
        return (this->ports[PORT_INDEX(XELEmpty)].Data()==1);
    }
    inline PortDataType LeftBufIn(){
        return (this->ports[PORT_INDEX(XELIn   )].Data());
    }
    inline bool         LeftSentCtrl(){
        return (this->ports[PORT_INDEX(XELIn)].Data() & 1)==0;
    }
    inline bool         LeftIsEOX(){
        return (RU::DecompressEOX(this->ports[PORT_INDEX(XELIn    )].Data())==1);
    }
    inline bool         DownIsEOX(){
        return (RU::DecompressEOX(this->ports[PORT_INDEX(BufInDown)].Data())==1);
    }
    inline PortDataType DownBufData(){
        return (this->ports[PORT_INDEX(BufInDown   )].Data());
    }
    inline bool         DownBufEmpty(){
        return (this->ports[PORT_INDEX(BufEmptyDown)].Data()==1);
    }
    inline bool RightBufFull(){
        return (this->ports[PORT_INDEX(OFull   )].Data()==1);
    }
    /** action handles **/
    inline void  LeftBufPop (){
        this->leftFIFOPop = 1;
        return;
    }
    inline void  LeftBufNotPop (){
        this->leftFIFOPop = 0;
        return;
    }
    inline void  DownBufPop (){
        this->downFIFOPop = 1;
        this->ports[CE::BufInDown.ToIndex()].Read();
        return;
    }
    inline void  DownBufNotPop (){
        this->downFIFOPop = 0;
        return;
    }
    inline void RightBufPushZero(){
        if(!this->hasBZ){
            this->hasBZ = true;
            this->xexlFIFOOut  = RU::CompressXEXL( 0,1, 1,1);
            this->xexlFIFOPush = 1;
        }
        else{
            this->xexlFIFOOut  = 0;
            this->xexlFIFOPush = 0;
        }
        return;
    }
    inline void RightBufPushAsyn(){
        this->xexlFIFOOut  = this->ports[PORT_INDEX(XELIn)].Data();
        this->xexlFIFOPush = 1;
        return;
    }
    inline void RightBufPushSync(){
        this->xexlFIFOOut  = this->ports[PORT_INDEX(BufInDown)].Data();
        this->xexlFIFOPush = 1;
        return;
    }
    inline void RightBufNotPush(){
        this->xexlFIFOOut  = 0;
        this->xexlFIFOPush = 0;
        return;
    }
    inline void    UpBufPushAsyn(){
        this->bufFIFOIn   = this->ports[PORT_INDEX(XELIn)].Data();
        this->bufFIFOPush = 1;
        return;
    }
    inline void    UpBufPushSync(){
        this->bufFIFOIn   = this->ports[PORT_INDEX(BufInDown)].Data();
        this->bufFIFOPush = 1;
        return;
    }
    inline void    UpBufNotPush(){
        this->bufFIFOIn   = 0;
        this->bufFIFOPush = 0;
        return;
    }
    inline void DoNothing(){
        this->   UpBufNotPush();
        this->RightBufNotPush();
        this->DownBufNotPop();
        this->LeftBufNotPop();
        return;
    }

public:

    /** ---------- up -------- **/
    static const CE::PortName BufPopUp;     /**          1-bit  in  **/
    static const CE::PortName BufOutUp;     /** 2+X_BIT+XL_BIT  out **/
    static const CE::PortName BufEmptyUp;   /**          1-bit  out **/
    /** --------- down ------- **/
    static const CE::PortName BufPopDown;   /**          1-bit  out **/
    static const CE::PortName BufInDown;    /** 2+X_BIT+XL_BIT  in  **/
    static const CE::PortName BufEmptyDown; /**          1-bit  in  **/
    /** --------- left ------- **/
    static const CE::PortName XELIn;        /** 2+X_BIT+XL_BIT  in  **/
    static const CE::PortName XELPop;       /**          1-bit  out **/
    static const CE::PortName XELEmpty;     /**          1-bit  in  **/
    /** -------- right ------- **/
    static const CE::PortName OOut;         /** 2+X_BIT+XL_BIT  out **/
    static const CE::PortName OPush;        /**          1-bit  out **/
    static const CE::PortName OFull;        /**          1-bit  in  **/

    inline static PortDataType Encode(enum CE_CTRL ceCtrl){
        return (static_cast<PortNumType>(ceCtrl)<<1);
    }

    CE( PortDataType  downFIFOPop  = 0,
        PortDataType  leftFIFOPop  = 0,
        PortDataType  xexlFIFOOut  = 0,
        PortDataType  xexlFIFOPush = 0,
        PortDataType   bufFIFOIn   = 0,
        PortDataType   bufFIFOPush = 0,
        bool hasNotForward = true,
        enum CE_CTRL state = CE_CTRL::NONE
       ):bufFIFO       (2
        +RU:: X_BIT_WIDTH
        +RU::XL_BIT_WIDTH,CE::BUF_FIFO_LEN,
         ClockedObject::ClockEdge::POS_EDGE ),/** X_BIT + XL_BIT + 2 **/
         pBufFIFOFull  (1,PortDirection::IN ),
         pBufFIFOIn    (2
        +RU:: X_BIT_WIDTH
        +RU::XL_BIT_WIDTH,PortDirection::OUT),
         pBufFIFOPush  (1,PortDirection::OUT)
    {
        this->downFIFOPop  = downFIFOPop ;
        this->leftFIFOPop  = leftFIFOPop ;
        this->xexlFIFOOut  = xexlFIFOOut ;
        this->xexlFIFOPush = xexlFIFOPush;
        this-> bufFIFOIn   =  bufFIFOIn  ;
        this-> bufFIFOPush =  bufFIFOPush;
        this->hasNotForward= hasNotForward,
        this->state        = CE_CTRL::NONE;
        this->InitializePorts();
        this->OutputConnect  ();
        return;
    }
    CE(const CE& ce)
        :bufFIFO       (2
        +RU:: X_BIT_WIDTH
        +RU::XL_BIT_WIDTH,CE::BUF_FIFO_LEN,
         ClockedObject::ClockEdge::POS_EDGE ),/** X_BIT + XL_BIT + 2 **/
         pBufFIFOFull  (1,PortDirection::IN ),
         pBufFIFOIn    (2
        +RU:: X_BIT_WIDTH
        +RU::XL_BIT_WIDTH,PortDirection::OUT),
         pBufFIFOPush  (1,PortDirection::OUT)
    {
        this->downFIFOPop  = ce.downFIFOPop ;
        this->leftFIFOPop  = ce.leftFIFOPop ;
        this->xexlFIFOOut  = ce.xexlFIFOOut ;
        this->xexlFIFOPush = ce.xexlFIFOPush;
        this-> bufFIFOIn   = ce. bufFIFOIn  ;
        this-> bufFIFOPush = ce. bufFIFOPush;
        this->hasNotForward= ce.hasNotForward,
        this->state = ce.state;
        this->InitializePorts();
        this->OutputConnect  ();
        return;
    }

    inline bool OutputConnectedCorrect() const override{
        return
        /** --------- up ------- **/
            this->bufFIFO.SameTo(
            this->ports[PORT_INDEX(BufOutUp)],
            FIFO::Out)
         && this->bufFIFO.SameTo(
            this->ports[PORT_INDEX(BufEmptyUp)],
            FIFO::Empty)
        /** --------- down ------- **/
         && this->ports[PORT_INDEX(BufPopDown)].SameTo(
            this->downFIFOPop)
        /** --------- left ------- **/
         && this->ports[PORT_INDEX(XELPop)].SameTo(
            this->leftFIFOPop)
        /** --------- right ------- **/
         && this->ports[PORT_INDEX(OPush)].SameTo(
            this->xexlFIFOPush)
         && this->ports[PORT_INDEX(OOut)].SameTo(
            this->xexlFIFOOut);
    }
    inline bool atPosEdge() override{
        return this->bufFIFO.atPosEdge();
    }
    inline bool atNegEdge() override{
        assert(false);
        return false;
    }
    inline void InnerConnect() override{
        assert(this->InputConnected());
        this->pBufFIFOIn  .Clone(
            this->bufFIFOIn
        );/** 2+X_BIT+XL_BIT bufFIFO out **/
        this->pBufFIFOPush.Clone(
            this->bufFIFOPush
        );/**          1-bit bufFIFO out **/

        this->bufFIFO.Clone(
            FIFO::Pop,
            this->ports[CE::BufPopUp.ToIndex()]);
        this->bufFIFO.ConnectTo(
            this->pBufFIFOFull,
            FIFO::Full);
        this->bufFIFO.ConnectTo(
            FIFO::Push,
            this->pBufFIFOPush);
        this->bufFIFO.ConnectTo(
            FIFO::In,
            this->pBufFIFOIn);
        assert(this->InnerConnectedCorrect());
        return;
    }

    inline bool InnerConnected() const override{
        return this->bufFIFO.Connected();
    }

    inline bool InnerConnectedCorrect() const override{
        return
            this->pBufFIFOIn  .SameTo(
            this->bufFIFOIn  )/** 2+X_BIT+XL_BIT bufFIFO out **/
         && this->pBufFIFOPush.SameTo(
            this->bufFIFOPush)/**          1-bit bufFIFO out **/
         && this->bufFIFO.SameTo(
            FIFO::Pop,
            this->ports[CE::BufPopUp.ToIndex()])
         && this->bufFIFO.ConnectedTo(
            this->pBufFIFOFull,
            FIFO::Full)
         && this->bufFIFO.ConnectedTo(
            FIFO::Push,
            this->pBufFIFOPush)
         && this->bufFIFO.ConnectedTo(
            FIFO::In,
            this->pBufFIFOIn);
    }


    inline bool Run(){
        this->bufFIFO.Run();
        switch(this->state){
        case CE_CTRL::RAB  : /// read from asyn FIFO, broadcast
            if (this->RightBufFull ()
             || this-> LeftBufEmpty()){
                this->DoNothing();
            }
            else{
                this-> LeftBufPop     ();
                this-> DownBufNotPop  ();
                this->RightBufPushAsyn();
                this->   UpBufNotPush ();
                if (this->LeftIsEOX())
                    this->state = CE_CTRL::NONE;
            }
            break;
        case CE_CTRL::RAW  : /// read from asyn FIFO, write to sync FIFO
            if (this->  UpBufFull ()
             || this->LeftBufEmpty()){
                this->DoNothing();
            }
            else{
                this-> LeftBufPop     ();
                this-> DownBufNotPop  ();
                this->RightBufNotPush ();
                this->   UpBufPushAsyn();
                if (this->LeftIsEOX())
                    this->state = CE_CTRL::NONE;
            }
            break;
        case CE_CTRL::RAWB : /// read from asyn FIFO, write to sync FIFO, broadcast
            if (this->RightBufFull()
             || this->LeftBufEmpty()){
                this->DoNothing();
            }
            else{
                if (this->UpBufFull()){
                    if (this->hasNotForward){
                        this->hasNotForward = false;
                        this-> LeftBufNotPop  ();
                        this-> DownBufNotPop  ();
                        this->RightBufPushAsyn();
                        this->   UpBufNotPush ();
                    }
                    else
                        this->DoNothing();
                }
                else{
                    this-> LeftBufPop     ();
                    this-> DownBufNotPop  ();
                    if (this->hasNotForward)
                        this->RightBufPushAsyn();
                    else{
                        this->hasNotForward = true;
                        this->RightBufNotPush();
                    }
                    this->   UpBufPushAsyn();
                    if (this->LeftIsEOX())
                        this->state = CE_CTRL::NONE;
                }
            }
            break;
        case CE_CTRL::RAWBZ: /// read from asyn FIFO, write to sync FIFO, broadcast zero
            if (this->RightBufFull()
             || this->LeftBufEmpty()){
                this->DoNothing();
            }
            else{
                if (this->UpBufFull()){
                    if (this->hasNotForward){
                        this->hasNotForward = false;
                        this-> LeftBufNotPop  ();
                        this-> DownBufNotPop  ();
                        this->RightBufPushZero();
                        this->   UpBufNotPush ();
                    }
                    else
                        this->DoNothing();
                }
                else{
                    this-> LeftBufPop     ();
                    this-> DownBufNotPop  ();
                    if (this->hasNotForward)
                        this->RightBufPushZero();
                    else{
                        this->hasNotForward = true;
                        this->RightBufNotPush();
                    }
                    this->   UpBufPushAsyn();
                    if (this->LeftIsEOX())
                        this->state = CE_CTRL::NONE;
                }
            }
            break;
        case CE_CTRL::RSB  : /// read from sync FIFO, broadcast
            if (this->RightBufFull()
             || this->DownBufEmpty()){
                this->DoNothing();
            }
            else{
                this-> LeftBufNotPop  ();
                this-> DownBufPop     ();
                this->RightBufPushSync();
                this->   UpBufNotPush ();
                if (this->DownIsEOX())
                    this->state = CE_CTRL::NONE;
            }
            break;
        case CE_CTRL::RSBZ : /// read from sync FIFO, broadcast zero
            if (this->RightBufFull()
             || this->DownBufEmpty()){
                this->DoNothing();
            }
            else{
                this-> LeftBufNotPop  ();
                this-> DownBufPop     ();
                this->RightBufPushZero();
                this->   UpBufNotPush ();
                if (this->DownIsEOX())
                    this->state = CE_CTRL::NONE;
            }
            break;
        case CE_CTRL::RSWB : /// read from sync FIFO, write to sync FIFO, broadcast
            if (this->RightBufFull()
             || this->DownBufEmpty()){
                this->DoNothing();
            }
            else{
                if (this->UpBufFull()){
                    if (this->hasNotForward){
                        this->hasNotForward = false;
                        this-> LeftBufNotPop  ();
                        this-> DownBufNotPop  ();
                        this->RightBufPushSync();
                        this->   UpBufNotPush ();
                    }
                    else
                        this->DoNothing();
                }
                else{
                    this-> LeftBufNotPop  ();
                    this-> DownBufPop     ();
                    if (this->hasNotForward)
                        this->RightBufPushSync();
                    else{
                        this->hasNotForward = true;
                        this->RightBufNotPush();
                    }
                    this->   UpBufPushSync();
                    if (this->DownIsEOX())
                        this->state = CE_CTRL::NONE;
                }
            }
            break;
        case CE_CTRL::RSWBZ: /// read from sync FIFO, write to sync FIFO, broadcast zero
            if (this->RightBufFull()
             || this-> DownBufEmpty()){
                this-> DoNothing();
            }
            else{
                if (this->UpBufFull()){
                    if (this->hasNotForward){
                        this->hasNotForward = false;
                        this-> LeftBufNotPop  ();
                        this-> DownBufNotPop  ();
                        this->RightBufPushZero();
                        this->   UpBufNotPush ();
                    }
                    else
                        this->DoNothing();
                }
                else{
                    this-> LeftBufNotPop  ();
                    this-> DownBufPop     ();
                    if (this->hasNotForward)
                        this->RightBufPushZero();
                    else{
                        this->hasNotForward = true;
                        this->RightBufNotPush();
                    }
                    this->   UpBufPushSync();
                    if (this->DownIsEOX())
                        this->state = CE_CTRL::NONE;
                }
            }
            break;
        case CE_CTRL::BZ   : /// broadcast zero
            ///wenzhi: ToDo: add forward operation
            if (this->RightBufFull()){
                this->DoNothing();
            }
            else{
                this-> LeftBufNotPop  ();
                this-> DownBufNotPop  ();
                this->RightBufPushZero();
                this->   UpBufNotPush ();
                this->state = CE_CTRL::NONE;
            }
            break;
        case CE_CTRL::NONE : /// idle & initialize state
            this->hasBZ = false;
            if (this->LeftBufEmpty()){
                this->DoNothing();
            }
            else{
                assert(this->LeftSentCtrl());
                this->state = CE::Decode(this->LeftBufIn());
                this-> LeftBufPop ();
                this-> DownBufNotPop ();
                this->RightBufNotPush();
                this->   UpBufNotPush();
            }
            break;
        default:
            assert(false);
        }
//        switch(this->state){
//        case CE_CTRL::RAB  : /// read from asyn FIFO, broadcast
//            if (this->RightBufFull ()
//             || this-> LeftBufEmpty()){
//                this->DoNothing();
//            }
//            else{
//                this-> LeftBufPop     ();
//                this-> DownBufNotPop  ();
//                this->RightBufPushAsyn();
//                this->   UpBufNotPush ();
//                if (this->LeftIsEOX())
//                    this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::RAW  : /// read from asyn FIFO, write to sync FIFO
//            if (this->  UpBufFull ()
//             || this->LeftBufEmpty()){
//                this->DoNothing();
//            }
//            else{
//                this-> LeftBufPop     ();
//                this-> DownBufNotPop  ();
//                this->RightBufNotPush ();
//                this->   UpBufPushAsyn();
//                if (this->LeftIsEOX())
//                    this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::RAWB : /// read from asyn FIFO, write to sync FIFO, broadcast
//            if (this->RightBufFull()
//             || this->   UpBufFull()
//             || this->LeftBufEmpty()){
//                this->DoNothing();
//            }
//            else{
//                this-> LeftBufPop     ();
//                this-> DownBufNotPop  ();
//                this->RightBufPushAsyn();
//                this->   UpBufPushAsyn();
//                if (this->LeftIsEOX())
//                    this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::RAWBZ: /// read from asyn FIFO, write to sync FIFO, broadcast zero
//            if (this->RightBufFull()
//             || this->   UpBufFull()
//             || this->LeftBufEmpty()){
//                this->DoNothing();
//            }
//            else{
//                this-> LeftBufPop     ();
//                this-> DownBufNotPop  ();
//                this->RightBufPushZero();
//                this->   UpBufPushAsyn();
//                if (this->LeftIsEOX())
//                    this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::RSB  : /// read from sync FIFO, broadcast
//            if (this->RightBufFull()
//             || this->DownBufEmpty()){
//                this->DoNothing();
//            }
//            else{
//                this-> LeftBufNotPop  ();
//                this-> DownBufPop     ();
//                this->RightBufPushSync();
//                this->   UpBufNotPush ();
//                if (this->DownIsEOX())
//                    this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::RSBZ : /// read from sync FIFO, broadcast zero
//            if (this->RightBufFull()
//             || this->DownBufEmpty()){
//                this->DoNothing();
//            }
//            else{
//                this-> LeftBufNotPop  ();
//                this-> DownBufPop     ();
//                this->RightBufPushZero();
//                this->   UpBufNotPush ();
//                if (this->DownIsEOX())
//                    this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::RSWB : /// read from sync FIFO, write to sync FIFO, broadcast
//            if (this->RightBufFull()
//             || this->UpBufFull()
//             || this->DownBufEmpty()){
//                this->DoNothing();
//            }
//            else{
//                this-> LeftBufNotPop  ();
//                this-> DownBufPop     ();
//                this->RightBufPushSync();
//                this->   UpBufPushSync();
//                if (this->DownIsEOX())
//                    this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::RSWBZ: /// read from sync FIFO, write to sync FIFO, broadcast zero
//            if (this->RightBufFull()
//             || this->   UpBufFull()
//             || this-> DownBufEmpty()){
//                this-> DoNothing();
//            }
//            else{
//                this-> LeftBufNotPop  ();
//                this-> DownBufPop     ();
//                this->RightBufPushZero();
//                this->   UpBufPushSync();
//                if (this->DownIsEOX())
//                    this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::BZ   : /// broadcast zero
//            ///wenzhi: ToDo: add forward operation
//            if (this->RightBufFull()){
//                this->DoNothing();
//            }
//            else{
//                this-> LeftBufNotPop  ();
//                this-> DownBufNotPop  ();
//                this->RightBufPushZero();
//                this->   UpBufNotPush ();
//                this->state = CE_CTRL::NONE;
//            }
//            break;
//        case CE_CTRL::NONE : /// idle & initialize state
//            this->hasBZ = false;
//            if (this->LeftBufEmpty()){
//                this->DoNothing();
//            }
//            else{
//                assert(this->LeftSentCtrl());
//                this->state = CE::Decode(this->LeftBufIn());
//                this-> LeftBufPop ();
//                this-> DownBufNotPop ();
//                this->RightBufNotPush();
//                this->   UpBufNotPush();
//            }
//            break;
//        default:
//            assert(false);
//        }
        return true;
    }
    inline void Print(){
        /** ---------- up -------- **/
        std::cout<<"BufPopUp    :"
                 <<this->ports[PORT_INDEX(BufPopUp)].Data()    /**          1-bit  in  **/
                 <<std::endl;
        /** --------- down ------- **/
        std::cout<<"BufInDown   :"
                 <<this->ports[PORT_INDEX(BufInDown)].Data()   /** 2+X_BIT+XL_BIT  in  **/
                 <<std::endl;
        std::cout<<"BufEmptyDown:"
                 <<this->ports[PORT_INDEX(BufEmptyDown)].Data()/**          1-bit  in  **/
                 <<std::endl;
        /** --------- left ------- **/
        std::cout<<"XELIn       :"
                 <<this->ports[PORT_INDEX(XELIn)].Data()      /** 2+X_BIT+XL_BIT  in  **/
                 <<std::endl;
        std::cout<<"XELEmpty    :"
                 <<this->ports[PORT_INDEX(XELEmpty)].Data()   /**          1-bit  in  **/
                 <<std::endl;
        /** -------- right ------- **/
        std::cout<<"OFull       :"
                 <<this->ports[PORT_INDEX(OFull)].Data()      /**          1-bit  in  **/
                 <<std::endl;

        /** ---------- up -------- **/
//            BufOutUp,    /** 2+X_BIT+XL_BIT  out **/
//            BufEmptyUp,  /**          1-bit  out **/
        /** --------- down ------- **/
        std::cout<<"BufPopDown  :"
                 <<downFIFOPop
                 <<std::endl;
        /** --------- left ------- **/
        std::cout<<"XELPop      :"
                 <<leftFIFOPop
                 <<std::endl;
        /** -------- right ------- **/
        std::cout<<"OOut        :"
                 <<xexlFIFOOut
                 <<std::endl;
        std::cout<<"OPush       :"
                 <<xexlFIFOPush
                 <<std::endl;

        std::cout<<"bufFIFO:"<<std::endl;
        this->bufFIFO.Print();


        switch(this->state){
        case   CE_CTRL::RAB  : /// read from asyn FIFO, broadcast
            std::cout<<"RAB"<<std::endl;
            break;
        case   CE_CTRL::RAW  : /// read from asyn FIFO, write to sync FIFO
            std::cout<<"RAW"<<std::endl;
            break;
        case   CE_CTRL::RAWB : /// read from asyn FIFO, write to sync FIFO, broadcast
            std::cout<<"RAWB"<<std::endl;
            break;
        case   CE_CTRL::RAWBZ: /// read from asyn FIFO, write to sync FIFO, broadcast zero
            std::cout<<"RAWBZ"<<std::endl;
            break;
        case   CE_CTRL::RSB  : /// read from sync FIFO, broadcast
            std::cout<<"RSB"<<std::endl;
            break;
        case   CE_CTRL::RSBZ : /// read from sync FIFO, broadcast zero
            std::cout<<"RSBZ"<<std::endl;
            break;
        case   CE_CTRL::RSWB : /// read from sync FIFO, write to sync FIFO, broadcast
            std::cout<<"RSWB"<<std::endl;
            break;
        case   CE_CTRL::RSWBZ: /// read from sync FIFO, write to sync FIFO, broadcast zero
            std::cout<<"RSWBZ"<<std::endl;
            break;
        case   CE_CTRL::BZ   : /// broadcast zero
            ///wenzhi: ToDo: add forward operation
            std::cout<<"BZ"<<std::endl;
            break;
        case   CE_CTRL::NONE : /// idle & initialize state
            std::cout<<"NONE"<<std::endl;
            break;
        default:
            assert(false);
        }
        std::cout<<std::endl;
        return;
    }
};

#endif // __COMPONENT_CE_HPP
