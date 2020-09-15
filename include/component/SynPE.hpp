#ifndef __COMPONENT_SYNCHRONIZED_PE_HPP
#define __COMPONENT_SYNCHRONIZED_PE_HPP

#include "../../define.hpp"
#include "../base/Object.hpp"
#include "../resource/BaseSequLogic.hpp"
#include "../resource/BaseCombiLogic.hpp"

class SynPE:public ClockedObject{
private:
    PortDataType
        sOut, /** 24-bit out **/
        sEnO, /**  1-bit out **/
        fOut; /**  1-bit out **/

    Reg Sum,
        Feature,
        Weight,
        ParSum,
        OutMark;

    MarkReg Forward;

    Adder  adder;
    Multer multer;

    Port pORegOut, /**  1-bit  IN      Reg OutMark out **/
         pSRegOut, /** 24-bit  IN      Reg Sum     out **/
         pFRegOut, /**  1-bit  IN  markReg Forward out **/
         pFRegClr, /**  1-bit OUT  markReg Forward Clr **/
         pPRegEn , /**  1-bit OUT      Reg ParSum  en  **/
         pPRegIn , /** 24-bit OUT      Reg ParSum  in  **/
         pAddOut ; /** 24-bit  IN    Adder adder   out **/

    PortDataType
        fRegClr,
        pRegEn ,
        pRegIn ;

    static PortDataType _high;
    static PortDataType _low;

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::Number());
        /** ---------- up -------- **/
        this->ports.push_back(Port( 8,PortDirection::IN ));/** WIn   8-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** OIn   1-bit in  **/
        /** --------- down ------- **/
        this->ports.push_back(Port( 8,PortDirection::OUT));/** WOut  8-bit out **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** OOut  1-bit out **/
        /** -------- left -------- **/
        this->ports.push_back(Port( 8,PortDirection::IN ));/**  XIn  8-bit in  **/
        /** -------- right ------- **/
        this->ports.push_back(Port( 8,PortDirection::OUT));/** XOut  8-bit out **/
        /** -------- up-up ------- **/
        this->ports.push_back(Port(24,PortDirection::IN ));/**  SIn 24-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/**  SEn  1-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/**  FIn  1-bit in  **/
        /** -------- ddown ------- **/
        this->ports.push_back(Port(24,PortDirection::OUT));/** SOut 24-bit out **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** SEnO  1-bit out **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** FOut  1-bit out **/
        assert(this->ports.size()==PortName::Number());
    }

    inline void OutputConnect() override{
        this->Weight .Clone(this->ports[PORT_INDEX(WOut)],Reg::out);
        this->OutMark.Clone(this->ports[PORT_INDEX(OOut)],Reg::out);
        this->Feature.Clone(this->ports[PORT_INDEX(XOut)],Reg::out);
        this->ports[PORT_INDEX(SEnO)].Clone(this->sEnO);
        this->ports[PORT_INDEX(FOut)].Clone(this->fOut);
        this->ports[PORT_INDEX(SOut)].Clone(this->sOut);
        assert(this->OutputConnected());
        return;
    }

    inline PortDataType ORegOut(){ /**  1-bit  IN      Reg OutMark out **/
        return this->pORegOut.Data();
    }
    inline PortDataType SRegOut(){ /** 24-bit  IN      Reg Sum     out **/
        return this->pSRegOut.Data();
    }
    inline PortDataType FRegOut(){ /**  1-bit  IN  markReg Forward out **/
        return this->pFRegOut.Data();
    }
    inline PortDataType AddOut (){ /**  1-bit  IN    Adder adder   out **/
        return this->pAddOut .Data();
    }

    const static Object::Port _High;
    const static Object::Port _Low;

public:

    const static PortBitWidthType SUM_BIT_WIDTH;

    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
        /** ---------- up -------- **/
          WIn  = 0, /**  8-bit in  **/
          OIn  = 1, /**  1-bit in  **/
        /** --------- down ------- **/
          WOut = 2, /**  8-bit out **/
          OOut = 3, /**  1-bit out **/
        /** -------- left -------- **/
          XIn  = 4, /**  8-bit in  **/
        /** -------- right ------- **/
          XOut = 5, /**  8-bit out **/
        /** -------- up-up ------- **/
          SIn  = 6, /** 24-bit in  **/
          SEn  = 7, /**  1-bit in  **/
          FIn  = 8, /**  1-bit in  **/
        /** ------ down-down ----- **/
          SOut = 9, /** 24-bit out **/
          SEnO =10, /**  1-bit out **/
          FOut =11, /**  1-bit out **/
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


    /** ---------- up -------- **/
    static const SynPE::PortName WIn ; /**  8-bit in  **/
    static const SynPE::PortName OIn ; /**  1-bit in  **/
    /** --------- down ------- **/
    static const SynPE::PortName OOut; /**  1-bit out **/
    static const SynPE::PortName WOut; /**  8-bit out **/
    /** -------- left -------- **/
    static const SynPE::PortName XIn ; /**  8-bit in  **/
    /** -------- right ------- **/
    static const SynPE::PortName XOut; /**  8-bit out **/
    /** -------- up-up ------- **/
    static const SynPE::PortName SIn ; /** 24-bit in  **/
    static const SynPE::PortName SEn ; /**  1-bit in  **/
    static const SynPE::PortName FIn ; /**  1-bit in  **/
    /** -------- up-up ------- **/
    static const SynPE::PortName SOut; /** 24-bit out **/
    static const SynPE::PortName SEnO; /**  1-bit out **/
    static const SynPE::PortName FOut; /**  1-bit out **/

    SynPE(PortDataType sOut=0, /** 24-bit out **/
          PortDataType sEnO=0, /**  1-bit out **/
          PortDataType fOut=0, /**  1-bit out **/
          PortDataType fRegClr = 0,
          PortDataType pRegEn  = 0,
          PortDataType pRegIn  = 0
        ):Sum    (24,ClockedObject::ClockEdge::POS_EDGE),
          Feature( 8,ClockedObject::ClockEdge::POS_EDGE),
          Weight ( 8,ClockedObject::ClockEdge::POS_EDGE),
          ParSum (24,ClockedObject::ClockEdge::POS_EDGE),
          OutMark( 1,ClockedObject::ClockEdge::POS_EDGE),
          Forward(   ClockedObject::ClockEdge::POS_EDGE),
          adder  (16,24,24,true),
          multer ( 8, 8,16,true),
          pORegOut( 1,Object::PortDirection::IN ), /**  1-bit  IN      Reg OutMark out **/
          pSRegOut(24,Object::PortDirection::IN ), /** 24-bit  IN      Reg Sum     out **/
          pFRegOut( 1,Object::PortDirection::IN ), /**  1-bit  IN  markReg Forward out **/
          pFRegClr( 1,Object::PortDirection::OUT), /**  1-bit OUT  markReg Forward Clr **/
          pPRegEn ( 1,Object::PortDirection::OUT), /**  1-bit OUT      Reg ParSum  en  **/
          pPRegIn (24,Object::PortDirection::OUT), /** 24-bit OUT      Reg ParSum  in  **/
          pAddOut (24,Object::PortDirection::IN )  /** 24-bit  IN    Adder adder   out **/
        {
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        this->sOut    = sOut, /** 24-bit out **/
        this->sEnO    = sEnO, /**  1-bit out **/
        this->fOut    = fOut; /**  1-bit out **/
        this->fRegClr = fRegClr;
        this->pRegEn  = pRegEn;
        this->pRegIn  = pRegIn;
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

    SynPE(const SynPE& synPE):
          Sum    (24,ClockedObject::ClockEdge::POS_EDGE),
          Feature( 8,ClockedObject::ClockEdge::POS_EDGE),
          Weight ( 8,ClockedObject::ClockEdge::POS_EDGE),
          ParSum (24,ClockedObject::ClockEdge::POS_EDGE),
          OutMark( 1,ClockedObject::ClockEdge::POS_EDGE),
          Forward(   ClockedObject::ClockEdge::POS_EDGE),
          adder  (16,24,24,true),
          multer ( 8, 8,16,true),
          pORegOut( 1,Object::PortDirection::IN ), /**  1-bit  IN      Reg OutMark out **/
          pSRegOut(24,Object::PortDirection::IN ), /** 24-bit  IN      Reg Sum     out **/
          pFRegOut( 1,Object::PortDirection::IN ), /**  1-bit  IN  markReg Forward out **/
          pFRegClr( 1,Object::PortDirection::OUT), /**  1-bit OUT  markReg Forward Clr **/
          pPRegEn ( 1,Object::PortDirection::OUT), /**  1-bit OUT      Reg ParSum  en  **/
          pPRegIn (24,Object::PortDirection::OUT), /** 24-bit OUT      Reg ParSum  in  **/
          pAddOut (24,Object::PortDirection::IN )  /** 24-bit  IN    Adder adder   out **/
        {
        this->sOut    = synPE.sOut, /** 24-bit out **/
        this->sEnO    = synPE.sEnO, /**  1-bit out **/
        this->fOut    = synPE.fOut; /**  1-bit out **/
        this->fRegClr = synPE.fRegClr;
        this->pRegEn  = synPE.pRegEn;
        this->pRegIn  = synPE.pRegIn;

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
        assert(this->OuterConnected());
        /******************************************
         ** connect input ports with inner logic **
         ******************************************/
        this->Weight .Clone(    Reg::in, this->ports[PORT_INDEX(WIn)]);
        this->OutMark.Clone(    Reg::in, this->ports[PORT_INDEX(OIn)]);
        this->Feature.Clone(    Reg::in, this->ports[PORT_INDEX(XIn)]);
        this->Sum    .Clone(    Reg::in, this->ports[PORT_INDEX(SIn)]);
        this->Sum    .Clone(    Reg::en, this->ports[PORT_INDEX(SEn)]);
        this->Forward.Clone(MarkReg::Set,this->ports[PORT_INDEX(FIn)]);
        /*************************
         ** internal connection **
         *************************/
        this->Weight .ConnectTo(Reg::en, this->_High);
        this->OutMark.ConnectTo(Reg::en, this->_High);
        this->Feature.ConnectTo(Reg::en, this->_High);
        this->multer .ConnectTo(Multer::in0,this->Feature,   Reg::out);
        this->multer .ConnectTo(Multer::in1,this->Weight ,   Reg::out);
        this->adder  .ConnectTo( Adder::in0,this->multer ,Multer::out);
        this->adder  .ConnectTo( Adder::in1,this->ParSum ,   Reg::out);
        /*****************
         ** stray ports **
         *****************/

        this->Sum     .ConnectTo(this->pSRegOut,    Reg::out);
        this->OutMark .ConnectTo(this->pORegOut,    Reg::out);
        this->Forward .ConnectTo(this->pFRegOut,MarkReg::out);
        this->adder   .ConnectTo(this->pAddOut ,  Adder::out);

        this->pFRegClr.Clone(this->fRegClr); /**  1-bit OUT **/
        this->pPRegEn .Clone(this->pRegEn ); /**  1-bit OUT **/
        this->pPRegIn .Clone(this->pRegIn ); /**  1-bit OUT **/

        this->Forward.ConnectTo(MarkReg::Clr,this->pFRegClr);
        this->ParSum .ConnectTo(Reg::en     ,this->pPRegEn);
        this->ParSum .ConnectTo(Reg::in     ,this->pPRegIn);

//        assert(this->multer .Connected());
//        assert(this->adder  .Connected());
//        assert(this->Weight .Connected());
//        assert(this->OutMark.Connected());
//        assert(this->Feature.Connected());
//        assert(this->Sum    .Connected());
//        assert(this->ParSum .Connected());
//        assert(this->Forward.Connected());
//        assert(this->multer .InnerConnectedCorrect());
//        assert(this->adder  .InnerConnectedCorrect());
//        assert(this->Weight .InnerConnectedCorrect());
//        assert(this->OutMark.InnerConnectedCorrect());
//        assert(this->Feature.InnerConnectedCorrect());
//        assert(this->Sum    .InnerConnectedCorrect());
//        assert(this->ParSum .InnerConnectedCorrect());
//        assert(this->Forward.InnerConnectedCorrect());
//        /******************************************/
//        assert(this->Weight .SameTo(    Reg::in, this->ports[PORT_INDEX(WIn)]));
//        assert(this->OutMark.SameTo(    Reg::in, this->ports[PORT_INDEX(OIn)]));
//        assert(this->Feature.SameTo(    Reg::in, this->ports[PORT_INDEX(XIn)]));
//        assert(this->Sum    .SameTo(    Reg::in, this->ports[PORT_INDEX(SIn)]));
//        assert(this->Sum    .SameTo(    Reg::en, this->ports[PORT_INDEX(SEn)]));
//        assert(this->Forward.SameTo(MarkReg::Set,this->ports[PORT_INDEX(FIn)]));
//        /******************************************/
//        assert(this->multer.ConnectedTo(Multer::in0,this->Feature,   Reg::out));
//        assert(this->multer.ConnectedTo(Multer::in1,this->Weight ,   Reg::out));
//        assert(this->adder .ConnectedTo( Adder::in0,this->multer ,Multer::out));
//        assert(this->adder .ConnectedTo( Adder::in1,this->ParSum ,   Reg::out));
//        /******************************************/
//        assert(this->Sum     .ConnectedTo(this->pSRegOut,    Reg::out));
//        assert(this->OutMark .ConnectedTo(this->pORegOut,    Reg::out));
//        assert(this->Forward .ConnectedTo(this->pFRegOut,MarkReg::out));
//        assert(this->adder   .ConnectedTo(this->pAddOut ,  Adder::out));
//        assert(this->pFRegClr.SameTo(this->fRegClr));  /**  1-bit OUT **/
//        assert(this->pPRegEn .SameTo(this->pRegEn ));  /**  1-bit OUT **/
//        assert(this->pPRegIn .SameTo(this->pRegIn ));  /**  1-bit OUT **/
        assert(this->InnerConnectedCorrect());
        return;
    }

    inline bool InnerConnected() const override{
        return  this->multer .Connected()
             && this->adder  .Connected()
             && this->Weight .Connected()
             && this->OutMark.Connected()
             && this->Feature.Connected()
             && this->Sum    .Connected()
             && this->ParSum .Connected()
             && this->Forward.Connected();
    }
    inline bool OutputConnectedCorrect() const override{
        return  this->Weight .SameTo(this->ports[PORT_INDEX(WOut)],Reg::out)
             && this->OutMark.SameTo(this->ports[PORT_INDEX(OOut)],Reg::out)
             && this->Feature.SameTo(this->ports[PORT_INDEX(XOut)],Reg::out)
             && this->ports[PORT_INDEX(SEnO)].SameTo(this->sEnO)
             && this->ports[PORT_INDEX(FOut)].SameTo(this->fOut)
             && this->ports[PORT_INDEX(SOut)].SameTo(this->sOut);
    }
    inline bool InnerConnectedCorrect() const override{
        return
            this->multer .InnerConnectedCorrect()
         && this->adder  .InnerConnectedCorrect()
         && this->Weight .InnerConnectedCorrect()
         && this->OutMark.InnerConnectedCorrect()
         && this->Feature.InnerConnectedCorrect()
         && this->Sum    .InnerConnectedCorrect()
         && this->ParSum .InnerConnectedCorrect()
         && this->Forward.InnerConnectedCorrect()
        /******************************************/
         && this->Weight .SameTo(    Reg::in, this->ports[PORT_INDEX(WIn)])
         && this->OutMark.SameTo(    Reg::in, this->ports[PORT_INDEX(OIn)])
         && this->Feature.SameTo(    Reg::in, this->ports[PORT_INDEX(XIn)])
         && this->Sum    .SameTo(    Reg::in, this->ports[PORT_INDEX(SIn)])
         && this->Sum    .SameTo(    Reg::en, this->ports[PORT_INDEX(SEn)])
         && this->Forward.SameTo(MarkReg::Set,this->ports[PORT_INDEX(FIn)])
        /******************************************/
         && this->multer.ConnectedTo(Multer::in0,this->Feature,   Reg::out)
         && this->multer.ConnectedTo(Multer::in1,this->Weight ,   Reg::out)
         && this->adder .ConnectedTo( Adder::in0,this->multer ,Multer::out)
         && this->adder .ConnectedTo( Adder::in1,this->ParSum ,   Reg::out)
        /******************************************/
         && this->Sum     .ConnectedTo(this->pSRegOut,    Reg::out)
         && this->OutMark .ConnectedTo(this->pORegOut,    Reg::out)
         && this->Forward .ConnectedTo(this->pFRegOut,MarkReg::out)
         && this->adder   .ConnectedTo(this->pAddOut ,  Adder::out)
         && this->pFRegClr.SameTo(this->fRegClr)  /**  1-bit OUT **/
         && this->pPRegEn .SameTo(this->pRegEn )  /**  1-bit OUT **/
         && this->pPRegIn .SameTo(this->pRegIn ); /**  1-bit OUT **/
//         &&(this->Forward.ConnectedTo((MarkReg::Clr),this->pFRegClr))
//         &&(this->ParSum .ConnectedTo(Reg::en     ,this->pPRegEn ))
//         &&(this->Weight .ConnectedTo(Reg::en, this->_High))
//         &&(this->OutMark.ConnectedTo(Reg::en, this->_High))
//         &&(this->Feature.ConnectedTo(Reg::en, this->_High));
    }
    inline bool atPosEdge() override{
        bool hasChanged = false;

        hasChanged = this->Sum    .atPosEdge() || hasChanged;
        hasChanged = this->Feature.atPosEdge() || hasChanged;
        hasChanged = this->Weight .atPosEdge() || hasChanged;
        hasChanged = this->ParSum .atPosEdge() || hasChanged;
        hasChanged = this->OutMark.atPosEdge() || hasChanged;
        hasChanged = this->Forward.atPosEdge() || hasChanged;
        return hasChanged;
    }
    inline bool atNegEdge() override{
        assert(false);
        return false;
    }
    inline bool Run() override{
        bool hasChanged = false;
        hasChanged = this->Sum    .Run() || hasChanged;
        hasChanged = this->Feature.Run() || hasChanged;
        hasChanged = this->Weight .Run() || hasChanged;
        hasChanged = this->ParSum .Run() || hasChanged;
        hasChanged = this->OutMark.Run() || hasChanged;
        hasChanged = this->Forward.Run() || hasChanged;
        hasChanged = this->multer .Run() || hasChanged;
        hasChanged = this->adder  .Run() || hasChanged;

        if (this->ORegOut()==1){
            this->fOut    = 1;
            this->sOut    = this->AddOut();
            this->sEnO    = 1;
            this->fRegClr = 1;
            this->pRegEn  = 1;
            this->pRegIn  = 0;
        }
        else if (this->FRegOut()==1){
            this->fOut    = 1;
            this->sOut    = this->SRegOut();
            this->sEnO    = 1;
            this->fRegClr = 0;
            this->pRegEn  = 1;
            this->pRegIn  = this->AddOut();
        }
        else{
            this->fOut    = 0;
            this->sOut    = this->AddOut();
            this->sEnO    = 0;
            this->fRegClr = 0;
            this->pRegEn  = 1;
            this->pRegIn  = this->AddOut();
        }
        return true;
    }
    inline void PrintW(FILE* fp) const{
        this-> Weight.PrintTo(fp);
        return;
    }
    inline void PrintO(FILE* fp) const{
        this->OutMark.PrintTo(fp);
        return;
    }
    inline void PrintX(FILE* fp) const{
        this->Feature.PrintTo(fp);
        return;
    }
    inline void PrintP(FILE* fp) const{
        this->ParSum.PrintTo(fp);
        return;
    }
    inline void PrintS(FILE* fp) const{
        this->Sum.PrintTo(fp);
        return;
    }
    inline void Print(){
        std::cout<<"reg Feature state:"<<std::endl;
        this->Feature.Print();
        std::cout<<"reg Weight state:"<<std::endl;
        this->Weight.Print();
        std::cout<<"reg OutMark state:"<<std::endl;
        this->OutMark.Print();
        std::cout<<"reg Sum state:"<<std::endl;
        this->Sum.Print();
        std::cout<<"reg ParSum state:"<<std::endl;
        this->ParSum.Print();

        Port pWOut(8,Object::PortDirection::IN);
        Port pXOut(8,Object::PortDirection::IN);
        Port pOOut(1,Object::PortDirection::IN);
        this->Weight .ConnectTo(pWOut,Reg::out);
        this->Feature.ConnectTo(pXOut,Reg::out);
        this->OutMark.ConnectTo(pOOut,Reg::out);
        std::cout<<"wOut:  "   <<pWOut.Data() <<std::endl;
        std::cout<<"oOut:  "   <<pOOut.Data() <<std::endl;
        std::cout<<"xOut:  "   <<pXOut.Data() <<std::endl;
        std::cout<<"sOut:  "   <<this->sOut   <<std::endl;
        std::cout<<"fOut:  "   <<this->fOut   <<std::endl;
        std::cout<<"sEnO:  "   <<this->sEnO   <<std::endl;
        std::cout<<"[fRegClr]:"<<this->fRegClr<<std::endl;
        std::cout<<"[pRegEn ]:"<<this->pRegEn <<std::endl;
        std::cout<<"[pRegIn ]:"<<this->pRegIn <<std::endl;
        return;
    }
};

#endif // __COMPONENT_SYNCHRONIZED_PE_HPP
