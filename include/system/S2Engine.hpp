#ifndef __SYSTEM_S2_ENGINE_HPP
#define __SYSTEM_S2_ENGINE_HPP

#include "PEArray.hpp"
#include "RUArray.hpp"

class S2Engine: public ClockedObject{
private:
    typedef uint32_t SerialType;

    typedef uint32_t CycleCountType;

    typedef uint32_t LenType;
    typedef uint32_t ClockFeqType;
    typedef uint32_t FIFOLengthType;

    PortDataType height;
    PortDataType width ;

    PortDataType ruAct,
                 peAct;

       RUArray ruArray;
    SynPEArray peArray;

    static PortDataType _low;

    const static Port _Low;
    const static Port _Low_S;

//    vector<FIFO> fifos;
    vector<queue<PortDataType> > fifos;

    vector<PortDataType> oPop;

    vector<PortDataType> xIn;
    /// xIn of PE Array,
    /// the front elements of each queue

    vector<Port> pOPop;
    vector<Port> pOOut;
    vector<Port> pOEmpty;
    vector<Port> pXIn;

    CycleCountType clock;
    CycleCountType ruPer;
    CycleCountType pePer;

    bool runPEArray;

    class PortName:public BasePortName{
    private:
        static SerialType serialCount;
        SerialType serial;
    public:
        enum class _PortName{
            RUAct = 0,
            PEAct = 1
        };

        ///wenzhi: need to make the port serial allocation process safer
        PortName(CE::PortName::_PortName _portName){
            this->serial = PortName::serialCount++;
            return;
        }
        PortName(RU::PortName::_PortName _portName){
            this->serial = PortName::serialCount++;
            return;
        }
        PortName(S2Engine::PortName::_PortName _portName){
            this->serial = PortName::serialCount++;
            return;
        }
        PortName(SynPE::PortName::_PortName _portName){
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
        PortName(SynPE::PortName::_PortName _portName,bool notAllocSerial){
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

    void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::Number());
        /** ##################### **/
        /** ##   Signal Port   ## **/
        /** ##################### **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** RUAct  1-bit  out  **/
        this->ports.push_back(Port(1,PortDirection::OUT));/** PEAct  1-bit  out  **/
        /** ###################### **/
        /** ##     RU Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        /// the up ports of reorder units
        this->ports.push_back(Port(1,PortDirection::IN ));  /** BufPushUp    1-bit  in  **/
        this->ports.push_back(Port(1
                  + RU::WL_BIT_WIDTH,PortDirection::IN ));  /** BufInUp   1+WL_BIT  in  **/
        this->ports.push_back(Port(1,PortDirection::OUT));  /** BufFullUp    1-bit  out **/
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
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port( 8,PortDirection::IN ));/** WIn   8-bit in  **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::IN ));/** OIn   1-bit in  **/
        /** -------- ddown ------- **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port(24,PortDirection::OUT));/** SOut 24-bit out **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::OUT));/** SEnO  1-bit out **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::OUT));/** FOut  1-bit out **/
        assert(this->ports.size()==PortName::Number());
        return;
    }

    void OutputConnect() override{
        /** ##################### **/
        /** ##   Signal Port   ## **/
        /** ##################### **/
        this->ports[this->RUAct.ToIndex()].Clone(this->ruAct);/** RUAct  1-bit  out  **/
        this->ports[this->PEAct.ToIndex()].Clone(this->peAct);/** PEAct  1-bit  out  **/
        /** ###################### **/
        /** ##     RU Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        /// the up ports of reorder units
        this->ruArray.Clone(this->ports[this->BufFullUp.ToIndex()],
                            ruArray.BufFullUp);
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<this->height;i++){
            ///  XELPopList
            this->ruArray.Clone
            (this->ports[this->XELPopList[i].ToIndex()],
                 this->ruArray.XELPopList[i]);
        }/**          1-bit  out    **/
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** -------- ddown ------- **/
        for (int i=0;i<this->width;i++){
            this->peArray.Clone(
                this->ports[this->SOutList[ 2*i ].ToIndex()],
                    this->peArray.SOutList[ 2*i ]
            ); /** SOut 24-bit out **/
            this->peArray.Clone(
                this->ports[this->SOutList[2*i+1].ToIndex()],
                    this->peArray.SOutList[2*i+1]
            ); /** SOut 24-bit out **/
        }
        for (int i=0;i<this->width;i++){
            this->peArray.Clone(
                this->ports[this->SEnOList[ 2*i ].ToIndex()],
                    this->peArray.SEnOList[ 2*i ]
            ); /** SEnO 24-bit out **/
            this->peArray.Clone(
                this->ports[this->SEnOList[
                2*i+1].ToIndex()],
                    this->peArray.SEnOList[2*i+1]
            ); /** SEnO 24-bit out **/
        }
        for (int i=0;i<this->width;i++){
            this->peArray.Clone(
                this->ports[this->FOutList[ 2*i ].ToIndex()],
                    this->peArray.FOutList[ 2*i ]
            ); /** FOut 24-bit out **/
            this->peArray.Clone(
                this->ports[this->FOutList[2*i+1].ToIndex()],
                    this->peArray.FOutList[2*i+1]
            ); /** FOut 24-bit out **/
        }
        assert(this->OutputConnected());
        return;
    }

public:
    /** ##################### **/
    /** ##   Signal Port   ## **/
    /** ##################### **/
    const PortName RUAct; /** 1-bit  out  **/
    const PortName PEAct; /** 1-bit  out  **/

    /** ###################### **/
    /** ##     RU Array     ## **/
    /** ###################### **/
    /** ---------- up -------- **/
    /// the up ports of reorder units
    const PortName BufPushUp;  /**    1-bit  in  **/
    const PortName BufInUp;    /** 1+WL_BIT  in  **/
    const PortName BufFullUp;  /**    1-bit  out **/
    /** -------- left -------- **/
    /// CE's left ports
    const vector<PortName> XELInList;    /** 2+X_BIT+XL_BIT  in  **/
    const vector<PortName> XELPopList;   /**          1-bit  out **/
    const vector<PortName> XELEmptyList; /**          1-bit  in  **/

    /** ###################### **/
    /** ##     PE Array     ## **/
    /** ###################### **/
    /** ---------- up -------- **/
    const vector<PortName> WInList;  /**  8-bit in  **/
    const vector<PortName> OInList;  /**  1-bit in  **/
    /** ------ down-down ----- **/
    const vector<PortName> SOutList; /** 24-bit out **/
    const vector<PortName> SEnOList; /**  1-bit out **/
    const vector<PortName> FOutList; /**  1-bit out **/

    S2Engine(LenType sysHeight,
             LenType sysWidth,
             LenType sysGroup,
             ClockFeqType ruPer,
             ClockFeqType pePer):
        ruArray(sysHeight,sysHeight
                         *sysGroup),
        peArray(sysHeight,sysWidth),
            oPop(sysHeight,0),
            xIn (sysHeight,0),
         pOPop  (sysHeight,Port(1,PortDirection::OUT)),
         pOOut  (sysHeight,Port(8,PortDirection::IN )),
         pOEmpty(sysHeight,Port(1,PortDirection::IN )),
         pXIn   (sysHeight,Port(8,PortDirection::OUT)),
        /** ##################### **/
        /** ##   Signal Port   ## **/
        /** ##################### **/
        RUAct(S2Engine::PortName::_PortName::RUAct), /** 1-bit  out  **/
        PEAct(S2Engine::PortName::_PortName::PEAct), /** 1-bit  out  **/
        /** ###################### **/
        /** ##     RU Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        BufPushUp(RU::PortName::_PortName::BufPushUp), /**    1-bit  in  **/
        BufInUp  (RU::PortName::_PortName::BufInUp  ), /** 1+WL_BIT  in  **/
        BufFullUp(RU::PortName::_PortName::BufFullUp), /**    1-bit  out **/
        /** -------- left -------- **/
        XELInList   (sysHeight,
            PortName(CE::PortName::_PortName::XELIn   ,true)),/** 2+X_BIT+XL_BIT  in  **/
        XELPopList  (sysHeight,
            PortName(CE::PortName::_PortName::XELPop  ,true)),/**          1-bit  out **/
        XELEmptyList(sysHeight,
            PortName(CE::PortName::_PortName::XELEmpty,true)),/**          1-bit  in  **/
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
         WInList(  sysWidth,PortName(SynPE::PortName::_PortName::WIn ,true)), /**  8-bit in  **/
         OInList(  sysWidth,PortName(SynPE::PortName::_PortName::OIn ,true)), /**  1-bit in  **/
        /** ------ down-down ----- **/
        SOutList(2*sysWidth,PortName(SynPE::PortName::_PortName::SOut,true)), /** 24-bit out **/
        SEnOList(2*sysWidth,PortName(SynPE::PortName::_PortName::SEnO,true)), /**  1-bit out **/
        FOutList(2*sysWidth,PortName(SynPE::PortName::_PortName::FOut,true))  /**  1-bit out **/
    {
        assert( sysGroup == 1 );
        this->height = sysHeight;
        this->width  = sysWidth;
        this->clock  = 0;
        this->ruAct  = 0;
        this->peAct  = 0;
        this->ruPer  = ruPer;
        this->pePer  = pePer;
        this->runPEArray = false;
        this->fifos.clear();
        this->fifos.resize(this->height);
        for (int i=0;i<this->height;i++){
            for (int j=0;j<i;j++)
                this->fifos[i].push(0);
        }
        this->InitializePorts();
        this->OutputConnect();
        return;
    }

private:
    S2Engine(const S2Engine&);
    S2Engine &operator=(const S2Engine&);

public:

    void InnerConnect() override{
        assert(this->InputConnected());
        /** ###################### **/
        /** ##     RU Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        /// the up ports of reorder units
        this->ruArray.Clone(this->ruArray.BufPushUp,
              this->ports[this->BufPushUp.ToIndex()]);
        this->ruArray.Clone(this->ruArray.BufInUp  ,
              this->ports[this->BufInUp  .ToIndex()]);
        /** -------- left -------- **/
        /// CE left
        for (int i=0;i<this->height;i++){
            this->ruArray.Clone(
                ruArray.XELInList[i],
                this->ports[this->XELInList[i].ToIndex()]
            );
            this->ruArray.Clone(
                ruArray.XELEmptyList[i],
                this->ports[this->XELEmptyList[i].ToIndex()]
            );
        }
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int w=0;w<this->width;w++){
            this->peArray.Clone(
                peArray.WInList[w],
                this->ports[this->WInList[w].ToIndex()]
            ); /** WIn  8-bit in  **/
            this->peArray.Clone(
                peArray.OInList[w],
                this->ports[this->OInList[w].ToIndex()]
            ); /** OIn  1-bit in  **/
        }

        for (int h=0;h<this->height;h++){
            /**  oPop  1-bit **/
            this->pOPop[h].Clone(this->oPop[h]);
            this->ruArray.ConnectTo(
                this->ruArray.OPopList[h],
                this->pOPop[h]
            );
            /**  oOut    8-bit out **/
            this->ruArray.ConnectTo(
                this->pOOut[h],
                this->ruArray.OOutList[h]
            );
            /**  oEmpty  8-bit in  **/
            this->ruArray.ConnectTo(
                this->pOEmpty[h],
                this->ruArray.OEmptyList[h]
            );
            /**  XIn  8-bit in  **/
            this->pXIn[h].Clone(
                this->xIn[h]
            );
            this->peArray.ConnectTo(
                this->peArray.XInList[h],
                this->pXIn[h]
            );
        }
        /** ---------- up-up -------- **/
        for (int w=0;w<this->width;w++){
            this->peArray.ConnectTo(
                this->peArray.SInList[ 2*w ],
                S2Engine::_Low_S
            ); /**  SIn 24-bit in  **/
            this->peArray.ConnectTo(
                this->peArray.SInList[2*w+1],
                S2Engine::_Low_S
            ); /**  SIn 24-bit in  **/

            this->peArray.ConnectTo(
                this->peArray.SEnList[ 2*w ],
                S2Engine::_Low
            ); /**  SEn  1-bit in  **/
            this->peArray.ConnectTo(
                this->peArray.SEnList[2*w+1],
                S2Engine::_Low
            ); /**  SEn  1-bit in  **/

            this->peArray.ConnectTo(
                this->peArray.FInList[ 2*w ],
                S2Engine::_Low
            ); /**  FIn  1-bit in  **/
            this->peArray.ConnectTo(
                this->peArray.FInList[2*w+1],
                S2Engine::_Low
            ); /**  FIn  1-bit in  **/
        }
        this->ruArray.InnerConnect();
        this->peArray.InnerConnect();
        assert(this->InnerConnectedCorrect());
        return;
    }
    ~S2Engine(){
        PortName::ResetCount();
        return;
    }

    inline bool InnerConnected() const override{
        if(!this->peArray.Connected()
        || !this->ruArray.Connected())
            return false;
        return true;
    }
    bool InnerConnectedCorrect() const override{
        /** ###################### **/
        /** ##     RU Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        /// the up ports of reorder units
        if(!this->ruArray.SameTo(RU::BufPushUp,
            this->ports[this->BufPushUp.ToIndex()])
        || !this->ruArray.SameTo(RU::BufInUp  ,
            this->ports[this->BufInUp  .ToIndex()]))
            return false;
        /** -------- left -------- **/
        /// CE left
        for (int i=0;i<this->height;i++){
            if(!this->ruArray.SameTo(
                ruArray.XELInList[i],
                this->ports[this->XELInList[i].ToIndex()])
            || !this->ruArray.SameTo(
                ruArray.XELEmptyList[i],
                this->ports[this->XELEmptyList[i].ToIndex()]))
                return false;
        }
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int w=0;w<this->width;w++){
            if(!this->peArray.SameTo(
                peArray.WInList[w], /** WIn  8-bit in  **/
                this->ports[this->WInList[w].ToIndex()])
            || !this->peArray.SameTo(
                peArray.OInList[w], /** OIn  1-bit in  **/
                this->ports[this->OInList[w].ToIndex()]))
                return false;
        }

        for (int h=0;h<this->height;h++){
            /**  oPop  1-bit **/
            if(!this->pOPop[h].SameTo(this->oPop[h])
            || !this->ruArray.ConnectedTo(
                this->ruArray.OPopList[h],
                this->pOPop[h])
            /**  oEmpty  8-bit in  **/
            || !this->ruArray.ConnectedTo(
                this->pOEmpty[h],
                this->ruArray.OEmptyList[h])
            /**  XIn  8-bit in  **/
            || !this->pXIn[h].SameTo(
                this->xIn[h])
            || !this->peArray.ConnectedTo(
                this->peArray.XInList[h],
                this->pXIn[h]))
                return false;
        }
        return true;
    }
    bool OutputConnectedCorrect() const override{
        /** ##################### **/
        /** ##   Signal Port   ## **/
        /** ##################### **/
        if (!this->ports[this->RUAct.ToIndex()].SameTo(this->ruAct) /** RUAct  1-bit  out  **/
         || !this->ports[this->PEAct.ToIndex()].SameTo(this->peAct))/** PEAct  1-bit  out  **/
            return false;
        /** ###################### **/
        /** ##     RU Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        /// the up ports of reorder units
        if (!this->ruArray.SameTo(this->ports[this->BufFullUp.ToIndex()],ruArray.BufFullUp))
            return false;
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<this->height;i++){
            ///  XELPopList
            if (!this->ruArray.SameTo
                (this->ports[this->XELPopList[i].ToIndex()],
                     this->ruArray.XELPopList[i]))
                return false;
        }/**          1-bit  out    **/
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** -------- ddown ------- **/
        for (int i=0;i<this->width;i++){
            if(!this->peArray.SameTo( /** SOut 24-bit out **/
                this->ports[this->SOutList[ 2*i ].ToIndex()],
                    this->peArray.SOutList[ 2*i ])
            || !this->peArray.SameTo(/** SOut 24-bit out **/
                this->ports[this->SOutList[2*i+1].ToIndex()],
                    this->peArray.SOutList[2*i+1]))
                return false;
        }
        for (int i=0;i<this->width;i++){
            if(!this->peArray.SameTo( /** SEnO 24-bit out **/
                this->ports[this->SEnOList[ 2*i ].ToIndex()],
                    this->peArray.SEnOList[ 2*i ])
            || !this->peArray.SameTo( /** SEnO 24-bit out **/
                this->ports[this->SEnOList[2*i+1].ToIndex()],
                    this->peArray.SEnOList[2*i+1]))
                return false;
        }
        for (int i=0;i<this->width;i++){
            if(!this->peArray.SameTo( /** FOut 24-bit out **/
                this->ports[this->FOutList[ 2*i ].ToIndex()],
                    this->peArray.FOutList[ 2*i ])
            || !this->peArray.SameTo( /** FOut 24-bit out **/
                this->ports[this->FOutList[2*i+1].ToIndex()],
                    this->peArray.FOutList[2*i+1]))
                return false;
        }
        return true;
    }

    bool atPosEdge() override{
        if (this->clock % this->ruPer == 0)
            this->ruArray.atPosEdge();
        if (this->clock % this->pePer == 0){
            this->runPEArray = true;
            for (auto& fifo : fifos)
                if (fifo.empty()){
                    this->runPEArray = false;
                    break;
                }
            if (this->runPEArray){
                for (int h=0;h<this->height;h++){
                    this->xIn[h] = this->fifos[h].front();
                    this->fifos[h].pop();
                }
                this->peArray.atPosEdge();
            }
        }
        return true;
    }
    bool atNegEdge() override{
        assert(false);
        return false;
    }
    bool Run() override{
        if (this->clock % this->ruPer == 0){
            this->ruArray.Run();
            for (int h=0;h<this->height;h++)
                if (this->pOEmpty[h].Data()==0
                 && this->fifos[h].size()<this->height){
                    this->fifos[h].push(this->pOOut[h].Data());
                    this->oPop [h]=1;
                }
                else
                    this->oPop [h]=0;
            this->ruAct = 1;
        }
        else
            this->ruAct = 0;

        if (this->runPEArray){
            this->peArray.Run();
            this->runPEArray = false;
            this->peAct = 1;
        }
        else
            this->peAct = 0;
        this->clock ++;
        return true;
    }
    inline CycleCountType Clock() const{
        return this->clock;
    }
};

#endif//__SYSTEM_SS_ENGINE_HPP
