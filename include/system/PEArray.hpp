#ifndef __SYSTEM_PE_ARRAY_HPP
#define __SYSTEM_PE_ARRAY_HPP

#include<iomanip>

#include "../component/SynPE.hpp"

#include "../base/ExpHandler.hpp"

class SynPEArray: public ClockedObject{
    typedef uint32_t  ArrayLenType;
    typedef uint32_t   SerialType;

private:
    static FILE* peStatefp;

    vector<vector<SynPE> > peArray;
    ArrayLenType height,width;

    class PortName:public SynPE::PortName{
    private:
        static SerialType serialCount;
        SerialType serial;

    public:
        PortName(SynPE::PortName _portName)
                 :SynPE::PortName(_portName){
            /// wenzhi: ToDo: modify the counting process for better robust
//            this->serial = SynPEArray::PortName::serialCount++;
            return;
        }
        PortName(const PortName& portName)
                :SynPE::PortName(portName){
            this->serial = SynPEArray::PortName::serialCount++;
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

    void InitializePorts(){
        this->ports.clear();
        this->ports.reserve(PortName::Number());
            /** ---------- up -------- **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port( 8,PortDirection::IN ));/** WIn   8-bit in  **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::IN ));/** OIn   1-bit in  **/
            /** --------- down ------- **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port( 8,PortDirection::OUT));/** WOut  8-bit out **/
        for (int i=0;i<  this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::OUT));/** OOut  1-bit out **/
            /** -------- left -------- **/
        for (int i=0;i< this->height;i++)
            this->ports.push_back(Port( 8,PortDirection::IN ));/**  XIn  8-bit in  **/
            /** -------- right ------- **/
        for (int i=0;i< this->height;i++)
            this->ports.push_back(Port( 8,PortDirection::OUT));/** XOut  8-bit out **/
            /** -------- up-up ------- **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port(24,PortDirection::IN ));/**  SIn 24-bit in  **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::IN ));/**  SEn  1-bit in  **/
        for (int i=0;i<2*this->width;i++)
            this->ports.push_back(Port( 1,PortDirection::IN ));/**  FIn  1-bit in  **/
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
        /** --------- down ------- **/
        for (int i=0;i<  this->width;i++)
            this->peArray[this->height-1][i].Clone(
                this->ports[SynPEArray::WOutList[i].ToIndex()],
                SynPE::WOut
            ); /** WOut  8-bit out **/
        for (int i=0;i<  this->width;i++)
            this->peArray[this->height-1][i].Clone(
                this->ports[SynPEArray::OOutList[i].ToIndex()],
                SynPE::OOut
            ); /** OOut  1-bit out **/
        /** -------- right ------- **/
        for (int i=0;i< this->height;i++)
            this->peArray[i][this->width-1].Clone(
                this->ports[SynPEArray::XOutList[i].ToIndex()],
                SynPE::XOut
            ); /** XOut  8-bit out **/


        /** -------- ddown ------- **/
        for (int i=0;i<this->width;i++){
            this->peArray[this->height-2][i].Clone(
                this->ports[SynPEArray::SOutList[ 2*i ].ToIndex()],
                SynPE::SOut
            ); /** SOut 24-bit out **/
            this->peArray[this->height-1][i].Clone(
                this->ports[SynPEArray::SOutList[2*i+1].ToIndex()],
                SynPE::SOut
            ); /** SOut 24-bit out **/
        }
        for (int i=0;i<this->width;i++){
            this->peArray[this->height-2][i].Clone(
                this->ports[SynPEArray::SEnOList[ 2*i ].ToIndex()],
                SynPE::SEnO
            ); /** SEnO  1-bit out **/
            this->peArray[this->height-1][i].Clone(
                this->ports[SynPEArray::SEnOList[2*i+1].ToIndex()],
                SynPE::SEnO
            ); /** SEnO  1-bit out **/
        }
        for (int i=0;i<this->width;i++){
            this->peArray[this->height-2][i].Clone(
                this->ports[SynPEArray::FOutList[ 2*i ].ToIndex()],
                SynPE::FOut
            ); /** FOut  1-bit out **/
            this->peArray[this->height-1][i].Clone(
                this->ports[SynPEArray::FOutList[2*i+1].ToIndex()],
                SynPE::FOut
            ); /** FOut  1-bit out **/
        }
        assert(this->OutputConnected());
        return;
    }

public:
    /** ---------- up -------- **/
    const vector<PortName> WInList;  /**  8-bit in  **/
    const vector<PortName> OInList;  /**  1-bit in  **/
    /** --------- down ------- **/
    const vector<PortName> WOutList; /**  8-bit out **/
    const vector<PortName> OOutList; /**  1-bit out **/
    /** -------- left -------- **/
    const vector<PortName> XInList;  /**  8-bit in  **/
    /** -------- right ------- **/
    const vector<PortName> XOutList; /**  8-bit out **/
    /** -------- up-up ------- **/
    const vector<PortName> SInList;  /** 24-bit in  **/
    const vector<PortName> SEnList;  /**  1-bit in  **/
    const vector<PortName> FInList;  /**  1-bit in  **/
    /** ------ down-down ----- **/
    const vector<PortName> SOutList; /** 24-bit out **/
    const vector<PortName> SEnOList; /**  1-bit out **/
    const vector<PortName> FOutList; /**  1-bit out **/

    SynPEArray(ArrayLenType height,ArrayLenType width):
        /** ---------- up -------- **/
         WInList(  width,PortName(SynPE::WIn )),  /**  8-bit in  **/
         OInList(  width,PortName(SynPE::OIn )),  /**  1-bit in  **/
        /** --------- down ------- **/
        WOutList(  width,PortName(SynPE::WOut)), /**  8-bit out **/
        OOutList(  width,PortName(SynPE::OOut)), /**  1-bit out **/
        /** -------- left -------- **/
         XInList( height,PortName(SynPE::WIn )),  /**  8-bit in  **/
        /** -------- right ------- **/
        XOutList( height,PortName(SynPE::XOut)), /**  8-bit out **/
        /** -------- up-up ------- **/
         SInList(2*width,PortName(SynPE::SIn )),  /** 24-bit in  **/
         SEnList(2*width,PortName(SynPE::SEn )),  /**  1-bit in  **/
         FInList(2*width,PortName(SynPE::FIn )),  /**  1-bit in  **/
        /** ------ down-down ----- **/
        SOutList(2*width,PortName(SynPE::SOut)), /** 24-bit out **/
        SEnOList(2*width,PortName(SynPE::SEnO)), /**  1-bit out **/
        FOutList(2*width,PortName(SynPE::FOut))  /**  1-bit out **/
    {
        assert(height>=2 && width>=2);
        this->height = height;
        this->width  = width;

        for (int h=0;h<height;h++){
            this->peArray.emplace_back();
            this->peArray.back().reserve(width);
            for(int w=0;w<width;w++)
                this->peArray.back().emplace_back();
        }
        this->InitializePorts();
        this->OutputConnect();
        return;
    }
private:
    SynPEArray(const SynPEArray&);
    SynPEArray &operator=(const SynPEArray&);
public:
    ~SynPEArray(){
        PortName::ResetCount();
        return;
    }

    inline void InnerConnect() override{
        assert(this->OuterConnected());
        /******************************************
         ** connect input ports with inner logic **
         ******************************************/
        /** ---------- up -------- **/
        for (int w=0;w<this->width;w++){
            this->peArray[0][w].Clone(
                SynPE::WIn,
                this->ports[SynPEArray::WInList[w].ToIndex()]
            ); /** WIn  8-bit in  **/
            this->peArray[0][w].Clone(
                SynPE::OIn,
                this->ports[SynPEArray::OInList[w].ToIndex()]
            ); /** OIn  1-bit in  **/
        }
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++)
            this->peArray[h][0].Clone(
                SynPE::XIn,
                this->ports[SynPEArray::XInList[h].ToIndex()]
            ); /**  XIn  8-bit in  **/
        /** -------- up-up ------- **/
        for (int w=0;w<this->width;w++){
            this->peArray[0][w].Clone(
                SynPE::SIn,
                this->ports[SynPEArray::SInList[ 2*w ].ToIndex()]
            ); /**  SIn 24-bit in  **/
            this->peArray[1][w].Clone(
                SynPE::SIn,
                this->ports[SynPEArray::SInList[2*w+1].ToIndex()]
            ); /**  SIn 24-bit in  **/

            this->peArray[0][w].Clone(
                SynPE::SEn,
                this->ports[SynPEArray::SEnList[ 2*w ].ToIndex()]
            ); /**  SEn  1-bit in  **/
            this->peArray[1][w].Clone(
                SynPE::SEn,
                this->ports[SynPEArray::SEnList[2*w+1].ToIndex()]
            ); /**  SEn  1-bit in  **/

            this->peArray[0][w].Clone(
                SynPE::FIn,
                this->ports[SynPEArray::FInList[ 2*w ].ToIndex()]
            ); /**  FIn  1-bit in  **/
            this->peArray[1][w].Clone(
                SynPE::FIn,
                this->ports[SynPEArray::FInList[2*w+1].ToIndex()]
            ); /**  FIn  1-bit in  **/
        }

        /*************************
         ** internal connection **
         *************************/
        /** ---------- up -------- **/
        for (int h=1;h<this->height;h++)
            for (int w=0;w<this->width;w++){
                this->peArray[h][w].ConnectTo(
                    SynPE::WIn,
                    this->peArray[h-1][w],
                    SynPE::WOut
                ); /** WIn  8-bit in  **/
                this->peArray[h][w].ConnectTo(
                    SynPE::OIn,
                    this->peArray[h-1][w],
                    SynPE::OOut
                ); /** OIn  1-bit in  **/
            }
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++)
            for (int w=1;w<this->width;w++)
                this->peArray[h][w].ConnectTo(
                    SynPE::XIn,
                    this->peArray[h][w-1],
                    SynPE::XOut
                ); /**  XIn  8-bit in  **/

        /** -------- up-up ------- **/
        for (int h=2;h<this->height;h++)
            for (int w=0;w<this->width;w++){
                this->peArray[h][w].ConnectTo(
                    SynPE::SIn,
                    this->peArray[h-2][w],
                    SynPE::SOut
                ); /**  SIn 24-bit in  **/
                this->peArray[h][w].ConnectTo(
                    SynPE::SEn,
                    this->peArray[h-2][w],
                    SynPE::SEnO
                ); /**  SEn  1-bit in  **/
                this->peArray[h][w].ConnectTo(
                    SynPE::FIn,
                    this->peArray[h-2][w],
                    SynPE::FOut
                ); /**  FIn  1-bit in  **/
            }
        for (int h=0;h<this->height;h++)
            for (int w=0;w<this->width;w++)
                this->peArray[h][w].InnerConnect();
        assert(this->InnerConnectedCorrect());
        return;
    }
    inline bool InnerConnectedCorrect() const override{
        /******************************************
         ** connect input ports with inner logic **
         ******************************************/
        /** ---------- up -------- **/
        for (int w=0;w<this->width;w++){
            if (!this->peArray[0][w].SameTo(
                    SynPE::WIn,  /** WIn  1-bit in  **/
                    this->ports[SynPEArray::WInList[w].ToIndex()])
             || !this->peArray[0][w].SameTo(
                    SynPE::OIn,  /** OIn  1-bit in  **/
                    this->ports[SynPEArray::OInList[w].ToIndex()]))
                return false;
        }
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++)
            if (!this->peArray[h][0].SameTo(
                    SynPE::XIn, /**  XIn  8-bit in  **/
                    this->ports[SynPEArray::XInList[h].ToIndex()]))
                return false;
        /** -------- up-up ------- **/
        for (int w=0;w<this->width;w++){
            if (!this->peArray[0][w].SameTo(
                    SynPE::SIn, /**  SIn 24-bit in  **/
                    this->ports[SynPEArray::SInList[ 2*w ].ToIndex()])
             || !this->peArray[1][w].SameTo(
                    SynPE::SIn, /**  SIn 24-bit in  **/
                    this->ports[SynPEArray::SInList[2*w+1].ToIndex()])
             || !this->peArray[0][w].SameTo(
                    SynPE::SEn, /**  SEn  1-bit in  **/
                    this->ports[SynPEArray::SEnList[ 2*w ].ToIndex()])
             || !this->peArray[1][w].SameTo(
                    SynPE::SEn, /**  SEn  1-bit in  **/
                    this->ports[SynPEArray::SEnList[2*w+1].ToIndex()])
             || !this->peArray[0][w].SameTo(
                    SynPE::FIn, /**  FIn  1-bit in  **/
                    this->ports[SynPEArray::FInList[ 2*w ].ToIndex()])
             || !this->peArray[1][w].SameTo(
                    SynPE::FIn, /**  FIn  1-bit in  **/
                    this->ports[SynPEArray::FInList[2*w+1].ToIndex()]))
                return false;
        }
        /*************************
         ** internal connection **
         *************************/
        /** ---------- up -------- **/
        for (int h=1;h<this->height;h++)
            for (int w=0;w<this->width;w++){
                if (!this->peArray[h][w].ConnectedTo(
                        SynPE::WIn, /** WIn  8-bit in  **/
                        this->peArray[h-1][w],
                        SynPE::WOut)
                 || !this->peArray[h][w].ConnectedTo(
                        SynPE::OIn, /** OIn  1-bit in  **/
                        this->peArray[h-1][w],
                        SynPE::OOut))
                    return false;
            }
        /** -------- left -------- **/
        for (int h=0;h<this->height;h++)
            for (int w=1;w<this->width;w++)
                if (!this->peArray[h][w].ConnectedTo(
                        SynPE::XIn, /**  XIn  8-bit in  **/
                        this->peArray[h][w-1],
                        SynPE::XOut))
                    return false;
        /** -------- up-up ------- **/
        for (int h=2;h<this->height;h++)
            for (int w=0;w<this->width;w++){
                if (!this->peArray[h][w].ConnectedTo(
                        SynPE::SIn, /**  SIn 24-bit in  **/
                        this->peArray[h-2][w],
                        SynPE::SOut)
                 || !this->peArray[h][w].ConnectedTo(
                        SynPE::SEn, /**  SEn  1-bit in  **/
                        this->peArray[h-2][w],
                        SynPE::SEnO)
                 || !this->peArray[h][w].ConnectedTo(
                        SynPE::FIn, /**  FIn  1-bit in  **/
                        this->peArray[h-2][w],
                        SynPE::FOut))
                    return false;
            }
        for (int h=0;h<this->height;h++)
            for (int w=0;w<this->width;w++)
                if (!this->peArray[h][w].InnerConnectedCorrect())
                    return false;
        return true;
    }
    bool OutputConnectedCorrect() const override{
        /** --------- down ------- **/
        for (int i=0;i<  this->width;i++)
            if(!this->peArray[this->height-1][i].SameTo(
                this->ports[SynPEArray::WOutList[i].ToIndex()],
                SynPE::WOut)) /** WOut  8-bit out **/
                return false;
        for (int i=0;i<  this->width;i++)
            if(!this->peArray[this->height-1][i].SameTo(
                this->ports[SynPEArray::OOutList[i].ToIndex()],
                SynPE::OOut)) /** OOut  1-bit out **/
                return false;
        /** -------- right ------- **/
        for (int i=0;i< this->height;i++)
            if(!this->peArray[i][this->width-1].SameTo(
                this->ports[SynPEArray::XOutList[i].ToIndex()],
                SynPE::XOut)) /** XOut  8-bit out **/
                return false;
        /** -------- ddown ------- **/
        for (int i=0;i<this->width;i++){
            if(!this->peArray[this->height-2][i].SameTo(
                this->ports[SynPEArray::SOutList[ 2*i ].ToIndex()],
                SynPE::SOut)  /** SOut 24-bit out **/
             ||!this->peArray[this->height-1][i].SameTo(
                this->ports[SynPEArray::SOutList[2*i+1].ToIndex()],
                SynPE::SOut)) /** SOut 24-bit out **/
                return false;
        }
        for (int i=0;i<this->width;i++){
            if(!this->peArray[this->height-2][i].SameTo(
                this->ports[SynPEArray::SEnOList[ 2*i ].ToIndex()],
                SynPE::SEnO)  /** SEnO  1-bit out **/
             ||!this->peArray[this->height-1][i].SameTo(
                this->ports[SynPEArray::SEnOList[2*i+1].ToIndex()],
                SynPE::SEnO)) /** SEnO  1-bit out **/
                return false;
        }
        for (int i=0;i<this->width;i++){
            if(!this->peArray[this->height-2][i].SameTo(
                this->ports[SynPEArray::FOutList[ 2*i ].ToIndex()],
                SynPE::FOut)  /** FOut  1-bit out **/
             ||!this->peArray[this->height-1][i].SameTo(
                this->ports[SynPEArray::FOutList[2*i+1].ToIndex()],
                SynPE::FOut)) /** FOut  1-bit out **/
                return false;
        }
        return true;
    }
    bool InnerConnected() const override{
        for (int h=0;h<this->height;h++)
            for (int w=0;w<this->width;w++)
                if (!this->peArray[h][w].InnerConnected())
                    return false;
        return true;
    }
    inline bool atPosEdge() override{
        for (int h=0;h<this->height;h++)
            for (int w=0;w<this->width;w++){
                this->peArray[h][w].atPosEdge();
            }
        return true;
    }
    inline bool atNegEdge() override{
        return false;
    }
    inline bool Run() override{
        for (int h=0;h<this->height;h++)
            for (int w=0;w<this->width;w++)
                this->peArray[h][w].Run();
        return false;
    }
    inline void PrintW(){
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<"weight:"<<std::endl;
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(SynPEArray::peStatefp,"weight:\n");
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        for (int h=0;h<height;h++){
            for (int w=0;w<width;w++)
                this->peArray[h][w].PrintW(SynPEArray::peStatefp);
            #ifdef PRINT_RUNTIME_INFO_TO_STD
            std::cout<<std::endl;
            #endif // PRINT_RUNTIME_INF O_TO_STD
            #ifdef PRINT_RUNTIME_INFO_TO_FILE
            fprintf(SynPEArray::peStatefp,"\n");
            #endif // PRINT_RUNTIME_INFO_TO_FILE
        }
        return;
    }
    inline void PrintO() const{
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<"OutMark:"<<std::endl;
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(SynPEArray::peStatefp,"OutMark:\n");
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        for (int h=0;h<height;h++){
            for (int w=0;w<width;w++)
                this->peArray[h][w].PrintO(SynPEArray::peStatefp);
            #ifdef PRINT_RUNTIME_INFO_TO_STD
            std::cout<<std::endl;
            #endif // PRINT_RUNTIME_INFO_TO_STD
            #ifdef PRINT_RUNTIME_INFO_TO_FILE
            fprintf(SynPEArray::peStatefp,"\n");
            #endif // PRINT_RUNTIME_INFO_TO_FILE
        }
        return;
    }
    inline void PrintX() const{
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<"Feature:"<<std::endl;
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(SynPEArray::peStatefp,"Feature:\n");
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        for (int h=0;h<height;h++){
            for (int w=0;w<width;w++)
                this->peArray[h][w].PrintX(SynPEArray::peStatefp);
            #ifdef PRINT_RUNTIME_INFO_TO_STD
            std::cout<<std::endl;
            #endif // PRINT_RUNTIME_INFO_TO_STD
            #ifdef PRINT_RUNTIME_INFO_TO_FILE
            fprintf(SynPEArray::peStatefp,"\n");
            #endif // PRINT_RUNTIME_INFO_TO_FILE
        }
        return;
    }
    inline void PrintP() const{
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<"ParSum:"<<std::endl;
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(SynPEArray::peStatefp,"ParSum:\n");
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        for (int h=0;h<height;h++){
            for (int w=0;w<width;w++)
                this->peArray[h][w].PrintP(SynPEArray::peStatefp);
            #ifdef PRINT_RUNTIME_INFO_TO_STD
            std::cout<<std::endl;
            #endif // PRINT_RUNTIME_INFO_TO_STD
            #ifdef PRINT_RUNTIME_INFO_TO_FILE
            fprintf(SynPEArray::peStatefp,"\n");
            #endif // PRINT_RUNTIME_INFO_TO_FILE
        }
        return;
    }
    inline void PrintS() const{
        #ifdef PRINT_RUNTIME_INFO_TO_STD
        std::cout<<"Sum:"<<std::endl;
        #endif // PRINT_RUNTIME_INFO_TO_STD
        #ifdef PRINT_RUNTIME_INFO_TO_FILE
        fprintf(SynPEArray::peStatefp,"Sum:\n");
        #endif // PRINT_RUNTIME_INFO_TO_FILE
        for (int h=0;h<height;h++){
            for (int w=0;w<width;w++)
                this->peArray[h][w].PrintS(SynPEArray::peStatefp);
            #ifdef PRINT_RUNTIME_INFO_TO_STD
            std::cout<<std::endl;
            #endif // PRINT_RUNTIME_INFO_TO_STD
            #ifdef PRINT_RUNTIME_INFO_TO_FILE
            fprintf(SynPEArray::peStatefp,"\n");
            #endif // PRINT_RUNTIME_INFO_TO_FILE
        }
        return;
    }
};

#endif //__SYSTEM_PE_ARRAY_HPP
