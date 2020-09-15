#ifndef __SYSTEM_REFORMED_S2_ENGINE_HPP
#define __SYSTEM_REFORMED_S2_ENGINE_HPP

#include "RePEArray.hpp"
#include "../component/CE.hpp"

class ReformedS2Engine: public ClockedObject{
    typedef uint32_t  ArrayLenType;
    typedef uint32_t   SerialType;
    typedef uint32_t ClockFeqType;
    typedef uint32_t CycleCountType;

private:
    ReformedPEArray rePEArray;
    vector<CE> ce;

    vector<Port>    pOOutList;
    vector<Port> pXInLeftList; /**   X+V+Off+E-bit in  **/


    const static Object::Port _Low;
    const static Object::Port _Low_2_X_XL;
    const static Object::Port _High;

    static PortDataType _high;
    static PortDataType _low ;

    vector<PortDataType> oOutList;

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
        PortName(ReformedPE::PortName _portName){
            /// wenzhi: ToDo: modify the counting process for better robust
//            this->serial = ReformedPEArray::PortName::serialCount++;
            return;
        }
        PortName(CE::PortName _portName){
            /// wenzhi: ToDo: modify the counting process for better robust
//            this->serial = ReformedPEArray::PortName::serialCount++;
            return;
        }
        PortName(const PortName& portName){
            this->serial = ReformedS2Engine::PortName::serialCount++;
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
            this->ports.emplace_back(2
                                   + RU:: X_BIT_WIDTH
                                   + RU::XL_BIT_WIDTH,
                                       PortDirection::IN );/**    XELInList 2+X_BIT+XL_BIT  in  **/
        for (int i=0;i<this->height;i++)
            this->ports.emplace_back(1,PortDirection::OUT);/**   XELPopList          1-bit  out **/
        for (int i=0;i<this->height;i++)
            this->ports.emplace_back(1,PortDirection::IN );/** XELEmptyList          1-bit  in  **/
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int i=0;i<  this->width;i++)
            this->ports.emplace_back(ReformedPE::   OUT_BIT_WIDTH
                                   + ReformedPE::     W_BIT_WIDTH
                                   + ReformedPE::    WE_BIT_WIDTH
                                   + ReformedPE::OFFSET_BIT_WIDTH,
                                       PortDirection::IN );/**   OWInUp  O+W+V+Off+E-bit in  **/
        for (int i=0;i<  this->width;i++)
            this->ports.emplace_back(1,PortDirection::IN );/** OWPushUp            1-bit in  **/
        for (int i=0;i<  this->width;i++)
            this->ports.emplace_back(1,PortDirection::OUT);/** OWFullUp            1-bit out **/
        /** -------- ddown ------- **/
        for (int i=0;i<2*this->width;i++)
            this->ports.emplace_back(ReformedPE:: S_BIT_WIDTH
                                   + ReformedPE::SE_BIT_WIDTH,
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
                this->ports[ReformedS2Engine::XELPopList[i].ToIndex()],
                CE::XELPop
            );/**   XELPopList  1-bit  out **/
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int i=0;i<  this->width;i++)
            this->rePEArray.Clone(this->ports[ReformedS2Engine::OWFullUpList[i].ToIndex()],
                                  this->rePEArray.OWFullUpList[i]);
        /** -------- ddown ------- **/
        for (int i=0;i<2*this->width;i++){
            /**  SOutDDownList S+E-bit out **/
            this->rePEArray.Clone(this->ports[ReformedS2Engine:: SOutDDownList[i].ToIndex()],
                                  this->rePEArray. SOutDDownList[i]);
            /** SPushDDownList   1-bit out **/
            this->rePEArray.Clone(this->ports[ReformedS2Engine::SPushDDownList[i].ToIndex()],
                                  this->rePEArray.SPushDDownList[i]);
        }
        assert(this->OutputConnected());
        return;
    }

private:
    ReformedS2Engine(const ReformedS2Engine&);
    ReformedS2Engine &operator=(const ReformedS2Engine&);

public:
    /** ###################### **/
    /** ##     CE Array     ## **/
    /** ###################### **/
    /** -------- left -------- **/
    /// CE's left ports
    const vector<ReformedS2Engine::PortName> XELInList;    /** 2+X_BIT+XL_BIT  in  **/
    const vector<ReformedS2Engine::PortName> XELPopList;   /**          1-bit  out **/
    const vector<ReformedS2Engine::PortName> XELEmptyList; /**          1-bit  in  **/

    /** ######################### **/
    /** ##  Reformed PE Array  ## **/
    /** ######################### **/
    /** ---------- up -------- **/
    const vector<ReformedS2Engine::PortName>   OWInUpList; /**    O+W+Off+E-bit in  **/
    const vector<ReformedS2Engine::PortName> OWPushUpList; /**            1-bit in  **/
    const vector<ReformedS2Engine::PortName> OWFullUpList; /**            1-bit out **/
    /** -------- down-down ------- **/
    const vector<ReformedS2Engine::PortName>  SOutDDownList; /**        S+E-bit out **/
    const vector<ReformedS2Engine::PortName> SPushDDownList; /**          1-bit out **/
    const vector<ReformedS2Engine::PortName> SFullDDownList; /**          1-bit in  **/

    ReformedS2Engine(
        ArrayLenType height,
        ArrayLenType  width,
        ReformedPE::ClockFeqType  ruPer,
        ReformedPE::ClockFeqType macPer,
        ReformedPE::ClockFeqType  cePer)
        :    ruPer( ruPer),
             cePer( cePer),
            macPer(macPer),
         rePEArray(height,width,ruPer,macPer),
      pXInLeftList(height,Port(
                ReformedPE::     X_BIT_WIDTH
              + ReformedPE::    XE_BIT_WIDTH
              + ReformedPE::OFFSET_BIT_WIDTH,PortDirection::OUT)), /**   X+V+Off+E-bit in  **/
          pOOutList(height,Port(2
               + RU:: X_BIT_WIDTH
               + RU::XL_BIT_WIDTH,PortDirection::IN)),
          oOutList(height,0),
    /** -------- left -------- **/
      XELInList   (height,
          PortName(CE::PortName::_PortName::XELIn   )),/** 2+X_BIT+XL_BIT  in  **/
      XELPopList  (height,
          PortName(CE::PortName::_PortName::XELPop  )),/**          1-bit  out **/
      XELEmptyList(height,
          PortName(CE::PortName::_PortName::XELEmpty)),/**          1-bit  in  **/
    /** ---------- up -------- **/
        OWInUpList( width ,PortName(ReformedPE::    OWInUp)), /**  O+W+Off+E-bit in  **/
      OWPushUpList( width ,PortName(ReformedPE::  OWPushUp)), /**          1-bit in  **/
      OWFullUpList( width ,PortName(ReformedPE::  OWFullUp)), /**          1-bit out **/
    /** -------- down-down ------- **/
     SOutDDownList(2*width,PortName(ReformedPE:: SOutDDown)), /**          S+E-bit out **/
    SPushDDownList(2*width,PortName(ReformedPE::SPushDDown)), /**            1-bit out **/
    SFullDDownList(2*width,PortName(ReformedPE::SFullDDown)), /**            1-bit in  **/
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

    ~ReformedS2Engine(){
        PortName::ResetCount();
        return;
    }

    void InnerConnect() override{
        assert(this->InputConnected());
        /*****************
         ** stray ports **
         *****************/
        for (int h=0;h<this->height;h++)
            this->pXInLeftList[h].Clone(this->oOutList[h]);
        /** ######################
            ##     CE Array     ##
            ###################### **/
        for (int h=0;h<this->height;h++){
            /** -------- left -------- **/
            this->ce[h].Clone(CE::XELIn,   /**    XELInList 2+X_BIT+XL_BIT  in  **/
                              this->ports[ReformedS2Engine::   XELInList[h].ToIndex()]);
            this->ce[h].Clone(CE::XELEmpty,/** XELEmptyList          1-bit  in  **/
                              this->ports[ReformedS2Engine::XELEmptyList[h].ToIndex()]);
            /** -------- right -------- **/
            this->ce[h].ConnectTo(
                    this->pOOutList[h],
                    CE::OOut
            );
            this->ce[h].ConnectTo(
                    CE::OFull,
                    this->rePEArray,
                    this->rePEArray.XFullLeftList[h]
            );
        }
        /// CE up
        this->ce[0].ConnectTo(CE::BufPopUp,ReformedS2Engine::_Low);
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
        /** ###################### **/
        /** ##     PE Array     ## **/
        /** ###################### **/
        /** ---------- up -------- **/
        for (int w=0;w<  this->width;w++)/**   OWInUp O+W+V+Off+E-bit in  **/
            this->rePEArray.Clone(
                rePEArray.OWInUpList[w],
                this->ports[ReformedS2Engine::  OWInUpList[w].ToIndex()]
            );
        for (int w=0;w<  this->width;w++)/** OWPushUp           1-bit in  **/
            this->rePEArray.Clone(
                rePEArray.OWPushUpList[w],
                this->ports[ReformedS2Engine::OWPushUpList[w].ToIndex()]
            );
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++){
            this->rePEArray.ConnectTo(
                rePEArray.XInLeftList[h],
                this->pXInLeftList[h]
            );
            this->rePEArray.ConnectTo(
                this->rePEArray.XPushLeftList[h],
                this->ce[h],
                CE::OPush
            );
        }
        /** -------- ddown ------- **/
        for (int w=0;w<2*this->width;w++)/** SFull   1-bit in  **/
            this->rePEArray.Clone(
                rePEArray.SFullDDownList[w],
                this->ports[ReformedS2Engine::SFullDDownList[w].ToIndex()]
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
            for (int h=this->height-1;h>=0;h--){
                this->ce[h].Run();
                this->oOutList[h] = ReformedPE::CompressXEO
                            (RU::DecompressX  (this->pOOutList[h].Data())
                            ,RU::DecompressEOX(this->pOOutList[h].Data())
                            ,RU::DecompressXL (this->pOOutList[h].Data()));
            }
        this->clock++;
        return true;
    }

    inline void PrintFIFOState(){
        this->rePEArray.PrintFIFOState();
    }
};

#endif//__SYSTEM_SS_ENGINE_HPP
