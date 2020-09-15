#ifndef __SYSTEM_REFORMED_MIXED_S2_ENGINE_HPP
#define __SYSTEM_REFORMED_MIXED_S2_ENGINE_HPP

#include "../component/MixedCE.hpp"
#include "ReMixedPEArray.hpp"
#include "../component/CE.hpp"

class ReMixedS2Engine: public ClockedObject{
    typedef uint32_t   ArrayLenType;
    typedef uint32_t     SerialType;
    typedef uint32_t   ClockFeqType;
    typedef uint32_t CycleCountType;

private:
    ReMixedPEArray rePEArray;
    vector<MixedCE> ce;

    vector<Port> pXInLeftList; /**   X+V+Off+E-bit in  **/

    const static Object::Port _Low;
    const static Object::Port _Low_2_X_XL;
    const static Object::Port _High;

    static PortDataType _high;
    static PortDataType _low ;

    const ArrayLenType height;
    const ArrayLenType  width;

    const ClockFeqType ruPer,
                       cePer,
                       macPer;

    CycleCountType clock;

    class PortName:public BasePortName{
    private:
        PortName &operator=(const PortName&);

        static SerialType serialCount;
        SerialType serial;

    public:
        PortName(ReMixedPE::PortName _portName){
            /// wenzhi: ToDo: modify the counting process for better robust
//            this->serial = ReMixedPEArray::PortName::serialCount++;
            return;
        }
        PortName(MixedCE::PortName _portName){
            /// wenzhi: ToDo: modify the counting process for better robust
//            this->serial = ReMixedPEArray::PortName::serialCount++;
            return;
        }
        PortName(const PortName& portName){
            this->serial = ReMixedS2Engine::PortName::serialCount++;
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

    inline void InitializePorts() override {
        this->ports.clear();
        this->ports.reserve(PortName::Number());
        /** ###################### **/
        /** ##     CE Array     ## **/
        /** ###################### **/
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<this->height;i++)
            this->ports.emplace_back(1
         + ReMixedPE::  MARK_BIT_WIDTH
         + ReMixedPE::     X_BIT_WIDTH
         + ReMixedPE::    XE_BIT_WIDTH
         + ReMixedPE::OFFSET_BIT_WIDTH,PortDirection::IN );/**    XELInList 2+X_BIT+XL_BIT  in  **/
        for (int i=0;i<this->height;i++)
            this->ports.emplace_back(1,PortDirection::OUT);/**   XELPopList          1-bit  out **/
        for (int i=0;i<this->height;i++)
            this->ports.emplace_back(1,PortDirection::IN );/** XELEmptyList          1-bit  in  **/
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int i=0;i<  this->width;i++)
            this->ports.emplace_back(ReMixedPE::  MARK_BIT_WIDTH
                                   + ReMixedPE::   OUT_BIT_WIDTH
                                   + ReMixedPE::     W_BIT_WIDTH
                                   + ReMixedPE::    WE_BIT_WIDTH
                                   + ReMixedPE::OFFSET_BIT_WIDTH,
                                       PortDirection::IN );/**   OWInUp  O+W+V+Off+E-bit in  **/
        for (int i=0;i<  this->width;i++)
            this->ports.emplace_back(1,PortDirection::IN );/** OWPushUp            1-bit in  **/
        for (int i=0;i<  this->width;i++)
            this->ports.emplace_back(1,PortDirection::OUT);/** OWFullUp            1-bit out **/
        /** -------- ddown ------- **/
        for (int i=0;i<2*this->width;i++)
            this->ports.emplace_back(ReMixedPE:: S_BIT_WIDTH
                                   + ReMixedPE::SE_BIT_WIDTH,
                                       PortDirection::OUT);/**  SOut S+E-bit out **/
        for (int i=0;i<2*this->width;i++)
            this->ports.emplace_back(1,PortDirection::OUT);/** SPush   1-bit out **/
        for (int i=0;i<2*this->width;i++)
            this->ports.emplace_back(1,PortDirection::IN );/** SFull   1-bit in  **/
        assert(this->ports.size()==PortName::Number());
        return;
    }

    inline void OutputConnect() override {
        /** ###################### **/
        /** ##     CE Array     ## **/
        /** ###################### **/
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<this->height;i++)
            this->ce[i].Clone(
                this->ports[ReMixedS2Engine::XELPopList[i].ToIndex()],
                MixedCE::XELPop
            );/**   XELPopList  1-bit  out **/
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int i=0;i<  this->width;i++)
            this->rePEArray.Clone(this->ports[ReMixedS2Engine::OWFullUpList[i].ToIndex()],
                                  this->rePEArray.OWFullUpList[i]);
        /** -------- ddown ------- **/
        for (int i=0;i<2*this->width;i++){
            /**  SOutDDownList S+E-bit out **/
            this->rePEArray.Clone(this->ports[ReMixedS2Engine:: SOutDDownList[i].ToIndex()],
                                  this->rePEArray. SOutDDownList[i]);
            /** SPushDDownList   1-bit out **/
            this->rePEArray.Clone(this->ports[ReMixedS2Engine::SPushDDownList[i].ToIndex()],
                                  this->rePEArray.SPushDDownList[i]);
        }
        assert(this->OutputConnected());
        return;
    }

private:
    ReMixedS2Engine(const ReMixedS2Engine&);
    ReMixedS2Engine &operator=(const ReMixedS2Engine&);

public:
    /** ###################### **/
    /** ##     CE Array     ## **/
    /** ###################### **/
    /** -------- left -------- **/
    /// CE's left ports
    const vector<ReMixedS2Engine::PortName> XELInList;    /** 2+X_BIT+XL_BIT  in  **/
    const vector<ReMixedS2Engine::PortName> XELPopList;   /**          1-bit  out **/
    const vector<ReMixedS2Engine::PortName> XELEmptyList; /**          1-bit  in  **/

    /** ######################### **/
    /** ##  Reformed PE Array  ## **/
    /** ######################### **/
    /** ---------- up -------- **/
    const vector<ReMixedS2Engine::PortName>   OWInUpList; /**    O+W+Off+E-bit in  **/
    const vector<ReMixedS2Engine::PortName> OWPushUpList; /**            1-bit in  **/
    const vector<ReMixedS2Engine::PortName> OWFullUpList; /**            1-bit out **/
    /** -------- down-down ------- **/
    const vector<ReMixedS2Engine::PortName>  SOutDDownList; /**        S+E-bit out **/
    const vector<ReMixedS2Engine::PortName> SPushDDownList; /**          1-bit out **/
    const vector<ReMixedS2Engine::PortName> SFullDDownList; /**          1-bit in  **/

    ReMixedS2Engine(
        ArrayLenType height,
        ArrayLenType  width,
        ReMixedPE::ClockFeqType  ruPer,
        ReMixedPE::ClockFeqType macPer,
        ReMixedPE::ClockFeqType  cePer)
        :    ruPer( ruPer),
             cePer( cePer),
            macPer(macPer),
         rePEArray(height,width,ruPer,macPer),
      pXInLeftList(height,Port(
                ReMixedPE::     X_BIT_WIDTH
              + ReMixedPE::    XE_BIT_WIDTH
              + ReMixedPE::OFFSET_BIT_WIDTH,PortDirection::OUT)), /**   X+V+Off+E-bit in  **/
    /** -------- left -------- **/
      XELInList   (height,
          PortName(MixedCE::PortName::_PortName::XELIn   )),/** 2+X_BIT+XL_BIT  in  **/
      XELPopList  (height,
          PortName(MixedCE::PortName::_PortName::XELPop  )),/**          1-bit  out **/
      XELEmptyList(height,
          PortName(MixedCE::PortName::_PortName::XELEmpty)),/**          1-bit  in  **/
    /** ---------- up -------- **/
        OWInUpList( width ,PortName(ReMixedPE::    OWInUp)), /**  O+W+Off+E-bit in  **/
      OWPushUpList( width ,PortName(ReMixedPE::  OWPushUp)), /**          1-bit in  **/
      OWFullUpList( width ,PortName(ReMixedPE::  OWFullUp)), /**          1-bit out **/
    /** -------- down-down ------- **/
     SOutDDownList(2*width,PortName(ReMixedPE:: SOutDDown)), /**          S+E-bit out **/
    SPushDDownList(2*width,PortName(ReMixedPE::SPushDDown)), /**            1-bit out **/
    SFullDDownList(2*width,PortName(ReMixedPE::SFullDDown)), /**            1-bit in  **/
            height(height),
             width(width){
        this->clock = 0;
        this->ce.reserve(height);
        for (int h=0;h<height;h++)
            this->ce.emplace_back();
        this->InitializePorts();
        this->OutputConnect();
        return;
    }

    ~ReMixedS2Engine(){
        PortName::ResetCount();
        return;
    }

    void InnerConnect() override{
        assert(this->InputConnected());
        /** ######################
            ##     CE Array     ##
            ###################### **/
        for (int h=0;h<this->height;h++){
            /** -------- left -------- **/
            this->ce[h].Clone(MixedCE::XELIn,   /**    XELInList 2+X_BIT+XL_BIT  in  **/
                              this->ports[ReMixedS2Engine::   XELInList[h].ToIndex()]);
            this->ce[h].Clone(MixedCE::XELEmpty,/** XELEmptyList          1-bit  in  **/
                              this->ports[ReMixedS2Engine::XELEmptyList[h].ToIndex()]);
            /** -------- right -------- **/
            this->ce[h].ConnectTo(
                    MixedCE::OFull,
                    this->rePEArray,
                    this->rePEArray.XFullLeftList[h]
            );
        }
        /// CE up
        this->ce[0].ConnectTo(MixedCE::BufPopUp,ReMixedS2Engine::_Low);
        for (int i=1;i<this->height;i++){
            this->ce[i].ConnectTo(
                MixedCE::BufPopUp,
                this->ce[i-1],
                MixedCE::BufPopDown
            );
        }
        /// CE down
        for (int i=1;i<this->height;i++){
            this->ce[i-1].ConnectTo(
                MixedCE::BufInDown,
                this->ce[i],
                MixedCE::BufOutUp
            );
            this->ce[i-1].ConnectTo(
                MixedCE::BufEmptyDown,
                this->ce[i],
                MixedCE::BufEmptyUp
            );
        }
        this->ce[this->height-1].ConnectTo(
            MixedCE::BufInDown,
            this->_Low_2_X_XL
        );
        this->ce[this->height-1].ConnectTo(
            MixedCE::BufEmptyDown,
            this->_High
        );
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int w=0;w<  this->width;w++)/**   OWInUp O+W+V+Off+E-bit in  **/
            this->rePEArray.Clone(
                rePEArray.OWInUpList[w],
                this->ports[ReMixedS2Engine::  OWInUpList[w].ToIndex()]
            );
        for (int w=0;w<  this->width;w++)/** OWPushUp           1-bit in  **/
            this->rePEArray.Clone(
                rePEArray.OWPushUpList[w],
                this->ports[ReMixedS2Engine::OWPushUpList[w].ToIndex()]
            );
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++){
            this->rePEArray.ConnectTo(
                rePEArray.XInLeftList[h],
                this->ce[h],
                MixedCE::OOut
            );
            this->rePEArray.ConnectTo(
                this->rePEArray.XPushLeftList[h],
                this->ce[h],
                MixedCE::OPush
            );
        }
        /** -------- ddown ------- **/
        for (int w=0;w<2*this->width;w++)/** SFull   1-bit in  **/
            this->rePEArray.Clone(
                rePEArray.SFullDDownList[w],
                this->ports[ReMixedS2Engine::SFullDDownList[w].ToIndex()]
            );
        for (int h=0;h<this->height;h++)
            this->ce[h].InnerConnect();
        this->rePEArray.InnerConnect();
        return;
    }
    inline bool  InnerConnectedCorrect() const override{
        if (!this->rePEArray.Connected())
            return false;
        for (int h=0;h<this->height;h++)
            if (!this->ce[h].Connected()){
                std::cout<<h<<std::endl;
                return false;
            }
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return true;
    }
    inline bool InnerConnected() const override{
        if (!this->rePEArray.Connected())
            return false;
        for (int h=0;h<this->height;h++)
            if (!this->ce[h].Connected()){
                std::cout<<h<<std::endl;
                return false;
            }
        return true;
    }
    inline bool atNegEdge() override{
        assert(false);
        return false;
    }

    inline bool atPosEdge() override{
        this->rePEArray.atPosEdge();
        if (this->clock % ruPer ==0)
            for (auto& _ce : this->ce)
                _ce.atPosEdge();
        return true;
    }

    inline bool Run() override{
        this->rePEArray.Run();
        if (this->clock % ruPer ==0)
            for (int h=this->height-1;h>=0;h--)
                this->ce[h].Run();
        this->clock++;
        return true;
    }

    inline void PrintFIFOState(){
        this->rePEArray.PrintFIFOState();
    }
};

#endif//__SYSTEM_REFORMED_MIXED_S2_ENGINE_HPP
