#ifndef __SYSTEM_REFORMED_PE_ARRAY_HPP
#define __SYSTEM_REFORMED_PE_ARRAY_HPP

#include<iomanip>

#include "../component/RePE.hpp"

#include "../base/ExpHandler.hpp"

class ReformedPEArray: public ClockedObject{
    typedef uint32_t  ArrayLenType;
    typedef uint32_t   SerialType ;

private:
    static FILE* peStatefp;

    const ArrayLenType  width,
                        height;

    vector<vector<ReformedPE> > peArray;

    static PortDataType _low;

    const static Object::Port _Low_1;
    const static Object::Port _Low_S_E;

    vector<Port> pOWOutDownList,
                pOWPushDownList,
                 pXOutRightList,
                pXPushRightList;

    void InitializePorts(){
        this->ports.clear();
        this->ports.reserve(PortName::Number());

        /** ---------- up -------- **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port(   ReformedPE::   OUT_BIT_WIDTH
                                       +  ReformedPE::     W_BIT_WIDTH
                                       +  ReformedPE::    WE_BIT_WIDTH
                                       +  ReformedPE::OFFSET_BIT_WIDTH,
                                          PortDirection::IN ));/**   OWInUp  O+W+V+Off+E-bit in  **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::IN ));/** OWPushUp            1-bit in  **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::OUT));/** OWFullUp            1-bit out **/

        /** -------- left -------- **/
        for (int i=0;i< this->height;i++)
            this->ports.push_back(Port(   ReformedPE::     X_BIT_WIDTH
                                       +  ReformedPE::    XE_BIT_WIDTH
                                       +  ReformedPE::OFFSET_BIT_WIDTH,
                                          PortDirection::IN ));/**    XInLeft  X+V+Off+E-bit in  **/
        for (int i=0;i< this->height;i++)
            this->ports.push_back(Port( 1,PortDirection::IN ));/**  XPushLeft          1-bit in  **/
        for (int i=0;i< this->height;i++)
            this->ports.push_back(Port( 1,PortDirection::OUT));/**  XFullLeft          1-bit out **/

        /** -------- ddown ------- **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port(   ReformedPE:: S_BIT_WIDTH
                                       +  ReformedPE::SE_BIT_WIDTH,
                                          PortDirection::OUT));/**  SOut S+E-bit out **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::OUT));/** SPush   1-bit out **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::IN ));/** SFull   1-bit in  **/
        assert(this->ports.size()==PortName::Number());
        return;
    }

    void OutputConnect() override{
        /** ---------- up -------- **/
        for (int w=0;w<this->width;w++){
            this->peArray[0][w].Clone(
                this->ports[ReformedPEArray::OWFullUpList[w].ToIndex()],
                ReformedPE::OWFullUp
            ); /**            1-bit out **/
        }
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++){
            this->peArray[h][0].Clone(
                this->ports[ReformedPEArray::XFullLeftList[h].ToIndex()],
                ReformedPE::XFullLeft
            ); /**            1-bit out **/
        }
        /** -------- down-down ------- **/
        for (int w=0;w<this->width;w++){
            this->peArray[this->height-2][w].Clone(
                this->ports[ReformedPEArray::SOutDDownList[ 2*w ].ToIndex()],
                ReformedPE::SOutDDown
            ); /**          S+E-bit out **/
            this->peArray[this->height-1][w].Clone(
                this->ports[ReformedPEArray::SOutDDownList[2*w+1].ToIndex()],
                ReformedPE::SOutDDown
            ); /**          S+E-bit out **/
            this->peArray[this->height-2][w].Clone(
                this->ports[ReformedPEArray::SPushDDownList[ 2*w ].ToIndex()],
                ReformedPE::SPushDDown
            ); /**            1-bit out **/
            this->peArray[this->height-1][w].Clone(
                this->ports[ReformedPEArray::SPushDDownList[2*w+1].ToIndex()],
                ReformedPE::SPushDDown
            ); /**            1-bit out **/
        }
        assert(this->OutputConnected());
        return;
    }

public:

    class PortName:public ReformedPE::PortName{
    private:
        static SerialType serialCount;
        SerialType serial;
        PortName &operator=(const PortName&);

    public:
        PortName(ReformedPE::PortName _portName)
                :ReformedPE::PortName(_portName){
            /// wenzhi: ToDo: modify the counting process for better robust
//            this->serial = ReformedPEArray::PortName::serialCount++;
            return;
        }
        PortName(const PortName& portName)
          :ReformedPE::PortName(portName){
            this->serial = ReformedPEArray::PortName::serialCount++;
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

    /** ---------- up -------- **/
    const vector<ReformedPEArray::PortName>   OWInUpList; /**  O+W+V+Off+E-bit in  **/
    const vector<ReformedPEArray::PortName> OWPushUpList; /**            1-bit in  **/
    const vector<ReformedPEArray::PortName> OWFullUpList; /**            1-bit out **/
    /** -------- left -------- **/
    const vector<ReformedPEArray::PortName>   XInLeftList; /**   X+V+Off+E-bit in  **/
    const vector<ReformedPEArray::PortName> XPushLeftList; /**           1-bit in  **/
    const vector<ReformedPEArray::PortName> XFullLeftList; /**           1-bit out **/
    /** -------- down-down ------- **/
    const vector<ReformedPEArray::PortName>  SOutDDownList; /**        S+E-bit out **/
    const vector<ReformedPEArray::PortName> SPushDDownList; /**          1-bit out **/
    const vector<ReformedPEArray::PortName> SFullDDownList; /**          1-bit in  **/

    ReformedPEArray(
            ArrayLenType height,
            ArrayLenType width ,
            ReformedPE::ClockFeqType  ruPer,
            ReformedPE::ClockFeqType macPer):
            height(height),
             width( width),
        /** ---------- up -------- **/
          OWInUpList(  width,PortName(ReformedPE::    OWInUp)), /**  O+W+V+Off+E-bit in  **/
        OWPushUpList(  width,PortName(ReformedPE::  OWPushUp)), /**            1-bit in  **/
        OWFullUpList(  width,PortName(ReformedPE::  OWFullUp)), /**            1-bit out **/
        /** -------- left -------- **/
        XInLeftList(  height,PortName(ReformedPE::   XInLeft)), /**    X+V+Off+E-bit in  **/
      XPushLeftList(  height,PortName(ReformedPE:: XPushLeft)), /**            1-bit in  **/
      XFullLeftList(  height,PortName(ReformedPE:: XFullLeft)), /**            1-bit out **/
        /** -------- down-down ------- **/
       SOutDDownList(2*width,PortName(ReformedPE:: SOutDDown)), /**          S+E-bit out **/
      SPushDDownList(2*width,PortName(ReformedPE::SPushDDown)), /**            1-bit out **/
      SFullDDownList(2*width,PortName(ReformedPE::SFullDDown)), /**            1-bit in  **/
      pOWOutDownList(  width,Port(ReformedPE::   OUT_BIT_WIDTH
                               +  ReformedPE::     W_BIT_WIDTH
                               +  ReformedPE::    WE_BIT_WIDTH
                               +  ReformedPE::OFFSET_BIT_WIDTH,
                                    PortDirection::IN )), /**    O+W+E+Off-bit in  **/
     pOWPushDownList(  width,Port(1,PortDirection::IN )), /**            1-bit in  **/
      pXOutRightList( height,Port(ReformedPE::     X_BIT_WIDTH
                               +  ReformedPE::    XE_BIT_WIDTH
                               +  ReformedPE::OFFSET_BIT_WIDTH,
                                    PortDirection::IN )), /**      X+E+Off-bit in  **/
     pXPushRightList( height,Port(1,PortDirection::IN ))  /**            1-bit in  **/
    {
        assert(height>=2 && width>=2);
        this->peArray.reserve(height);

        this->peArray.emplace_back();
        this->peArray.back().reserve(width);
        for(int w=0;w<width;w++)
            this->peArray.back().emplace_back(ruPer,macPer,false);

        this->peArray.emplace_back();
        this->peArray.back().reserve(width);
        for(int w=0;w<width;w++)
            this->peArray.back().emplace_back(ruPer,macPer,false);

        for (int h=2;h<height;h++){
            this->peArray.emplace_back();
            this->peArray.back().reserve(width);
            for (int w=0;w<width;w++)
                this->peArray.back().emplace_back(ruPer,macPer,true);
        }
        this->InitializePorts();
        this->OutputConnect();
        return;
    }

private:
    ReformedPEArray(const ReformedPEArray&);
    ReformedPEArray &operator=(const ReformedPEArray&);
public:
    ~ReformedPEArray(){
        PortName::ResetCount();
        return;
    }

    inline void InnerConnect() override{
        assert(this->InputConnected());
        /******************************************
         ** connect input ports with inner logic **
         ******************************************/
        /** ---------- up -------- **/
        for (int w=0;w<this->width;w++){
            this->peArray[0][w].Clone(
                ReformedPE::OWInUp,
                this->ports[ReformedPEArray::OWInUpList[w].ToIndex()]
            ); /**   OWInUp O+W+V+Off+E-bit in  **/
            this->peArray[0][w].Clone(
                ReformedPE::OWPushUp,
                this->ports[ReformedPEArray::OWPushUpList[w].ToIndex()]
            ); /** OWPushUp           1-bit in  **/
        }
        /** --------- down ------- **/
        for (int w=0;w<this->width;w++){
            this->peArray[this->height-1][w].ConnectTo(
                ReformedPE::OWFullDown,
                ReformedPEArray::_Low_1
            ); /** OWFullDown           1-bit in  **/
            this->peArray[this->height-1][w].ConnectTo(
                this->pOWOutDownList[w],
                ReformedPE::OWOutDown
            ); /** OWOutDown    O+W+E+Off-bit in  **/
            this->peArray[this->height-1][w].ConnectTo(
                this->pOWPushDownList[w],
                ReformedPE::OWPushDown
            ); /** OWPushDown           1-bit in  **/
        }
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++){
            this->peArray[h][0].Clone(
                ReformedPE::XInLeft,
                this->ports[ReformedPEArray::  XInLeftList[h].ToIndex()]
            ); /**   XInLeft  X+V+Off+E-bit in  **/
            this->peArray[h][0].Clone(
                ReformedPE::XPushLeft,
                this->ports[ReformedPEArray::XPushLeftList[h].ToIndex()]
            ); /** XPushLeft          1-bit in  **/
        }
        /** -------- right -------- **/
        for (int h=0;h<this->height;h++){
            this->peArray[h][this->width-1].ConnectTo(
                ReformedPE::XFullRight,
                ReformedPEArray::_Low_1
            ); /** XFullRight         1-bit in  **/
            this->peArray[h][this->width-1].ConnectTo(
                this->pXOutRightList[h],
                ReformedPE::XOutRight
            ); /** XOutRight    X+E+Off-bit in  **/
            this->peArray[h][this->width-1].ConnectTo(
                this->pXPushRightList[h],
                ReformedPE::XPushRight
            ); /** XPushRight         1-bit in  **/
        }
        /** ----- uup & ddown ---- **/
        for (int w=0;w<this->width;w++){
            this->peArray[0][w].ConnectTo(
                ReformedPE::SInUUp,
                ReformedPEArray::_Low_S_E
            ); /**  SInUUp S+E-bit in  **/
            this->peArray[1][w].ConnectTo(
                ReformedPE::SInUUp,
                ReformedPEArray::_Low_S_E
            ); /**  SInUUp S+E-bit in  **/

            this->peArray[0][w].ConnectTo(
                ReformedPE::SPushUUp,
                ReformedPEArray::_Low_1
            ); /**  SPushUUp  1-bit in  **/
            this->peArray[1][w].ConnectTo(
                ReformedPE::SPushUUp,
                ReformedPEArray::_Low_1
            ); /**  SPushUUp  1-bit in  **/

            this->peArray[this->height-1][w].Clone(
                ReformedPE::SFullDDown,
                this->ports[ReformedPEArray::SFullDDownList[ 2*w ].ToIndex()]
            ); /**  SFullDDown  1-bit in  **/
            this->peArray[this->height-2][w].Clone(
                ReformedPE::SFullDDown,
                this->ports[ReformedPEArray::SFullDDownList[2*w+1].ToIndex()]
            ); /**  SFullDDown  1-bit in  **/
        }

        /*************************
         ** internal connection **
         *************************/
        /** ---------- up -------- **/
        for (int h=1;h<this->height;h++)
            for (int w=0;w<this->width;w++){
                this->peArray[ h ][w].ConnectTo(
                    ReformedPE::OWInUp,
                    this->peArray[h-1][w],
                    ReformedPE::OWOutDown
                ); /** OWIn O+W+V+Off+E-bit **/
                this->peArray[ h ][w].ConnectTo(
                    ReformedPE::OWPushUp,
                    this->peArray[h-1][w],
                    ReformedPE::OWPushDown
                ); /** OWPush         1-bit **/
                this->peArray[h-1][w].ConnectTo(
                    ReformedPE::OWFullDown,
                    this->peArray[ h ][w],
                    ReformedPE::OWFullUp
                ); /** OWFull         1-bit **/
            }
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++)
            for (int w=1;w<this->width;w++){
                this->peArray[h][ w ].ConnectTo(
                    ReformedPE::XInLeft,
                    this->peArray[h][w-1],
                    ReformedPE::XOutRight
                ); /**  XIn   X+V+Off+E-bit **/
                this->peArray[h][ w ].ConnectTo(
                    ReformedPE::XPushLeft,
                    this->peArray[h][w-1],
                    ReformedPE::XPushRight
                ); /**  XPush         1-bit **/
                this->peArray[h][w-1].ConnectTo(
                    ReformedPE::XFullRight,
                    this->peArray[h][ w ],
                    ReformedPE::XFullLeft
                ); /**  XFull         1-bit **/
            }
        /** -------- up-up ------- **/
        for (int h=2;h<this->height;h++)
            for (int w=0;w<this->width;w++){
                this->peArray[ h ][w].ConnectTo(
                    ReformedPE::SInUUp,
                    this->peArray[h-2][w],
                    ReformedPE::SOutDDown
                ); /**  SIn   S+E-bit in  **/
                this->peArray[ h ][w].ConnectTo(
                    ReformedPE::SPushUUp,
                    this->peArray[h-2][w],
                    ReformedPE::SPushDDown
                ); /**  SPush   1-bit in  **/
                this->peArray[h-2][w].ConnectTo(
                    ReformedPE::SFullDDown,
                    this->peArray[ h ][w],
                    ReformedPE::SFullUUp
                ); /**  SFull   1-bit in  **/
            }
        for (int h=0;h<this->height;h++)
            for (int w=0;w<this->width;w++)
                this->peArray[h][w].InnerConnect();
        assert(this->InnerConnectedCorrect());
        return;
    }
    inline bool InnerConnectedCorrect() const override{
        return true;
    }
    inline bool OutputConnectedCorrect() const override{
        return true;
    }
    inline bool InnerConnected() const override{
        for (int h=0;h<this->height;h++)
            for (int w=0;w<this->width;w++)
                if (!this->peArray[h][w].Connected())
                    return false;
        return true;
    }
    inline bool atPosEdge() override{
        for (int h=this->height-1;h>=0;h--)
            for (int w=this->width-1;w>=0;w--){
                this->peArray[h][w].atPosEdge();
            }
        for (int h=0;h<this->height;h++)
            if (this->pXPushRightList[h].Data()==1)
                this-> pXOutRightList[h].Read();
        for (int w=0;w<this->width ;w++)
            if (this->pOWPushDownList[w].Data()==1)
                this-> pOWOutDownList[w].Read();
        return true;
    }
    inline bool atNegEdge() override{
        return false;
    }
    inline bool Run() override{
        for (int h=this->height-1;h>=0;h--)
            for (int w=this->width-1;w>=0;w--){
                this->peArray[h][w].Run();
            }
        return false;
    }
    inline void Print(ArrayLenType h,ArrayLenType w){
        this->peArray[h][w].Print();
    }
    inline void PrintW() const{
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<"Weight:"<<std::endl;
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(ReformedPEArray::peStatefp,"Weight:\n");
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        for (int h=0;h<height;h++){
            for (int w=0;w<width;w++)
                this->peArray[h][w].PrintOW(ReformedPEArray::peStatefp);
            #ifdef PRINT_RUNTIME_INFO_TO_STD
            std::cout<<std::endl;
            #endif // PRINT_RUNTIME_INFO_TO_STD
            #ifdef PRINT_RUNTIME_INFO_TO_FILE
            fprintf(ReformedPEArray::peStatefp,"\n");
            #endif // PRINT_RUNTIME_INFO_TO_FILE
        }
    }
    inline void PrintX() const{
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<"X:"<<std::endl;
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(ReformedPEArray::peStatefp,"X:\n");
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        for (int h=0;h<height;h++){
            for (int w=0;w<width;w++)
                this->peArray[h][w].PrintX(ReformedPEArray::peStatefp);
            #ifdef PRINT_RUNTIME_INFO_TO_STD
            std::cout<<std::endl;
            #endif // PRINT_RUNTIME_INFO_TO_STD
            #ifdef PRINT_RUNTIME_INFO_TO_FILE
            fprintf(ReformedPEArray::peStatefp,"\n");
            #endif // PRINT_RUNTIME_INFO_TO_FILE
        }
    }
    inline void PrintP() const{
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<"ParSum:"<<std::endl;
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(ReformedPEArray::peStatefp,"ParSum:\n");
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        for (int h=0;h<height;h++){
            for (int w=0;w<width;w++)
                this->peArray[h][w].PrintP(ReformedPEArray::peStatefp);
            #ifdef PRINT_RUNTIME_INFO_TO_STD
            std::cout<<std::endl;
            #endif // PRINT_RUNTIME_INFO_TO_STD
            #ifdef PRINT_RUNTIME_INFO_TO_FILE
            fprintf(ReformedPEArray::peStatefp,"\n");
            #endif // PRINT_RUNTIME_INFO_TO_FILE
        }
        return;
    }
    inline void PrintFIFOState() const{
        for (int h=0;h<this->height;h++){
            for (int w=0;w<this->width;w++){
                this->peArray[h][w].PrintFIFOState();
                std::cout<<" ";
            }
            std::cout<<std::endl;
        }
        return;
    }
};

#endif //__SYSTEM_PE_ARRAY_HPP
