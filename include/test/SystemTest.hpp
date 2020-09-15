#ifndef __TEST_SYSTEM_TEST_HPP
#define __TEST_SYSTEM_TEST_HPP

#include <stdlib.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../base/ExpHandler.hpp"

#include "../resource/BaseCombiLogic.hpp"
#include "../resource/BaseSequLogic.hpp"

#include "../component/RU.hpp"
#include "../component/CE.hpp"
#include "../component/PE.hpp"
#include "../component/RePE.hpp"
#include "../component/SynPE.hpp"

#include "../system/PEArray.hpp"
#include "../system/RUArray.hpp"
#include "../system/S2Engine.hpp"
#include "../system/RePEArray.hpp"
#include "../system/ReS2Engine.hpp"
#include "../system/ReMixedPEArray.hpp"
#include "../system/ReMixedS2Engine.hpp"

#define SA_XIN_FOLDER_4_4 "./data/tb1_4_4/SA_XIn/"
#define SA_WIN_FOLDER_4_4 "./data/tb1_4_4/SA_WIn/"
#define SA_OUT_FOLDER_4_4 "./data/tb1_4_4/SA_Out/"

#define SA_XIN_TRANS_FILE_PATH_PREFIX string("SA_XIn/SA_XIn_Trans_0_")
#define SA_WIN_TRANS_FILE_PATH_PREFIX string("SA_WIn/SA_WIn_Trans_")
#define    SA_OUTPUT_FILE_PATH_PREFIX string("SA_Out/SA_Output_")
#define        TRANS_FILE_TYPE        string(".txt")

#define  RU_LIN_TRANS_FILE_PATH_PREFIX string("RU_LIn/RU_LIn_")
#define  RU_XIN_TRANS_FILE_PATH_PREFIX string("RU_XIn/RU_XIn_")
#define RU_XOUT_TRANS_FILE_PATH_PREFIX string("RU_XOut/SA_XIn_Trans_0_")

#define      W_OFF_IN_FILE_PATH_PREFIX string("RU_LIn/RU_LIn_")
#define      W_OUT_IN_FILE_PATH_PREFIX string("SA_WIn/SA_WIn_Trans_")
#define X_REFORMED_IN_FILE_PATH_PREFIX string("SA_XIn_reformed/SA_XIn_")
#define     SA_OUTPUT_FILE_PATH_PREFIX string("SA_Out/SA_Output_")


#define  REFORMED_PE_OW_IN_FILE_PATH_PREFIX string( "ow_in")
#define   REFORMED_PE_X_IN_FILE_PATH_PREFIX string(  "x_in")
#define   REFORMED_PE_S_IN_FILE_PATH_PREFIX string(  "s_in")
#define REFORMED_PE_OW_OUT_FILE_PATH_PREFIX string("ow_out")
#define  REFORMED_PE_X_OUT_FILE_PATH_PREFIX string( "x_out")
#define  REFORMED_PE_S_OUT_FILE_PATH_PREFIX string( "s_out")

#define  ANALYZE_OUT_FILE_PATH string( "analyze")
#define  MIXED_ANALYZE_OUT_FILE_PATH string( "analyze_mixed")

#define PRINT_PROCESS


class ReMixedS2EngineTest{
private:
    bool BasicTest(int height  ,int width,
                   int ruPeriod,int macPeriod,int cePeriod,
                   int baseFreq,double equalWorkLoad,
                   string pathPrefix){

        Object::CounterType WGlobReadBits=0,
                            XGlobReadBits=0;

        Reg           ::ResetStatis();
        FIFO          ::ResetStatis();
        ShiftAdder    ::ResetStatis();
        SignCtrlMulter::ResetStatis();
        Comparer      ::ResetStatis();

        ReMixedS2Engine reMixedS2Engine(height,width,ruPeriod,macPeriod,cePeriod);

        vector<queue<Object::PortDataType> >
           owInData(  width,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
          xelInData( height,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
           sOutData(2*width,queue<Object::PortDataType>());

        /** ---------- up -------- **/
        vector<Object::Port>    pOWInUpList; /**  O+W+V+Off+E-bit in  **/
        pOWInUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
              pOWInUpList.emplace_back(ReMixedPE::   OUT_BIT_WIDTH
                                    +  ReMixedPE::     W_BIT_WIDTH
                                    +  ReMixedPE::    WE_BIT_WIDTH
                                    +  ReMixedPE::OFFSET_BIT_WIDTH
                                    +  ReMixedPE::  MARK_BIT_WIDTH,
                                         Object::PortDirection::OUT);

        vector<Object::Port>  pOWPushUpList; /**            1-bit in  **/
        pOWPushUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
            pOWPushUpList.emplace_back(1,Object::PortDirection::OUT);

        vector<Object::Port>  pOWFullUpList; /**            1-bit out **/
        pOWFullUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
            pOWFullUpList.emplace_back(1,Object::PortDirection::IN );

        /** -------- left -------- **/
        vector<Object::Port>    pXELInList; /**    X+V+Off+E-bit in  **/
        pXELInList.reserve(height);
        for (uint32_t h=0;h<height;h++)
              pXELInList.emplace_back(ReMixedPE::  MARK_BIT_WIDTH
                                   +  ReMixedPE::     X_BIT_WIDTH
                                   +  ReMixedPE::    XE_BIT_WIDTH
                                   +  ReMixedPE::OFFSET_BIT_WIDTH
                                   +  1,///CE input, to tell the different between control and data
                                        Object::PortDirection::OUT);

        vector<Object::Port> pXELEmptyList; /**            1-bit in  **/
        pXELEmptyList.reserve(height);
        for (uint32_t h=0;h<height;h++)
            pXELEmptyList.emplace_back(1,Object::PortDirection::OUT);

        vector<Object::Port>   pXELPopList; /**            1-bit out **/
        pXELPopList.reserve(height);
        for (uint32_t h=0;h<height;h++)
            pXELPopList.emplace_back(1,Object::PortDirection::IN );

        /** ------ down-down ----- **/
        vector<Object::Port>  pSOutDDownList; /**        S+E-bit out **/
        pSOutDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
              pSOutDDownList.emplace_back(ReMixedPE:: S_BIT_WIDTH
                                       +  ReMixedPE::SE_BIT_WIDTH,
                                            Object::PortDirection::IN );

        vector<Object::Port> pSPushDDownList; /**          1-bit out **/
        pSPushDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
             pSPushDDownList.emplace_back(1,Object::PortDirection::IN );

        vector<Object::Port> pSFullDDownList; /**          1-bit in  **/
        pSFullDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
             pSFullDDownList.emplace_back(1,Object::PortDirection::OUT);

        /** connect to PE array **/
        /** ---------- up -------- **/
        vector<Object::PortDataType>     owInUpList( width ,0); /**  O+W+V+Off+E-bit in  **/
        vector<Object::PortDataType>   owPushUpList( width ,0); /**            1-bit in  **/
        /** -------- ddown ------- **/
        vector<Object::PortDataType> sFullDDownList(2*width,0); /**            1-bit in  **/
        /** connect to CE array **/
        /** --------- left ------- **/
        vector<Object::PortDataType>     xelInList( height,0); /**  O+W+V+Off+E-bit in  **/
        vector<Object::PortDataType>  xelEmptyList( height,0); /**            1-bit in  **/

        std::cout<<"#1#"<<std::endl;
        /** signal ports **/
        for (uint32_t w=0;w<width;w++){
              pOWInUpList[w].Clone(    owInUpList[w]);
            pOWPushUpList[w].Clone(  owPushUpList[w]);
        }
        std::cout<<"#2#"<<std::endl;
        for (uint32_t h=0;h<height;h++){
               pXELInList[h].Clone(   xelInList[h]);
            pXELEmptyList[h].Clone(xelEmptyList[h]);
        }
        std::cout<<"#3#"<<std::endl;
        for (uint32_t w=0;w<2*width;w++)
            pSFullDDownList[w].Clone(sFullDDownList[w]);

        /** ---------- up -------- **/
        std::cout<<"#4#"<<std::endl;
        for (uint32_t w=0;w<width;w++){
            reMixedS2Engine.ConnectTo(reMixedS2Engine.  OWInUpList[w],
                                                       pOWInUpList[w]);
            reMixedS2Engine.ConnectTo(reMixedS2Engine.OWPushUpList[w],
                                                     pOWPushUpList[w]);
            reMixedS2Engine.ConnectTo(          pOWFullUpList[w],
                                 reMixedS2Engine.OWFullUpList[w]);
        }
        /** --------- left ------- **/
        std::cout<<"#5#"<<std::endl;
        for (uint32_t h=0;h<height;h++){
            reMixedS2Engine.ConnectTo(reMixedS2Engine.XELInList[h],
                                                     pXELInList[h]);
            reMixedS2Engine.ConnectTo(reMixedS2Engine.XELEmptyList[h],
                                                     pXELEmptyList[h]);
            reMixedS2Engine.ConnectTo(          pXELPopList[h],
                                 reMixedS2Engine.XELPopList[h]);
        }
        /** -------- ddown ------- **/
        std::cout<<"#6#"<<std::endl;
        for (uint32_t w=0;w<2*width;w++){
            reMixedS2Engine.ConnectTo(reMixedS2Engine.SFullDDownList[w],
                                                     pSFullDDownList[w]);
            reMixedS2Engine.ConnectTo(          pSPushDDownList[w],
                                 reMixedS2Engine.SPushDDownList[w]);
            reMixedS2Engine.ConnectTo(           pSOutDDownList[w],
                                 reMixedS2Engine. SOutDDownList[w]);
        }

        /** ##  load data  ## **/
        std::cout<<"#7#"<<std::endl;
        for (uint32_t w=0;w<width;w++){
            FILE* fpMAC = fopen(string(pathPrefix
                                  + W_OUT_IN_FILE_PATH_PREFIX
                                  + String::NumToString(w)
                                  + TRANS_FILE_TYPE).c_str(),"r");
            Object::PortDataType tempOffset,tempEOW,tempWeight,tempOutMark,tempMark;
            if (fpMAC==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            int counter = 0;
            while (fscanf(fpMAC,"%d%d%d%d%d",&tempWeight,&tempOutMark,&tempOffset,&tempEOW,&tempMark)>0){
                assert(!tempOutMark || tempEOW);
                tempWeight &= Object::mask[ReMixedPE::W_BIT_WIDTH];
                owInData[w].push(ReMixedPE::CompressOWEO(tempOutMark,tempWeight,tempEOW,tempOffset,tempMark));
                counter++;
            }
            WGlobReadBits += (ReMixedPE::   OUT_BIT_WIDTH
                            + ReMixedPE::     W_BIT_WIDTH
                            + ReMixedPE::    WE_BIT_WIDTH
                            + ReMixedPE::  MARK_BIT_WIDTH
                            + ReMixedPE::OFFSET_BIT_WIDTH) * owInData[w].size();
            fclose(fpMAC);
        }
        for (uint32_t w=0;w<width;w++)
            for (uint32_t h=0;h<height;h++)
                owInData[w].push(ReMixedPE::CompressOWEO(0,0,1,0,0));
        std::cout<<"#8#"<<std::endl;
        for (int i=0;i<height;i++){
            FILE* fp = fopen(string(pathPrefix
                                  + RU_XIN_TRANS_FILE_PATH_PREFIX
                                  + String::NumToString(i)
                                  + string("_MF")
                                  + TRANS_FILE_TYPE).c_str(),"r");
            if (fp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            int dataType;
            while (fscanf(fp,"%d",&dataType)>0){
                int tempCmd,tempVal,
                    tempLoc,tempEOX,tempBitMark;
                assert(dataType==0 || dataType==1);
                if (dataType!=0 && dataType!=1){
                    assert(false);
                    return false;
                }
                if (dataType == 0){
                    /// command
                    fscanf(fp,"%d",&tempCmd);
                    if (tempCmd<0 || tempCmd>=MixedCE::CtrlNum){
                        assert(false);
                        return false;
                    }
                    xelInData[i].push(MixedCE::Encode(MixedCE::InsTable[tempCmd]));
                }
                else{
                    /// data
                    fscanf(fp,"%d%d%d%d",&tempVal,&tempLoc,&tempEOX,&tempBitMark);
                    if (tempLoc<0
                     ||(tempEOX!=0 && tempEOX!=1)){
                        assert(false);
                        return false;
                    }
                    xelInData[i].push(MixedCE::CompressXEO(tempVal,tempEOX,tempLoc,tempBitMark));
                }
            }
            XGlobReadBits += (ReMixedPE::     X_BIT_WIDTH
                            + ReMixedPE::    XE_BIT_WIDTH
                            + ReMixedPE::OFFSET_BIT_WIDTH
                            + ReMixedPE::  MARK_BIT_WIDTH
                            + 1) * xelInData[i].size();
            fclose(fp);
        }
        for (int h=0;h<height;h++){
            xelInData[h].push(MixedCE::Encode(MixedCE::CE_CTRL::BZ));
            xelInData[h].push(MixedCE::Encode(MixedCE::CE_CTRL::BZ));
        }
        std::cout<<"#9#"<<std::endl;
        for (uint32_t i=0;i<width;i++){
            FILE *outfp = fopen(string(pathPrefix
                                     + SA_OUTPUT_FILE_PATH_PREFIX
                                     + String::NumToString(i)
                                     + TRANS_FILE_TYPE).c_str(),"r");
            if (outfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOut0,tempOut1;
            while (fscanf(outfp,"%d%d",&tempOut0,&tempOut1)>0){
                sOutData[2*i  ].push(tempOut0);
                sOutData[2*i+1].push(tempOut1);
            }
            fclose(outfp);
        }

        reMixedS2Engine.InnerConnect();
        assert(reMixedS2Engine.Connected());

        uint32_t clock = 0;

        while (true){
            if (clock %  ruPeriod == 0){
                #ifdef PRINT_PROCESS
                if (clock % 10000 == 0){
                    std::cout<<"@clock"<<clock<<std::endl;
                    for (int w=0;w<  width;w++)
                        std::cout<< owInData[w].size()<<" ";
                    std::cout<<std::endl;
                    for (int h=0;h< height;h++)
                        std::cout<<xelInData[h].size()<<" ";
                    std::cout<<std::endl;
                    for (int w=0;w<2*width;w++)
                        std::cout<< sOutData[w].size()<<" ";
                    std::cout<<std::endl;
                }
                #endif // PRINT_PROCESS

                for (uint32_t w=0;w<width;w++)
                    if (pOWFullUpList[w].Data()==0
                    && !owInData  [w].empty()){
                        owInUpList[w] = owInData[w].front();
                        owInData  [w].pop();
                        owPushUpList[w] = 1;
                    }
                    else
                        owPushUpList[w] = 0;

                for (int i=0;i<height;i++){
                    /** ----- left ----- **/
                    if (xelInData[i].empty()){
                        if (pXELPopList[i].Data()==1){
                            assert(false);
                            return false;
                        }
                        xelInList[i]  = 233;
                        xelEmptyList[i] = 1;
                    }
                    else{
                        if (pXELPopList[i].Data()==1){
                            xelInData[i].pop();
                        }
                        if (xelInData[i].empty()){
                            xelInList[i] = 233;
                            xelEmptyList[i] = 1;
                        }
                        else{
                            xelInList[i] = xelInData[i].front();/** 2+X_BIT+XL_BIT  in  **/
                            xelEmptyList[i] = 0;
                        }
                    }
                }

                bool allEnd = true;
                for (uint32_t w=0;w<2*width;w++)
                    if (pSPushDDownList[w].Data()==1){
                        if (sOutData[w].empty()){
                            assert(false);
                            continue;
                        }
                        allEnd = false;
                        if (((pSOutDDownList[w].SignedData() - (pSOutDDownList[w].SignedData() & 1))/2) != sOutData[w].front()){
                            std::cout<<"## sOut error! ##"<<std::endl;
                            std::cout<<"@"<<w<<" rest:"<<sOutData[w].size()<<std::endl;
                            assert(false);
                            return false;
                        }
                        else
                            sOutData[w].pop();
                    }
                    else
                        if (!sOutData[w].empty())
                            allEnd = false;

                if (allEnd)
                    break;
            }

            reMixedS2Engine.atPosEdge();
            reMixedS2Engine.Run();
//            reMixedS2Engine.PrintFIFOState();
            clock++;
        }

        std::cout<<"total clock used      :"<<clock   <<std::endl;
        std::cout<<"equivalent speed(Tops):"<<((1.0*equalWorkLoad)/clock*baseFreq)<<std::endl;

        FILE* analFp = fopen((pathPrefix
                            + MIXED_ANALYZE_OUT_FILE_PATH
                            + string("_S2Engine_")
                            + string("_" ) + String::NumToString(height)
                            + string("x" ) + String::NumToString( width)
                            + string("_" ) + String::NumToString( ruPeriod)
                            + string("-" ) + String::NumToString(macPeriod)
                            + string("_p" )+ String::NumToString(ReMixedPE::S_BIT_WIDTH)
                            + string("_w" )+ String::NumToString(ReMixedPE::wFIFO_LEN)
                            + string("_x" )+ String::NumToString(ReMixedPE::xFIFO_LEN)
                            + string("_wx")+ String::NumToString(ReMixedPE::owxFIFO_LEN)
                            + string("_s" )+ String::NumToString(ReMixedPE::sFIFO_LEN)
                            + string("_@") + String::NumToString(baseFreq)
                            + TRANS_FILE_TYPE).c_str(),"w+");

        fprintf(analFp,"###########################\n");
        fprintf(analFp,"    Performance analyze    \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"total clock used      :\t%d\n",clock);
        fprintf(analFp,"equivalent speed(Tops):\t%f\n",((1.0*equalWorkLoad)/clock*baseFreq));
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"       Power analyze       \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"Global buffer activation statistics:\n");
        fprintf(analFp,"\t   total      global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits+XGlobReadBits).c_str());
        fprintf(analFp,"\t  weight   global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits).c_str());
        fprintf(analFp,"\tactivation global buffer read bits:\t%s\n",String::NumToString(XGlobReadBits).c_str());
        Reg            :: PrintPower (analFp);
        FIFO           :: PrintPower (analFp);
        ShiftAdder     :: PrintPower (analFp);
        SignCtrlMulter :: PrintPower (analFp);
        Comparer       :: PrintPower (analFp);
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"        Area analyze       \n");
        fprintf(analFp,"###########################\n");
        Reg            :: PrintArea (analFp);
        FIFO           :: PrintArea (analFp);
        ShiftAdder     :: PrintArea (analFp);
        SignCtrlMulter :: PrintArea (analFp);
        Comparer       :: PrintArea (analFp);
        fclose(analFp);
        return true;
    }

public:
    void TestLayer(const string& layer = "./"){
        std::ifstream ifs;
        double workload;
        ifs.open(layer+"../workload.txt");
        if (!ifs){
            std::cout<<"Open file error! ["<<layer<<"]"<<std::endl;
            return;
        }
        ifs>>workload;
        ifs.close();
        if (BasicTest( 16, 16,1,4,1,2800,workload,layer))
            std::cout<<layer+" test passed"<<std::endl;
        else
            std::cout<<layer+" test failed"<<std::endl;
        return;
    }

    ReMixedS2EngineTest(){
        return;
    }

    void TestVGG16(){
        this->TestLayer("VGG16/conv1_1");
        this->TestLayer("VGG16/conv1_2");
        this->TestLayer("VGG16/conv2_1");
        this->TestLayer("VGG16/conv2_2");
        this->TestLayer("VGG16/conv3_1");
        this->TestLayer("VGG16/conv3_2");
        this->TestLayer("VGG16/conv3_3");
        this->TestLayer("VGG16/conv4_1");
        this->TestLayer("VGG16/conv4_2");
        this->TestLayer("VGG16/conv4_3");
        this->TestLayer("VGG16/conv5_1");
        this->TestLayer("VGG16/conv5_2");
        this->TestLayer("VGG16/conv5_3");
        return;
    }

    void TestAlex(){
        this->TestLayer("AlexNet/conv_0/transed/");
        this->TestLayer("AlexNet/conv_1/transed/");
        this->TestLayer("AlexNet/conv_2/transed/");
        this->TestLayer("AlexNet/conv_3/transed/");
        this->TestLayer("AlexNet/conv_4/transed/");

//        this->TestLayer("./conv_0");
//        this->TestLayer("./conv_1");
//        this->TestLayer("./conv_2");
//        this->TestLayer("./conv_3");
//        this->TestLayer("./conv_4");
        return;
    }
};


class ReMixedPEArrayTest{
private:
    inline void OutputNotMatch(
            Object::PortDataType actData,
            Object::PortDataType stdData
        ){
        cout<<"## Output Error ## should be: "<<stdData
                            <<" actually is: "<<actData
                            <<endl;
    }

    bool BasicTest(
            uint32_t height,uint32_t width ,
            uint32_t ruPer ,uint32_t macPer,
            int baseFreq   ,double equalWorkLoad,
            string pathPrefix
        ) {
        Object::CounterType WGlobReadBits=0,
                            XGlobReadBits=0;

        Reg           ::ResetStatis();
        FIFO          ::ResetStatis();
        ShiftAdder    ::ResetStatis();
        SignCtrlMulter::ResetStatis();
        Comparer      ::ResetStatis();

        ReMixedPEArray peArray(height,width,ruPer,macPer);

        vector<queue<Object::PortDataType> >
           owInData(  width,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
            xInData( height,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
           sOutData(2*width,queue<Object::PortDataType>());

        /** ---------- up -------- **/
        vector<Object::Port>    pOWInUpList; /**  O+W+V+Off+E-bit in  **/
        pOWInUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
              pOWInUpList.push_back(Object::Port(ReMixedPE::  MARK_BIT_WIDTH
                                              +  ReMixedPE::   OUT_BIT_WIDTH
                                              +  ReMixedPE::     W_BIT_WIDTH
                                              +  ReMixedPE::    WE_BIT_WIDTH
                                              +  ReMixedPE::OFFSET_BIT_WIDTH,
                                                    Object::PortDirection::OUT));

        vector<Object::Port>  pOWPushUpList; /**            1-bit in  **/
        pOWPushUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
            pOWPushUpList.push_back(Object::Port(1,Object::PortDirection::OUT));

        vector<Object::Port>  pOWFullUpList; /**            1-bit out **/
        pOWFullUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
            pOWFullUpList.push_back(Object::Port(1,Object::PortDirection::IN ));

        /** -------- left -------- **/
        vector<Object::Port>   pXInLeftList; /**    X+V+Off+E-bit in  **/
        pXInLeftList.reserve(height);
        for (uint32_t h=0;h<height;h++)
              pXInLeftList.push_back(Object::Port(ReMixedPE::  MARK_BIT_WIDTH
                                               +  ReMixedPE::     X_BIT_WIDTH
                                               +  ReMixedPE::    XE_BIT_WIDTH
                                               +  ReMixedPE::OFFSET_BIT_WIDTH,
                                                     Object::PortDirection::OUT));

        vector<Object::Port> pXPushLeftList; /**            1-bit in  **/
        pXPushLeftList.reserve(height);
        for (uint32_t h=0;h<height;h++)
            pXPushLeftList.push_back(Object::Port(1,Object::PortDirection::OUT));

        vector<Object::Port> pXFullLeftList; /**            1-bit out **/
        pXFullLeftList.reserve(height);
        for (uint32_t h=0;h<height;h++)
            pXFullLeftList.push_back(Object::Port(1,Object::PortDirection::IN ));

        /** -------- down-down ------- **/
        vector<Object::Port>  pSOutDDownList; /**        S+E-bit out **/
        pSOutDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
              pSOutDDownList.push_back(Object::Port(ReMixedPE:: S_BIT_WIDTH
                                                 +  ReMixedPE::SE_BIT_WIDTH,
                                                      Object::PortDirection::IN ));

        vector<Object::Port> pSPushDDownList; /**          1-bit out **/
        pSPushDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
             pSPushDDownList.push_back(Object::Port(1,Object::PortDirection::IN ));

        vector<Object::Port> pSFullDDownList; /**          1-bit in  **/
        pSFullDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
             pSFullDDownList.push_back(Object::Port(1,Object::PortDirection::OUT));

        /** ---------- up -------- **/
        vector<Object::PortDataType>     owInUpList( width ,0); /**  O+W+V+Off+E-bit in  **/
        vector<Object::PortDataType>   owPushUpList( width ,0); /**            1-bit in  **/
        /** --------- left ------- **/
        vector<Object::PortDataType>    xInLeftList( height,0); /**  O+W+V+Off+E-bit in  **/
        vector<Object::PortDataType>  xPushLeftList( height,0); /**            1-bit in  **/
        /** -------- ddown ------- **/
        vector<Object::PortDataType> sFullDDownList(2*width,0); /**            1-bit in  **/

        for (uint32_t w=0;w<width;w++){
              pOWInUpList[w].Clone(    owInUpList[w]);
            pOWPushUpList[w].Clone(  owPushUpList[w]);
        }
        for (uint32_t h=0;h<height;h++){
              pXInLeftList[h].Clone(  xInLeftList[h]);
            pXPushLeftList[h].Clone(xPushLeftList[h]);
        }
        for (uint32_t w=0;w<2*width;w++)
            pSFullDDownList[w].Clone(sFullDDownList[w]);

        /** ---------- up -------- **/
        for (uint32_t w=0;w<width;w++){
            peArray.ConnectTo(peArray.  OWInUpList[w],
                                       pOWInUpList[w]);
            peArray.ConnectTo(peArray.OWPushUpList[w],
                                     pOWPushUpList[w]);
            peArray.ConnectTo(       pOWFullUpList[w],
                              peArray.OWFullUpList[w]);
        }
        /** --------- left ------- **/
        for (uint32_t h=0;h<height;h++){
            peArray.ConnectTo(peArray.  XInLeftList[h],
                                       pXInLeftList[h]);
            peArray.ConnectTo(peArray.XPushLeftList[h],
                                     pXPushLeftList[h]);
            peArray.ConnectTo(       pXFullLeftList[h],
                              peArray.XFullLeftList[h]);
        }
        /** -------- ddown ------- **/
        for (uint32_t w=0;w<2*width;w++){
            peArray.ConnectTo(peArray.SFullDDownList[w],
                                     pSFullDDownList[w]);
            peArray.ConnectTo(       pSPushDDownList[w],
                              peArray.SPushDDownList[w]);
            peArray.ConnectTo(        pSOutDDownList[w],
                              peArray. SOutDDownList[w]);
        }

        peArray.InnerConnect();
        assert(peArray.Connected());

        for (uint32_t w=0;w<width;w++){
            FILE* fpMAC = fopen(string(pathPrefix
                                  + W_OUT_IN_FILE_PATH_PREFIX
                                  + String::NumToString(w)
                                  + TRANS_FILE_TYPE).c_str(),"r");
            Object::PortDataType tempOffset,tempEOW,tempWeight,tempOutMark,tempMark;
            if (fpMAC==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            int counter = 0;
            while (fscanf(fpMAC,"%d%d%d%d%d",&tempWeight,&tempOutMark,&tempOffset,&tempEOW,&tempMark)>0){
                assert(!tempOutMark || tempEOW);
                tempWeight &= Object::mask[ReMixedPE::W_BIT_WIDTH];
                owInData[w].push(ReMixedPE::CompressOWEO(tempOutMark,tempWeight,tempEOW,tempOffset,tempMark));
                counter++;
            }
            WGlobReadBits += (ReMixedPE::  MARK_BIT_WIDTH
                            + ReMixedPE::   OUT_BIT_WIDTH
                            + ReMixedPE::     W_BIT_WIDTH
                            + ReMixedPE::    WE_BIT_WIDTH
                            + ReMixedPE::OFFSET_BIT_WIDTH) * owInData[w].size();
            fclose(fpMAC);
        }
        for (uint32_t h=0;h<height;h++){
            FILE* fp =  fopen(string(pathPrefix
                                + X_REFORMED_IN_FILE_PATH_PREFIX
                                + String::NumToString(h)
                                + TRANS_FILE_TYPE).c_str(),"r");
            if (fp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOffset,tempEOX,tempX,tempMark;
            while (fscanf(fp,"%d%d%d%d",&tempX,&tempOffset,&tempEOX,&tempMark)>0)
                xInData[h].push(ReMixedPE::CompressXEO(tempX,tempEOX,tempOffset,tempMark));
            XGlobReadBits += (ReMixedPE::  MARK_BIT_WIDTH
                            + ReMixedPE::     X_BIT_WIDTH
                            + ReMixedPE::    XE_BIT_WIDTH
                            + ReMixedPE::OFFSET_BIT_WIDTH) * xInData[h].size();
            fclose(fp);
        }
        for (uint32_t w=0;w<width;w++){
            FILE* outfp = fopen(string(pathPrefix
                                + SA_OUTPUT_FILE_PATH_PREFIX
                                + String::NumToString(w)
                                + TRANS_FILE_TYPE).c_str(),"r");
            if (outfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOut0,tempOut1;
            while (fscanf(outfp,"%d%d",&tempOut0,&tempOut1)>0){
                sOutData[2*w  ].push(tempOut0);
                sOutData[2*w+1].push(tempOut1);
            }
            fclose(outfp);
        }
        for (uint32_t w=0;w<width;w++)
            for (uint32_t h=0;h<height;h++)
                owInData[w].push(ReMixedPE::CompressOWEO(0,0,1,0,0));

        for (uint32_t h=0;h<height;h++)
            for (uint32_t w=0;w<width;w++)
                xInData[h].push(ReMixedPE::CompressXEO(0,1,0,0));

        uint32_t clock =0;
        while (true){
            if (clock %  ruPer == 0){
                #ifdef PRINT_PROCESS
                if (clock % 100000 == 0){
                    std::cout<<"@clock"<<clock<<std::endl;
                    for (int w=0;w<  width;w++)
                        std::cout<<owInData[w].size()<<" ";
                    std::cout<<std::endl;
                    for (int h=0;h< height;h++)
                        std::cout<< xInData[h].size()<<" ";
                    std::cout<<std::endl;
                    for (int w=0;w<2*width;w++)
                        std::cout<<sOutData[w].size()<<" ";
                    std::cout<<std::endl;
                }
                #endif // PRINT_PROCESS

                for (uint32_t w=0;w<width;w++)
                    if (pOWFullUpList[w].Data()==0
                    && !owInData  [w].empty()){
                        owInUpList[w] = owInData[w].front();
                        owInData  [w].pop();
                        owPushUpList[w] = 1;
                    }
                    else
                        owPushUpList[w] = 0;

                for (uint32_t h=0;h<height;h++)
                    if (pXFullLeftList[h].Data()==0
                    && !xInData    [h].empty()){
                        xInLeftList[h] = xInData[h].front();
                        xInData    [h].pop();
                        xPushLeftList[h] = 1;
                    }
                    else
                        xPushLeftList[h] = 0;

                bool allEnd = true;
                for (uint32_t w=0;w<2*width;w++)
                    if (pSPushDDownList[w].Data()==1){
                        if (sOutData[w].empty()){
                            assert(false);
                            continue;
                        }
                        allEnd = false;
                        if ((pSOutDDownList[w].SignedData()>>1) != sOutData[w].front()){
                            std::cout<<"## sOut error! ##"<<std::endl;
                            std::cout<<"@"<<w<<" rest:"<<sOutData[w].size()<<std::endl;
                            this->OutputNotMatch(pSOutDDownList[w].SignedData(),sOutData[w].front());
                            assert(false);
                            return false;
                        }
                        else
                            sOutData[w].pop();
                    }
                    else
                        if (!sOutData[w].empty())
                            allEnd = false;
                if (allEnd)
                    break;
            }
            peArray.atPosEdge();
            peArray.Run();
            clock++;
        }
        std::cout<<"total clock used      :"<<clock   <<std::endl;
        std::cout<<"equivalent speed(Tops):"<<((1.0*equalWorkLoad)/clock*baseFreq)<<std::endl;

        FILE* analFp = fopen((pathPrefix
                            + MIXED_ANALYZE_OUT_FILE_PATH
                            + string("_" ) + String::NumToString(height)
                            + string("x" ) + String::NumToString( width)
                            + string("_" ) + String::NumToString( ruPer)
                            + string("-" ) + String::NumToString(macPer)
                            + string("_p" )+ String::NumToString(ReMixedPE::S_BIT_WIDTH)
                            + string("_w" )+ String::NumToString(ReMixedPE::wFIFO_LEN)
                            + string("_x" )+ String::NumToString(ReMixedPE::xFIFO_LEN)
                            + string("_wx")+ String::NumToString(ReMixedPE::owxFIFO_LEN)
                            + string("_s" )+ String::NumToString(ReMixedPE::sFIFO_LEN)
                            + string("_@") + String::NumToString(baseFreq)
                            + TRANS_FILE_TYPE).c_str(),"w+");

        fprintf(analFp,"###########################\n");
        fprintf(analFp,"    Performance analyze    \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"total clock used      :\t%d\n",clock);
        fprintf(analFp,"equivalent speed(Tops):\t%f\n",((1.0*equalWorkLoad)/clock*baseFreq));
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"       Power analyze       \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"Global buffer activation statistics:\n");
        fprintf(analFp,"\t   total      global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits+XGlobReadBits).c_str());
        fprintf(analFp,"\t  weight   global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits).c_str());
        fprintf(analFp,"\tactivation global buffer read bits:\t%s\n",String::NumToString(XGlobReadBits).c_str());
        Reg            :: PrintPower (analFp);
        FIFO           :: PrintPower (analFp);
        ShiftAdder     :: PrintPower (analFp);
        SignCtrlMulter :: PrintPower (analFp);
        Comparer       :: PrintPower (analFp);
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"        Area analyze       \n");
        fprintf(analFp,"###########################\n");
        Reg            :: PrintArea (analFp);
        FIFO           :: PrintArea (analFp);
        ShiftAdder     :: PrintArea (analFp);
        SignCtrlMulter :: PrintArea (analFp);
        Comparer       :: PrintArea (analFp);
        fclose(analFp);
        return true;
    }

    void TestLayer(const string& layer = "./"){
        std::ifstream ifs;
        int workload;
        ifs.open("./"+layer+"/workload.txt");
        if (!ifs){
            std::cout<<"Open file error! ["<<layer<<"]"<<std::endl;
            return;
        }
        ifs>>workload;
        ifs.close();
        if (BasicTest( 16, 16,1,8,5600,workload,string("./"+layer+"/transed/")))
            std::cout<<layer+" test passed"<<std::endl;
        else
            std::cout<<layer+" test failed"<<std::endl;
        return;
    }

public:
    ReMixedPEArrayTest(){
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb1_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb2_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb3_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb4_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb5_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb6_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb8_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb9_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,220.5,string("./data/ReMixedPEArray_tb11_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,220.5,string("./data/ReMixedPEArray_tb12_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,220.5,string("./data/ReMixedPEArray_tb13_16_16/")))
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed mixed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;


//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb20_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.0/0.0  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.0/0.0  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb21_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.1/0.1  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.1/0.1  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb22_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.2/0.2  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.2/0.2  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb23_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.3/0.3  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.3/0.3  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb24_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.4/0.4  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.4/0.4  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb25_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.5/0.5  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.5/0.5  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb26_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.6/0.6  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.6/0.6  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb27_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.7/0.7  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.7/0.7  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb28_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.8/0.8  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.8/0.8  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,2.53125,string("./data/ReMixedPEArray_tb29_16_16/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.9/0.9  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.9/0.9  @4:1 test failed"<<std::endl;

//        if (BasicTest( 16, 16,1,4,2800,882,string("./data/synthesis/10-10/VGG16/conv1_1/transed/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.1/0.1  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.1/0.1  @4:1 test failed"<<std::endl;
//        return;

//        if (BasicTest( 16, 16,1,4,2800,3528,string("./data/synthesis/10-10/VGG16/conv1_1/transed/")))
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.1/0.1  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"mixed PE array and scheduling & mapping strategy 16_16 with sparsity: 0.1/0.1  @4:1 test failed"<<std::endl;
//        return;
    }

    void TestVGG16(){
        this->TestLayer("VGG16/conv1_1");
        this->TestLayer("VGG16/conv1_2");
        this->TestLayer("VGG16/conv2_1");
        this->TestLayer("VGG16/conv2_2");
        this->TestLayer("VGG16/conv3_1");
        this->TestLayer("VGG16/conv3_2");
        this->TestLayer("VGG16/conv3_3");
        this->TestLayer("VGG16/conv4_1");
        this->TestLayer("VGG16/conv4_2");
        this->TestLayer("VGG16/conv4_3");
        this->TestLayer("VGG16/conv5_1");
        this->TestLayer("VGG16/conv5_2");
        this->TestLayer("VGG16/conv5_3");
        return;
    }

    void TestVGG16_1(){
        this->TestLayer("VGG16/conv1_1_10");
        this->TestLayer("VGG16/conv1_1_11");
        this->TestLayer("VGG16/conv1_1_12");
        this->TestLayer("VGG16/conv1_1_13");
        this->TestLayer("VGG16/conv1_1_14");
        this->TestLayer("VGG16/conv1_1_15");
        this->TestLayer("VGG16/conv1_1_16");
    }

    void TestAlex(){
        this->TestLayer("AlexNet/conv1");
        this->TestLayer("AlexNet/conv2");
        this->TestLayer("AlexNet/conv3");
        this->TestLayer("AlexNet/conv4");
        this->TestLayer("AlexNet/conv5");
        return;
    }
};

class ReformedPEArrayTest{
private:
    inline void OutputNotMatch(
            Object::PortDataType actData,
            Object::PortDataType stdData
        ){
        cout<<"## Output Error ## should be: "<<stdData
                            <<" actually is: "<<actData
                            <<endl;
    }

    bool BasicTest(
            uint32_t height,uint32_t width ,
            uint32_t ruPer ,uint32_t macPer,
            int baseFreq   ,double equalWorkLoad,
            string pathPrefix
        ) {
        Object::CounterType WGlobReadBits=0,
                            XGlobReadBits=0;

        Reg     ::ResetStatis();
        FIFO    ::ResetStatis();
        Adder   ::ResetStatis();
        Multer  ::ResetStatis();
        Comparer::ResetStatis();

        ReformedPEArray peArray(height,width,ruPer,macPer);

        vector<queue<Object::PortDataType> >
           owInData(  width,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
            xInData( height,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
           sOutData(2*width,queue<Object::PortDataType>());

        /** ---------- up -------- **/
        vector<Object::Port>    pOWInUpList; /**  O+W+V+Off+E-bit in  **/
        pOWInUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
              pOWInUpList.push_back(Object::Port(ReformedPE::   OUT_BIT_WIDTH
                                              +  ReformedPE::     W_BIT_WIDTH
                                              +  ReformedPE::    WE_BIT_WIDTH
                                              +  ReformedPE::OFFSET_BIT_WIDTH,
                                                   Object::PortDirection::OUT));

        vector<Object::Port>  pOWPushUpList; /**            1-bit in  **/
        pOWPushUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
            pOWPushUpList.push_back(Object::Port(1,Object::PortDirection::OUT));

        vector<Object::Port>  pOWFullUpList; /**            1-bit out **/
        pOWFullUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
            pOWFullUpList.push_back(Object::Port(1,Object::PortDirection::IN ));

        /** -------- left -------- **/
        vector<Object::Port>   pXInLeftList; /**    X+V+Off+E-bit in  **/
        pXInLeftList.reserve(height);
        for (uint32_t h=0;h<height;h++)
              pXInLeftList.push_back(Object::Port(ReformedPE::     X_BIT_WIDTH
                                               +  ReformedPE::    XE_BIT_WIDTH
                                               +  ReformedPE::OFFSET_BIT_WIDTH,
                                                    Object::PortDirection::OUT));

        vector<Object::Port> pXPushLeftList; /**            1-bit in  **/
        pXPushLeftList.reserve(height);
        for (uint32_t h=0;h<height;h++)
            pXPushLeftList.push_back(Object::Port(1,Object::PortDirection::OUT));

        vector<Object::Port> pXFullLeftList; /**            1-bit out **/
        pXFullLeftList.reserve(height);
        for (uint32_t h=0;h<height;h++)
            pXFullLeftList.push_back(Object::Port(1,Object::PortDirection::IN ));

        /** -------- down-down ------- **/
        vector<Object::Port>  pSOutDDownList; /**        S+E-bit out **/
        pSOutDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
              pSOutDDownList.push_back(Object::Port(ReformedPE:: S_BIT_WIDTH
                                                 +  ReformedPE::SE_BIT_WIDTH,
                                                      Object::PortDirection::IN ));

        vector<Object::Port> pSPushDDownList; /**          1-bit out **/
        pSPushDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
             pSPushDDownList.push_back(Object::Port(1,Object::PortDirection::IN ));

        vector<Object::Port> pSFullDDownList; /**          1-bit in  **/
        pSFullDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
             pSFullDDownList.push_back(Object::Port(1,Object::PortDirection::OUT));

        /** ---------- up -------- **/
        vector<Object::PortDataType>     owInUpList( width ,0); /**  O+W+V+Off+E-bit in  **/
        vector<Object::PortDataType>   owPushUpList( width ,0); /**            1-bit in  **/
        /** --------- left ------- **/
        vector<Object::PortDataType>    xInLeftList( height,0); /**  O+W+V+Off+E-bit in  **/
        vector<Object::PortDataType>  xPushLeftList( height,0); /**            1-bit in  **/
        /** -------- ddown ------- **/
        vector<Object::PortDataType> sFullDDownList(2*width,0); /**            1-bit in  **/

        for (uint32_t w=0;w<width;w++){
              pOWInUpList[w].Clone(    owInUpList[w]);
            pOWPushUpList[w].Clone(  owPushUpList[w]);
        }
        for (uint32_t h=0;h<height;h++){
              pXInLeftList[h].Clone(  xInLeftList[h]);
            pXPushLeftList[h].Clone(xPushLeftList[h]);
        }
        for (uint32_t w=0;w<2*width;w++)
            pSFullDDownList[w].Clone(sFullDDownList[w]);

        /** ---------- up -------- **/
        for (uint32_t w=0;w<width;w++){
            peArray.ConnectTo(peArray.  OWInUpList[w],
                                       pOWInUpList[w]);
            peArray.ConnectTo(peArray.OWPushUpList[w],
                                     pOWPushUpList[w]);
            peArray.ConnectTo(       pOWFullUpList[w],
                              peArray.OWFullUpList[w]);
        }
        /** --------- left ------- **/
        for (uint32_t h=0;h<height;h++){
            peArray.ConnectTo(peArray.  XInLeftList[h],
                                       pXInLeftList[h]);
            peArray.ConnectTo(peArray.XPushLeftList[h],
                                     pXPushLeftList[h]);
            peArray.ConnectTo(       pXFullLeftList[h],
                              peArray.XFullLeftList[h]);
        }
        /** -------- ddown ------- **/
        for (uint32_t w=0;w<2*width;w++){
            peArray.ConnectTo(peArray.SFullDDownList[w],
                                     pSFullDDownList[w]);
            peArray.ConnectTo(       pSPushDDownList[w],
                              peArray.SPushDDownList[w]);
            peArray.ConnectTo(        pSOutDDownList[w],
                              peArray. SOutDDownList[w]);
        }

        peArray.InnerConnect();
        assert(peArray.Connected());

        for (uint32_t w=0;w<width;w++){
            FILE* fpMAC = fopen(string(pathPrefix
                                  + W_OUT_IN_FILE_PATH_PREFIX
                                  + String::NumToString(w)
                                  + TRANS_FILE_TYPE).c_str(),"r");
            Object::PortDataType tempOffset,tempEOW,tempWeight,tempOutMark,tempMark;
            if (fpMAC==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            int counter = 0;
            while (fscanf(fpMAC,"%d%d%d%d%d",&tempWeight,&tempOutMark,&tempOffset,&tempEOW,&tempMark)>0){
                assert(!tempOutMark || tempEOW);
                tempWeight &= Object::mask[ReformedPE::W_BIT_WIDTH];
                owInData[w].push(ReformedPE::CompressOWEO(tempOutMark,tempWeight,tempEOW,tempOffset));
                counter++;
            }
            WGlobReadBits += (ReformedPE::   OUT_BIT_WIDTH
                            + ReformedPE::     W_BIT_WIDTH
                            + ReformedPE::    WE_BIT_WIDTH
                            + ReformedPE::OFFSET_BIT_WIDTH) * owInData[w].size();
            fclose(fpMAC);
        }
        for (uint32_t h=0;h<height;h++){
            FILE* fp =  fopen(string(pathPrefix
                                + X_REFORMED_IN_FILE_PATH_PREFIX
                                + String::NumToString(h)
                                + TRANS_FILE_TYPE).c_str(),"r");
            if (fp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOffset,tempEOX,tempX,tempMark;
            while (fscanf(fp,"%d%d%d%d",&tempX,&tempOffset,&tempEOX,&tempMark)>0)
                xInData[h].push(ReformedPE::CompressXEO(tempX,tempEOX,tempOffset));
            XGlobReadBits += (ReformedPE::     X_BIT_WIDTH
                            + ReformedPE::    XE_BIT_WIDTH
                            + ReformedPE::OFFSET_BIT_WIDTH) * xInData[h].size();
            fclose(fp);
        }
        for (uint32_t w=0;w<width;w++){
            FILE* outfp = fopen(string(pathPrefix
                                + SA_OUTPUT_FILE_PATH_PREFIX
                                + String::NumToString(w)
                                + TRANS_FILE_TYPE).c_str(),"r");
            if (outfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOut0,tempOut1;
            while (fscanf(outfp,"%d%d",&tempOut0,&tempOut1)>0){
                sOutData[2*w  ].push(tempOut0);
                sOutData[2*w+1].push(tempOut1);
            }
            fclose(outfp);
        }

/*        for (uint32_t w=0;w<width;w++){
            FILE* fpRU  = fopen(string(pathPrefix
                                  + W_OFF_IN_FILE_PATH_PREFIX
                                  + String::NumToString(w)
                                  + TRANS_FILE_TYPE).c_str(),"r");
            FILE* fpMAC = fopen(string(pathPrefix
                                  + W_OUT_IN_FILE_PATH_PREFIX
                                  + String::NumToString(w)
                                  + TRANS_FILE_TYPE).c_str(),"r");
            Object::PortDataType tempOffset,tempEOW,tempWeight,tempOutMark;
            int counter = 0;
            while (fscanf(fpRU ,"%d%d",&tempOffset,&tempEOW    )>0
                && fscanf(fpMAC,"%d%d",&tempWeight,&tempOutMark)>0){
                assert(!tempOutMark || tempEOW);
                tempWeight &= Object::mask[ReformedPE::W_BIT_WIDTH];
                owInData[w].push(ReformedPE::CompressOWEO(tempOutMark,tempWeight,tempEOW,tempOffset));
                counter++;
            }
            WGlobReadBits += (ReformedPE::   OUT_BIT_WIDTH
                            + ReformedPE::     W_BIT_WIDTH
                            + ReformedPE::    WE_BIT_WIDTH
                            + ReformedPE::OFFSET_BIT_WIDTH) * owInData[w].size();
            fclose(fpRU );
            fclose(fpMAC);
        }
        for (uint32_t h=0;h<height;h++){
            FILE* fp =  fopen(string(pathPrefix
                                + X_REFORMED_IN_FILE_PATH_PREFIX
                                + String::NumToString(h)
                                + TRANS_FILE_TYPE).c_str(),"r");

            if (fp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOffset,tempEOX,tempX;
            while (fscanf(fp,"%d%d%d",&tempX,&tempOffset,&tempEOX)>0){
                xInData[h].push(ReformedPE::CompressXEO(tempX,tempEOX,tempOffset));
            }
            XGlobReadBits += (ReformedPE::     X_BIT_WIDTH
                            + ReformedPE::    XE_BIT_WIDTH
                            + ReformedPE::OFFSET_BIT_WIDTH
                            + 1) * xInData[h].size();
            fclose(fp);
        }
        for (uint32_t w=0;w<width;w++){
            FILE* outfp = fopen(string(pathPrefix
                                + SA_OUTPUT_FILE_PATH_PREFIX
                                + String::NumToString(w)
                                + TRANS_FILE_TYPE).c_str(),"r");

            if (outfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOut0,tempOut1;
            while (fscanf(outfp,"%d%d",&tempOut0,&tempOut1)>0){
                sOutData[2*w  ].push(tempOut0);
                sOutData[2*w+1].push(tempOut1);
            }
            fclose(outfp);
        }*/


        for (uint32_t w=0;w<width;w++)
            for (uint32_t h=0;h<height;h++)
                owInData[w].push(ReformedPE::CompressOWEO(0,0,1,0));

        for (uint32_t h=0;h<height;h++)
            for (uint32_t w=0;w<width;w++)
                xInData[h].push(ReformedPE::CompressXEO(0,1,0));

        uint32_t clock =0;
        while (true){
            if (clock %  ruPer == 0){
                #ifdef PRINT_PROCESS
                if (clock % 1000 == 0){
                    std::cout<<"@clock"<<clock<<std::endl;
                    for (int w=0;w<  width;w++)
                        std::cout<<owInData[w].size()<<" ";
                    std::cout<<std::endl;
                    for (int h=0;h< height;h++)
                        std::cout<< xInData[h].size()<<" ";
                    std::cout<<std::endl;
                    for (int w=0;w<2*width;w++)
                        std::cout<<sOutData[w].size()<<" ";
                    std::cout<<std::endl;
                }
                #endif // PRINT_PROCESS

                for (uint32_t w=0;w<width;w++)
                    if (pOWFullUpList[w].Data()==0
                    && !owInData  [w].empty()){
                        owInUpList[w] = owInData[w].front();
                        owInData  [w].pop();
                        owPushUpList[w] = 1;
                    }
                    else
                        owPushUpList[w] = 0;

                for (uint32_t h=0;h<height;h++)
                    if (pXFullLeftList[h].Data()==0
                    && !xInData    [h].empty()){
                        xInLeftList[h] = xInData[h].front();
                        xInData    [h].pop();
                        xPushLeftList[h] = 1;
                    }
                    else
                        xPushLeftList[h] = 0;

                bool allEnd = true;
                for (uint32_t w=0;w<2*width;w++)
                    if (pSPushDDownList[w].Data()==1){
                        if (sOutData[w].empty()){
                            assert(false);
                            continue;
                        }
                        allEnd = false;
                        if (((pSOutDDownList[w].SignedData() - (pSOutDDownList[w].SignedData() & 1))/2) != sOutData[w].front()){
                            std::cout<<"## sOut error! ##"<<std::endl;
                            std::cout<<"@"<<w<<" rest:"<<sOutData[w].size()<<std::endl;
                            this->OutputNotMatch(pSOutDDownList[w].SignedData(),sOutData[w].front());
                            assert(false);
                            return false;
                        }
                        else
                            sOutData[w].pop();
                    }
                    else
                        if (!sOutData[w].empty())
                            allEnd = false;
                if (allEnd)
                    break;
            }
            peArray.atPosEdge();
            peArray.Run();
            clock++;
        }
        std::cout<<"total clock used      :"<<clock   <<std::endl;
        std::cout<<"equivalent speed(Tops):"<<((1.0*equalWorkLoad)/clock*baseFreq)<<std::endl;

        FILE* analFp = fopen((pathPrefix
                            + ANALYZE_OUT_FILE_PATH
                            + string("_" ) + String::NumToString(height)
                            + string("x" ) + String::NumToString( width)
                            + string("_" ) + String::NumToString( ruPer)
                            + string("-" ) + String::NumToString(macPer)
                            + string("_w" )+ String::NumToString(ReformedPE::wFIFO_LEN)
                            + string("_x" )+ String::NumToString(ReformedPE::xFIFO_LEN)
                            + string("_wx")+ String::NumToString(ReformedPE::owxFIFO_LEN)
                            + string("_s" )+ String::NumToString(ReformedPE::sFIFO_LEN)
                            + string("_@") + String::NumToString(baseFreq)
                            + TRANS_FILE_TYPE).c_str(),"w+");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"    Performance analyze    \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"total clock used      :\t%d\n",clock);
        fprintf(analFp,"equivalent speed(Tops):\t%f\n",((1.0*equalWorkLoad)/clock*baseFreq));
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"       Power analyze       \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"Global buffer activation statistics:\n");
        fprintf(analFp,"\t   total      global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits+XGlobReadBits).c_str());
        fprintf(analFp,"\t  weight   global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits).c_str());
        fprintf(analFp,"\tactivation global buffer read bits:\t%s\n",String::NumToString(XGlobReadBits).c_str());
        Reg      :: PrintPower(analFp);
        FIFO     :: PrintPower(analFp);
        Adder    :: PrintPower(analFp);
        Multer   :: PrintPower(analFp);
        Comparer :: PrintPower(analFp);
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"        Area analyze       \n");
        fprintf(analFp,"###########################\n");
        Reg      :: PrintArea (analFp);
        FIFO     :: PrintArea (analFp);
        Adder    :: PrintArea (analFp);
        Multer   :: PrintArea (analFp);
        Comparer :: PrintArea (analFp);
        fclose(analFp);
        return true;
    }

public:
    void TestLayer(uint32_t height,uint32_t width,
                   uint32_t ruPer ,uint32_t macPer,const string& layer = "./"){
        std::ifstream ifs;
        int workload;
        ifs.open("./"+layer+"/workload.txt");
        if (!ifs){
            std::cout<<"Open file error! ["<<layer<<"]"<<std::endl;
            return;
        }
        ifs>>workload;
        ifs.close();
        if (BasicTest(height,width,ruPer,macPer,macPer*700,workload,string("./"+layer+"/transed/")))
            std::cout<<layer+" test passed"<<std::endl;
        else
            std::cout<<layer+" test failed"<<std::endl;
        return;
    }

    ReformedPEArrayTest(){
//        if (BasicTest( 4 , 4 ,2,5,3500,0.0,string("./data/ReformedS2Engine_tb1_4_4/")))
//            std::cout<<"basic reformed PE array   4_4   @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array   4_4   @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 4 , 4 ,1,2,1400,0.0,string("./data/ReformedS2Engine_tb1_4_4/")))
//            std::cout<<"basic reformed PE array   4_4   @2:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array   4_4   @2:1 test failed"<<std::endl;

//        if (BasicTest( 4 , 4 ,2,5,3500,0.0,string("./data/ReformedS2Engine_tb2_4_4/")))
//            std::cout<<"hard reformed PE array   4_4   @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array   4_4   @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 4 , 4 ,1,2,1400,0.0,string("./data/ReformedS2Engine_tb2_4_4/")))
//            std::cout<<"hard reformed PE array   4_4   @2:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array   4_4   @2:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,2,5,3500,220.5,string("./data/ReformedS2Engine_tb3_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,3,2100,220.5,string("./data/ReformedS2Engine_tb3_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @3:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @3:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,220.5,string("./data/ReformedS2Engine_tb3_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,5,3500,220.5,string("./data/ReformedS2Engine_tb3_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,6,4200,220.5,string("./data/ReformedS2Engine_tb3_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @6:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @6:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,7,4900,220.5,string("./data/ReformedS2Engine_tb3_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @7:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @7:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,7,5600,220.5,string("./data/ReformedS2Engine_tb3_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @8:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @8:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,2,5,3500,3528,string("./data/ReformedPEArray_tb5_16_16/")))
//            std::cout<<"hard reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,3,2100,3528,string("./data/ReformedPEArray_tb5_16_16/")))
//            std::cout<<"hard reformed PE array  16_16  @3:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array  16_16  @3:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,3528,string("./data/ReformedPEArray_tb5_16_16/")))
//            std::cout<<"hard reformed PE array  16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array  16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,2,5,3500,0,string("./data/ReformedS2Engine_tb4_16_16/")))
//            std::cout<<"hard reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,2,1400,0,string("./data/ReformedS2Engine_tb4_16_16/")))
//            std::cout<<"hard reformed PE array  16_16  @2:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array  16_16  @2:1 test failed"<<std::endl;
//
//        if (BasicTest( 64, 32,2,5,3500,0,string("./data/ReformedS2Engine_tb3_64_32/")))
//            std::cout<<"basic reformed PE array  64_32  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  64_32  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 64, 32,1,2,1400,0,string("./data/ReformedS2Engine_tb3_64_32/")))
//            std::cout<<"basic reformed PE array  64_32  @2:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  64_32  @2:1 test failed"<<std::endl;
//
//        if (BasicTest(256,256,2,5,3500,0,string("./data/ReformedS2Engine_tb4_256_256/")))
//            std::cout<<"basic reformed PE array 256_256 @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array 256_256 @5:2 test passed"<<std::endl;
//
//        if (BasicTest(256,256,1,2,string("./data/ReformedS2Engine_tb4_256_256/")))
//            std::cout<<"basic reformed PE array 256_256 @2:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array 256_256 @2:1 test passed"<<std::endl;

//        if (BasicTest( 16, 16,1,2,1400,3528,string("./data/ReformedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @2:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @2:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,2,5,3500,3528,string("./data/ReformedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,3,2100,3528,string("./data/ReformedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @3:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @3:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,2800,3528,string("./data/ReformedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @4:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,5,3500,3528,string("./data/ReformedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @5:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @5:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,6,4200,3528,string("./data/ReformedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @6:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @6:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,7,4900,3528,string("./data/ReformedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @7:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @7:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,8,5600,3528,string("./data/ReformedPEArray_tb7_16_16/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @8:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 16_16  @8:1 test failed"<<std::endl;


//        if (BasicTest(256,256,1,2,1400,3528,string("./data/ReformedPEArray_tb8_256_256/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @2:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @2:1 test failed"<<std::endl;
//
//        if (BasicTest(256,256,2,5,3500,3528,string("./data/ReformedPEArray_tb8_256_256/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @5:2 test failed"<<std::endl;
//
//        if (BasicTest(256,256,1,3,2100,3528,string("./data/ReformedPEArray_tb8_256_256/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @3:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @3:1 test failed"<<std::endl;
//
//        if (BasicTest(256,256,1,4,2800,3528,string("./data/ReformedPEArray_tb8_256_256/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @4:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @4:1 test failed"<<std::endl;
//
//        if (BasicTest(256,256,1,5,3500,3528,string("./data/ReformedPEArray_tb8_256_256/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @5:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @5:1 test failed"<<std::endl;
//
//        if (BasicTest(256,256,1,6,4200,3528,string("./data/ReformedPEArray_tb8_256_256/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @6:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @6:1 test failed"<<std::endl;
//
//        if (BasicTest(256,256,1,7,4900,3528,string("./data/ReformedPEArray_tb8_256_256/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @7:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @7:1 test failed"<<std::endl;
//
//        if (BasicTest(256,256,1,8,5600,3528,string("./data/ReformedPEArray_tb8_256_256/")))
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @8:1 test passed"<<std::endl;
//        else
//            std::cout<<"hard reformed PE array and scheduling & mapping strategy 256_256  @8:1 test failed"<<std::endl;

        return;
    }

//    void TestVGG16(){
//        this->TestLayer("VGG16/conv1_1");
//        this->TestLayer("VGG16/conv1_2");
//        this->TestLayer("VGG16/conv2_1");
//        this->TestLayer("VGG16/conv2_2");
//        this->TestLayer("VGG16/conv3_1");
//        this->TestLayer("VGG16/conv3_2");
//        this->TestLayer("VGG16/conv3_3");
//        this->TestLayer("VGG16/conv4_1");
//        this->TestLayer("VGG16/conv4_2");
//        this->TestLayer("VGG16/conv4_3");
//        this->TestLayer("VGG16/conv5_1");
//        this->TestLayer("VGG16/conv5_2");
//        this->TestLayer("VGG16/conv5_3");
//        return;
//    }
//
//    void TestVGG16_1(){
//        this->TestLayer("VGG16/conv1_1_10");
//        this->TestLayer("VGG16/conv1_1_11");
//        this->TestLayer("VGG16/conv1_1_12");
//        this->TestLayer("VGG16/conv1_1_13");
//        this->TestLayer("VGG16/conv1_1_14");
//        this->TestLayer("VGG16/conv1_1_15");
//        this->TestLayer("VGG16/conv1_1_16");
//    }
//
//    void TestAlex(){
//        this->TestLayer("AlexNet/conv1");
//        this->TestLayer("AlexNet/conv2");
//        this->TestLayer("AlexNet/conv3");
//        this->TestLayer("AlexNet/conv4");
//        this->TestLayer("AlexNet/conv5");
//
////        this->TestLayer("./conv_0");
////        this->TestLayer("./conv_1");
////        this->TestLayer("./conv_2");
////        this->TestLayer("./conv_3");
////        this->TestLayer("./conv_4");
//        return;
//    }
};

class ReformedS2EngineTest{
private:
    bool BasicTest(int height  ,int width,
                   int ruPeriod,int macPeriod,int cePeriod,
                   int baseFreq,double equalWorkLoad,
                   string pathPrefix){

        Object::CounterType WGlobReadBits=0,
                            XGlobReadBits=0;

        Reg     ::ResetStatis();
        FIFO    ::ResetStatis();
        Adder   ::ResetStatis();
        Multer  ::ResetStatis();
        Comparer::ResetStatis();

        ReformedS2Engine reS2Engine(height,width,ruPeriod,macPeriod,cePeriod);

        vector<queue<Object::PortDataType> >
           owInData(  width,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
          xelInData( height,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
           sOutData(2*width,queue<Object::PortDataType>());

        /** ---------- up -------- **/
        vector<Object::Port>    pOWInUpList; /**  O+W+V+Off+E-bit in  **/
        pOWInUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
              pOWInUpList.emplace_back(ReformedPE::   OUT_BIT_WIDTH
                                    +  ReformedPE::     W_BIT_WIDTH
                                    +  ReformedPE::    WE_BIT_WIDTH
                                    +  ReformedPE::OFFSET_BIT_WIDTH,
                                         Object::PortDirection::OUT);

        vector<Object::Port>  pOWPushUpList; /**            1-bit in  **/
        pOWPushUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
            pOWPushUpList.emplace_back(1,Object::PortDirection::OUT);

        vector<Object::Port>  pOWFullUpList; /**            1-bit out **/
        pOWFullUpList.reserve(width);
        for (uint32_t w=0;w<width;w++)
            pOWFullUpList.emplace_back(1,Object::PortDirection::IN );

        /** -------- left -------- **/
        vector<Object::Port>    pXELInList; /**    X+V+Off+E-bit in  **/
        pXELInList.reserve(height);
        for (uint32_t h=0;h<height;h++)
              pXELInList.emplace_back(ReformedPE::     X_BIT_WIDTH
                                   +  ReformedPE::    XE_BIT_WIDTH
                                   +  ReformedPE::OFFSET_BIT_WIDTH
                                   +  1,///CE input, to tell the different between control and data
                                        Object::PortDirection::OUT);

        vector<Object::Port> pXELEmptyList; /**            1-bit in  **/
        pXELEmptyList.reserve(height);
        for (uint32_t h=0;h<height;h++)
            pXELEmptyList.emplace_back(1,Object::PortDirection::OUT);

        vector<Object::Port>   pXELPopList; /**            1-bit out **/
        pXELPopList.reserve(height);
        for (uint32_t h=0;h<height;h++)
            pXELPopList.emplace_back(1,Object::PortDirection::IN );

        /** ------ down-down ----- **/
        vector<Object::Port>  pSOutDDownList; /**        S+E-bit out **/
        pSOutDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
              pSOutDDownList.emplace_back(ReformedPE:: S_BIT_WIDTH
                                       +  ReformedPE::SE_BIT_WIDTH,
                                            Object::PortDirection::IN );

        vector<Object::Port> pSPushDDownList; /**          1-bit out **/
        pSPushDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
             pSPushDDownList.emplace_back(1,Object::PortDirection::IN );

        vector<Object::Port> pSFullDDownList; /**          1-bit in  **/
        pSFullDDownList.reserve(2*width);
        for (uint32_t w=0;w<2*width;w++)
             pSFullDDownList.emplace_back(1,Object::PortDirection::OUT);

        /** connect to PE array **/
        /** ---------- up -------- **/
        vector<Object::PortDataType>     owInUpList( width ,0); /**  O+W+V+Off+E-bit in  **/
        vector<Object::PortDataType>   owPushUpList( width ,0); /**            1-bit in  **/
        /** -------- ddown ------- **/
        vector<Object::PortDataType> sFullDDownList(2*width,0); /**            1-bit in  **/
        /** connect to CE array **/
        /** --------- left ------- **/
        vector<Object::PortDataType>     xelInList( height,0); /**  O+W+V+Off+E-bit in  **/
        vector<Object::PortDataType>  xelEmptyList( height,0); /**            1-bit in  **/

        /** signal ports **/
        for (uint32_t w=0;w<width;w++){
              pOWInUpList[w].Clone(    owInUpList[w]);
            pOWPushUpList[w].Clone(  owPushUpList[w]);
        }
        for (uint32_t h=0;h<height;h++){
               pXELInList[h].Clone(   xelInList[h]);
            pXELEmptyList[h].Clone(xelEmptyList[h]);
        }
        for (uint32_t w=0;w<2*width;w++)
            pSFullDDownList[w].Clone(sFullDDownList[w]);

        /** ---------- up -------- **/
        for (uint32_t w=0;w<width;w++){
            reS2Engine.ConnectTo(reS2Engine.  OWInUpList[w],
                                             pOWInUpList[w]);
            reS2Engine.ConnectTo(reS2Engine.OWPushUpList[w],
                                           pOWPushUpList[w]);
            reS2Engine.ConnectTo(          pOWFullUpList[w],
                                 reS2Engine.OWFullUpList[w]);
        }
        /** --------- left ------- **/
        for (uint32_t h=0;h<height;h++){
            reS2Engine.ConnectTo(reS2Engine.XELInList[h],
                                           pXELInList[h]);
            reS2Engine.ConnectTo(reS2Engine.XELEmptyList[h],
                                           pXELEmptyList[h]);

            reS2Engine.ConnectTo(          pXELPopList[h],
                                 reS2Engine.XELPopList[h]);
        }
        /** -------- ddown ------- **/
        for (uint32_t w=0;w<2*width;w++){
            reS2Engine.ConnectTo(reS2Engine.SFullDDownList[w],
                                           pSFullDDownList[w]);
            reS2Engine.ConnectTo(          pSPushDDownList[w],
                                 reS2Engine.SPushDDownList[w]);
            reS2Engine.ConnectTo(           pSOutDDownList[w],
                                 reS2Engine. SOutDDownList[w]);
        }

        /** ##  load data  ## **/
        for (uint32_t w=0;w<width;w++){
            FILE* fpMAC = fopen(string(pathPrefix
                                  + W_OUT_IN_FILE_PATH_PREFIX
                                  + String::NumToString(w)
                                  + TRANS_FILE_TYPE).c_str(),"r");
            Object::PortDataType tempOffset,tempEOW,tempWeight,tempOutMark,tempMark;
            if (fpMAC==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            int counter = 0;
            while (fscanf(fpMAC,"%d%d%d%d%d",&tempWeight,&tempOutMark,&tempOffset,&tempEOW,&tempMark)>0){
                assert(!tempOutMark || tempEOW);
                tempWeight &= Object::mask[ReformedPE::W_BIT_WIDTH];
                owInData[w].push(ReformedPE::CompressOWEO(tempOutMark,tempWeight,tempEOW,tempOffset));
                counter++;
            }
            WGlobReadBits += (ReformedPE::   OUT_BIT_WIDTH
                            + ReformedPE::     W_BIT_WIDTH
                            + ReformedPE::    WE_BIT_WIDTH
                            + ReformedPE::OFFSET_BIT_WIDTH) * owInData[w].size();
            fclose(fpMAC);
        }
        for (uint32_t w=0;w<width;w++)
            for (uint32_t h=0;h<height;h++)
                owInData[w].push(ReformedPE::CompressOWEO(0,0,1,0));
        for (int i=0;i<height;i++){
            FILE* fp = fopen(string(pathPrefix
                                  + RU_XIN_TRANS_FILE_PATH_PREFIX
                                  + String::NumToString(i)
                                  + string("_MF")
                                  + TRANS_FILE_TYPE).c_str(),"r");
            if (fp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            int dataType;
            while (fscanf(fp,"%d",&dataType)>0){
                int tempCmd,tempVal,
                    tempLoc,tempEOX,tempBitMark;
                assert(dataType==0 || dataType==1);
                if (dataType!=0 && dataType!=1){
                    assert(false);
                    return false;
                }
                if (dataType == 0){
                    /// command
                    fscanf(fp,"%d",&tempCmd);
                    if (tempCmd<0 || tempCmd>=CE::CtrlNum){
                        assert(false);
                        return false;
                    }
                    xelInData[i].push(CE::Encode(CE::InsTable[tempCmd]));
                }
                else{
                    /// data
                    fscanf(fp,"%d%d%d%d",&tempVal,&tempLoc,&tempEOX,&tempBitMark);
                    if (tempLoc<0
                     ||(tempEOX!=0 && tempEOX!=1)){
                        assert(false);
                        return false;
                    }
                    xelInData[i].push(RU::CompressXEXL(tempVal,tempEOX,tempLoc,1));
                }
            }
            XGlobReadBits += (ReformedPE::     X_BIT_WIDTH
                            + ReformedPE::    XE_BIT_WIDTH
                            + ReformedPE::OFFSET_BIT_WIDTH
                            + 1) * xelInData[i].size();
            fclose(fp);
        }
        for (int h=0;h<height;h++){
            xelInData[h].push(CE::Encode(CE::CE_CTRL::BZ));
            xelInData[h].push(CE::Encode(CE::CE_CTRL::BZ));
        }
        for (uint32_t i=0;i<width;i++){
            FILE *outfp = fopen(string(pathPrefix
                                     + SA_OUTPUT_FILE_PATH_PREFIX
                                     + String::NumToString(i)
                                     + TRANS_FILE_TYPE).c_str(),"r");
            if (outfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOut0,tempOut1;
            while (fscanf(outfp,"%d%d",&tempOut0,&tempOut1)>0){
                sOutData[2*i  ].push(tempOut0);
                sOutData[2*i+1].push(tempOut1);
            }
            fclose(outfp);
        }

        reS2Engine.InnerConnect();
        assert(reS2Engine.Connected());

        uint32_t clock = 0;

        while (true){
            if (clock %  ruPeriod == 0){
                #ifdef PRINT_PROCESS
                if (clock % 500000 == 0){
                    std::cout<<"@clock"<<clock<<std::endl;
                    for (int w=0;w<  width;w++)
                        std::cout<< owInData[w].size()<<" ";
                    std::cout<<std::endl;
                    for (int h=0;h< height;h++)
                        std::cout<<xelInData[h].size()<<" ";
                    std::cout<<std::endl;
                    for (int w=0;w<2*width;w++)
                        std::cout<< sOutData[w].size()<<" ";
                    std::cout<<std::endl;
                }
                #endif // PRINT_PROCESS

                for (uint32_t w=0;w<width;w++)
                    if (pOWFullUpList[w].Data()==0
                    && !owInData  [w].empty()){
                        owInUpList[w] = owInData[w].front();
                        owInData  [w].pop();
                        owPushUpList[w] = 1;
                    }
                    else
                        owPushUpList[w] = 0;

                for (int i=0;i<height;i++){
                    /** ----- left ----- **/
                    if (xelInData[i].empty()){
                        if (pXELPopList[i].Data()==1){
                            assert(false);
                            return false;
                        }
                        xelInList[i]  = 233;
                        xelEmptyList[i] = 1;
                    }
                    else{
                        if (pXELPopList[i].Data()==1){
                            xelInData[i].pop();
                        }
                        if (xelInData[i].empty()){
                            xelInList[i] = 233;
                            xelEmptyList[i] = 1;
                        }
                        else{
                            xelInList[i] = xelInData[i].front();/** 2+X_BIT+XL_BIT  in  **/
                            xelEmptyList[i] = 0;
                        }
                    }
                }

                bool allEnd = true;
                for (uint32_t w=0;w<2*width;w++)
                    if (pSPushDDownList[w].Data()==1){
                        if (sOutData[w].empty()){
                            assert(false);
                            continue;
                        }
                        allEnd = false;
                        if (((pSOutDDownList[w].SignedData() - (pSOutDDownList[w].SignedData() & 1))/2) != sOutData[w].front()){
                            std::cout<<"## sOut error! ##"<<std::endl;
                            std::cout<<"@"<<w<<" rest:"<<sOutData[w].size()<<std::endl;
                            assert(false);
                            return false;
                        }
                        else
                            sOutData[w].pop();
                    }
                    else
                        if (!sOutData[w].empty())
                            allEnd = false;

                if (allEnd)
                    break;
            }

            reS2Engine.atPosEdge();
            reS2Engine.Run();
//            reS2Engine.PrintFIFOState();
            clock++;
        }

        std::cout<<"total clock used      :"<<clock   <<std::endl;
        std::cout<<"equivalent speed(Tops):"<<((1.0*equalWorkLoad)/clock*baseFreq)<<std::endl;

        FILE* analFp = fopen((pathPrefix
                            + ANALYZE_OUT_FILE_PATH
                            + string("_S2Engine_")
                            + string("_" ) + String::NumToString(height)
                            + string("x" ) + String::NumToString( width)
                            + string("_" ) + String::NumToString( ruPeriod)
                            + string("-" ) + String::NumToString(macPeriod)
                            + string("_w" )+ String::NumToString(ReformedPE::wFIFO_LEN)
                            + string("_x" )+ String::NumToString(ReformedPE::xFIFO_LEN)
                            + string("_wx")+ String::NumToString(ReformedPE::owxFIFO_LEN)
                            + string("_s" )+ String::NumToString(ReformedPE::sFIFO_LEN)
                            + string("_@") + String::NumToString(baseFreq)
                            + TRANS_FILE_TYPE).c_str(),"w+");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"    Performance analyze    \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"total clock used      :\t%d\n",clock);
        fprintf(analFp,"equivalent speed(Tops):\t%f\n",((1.0*equalWorkLoad)/clock*baseFreq));
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"       Power analyze       \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"Global buffer activation statistics:\n");
        fprintf(analFp,"\t   total      global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits+XGlobReadBits).c_str());
        fprintf(analFp,"\t  weight   global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits).c_str());
        fprintf(analFp,"\tactivation global buffer read bits:\t%s\n",String::NumToString(XGlobReadBits).c_str());
        Reg      :: PrintPower(analFp);
        FIFO     :: PrintPower(analFp);
        Adder    :: PrintPower(analFp);
        Multer   :: PrintPower(analFp);
        Comparer :: PrintPower(analFp);
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"        Area analyze       \n");
        fprintf(analFp,"###########################\n");
        Reg      :: PrintArea (analFp);
        FIFO     :: PrintArea (analFp);
        Adder    :: PrintArea (analFp);
        Multer   :: PrintArea (analFp);
        Comparer :: PrintArea (analFp);
        fclose(analFp);
        return true;
    }

public:
    void TestLayer(const string& layer = "./"){
        std::ifstream ifs;
        double workload;
        ifs.open(layer+"../workload.txt");
        if (!ifs){
            std::cout<<"Open file error! ["<<layer<<"]"<<std::endl;
            return;
        }
        ifs>>workload;
        ifs.close();
        if (BasicTest( 32, 32,1,4,1,2800,workload,layer))
            std::cout<<layer+" test passed"<<std::endl;
        else
            std::cout<<layer+" test failed"<<std::endl;
        return;
    }

    ReformedS2EngineTest(){
//        if (BasicTest( 16, 16,1,2,1,1400,3528,string("./data/ReS2Engine_tb1_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @2:1 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @2:1 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,2,5,2,3500,3528,string("./data/ReS2Engine_tb1_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,3,1,2100,3528,string("./data/ReS2Engine_tb1_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,4,1,2800,3528,string("./data/ReS2Engine_tb1_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,5,1,3500,3528,string("./data/ReS2Engine_tb1_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,6,1,4200,3528,string("./data/ReS2Engine_tb1_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,7,1,4900,3528,string("./data/ReS2Engine_tb1_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:2 test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,1,8,1,4900,3528,string("./data/ReS2Engine_tb1_16_16/")))
//            std::cout<<"basic reformed PE array  16_16  @5:2 test passed"<<std::endl;
//        else
//            std::cout<<"basic reformed PE array  16_16  @5:2 test failed"<<std::endl;
        return;
    }

    void TestVGG16(){
        this->TestLayer("VGG16/conv1_1");
        this->TestLayer("VGG16/conv1_2");
        this->TestLayer("VGG16/conv2_1");
        this->TestLayer("VGG16/conv2_2");
        this->TestLayer("VGG16/conv3_1");
        this->TestLayer("VGG16/conv3_2");
        this->TestLayer("VGG16/conv3_3");
        this->TestLayer("VGG16/conv4_1");
        this->TestLayer("VGG16/conv4_2");
        this->TestLayer("VGG16/conv4_3");
        this->TestLayer("VGG16/conv5_1");
        this->TestLayer("VGG16/conv5_2");
        this->TestLayer("VGG16/conv5_3");
        return;
    }

    void TestAlex(){
        this->TestLayer("AlexNet/conv_1");
        this->TestLayer("AlexNet/conv_2");
        this->TestLayer("AlexNet/conv_3");
        this->TestLayer("AlexNet/conv_4");
        this->TestLayer("AlexNet/conv_5");

//        this->TestLayer("./conv_0");
//        this->TestLayer("./conv_1");
//        this->TestLayer("./conv_2");
//        this->TestLayer("./conv_3");
//        this->TestLayer("./conv_4");
        return;
    }
};

class SynPEArray_SystemTest{
private:
    FILE* testbenchFp;

    Object::PortDataType
            sIn,sEn,fIn;

    vector<string> wInPath,xInPath,outPath;

    vector<Object::Port> pSEnList;/**  SEn  1-bit in  **/
    vector<Object::Port> pSInList;/**  SIn 24-bit in  **/
    vector<Object::Port> pFInList;/**  FIn  1-bit in  **/
    vector<Object::Port> pWInList;/**  WIn  8-bit in  **/
    vector<Object::Port> pOInList;/**  OIn  1-bit in  **/
    vector<Object::Port> pXInList;/**  XIn  8-bit in  **/

    vector<Object::Port> pSEnOList;/** SEnO  1-bit out **/
    vector<Object::Port> pSOutList;/** SOut 24-bit out **/
    vector<Object::Port> pFOutList;/** FOut  1-bit out **/
    vector<Object::Port> pWOutList;/** WOut  8-bit out **/
    vector<Object::Port> pOOutList;/** OOut  1-bit out **/
    vector<Object::Port> pXOutList;/** XOut  8-bit out **/

    vector<Object::PortDataType> wInList;
    vector<Object::PortDataType> oInList;
    vector<Object::PortDataType> xInList;

    vector<vector<Object::PortDataType> > wInData;
    vector<vector<Object::PortDataType> > oInData;
    vector<vector<Object::PortDataType> > xInData;
    vector<vector<Object::PortDataType> > outData;

    void RelevantInit(int height,int width,string pathPrefix){
        this->sIn = 0;
        this->sEn = 0;
        this->fIn = 0;

        this->wInPath.clear();
        for (int w=0;w<width;w++)
            wInPath.push_back(pathPrefix
                            + SA_WIN_TRANS_FILE_PATH_PREFIX
                            + String::NumToString(w)
                            + TRANS_FILE_TYPE);
        this->xInPath.clear();
        for (int h=0;h<height;h++)
            xInPath.push_back(pathPrefix
                            + SA_XIN_TRANS_FILE_PATH_PREFIX
                            + String::NumToString(h)
                            + TRANS_FILE_TYPE);
        this->outPath.clear();
        for (int w=0;w<width;w++)
            outPath.push_back(pathPrefix
                            + SA_OUTPUT_FILE_PATH_PREFIX
                            + String::NumToString(w)
                            + TRANS_FILE_TYPE);

        pSEnList.clear();/**  SEn  1-bit in  **/
        pSEnList.resize(2*width,Object::Port( 1,Object::PortDirection::OUT));
        pSInList.clear();/**  SIn 24-bit in  **/
        pSInList.resize(2*width,Object::Port(24,Object::PortDirection::OUT));
        pFInList.clear();/**  FIn  1-bit in  **/
        pFInList.resize(2*width,Object::Port( 1,Object::PortDirection::OUT));
        pWInList.clear();/**  WIn  8-bit in  **/
        pWInList.resize(  width,Object::Port( 8,Object::PortDirection::OUT));
        pOInList.clear();/**  OIn  1-bit in  **/
        pOInList.resize(  width,Object::Port( 1,Object::PortDirection::OUT));
        pXInList.clear();/**  XIn  8-bit in  **/
        pXInList.resize( height,Object::Port( 8,Object::PortDirection::OUT));
        pSEnOList.clear();/** SEnO  1-bit out **/
        pSEnOList.resize(2*width,Object::Port( 1,Object::PortDirection::IN));
        pSOutList.clear();/** SOut 24-bit out **/
        pSOutList.resize(2*width,Object::Port(24,Object::PortDirection::IN));
        pFOutList.clear();/** FOut  1-bit out **/
        pFOutList.resize(2*width,Object::Port( 1,Object::PortDirection::IN));
        pWOutList.clear();/** WOut  8-bit out **/
        pWOutList.resize(  width,Object::Port( 8,Object::PortDirection::IN));
        pOOutList.clear();/** OOut  1-bit out **/
        pOOutList.resize(  width,Object::Port( 1,Object::PortDirection::IN));
        pXOutList.clear();/** XOut  8-bit out **/
        pXOutList.resize( height,Object::Port( 8,Object::PortDirection::IN));

        wInList.clear();wInList.resize( width,0);
        oInList.clear();oInList.resize( width,0);
        xInList.clear();xInList.resize(height,0);

        wInData.clear();wInData.resize(  width,vector<Object::PortDataType>());
        oInData.clear();oInData.resize(  width,vector<Object::PortDataType>());
        xInData.clear();xInData.resize( height,vector<Object::PortDataType>());
        outData.clear();outData.resize(2*width,vector<Object::PortDataType>());
    }

public:
    SynPEArray_SystemTest(){
//        if (BasicTest(  4,  4,string("./data/tb1_4_4/")))
//            std::cout<<"basic synchronized PE array @  4_4   test passed"<<std::endl;
//        else
//            std::cout<<"basic synchronized PE array @  4_4   test failed"<<std::endl;
//
//        if (BasicTest( 16, 16,string("./data/tb2_16_16/")))
//            std::cout<<"basic synchronized PE array @ 16_16  test passed"<<std::endl;
//        else
//            std::cout<<"basic synchronized PE array @ 16_16  test failed"<<std::endl;
//
//        if (BasicTest( 64, 32,string("./data/tb3_64_32/")))
//            std::cout<<"basic synchronized PE array @ 64_32  test passed"<<std::endl;
//        else
//            std::cout<<"basic synchronized PE array @ 64_32  test failed"<<std::endl;
//
//        if (BasicTest(256,256,string("./data/tb4_256_256/")))
//            std::cout<<"basic synchronized PE array @256_256 test passed"<<std::endl;
//        else
//            std::cout<<"basic synchronized PE array @256_256 test passed"<<std::endl;
    }


    bool BasicTest(uint32_t width,uint32_t height,int baseFreq,double workload,string pathPrefix) {
        Reg     ::ResetStatis();
        MarkReg ::ResetStatis();
        FIFO    ::ResetStatis();
        Adder   ::ResetStatis();
        Multer  ::ResetStatis();
        Comparer::ResetStatis();

        int WGlobReadBits = 0,
            XGlobReadBits = 0;

        SynPEArray peArray(width,height);

        this->RelevantInit(width,height,pathPrefix);

        for (uint32_t i=0;i<2*width;i++){
            pSEnList[i].Clone(sEn);
            pSInList[i].Clone(sIn);
            pFInList[i].Clone(fIn);
        }
        for (uint32_t i=0;i<  width;i++){
            pWInList[i].Clone(wInList[i]);
            pOInList[i].Clone(oInList[i]);
        }
        for (uint32_t i=0;i< height;i++)
            pXInList[i].Clone(xInList[i]);

        for (uint32_t i=0;i<2*width;i++){
            peArray.ConnectTo(peArray.SEnList[i],pSEnList[i]);
            peArray.ConnectTo(peArray.SInList[i],pSInList[i]);
            peArray.ConnectTo(peArray.FInList[i],pFInList[i]);
        }
        for (uint32_t i=0;i<  width;i++){
            peArray.ConnectTo(peArray.WInList[i],pWInList[i]);
            peArray.ConnectTo(peArray.OInList[i],pOInList[i]);
        }
        for (uint32_t i=0;i< height;i++)
            peArray.ConnectTo(peArray.XInList[i],pXInList[i]);

        for (uint32_t i=0;i<2*width;i++){
            peArray.ConnectTo(pSEnOList[i],peArray.SEnOList[i]);
            peArray.ConnectTo(pSOutList[i],peArray.SOutList[i]);
            peArray.ConnectTo(pFOutList[i],peArray.FOutList[i]);
        }
        for (uint32_t i=0;i<  width;i++){
            peArray.ConnectTo(pWOutList[i],peArray.WOutList[i]);
            peArray.ConnectTo(pOOutList[i],peArray.OOutList[i]);
        }
        for (uint32_t i=0;i< height;i++)
            peArray.ConnectTo(pXOutList[i],peArray.XOutList[i]);

        peArray.InnerConnect();
        assert(peArray.Connected());

        for (uint32_t i=1;i<width;i++)
            for (uint32_t j=0;j<i;j++){
                wInData[i].push_back(0);
                oInData[i].push_back(0);
            }
        for (uint32_t i=1;i<height;i++)
            for (uint32_t j=0;j<i;j++)
                xInData[i].push_back(0);

        for (uint32_t i=0;i<width;i++){
            FILE *owInfp = fopen(wInPath[i].c_str(),"r");
            if (owInfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempWeight,tempOutMark,tempBitMark;
            while (fscanf(owInfp,"%d%d%d",&tempWeight,
                                          &tempOutMark,
                                          &tempBitMark)>0){
                wInData[i].push_back(tempWeight);
                oInData[i].push_back(tempOutMark);
            }
            WGlobReadBits += (ReformedPE::OUT_BIT_WIDTH
                            + ReformedPE::  W_BIT_WIDTH) * wInData[i].size();
            fclose(owInfp);
        }
        for (uint32_t i=0;i<height;i++){
            FILE *xInfp = fopen(xInPath[i].c_str(),"r");
            if (xInfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempFeature,tempBitMark;
            while (fscanf(xInfp,"%d%d",&tempFeature,&tempBitMark)>0)
                xInData[i].push_back(tempFeature);
            XGlobReadBits += ReformedPE::X_BIT_WIDTH * xInData[i].size();
            fclose(xInfp);
        }
        for (uint32_t i=0;i<width;i++){
            FILE *outfp = fopen(outPath[i].c_str(),"r");
            if (outfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempOut0,tempOut1;
            while (fscanf(outfp,"%d%d",&tempOut0,&tempOut1)>0){
                outData[2*i  ].push_back(tempOut0);
                outData[2*i+1].push_back(tempOut1);
            }
            fclose(outfp);
        }
        for (uint32_t i=0;i<width;i++)
            for (uint32_t j=i+1;j<width;j++){
                wInData[i].push_back(0);
                oInData[i].push_back(0);
            }
        for (uint32_t i=0;i<height;i++)
            for (uint32_t j=i+1;j<height;j++)
                xInData[i].push_back(0);

        for (uint32_t i=0;i<width;i++)
            for (uint32_t j=0;j<height;j++){
                wInData[i].push_back(0);
                oInData[i].push_back(0);
            }
        for (uint32_t i=0;i<height;i++)
            for (uint32_t j=0;j<width;j++)
                xInData[i].push_back(0);


        uint32_t inputLen=wInData[0].size();
        for (uint32_t i=0;i<width;i++)
            assert(wInData[i].size()==inputLen
                && oInData[i].size()==inputLen);
        for (uint32_t i=0;i<height;i++){
            if (xInData[i].size()!=inputLen)
                std::cout<<xInData[i].size()<<"_"<<inputLen<<std::endl;
            assert(xInData[i].size()==inputLen);
        }

        uint32_t outputLen=outData[0].size();
        for (uint32_t i=0;i<2*width;i++)
            assert(outData[i].size()==outputLen);

        vector<uint32_t> outputCount(2*width,0);
        for (uint32_t i=0;i<inputLen;i++){
            #ifdef PRINT_PROCESS
            if (i%1000==0)
                std::cout<<"@clock"<<i<<"/"<<inputLen<<std::endl;
            #endif // PRINT_PROCESS
            for (uint32_t w=0;w<width;w++){
                wInList[w] = wInData[w][i];
                oInList[w] = oInData[w][i];
            }
            for (uint32_t h=0;h<height;h++)
                xInList[h] = xInData[h][i];

            peArray.atPosEdge();
            peArray.Run();

            for (uint32_t w=0;w<2*width;w++){
                if (pSEnOList[w].Data()==1){
                    assert(pSOutList[w].SignedData()
                          == outData[w][outputCount[w]]);
                    if (pSOutList[w].SignedData()
                       != outData[w][outputCount[w]])
                       return false;
                    outputCount[w]++;
                }
            }
        }
        for (uint32_t w=0;w<2*width;w++){
            assert(outputCount[w] == outputLen);
            if (outputCount[w] != outputLen)
                return false;
        }

        std::cout<<"total clock used      :"<<inputLen   <<std::endl;
        std::cout<<"equivalent speed(Tops):"<<((1.0*workload)/inputLen*baseFreq)<<std::endl;

        FILE* analFp = fopen((pathPrefix
                            + ANALYZE_OUT_FILE_PATH
                            + string("_" ) + String::NumToString(height)
                            + string("x" ) + String::NumToString( width)
                            + string("_@") + String::NumToString(baseFreq)
                            + TRANS_FILE_TYPE).c_str(),"w+");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"    Performance analyze    \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"total clock used      :\t%d\n",inputLen);
        fprintf(analFp,"equivalent speed(Tops):\t%f\n",((1.0*workload)/inputLen*baseFreq));
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"       Power analyze       \n");
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"Global buffer activation statistics:\n");
        fprintf(analFp,"\t   total      global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits+XGlobReadBits).c_str());
        fprintf(analFp,"\t  weight   global buffer read bits:\t%s\n",String::NumToString(WGlobReadBits).c_str());
        fprintf(analFp,"\tactivation global buffer read bits:\t%s\n",String::NumToString(XGlobReadBits).c_str());
        Reg      :: PrintPower(analFp);
        MarkReg  :: PrintPower(analFp);
        FIFO     :: PrintPower(analFp);
        Adder    :: PrintPower(analFp);
        Multer   :: PrintPower(analFp);
        Comparer :: PrintPower(analFp);
        fprintf(analFp,"###########################\n");
        fprintf(analFp,"        Area analyze       \n");
        fprintf(analFp,"###########################\n");
        Reg      :: PrintArea (analFp);
        MarkReg  :: PrintArea (analFp);
        FIFO     :: PrintArea (analFp);
        Adder    :: PrintArea (analFp);
        Multer   :: PrintArea (analFp);
        Comparer :: PrintArea (analFp);
        fclose(analFp);
        return true;
    }

    void TestVGG16(){
        this->TestLayer("VGG16/conv1_1");
        this->TestLayer("VGG16/conv1_2");
        this->TestLayer("VGG16/conv2_1");
        this->TestLayer("VGG16/conv2_2");
        this->TestLayer("VGG16/conv3_1");
        this->TestLayer("VGG16/conv3_2");
        this->TestLayer("VGG16/conv3_3");
        this->TestLayer("VGG16/conv4_1");
        this->TestLayer("VGG16/conv4_2");
        this->TestLayer("VGG16/conv4_3");
        this->TestLayer("VGG16/conv5_1");
        this->TestLayer("VGG16/conv5_2");
        this->TestLayer("VGG16/conv5_3");
        return;
    }

    void TestAlex(){
        this->TestLayer("AlexNet/conv1");
        this->TestLayer("AlexNet/conv2");
        this->TestLayer("AlexNet/conv3");
        this->TestLayer("AlexNet/conv4");
        this->TestLayer("AlexNet/conv5");
        return;
    }

    void TestLayer(const string& layer = "./"){
        std::ifstream ifs;
        int workload;
        ifs.open("./"+layer+"/workload.txt");
        if (!ifs){
            std::cout<<"Open file error! ["<<layer<<"]"<<std::endl;
            return;
        }
        ifs>>workload;
        ifs.close();
        if (BasicTest( 16, 16,700,workload,string("./"+layer+"/transed/")))
            std::cout<<layer+" test passed"<<std::endl;
        else
            std::cout<<layer+" test failed"<<std::endl;
        return;
    }
};

#endif //__TEST_SYSTEM_TEST_HPP
