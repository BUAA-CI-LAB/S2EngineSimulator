#ifndef __SYSTEM_RU_ARRAY_HPP
#define __SYSTEM_RU_ARRAY_HPP

#include "../component/RU.hpp"
#include "../component/CE.hpp"

class RUArray: public ClockedObject{
private:
    typedef uint32_t   SerialType;

    typedef uint32_t RUSizeType;
    typedef uint32_t CESizeType;
    typedef CESizeType HeightType;

    vector<RU> ru;
    vector<CE> ce;

    HeightType height;

    static PortDataType _high;
    static PortDataType _low;

    const static Object::Port _High;
    const static Object::Port _Low;
    const static Object::Port _Low_2_X_XL;

    class PortName:public BasePortName{
    private:
        static SerialType serialCount;
        SerialType serial;

    public:
        ///wenzhi: need to make the port serial allocation process safer
        PortName(CE::PortName::_PortName _portName){
            this->serial = PortName::serialCount++;
            return;
        }
        PortName(RU::PortName::_PortName _portName){
            this->serial = PortName::serialCount++;
            return;
        }
        PortName(CE::PortName::_PortName _portName,bool notAllocSerial){
            assert(notAllocSerial);
            return;
        }
        PortName(RU::PortName::_PortName _portName,bool notAllocSerial){
            assert(notAllocSerial);
            return;
        }
        PortName(const PortName& portName){
            this->serial = PortName::serialCount++;
            return;
        }
        static inline PortNumType Number(){
            return static_cast<PortNumType>(PortName::serialCount);
        }
        PortNumType ToIndex() const override{
            return this->serial;
        }
        static PortNumType ToIndex(PortName portName){
            return portName.serial;
        }
        static void ResetCount(){
            PortName::serialCount = 0;
            return;
        }
    };

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(RUArray::PortName::Number());
        /** ---------- up -------- **/
        /// the up ports of reorder units
        this->ports.push_back(Port(1,PortDirection::IN ));  /** BufPushUp    1-bit  in  **/
        this->ports.push_back(Port(1
                  + RU::WL_BIT_WIDTH,PortDirection::IN ));  /** BufInUp   1+WL_BIT  in  **/
        this->ports.push_back(Port(1,PortDirection::OUT));  /** BufFullUp    1-bit  out **/
        /** --------- down ------- **/
        /// none
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<this->height;i++){
            ///  XELInList
            this->ports.push_back(
                Port(2+RU:: X_BIT_WIDTH+
                       RU::XL_BIT_WIDTH,
                       PortDirection::IN )
            );/** 2+X_BIT+XL_BIT  in  **/
        }
        for (int i=0;i<this->height;i++){
            ///  XELPopList
            this->ports.push_back(
                Port(1,PortDirection::OUT)
            );/**          1-bit  out **/
        }
        for (int i=0;i<this->height;i++){
            ///  XELEmptyList
            this->ports.push_back(
                Port(1,PortDirection::IN )
            );/**          1-bit  in  **/
        }
        /** -------- right ------- **/
        for (int i=0;i<this->height;i++){
            ///  OOutList
            this->ports.push_back(
                Port(RU::X_BIT_WIDTH,
                       PortDirection::OUT)
            );/** X_BIT   out **/
        }
        for (int i=0;i<this->height;i++){
            ///  OPopList
            this->ports.push_back(
                Port(1,PortDirection::IN )
            );/** 1-bit   in  **/
        }
        for (int i=0;i<this->height;i++){
            ///  OEmptyList
            this->ports.push_back(
                Port(1,PortDirection::OUT)
            );/** 1-bit   out **/
        }
        assert(this->ports.size()==RUArray::PortName::Number());
        return;
    }
    inline void OutputConnect() override{
        /** ---------- up -------- **/
        /// the up ports of reorder units
        this->ru[0].Clone(this->ports[this->BufFullUp.ToIndex()],RU::BufFullUp);
        /** --------- down ------- **/
        /// none
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<this->height;i++){
            ///  XELPopList
            this->ce[i].Clone(this->ports[this->XELPopList[i].ToIndex()],CE::XELPop);
        }
        /** -------- right ------- **/
        for (int i=0;i<this->height;i++){
            ///  OOutList
            this->ru[i].Clone(this->ports[this->OOutList[i].ToIndex()],RU::OOut);
        }
        for (int i=0;i<this->height;i++){
            ///  OEmptyList
            this->ru[i].Clone(this->ports[this->OEmptyList[i].ToIndex()],RU::OEmpty);
        }
        assert(this->OutputConnected());
        return;
    }

public:
    /** ---------- up -------- **/
    /// the up ports of reorder units
    const RUArray::PortName BufPushUp;  /**    1-bit  in  **/
    const RUArray::PortName BufInUp;    /** 1+WL_BIT  in  **/
    const RUArray::PortName BufFullUp;  /**    1-bit  out **/
    /** --------- down ------- **/
    /// none
    /** -------- left -------- **/
    /// CE's left ports
    const vector<PortName> XELInList;    /** 2+X_BIT+XL_BIT  in  **/
    const vector<PortName> XELPopList;   /**          1-bit  out **/
    const vector<PortName> XELEmptyList; /**          1-bit  in  **/
    /** -------- right ------- **/
    const vector<PortName> OOutList;     /** X_BIT   out **/
    const vector<PortName> OPopList;     /** 1-bit   in  **/
    const vector<PortName> OEmptyList;   /** 1-bit   out **/

    RUArray(RUSizeType ruSize,
            CESizeType ceSize):
        ru(ruSize,RU()),
        ce(ceSize,CE()),
        /** ---------- up -------- **/
        BufPushUp(RU::PortName::_PortName::BufPushUp), /**    1-bit  in  **/
        BufInUp  (RU::PortName::_PortName::BufInUp  ), /** 1+WL_BIT  in  **/
        BufFullUp(RU::PortName::_PortName::BufFullUp), /**    1-bit  out **/
        /** -------- left -------- **/
        XELInList   (ceSize,
            PortName(CE::PortName::_PortName::XELIn   ,true)),/** 2+X_BIT+XL_BIT  in  **/
        XELPopList  (ceSize,
            PortName(CE::PortName::_PortName::XELPop  ,true)),/**          1-bit  out **/
        XELEmptyList(ceSize,
            PortName(CE::PortName::_PortName::XELEmpty,true)),/**          1-bit  in  **/
        /** -------- right ------- **/
        OOutList  (ruSize,
            PortName(RU::PortName::_PortName::OOut  ,true)),/** X_BIT   out **/
        OPopList  (ruSize,
            PortName(RU::PortName::_PortName::OPop  ,true)),/** 1-bit   in  **/
        OEmptyList(ruSize,
            PortName(RU::PortName::_PortName::OEmpty,true)) /** 1-bit   out **/
    {
        assert(ruSize == ceSize);
        this->height  =  ceSize ;
        this->InitializePorts();
        this->OutputConnect  ();
        return;
    }
private:
    RUArray(const RUArray&);
    RUArray &operator=(const RUArray&);
public:

    ~RUArray(){
        RUArray::PortName::ResetCount();
        return;
    }
    void InnerConnect() override{
        assert(this->InputConnected());
        /// RU up
        this->ru[0].Clone(RU::BufPushUp,this->ports[this->BufPushUp.ToIndex()]);
        this->ru[0].Clone(RU::BufInUp  ,this->ports[this->BufInUp  .ToIndex()]);
        for (int i=1;i<this->height;i++){
            this->ru[i].ConnectTo(
                RU::BufPushUp,
                this->ru[i-1],
                RU::BufPushDown
            );
            this->ru[i].ConnectTo(
                RU::BufInUp,
                this->ru[i-1],
                RU::BufInDown
            );
        }
        /// RU down
        for (int i=0;i<this->height-1;i++){
            this->ru[i].ConnectTo(
                RU::BufFullDown,
                this->ru[i+1],
                RU::BufFullUp
            );
        }
        this->ru[this->height-1].ConnectTo(
            RU::BufFullDown,
            RUArray::_Low
        );
        /// RU left
        for (int i=0;i<this->height;i++){
            this->ru[i].ConnectTo(
                RU::XELIn,
                this->ce[i],
                CE::OOut
            );
            this->ru[i].ConnectTo(
                RU::XELPush,
                this->ce[i],
                CE::OPush
            );
        }
        /// RU right
        for (int i=0;i<this->height;i++){
            this->ru[i].Clone(
                RU::OPop,
                this->ports[this->OPopList[i].ToIndex()]
            );
        }
        /// CE up
        this->ce[0].ConnectTo(CE::BufPopUp,RUArray::_Low);
        for (int i=1;i<this->height;i++){
            this->ce[i].ConnectTo(
                CE::BufPopUp,
                this->ce[i-1],
                CE::BufPopDown
            );
        }
        /// CE down
        for (int i=1;i<this->height;i++){
            this->ce[i-1].ConnectTo(
                CE::BufInDown,
                this->ce[i],
                CE::BufOutUp
            );
            this->ce[i-1].ConnectTo(
                CE::BufEmptyDown,
                this->ce[i],
                CE::BufEmptyUp
            );
        }
        this->ce[this->height-1].ConnectTo(
            CE::BufInDown,
            this->_Low_2_X_XL
        );
        this->ce[this->height-1].ConnectTo(
            CE::BufEmptyDown,
            this->_High
        );
        /// CE left
        for (int i=0;i<this->height;i++){
            this->ce[i].Clone(
                CE::XELIn,
                this->ports[this->XELInList[i].ToIndex()]
            );
            this->ce[i].Clone(
                CE::XELEmpty,
                this->ports[this->XELEmptyList[i].ToIndex()]
            );
        }
        /// CE right
        for (int i=0;i<this->height;i++){
            this->ce[i].ConnectTo(
                CE::OFull,
                this->ru[i],
                RU::XELFull
            );
        }
        for (int i=0;i<this->height;i++){
            this->ce[i].InnerConnect();
            this->ru[i].InnerConnect();
        }
        assert(this->InnerConnected());
        return;
    }
    bool InnerConnected() const override{
        for (RUSizeType i=0;i<this->height;i++)
            if (!this->ru[i].Connected())
                return false;
        for (CESizeType i=0;i<this->height;i++)
            if (!this->ce[i].Connected())
                return false;
//        for (auto& _ru : ru)
//            if (!_ru.Connected())
//                return false;
//        for (auto& _ce : ce)
//            if (!_ce.Connected())
//                return false;
        return true;
    }
    bool InnerConnectedCorrect() const override{
        /// RU up
        if (!this->ru[0].SameTo(RU::BufPushUp,this->ports[this->BufPushUp.ToIndex()])
         || !this->ru[0].SameTo(RU::BufInUp  ,this->ports[this->BufInUp  .ToIndex()]))
            return false;
        for (int i=1;i<this->height;i++){
            if(!this->ru[i].ConnectedTo(
                    RU::BufPushUp,
                    this->ru[i-1],
                    RU::BufPushDown)
            || !this->ru[i].ConnectedTo(
                    RU::BufInUp,
                    this->ru[i-1],
                    RU::BufInDown))
                return false;
        }
        /// RU down
        for (int i=0;i<this->height-1;i++){
            if(!this->ru[i].ConnectedTo(
                RU::BufFullDown,
                this->ru[i+1],
                RU::BufFullUp))
                return false;
        }
        if(!this->ru[this->height-1].ConnectedTo(
            RU::BufFullDown,
            RUArray::_Low))
            return false;
        /// RU left
        for (int i=0;i<this->height;i++){
            if(!this->ru[i].ConnectedTo(
                RU::XELIn,
                this->ce[i],
                CE::OOut)
            || !this->ru[i].ConnectedTo(
                RU::XELPush,
                this->ce[i],
                CE::OPush))
                return false;
        }
        /// RU right
        for (int i=0;i<this->height;i++){
            if(!this->ru[i].SameTo(
                RU::OPop,
                this->ports[this->OPopList[i].ToIndex()]))
                return false;
        }
        /// CE up
        if (!this->ce[0].ConnectedTo(CE::BufPopUp,RUArray::_Low))
            return false;
        for (int i=1;i<this->height;i++){
            if(!this->ce[i].ConnectedTo(
                CE::BufPopUp,
                this->ce[i-1],
                CE::BufPopDown))
                return false;
        }
        /// CE down
        for (int i=1;i<this->height;i++){
            if(!this->ce[i-1].ConnectedTo(
                CE::BufInDown,
                this->ce[i],
                CE::BufOutUp)
            || !this->ce[i-1].ConnectedTo(
                CE::BufEmptyDown,
                this->ce[i],
                CE::BufEmptyUp))
                return false;
        }
        if(!this->ce[this->height-1].ConnectedTo(
            CE::BufInDown,
            this->_Low_2_X_XL)
        || !this->ce[this->height-1].ConnectedTo(
            CE::BufEmptyDown,
            this->_High))
            return false;
        /// CE left
        for (int i=0;i<this->height;i++){
            if(!this->ce[i].SameTo(
                CE::XELIn,
                this->ports[this->XELInList[i].ToIndex()])
            || !this->ce[i].SameTo(
                CE::XELEmpty,
                this->ports[this->XELEmptyList[i].ToIndex()]))
                return false;
        }
        /// CE right
        for (int i=0;i<this->height;i++){
            if(!this->ce[i].ConnectedTo(
                CE::OFull,
                this->ru[i],
                RU::XELFull))
                return false;
        }
        return true;
    }
    bool atPosEdge() override{
        for (int i=0;i<this->height;i++)
            this->ce[i].atPosEdge();
        for (int i=0;i<this->height;i++)
            this->ru[i].atPosEdge();
        return true;
    }
    bool atNegEdge() override{
        assert(false);
        return false;
    }
    bool Run() override{
        bool hasChanged = false;
        for (int i=this->height-1;i>=0;i--){
            hasChanged = this->ru[i].Run() || hasChanged;
            hasChanged = this->ce[i].Run() || hasChanged;
        }
        return hasChanged;
    }
    bool OutputConnectedCorrect() const override{
        /** ---------- up -------- **/
        /// the up ports of reorder units
        if (!this->ru[0].SameTo(this->ports[this->BufFullUp.ToIndex()],RU::BufFullUp))
            return false;
        /** --------- down ------- **/
        /// none
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<this->height;i++){
            ///  XELPopList
            if (!this->ce[i].SameTo(this->ports[this->XELPopList[i].ToIndex()],CE::XELPop))
                return false;
        }
        /** -------- right ------- **/
        for (int i=0;i<this->height;i++){
            ///  OOutList
            if (!this->ru[i].SameTo(this->ports[this->OOutList[i].ToIndex()],RU::OOut))
                return false;
        }
        for (int i=0;i<this->height;i++){
            ///  OEmptyList
            if (!this->ru[i].SameTo(this->ports[this->OEmptyList[i].ToIndex()],RU::OEmpty))
                return false;
        }
        return true;
    }
    inline void PrintRU(HeightType serial){
        this->ru[serial].Print();
        return;
    }
    inline void PrintCE(HeightType serial){
        this->ce[serial].Print();
        return;
    }
};

#endif // __SYSTEM_RU_ARRAY_HPP
