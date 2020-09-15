#ifndef __TEST_TEST_BASE_COMBI_LOGIC_HPP
#define __TEST_TEST_BASE_COMBI_LOGIC_HPP

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


#define  REFORMED_PE_OW_IN_FILE_PATH_PREFIX string( "ow_in")
#define   REFORMED_PE_X_IN_FILE_PATH_PREFIX string(  "x_in")
#define   REFORMED_PE_S_IN_FILE_PATH_PREFIX string(  "s_in")
#define REFORMED_PE_OW_OUT_FILE_PATH_PREFIX string("ow_out")
#define  REFORMED_PE_X_OUT_FILE_PATH_PREFIX string( "x_out")
#define  REFORMED_PE_S_OUT_FILE_PATH_PREFIX string( "s_out")



#define  ANALYZE_OUT_FILE_PATH string( "analyze")
//#define PRINT_PROCESS

class ReformedPETest{
private:
    inline void OutputNotMatch(
            Object::PortDataType actData,
            Object::PortDataType stdData
        ){
        cout<<"## Output Error ## should be: "<<stdData
                            <<" actually is: "<<actData
                            <<endl;
    }

    bool NormalTest(ReformedPE::ClockFeqType  ruPer,
                    ReformedPE::ClockFeqType macPer,
                    string pathPrefix){
        Reg     ::ResetStatis();
        FIFO    ::ResetStatis();
        Adder   ::ResetStatis();
        Multer  ::ResetStatis();
        Comparer::ResetStatis();

        ReformedPE rePE(ruPer,macPer,true);

        queue<Object::PortDataType>  owInData;
        queue<Object::PortDataType>   xInData;
        queue<Object::PortDataType>   sInData;
        queue<Object::PortDataType> owOutData;
        queue<Object::PortDataType>  xOutData;
        queue<Object::PortDataType>  sOutData;
        /** ---------- up -------- **/
        Object::Port   pOWInUp(
                            ReformedPE::   OUT_BIT_WIDTH
                          + ReformedPE::     W_BIT_WIDTH
                          + ReformedPE::OFFSET_BIT_WIDTH
                          + ReformedPE::    WE_BIT_WIDTH,
                                 Object::PortDirection::OUT);/** OWInUp   O+W+V+Off+E-bit in  **/
        Object::Port pOWPushUp(1,Object::PortDirection::OUT);/** OWPushUp           1-bit in  **/
        Object::Port pOWFullUp(1,Object::PortDirection::IN );/** OWFullUp           1-bit out **/
        /** --------- down ------- **/
        Object::Port  pOWOutDown(
                              ReformedPE::   OUT_BIT_WIDTH
                            + ReformedPE::     W_BIT_WIDTH
                            + ReformedPE::OFFSET_BIT_WIDTH
                            + ReformedPE::    WE_BIT_WIDTH,
                                   Object::PortDirection::IN );/** OWOutDown  O+W+V+Off+E-bit out **/
        Object::Port pOWPushDown(1,Object::PortDirection::IN );/** OWPushDown           1-bit out **/
        Object::Port pOWFullDown(1,Object::PortDirection::OUT);/** OWFullDown           1-bit in  **/
        /** -------- left -------- **/
        Object::Port   pXInLeft(
                            ReformedPE::     X_BIT_WIDTH
                          + ReformedPE::OFFSET_BIT_WIDTH
                          + ReformedPE::    XE_BIT_WIDTH,
                                  Object::PortDirection::OUT);/** XInLeft  X+V+Off+E-bit in  **/
        Object::Port pXPushLeft(1,Object::PortDirection::OUT);/** XPushLeft        1-bit in  **/
        Object::Port pXFullLeft(1,Object::PortDirection::IN );/** XFullLeft        1-bit out **/
        /** -------- right ------- **/
        Object::Port  pXOutRight(
                             ReformedPE::     X_BIT_WIDTH
                           + ReformedPE::OFFSET_BIT_WIDTH
                           + ReformedPE::    XE_BIT_WIDTH,
                                   Object::PortDirection::IN );/** XOutRight   X+V+Off+E-bit out **/
        Object::Port pXPushRight(1,Object::PortDirection::IN );/** XPushRight          1-bit out **/
        Object::Port pXFullRight(1,Object::PortDirection::OUT);/** XFullRight          1-bit in  **/
        /** -------- up-up ------- **/
        Object::Port   pSInUUp(
                              ReformedPE:: S_BIT_WIDTH
                            + ReformedPE::SE_BIT_WIDTH,
                                 Object::PortDirection::OUT);/** SInUUp   S+E-bit in  **/
        Object::Port pSPushUUp(1,Object::PortDirection::OUT);/** SPushUUp   1-bit in  **/
        Object::Port pSFullUUp(1,Object::PortDirection::IN );/** SFullUUp   1-bit out **/
        /** ------ down-down ----- **/
        Object::Port  pSOutDDown(
                                ReformedPE:: S_BIT_WIDTH
                              + ReformedPE::SE_BIT_WIDTH
                                  ,Object::PortDirection::IN );/** SOutDDown  S+E-bit out **/
        Object::Port pSPushDDown(1,Object::PortDirection::IN );/** SPushDDown   1-bit out **/
        Object::Port pSFullDDown(1,Object::PortDirection::OUT);/** SFullDDown   1-bit in  **/

        Object::PortDataType
        /** ---------- up -------- **/
              owInUp, /**  O+W+V+Off+E-bit in  **/
            owPushUp, /**            1-bit in  **/
        /** --------- down ------- **/
          owFullDown, /**            1-bit in  **/
        /** -------- left -------- **/
             xInLeft, /**    X+V+Off+E-bit in  **/
           xPushLeft, /**            1-bit in  **/
        /** -------- right ------- **/
          xFullRight, /**            1-bit in  **/
        /** -------- up-up ------- **/
              sInUUp, /**          S+E-bit in  **/
            sPushUUp, /**            1-bit in  **/
        /** -------- down-down ------- **/
          sFullDDown; /**            1-bit in  **/

        /** ---------- up -------- **/
            pOWInUp.Clone(    owInUp); /**  O+W+V+Off+E-bit in  **/
          pOWPushUp.Clone(  owPushUp); /**            1-bit in  **/
        /** --------- down ------- **/
        pOWFullDown.Clone(owFullDown); /**            1-bit in  **/
        /** -------- left -------- **/
           pXInLeft.Clone(   xInLeft); /**    X+V+Off+E-bit in  **/
         pXPushLeft.Clone( xPushLeft); /**            1-bit in  **/
        /** -------- right ------- **/
        pXFullRight.Clone(xFullRight); /**            1-bit in  **/
        /** -------- up-up ------- **/
            pSInUUp.Clone(    sInUUp); /**        S+E-bit in  **/
          pSPushUUp.Clone(  sPushUUp); /**          1-bit in  **/
        /** -------- down-down ------- **/
        pSFullDDown.Clone(sFullDDown); /**          1-bit in  **/

        /** ---------- up -------- **/
        rePE.ConnectTo(ReformedPE::    OWInUp,    pOWInUp); /**  O+W+V+Off+E-bit in  **/
        rePE.ConnectTo(ReformedPE::  OWPushUp,  pOWPushUp); /**            1-bit in  **/
        /** --------- down ------- **/
        rePE.ConnectTo(ReformedPE::OWFullDown,pOWFullDown); /**            1-bit in  **/
        /** -------- left -------- **/
        rePE.ConnectTo(ReformedPE::   XInLeft,   pXInLeft); /**    X+V+Off+E-bit in  **/
        rePE.ConnectTo(ReformedPE:: XPushLeft, pXPushLeft); /**            1-bit in  **/
        /** -------- right ------- **/
        rePE.ConnectTo(ReformedPE::XFullRight,pXFullRight); /**            1-bit in  **/
        /** -------- up-up ------- **/
        rePE.ConnectTo(ReformedPE::    SInUUp,    pSInUUp); /**        S+E-bit in  **/
        rePE.ConnectTo(ReformedPE::  SPushUUp,  pSPushUUp); /**          1-bit in  **/
        /** -------- down-down ------- **/
        rePE.ConnectTo(ReformedPE::SFullDDown,pSFullDDown); /**          1-bit in  **/

        /** ---------- up -------- **/
        rePE.ConnectTo(  pOWFullUp,ReformedPE::  OWFullUp); /**            1-bit out **/
        /** --------- down ------- **/
        rePE.ConnectTo( pOWOutDown,ReformedPE:: OWOutDown); /**  O+W+V+Off+E-bit out **/
        rePE.ConnectTo(pOWPushDown,ReformedPE::OWPushDown); /**            1-bit out **/
        /** -------- left -------- **/
        rePE.ConnectTo( pXFullLeft,ReformedPE:: XFullLeft); /**            1-bit out **/
        /** -------- right ------- **/
        rePE.ConnectTo( pXOutRight,ReformedPE:: XOutRight); /**    X+V+Off+E-bit out **/
        rePE.ConnectTo(pXPushRight,ReformedPE::XPushRight); /**            1-bit out **/
        /** -------- up-up ------- **/
        rePE.ConnectTo(  pSFullUUp,ReformedPE::  SFullUUp); /**          1-bit out **/
        /** -------- down-down ------- **/
        rePE.ConnectTo( pSOutDDown,ReformedPE:: SOutDDown); /**        S+E-bit out **/
        rePE.ConnectTo(pSPushDDown,ReformedPE::SPushDDown); /**          1-bit out **/

        rePE.InnerConnect();
        assert(rePE.Connected());

        FILE* fp;
        Object::PortDataType tempOff;
        Object::PortDataType tempOut,tempW,tempEOW;
        Object::PortDataType         tempX,tempEOX;
        Object::PortDataType         tempS,tempEOS;
        /** ow_in **/
        fp = fopen(string(pathPrefix
                        + REFORMED_PE_OW_IN_FILE_PATH_PREFIX
                        + TRANS_FILE_TYPE).c_str(),"r");
        if (fp==nullptr){
            std::cout<<"Error opening file"<<std::endl;
            exit(1);
        }
        while(fscanf(fp,"%d%d%d%d",&tempOut,&tempW,&tempOff,&tempEOW)>0)
            owInData.push(ReformedPE::CompressOWEO(tempOut,tempW,tempEOW,tempOff));
        fclose(fp);

        /** ow_out **/
        fp = fopen(string(pathPrefix
                        + REFORMED_PE_OW_OUT_FILE_PATH_PREFIX
                        + TRANS_FILE_TYPE).c_str(),"r");
        if (fp==nullptr){
            std::cout<<"Error opening file"<<std::endl;
            exit(1);
        }
        while(fscanf(fp,"%d%d%d%d",&tempOut,&tempW,&tempOff,&tempEOW)>0)
            owOutData.push(ReformedPE::CompressOWEO(tempOut,tempW,tempEOW,tempOff));
        fclose(fp);
        /** x_in **/
        fp = fopen(string(pathPrefix
                        + REFORMED_PE_X_IN_FILE_PATH_PREFIX
                        + TRANS_FILE_TYPE).c_str(),"r");
        if (fp==nullptr){
            std::cout<<"Error opening file"<<std::endl;
            exit(1);
        }
        while(fscanf(fp,"%d%d%d",&tempX,&tempOff,&tempEOX)>0)
            xInData.push(ReformedPE::CompressXEO(tempX,tempEOX,tempOff));
        fclose(fp);
        /** x_out **/
        fp = fopen(string(pathPrefix
                        + REFORMED_PE_X_OUT_FILE_PATH_PREFIX
                        + TRANS_FILE_TYPE).c_str(),"r");
        if (fp==nullptr){
            std::cout<<"Error opening file"<<std::endl;
            exit(1);
        }
        while(fscanf(fp,"%d%d%d",&tempX,&tempOff,&tempEOX)>0)
            xOutData.push(ReformedPE::CompressXEO(tempX,tempEOX,tempOff));
        fclose(fp);
        /** s_in **/
        fp = fopen(string(pathPrefix
                        + REFORMED_PE_S_IN_FILE_PATH_PREFIX
                        + TRANS_FILE_TYPE).c_str(),"r");
        if (fp==nullptr){
            std::cout<<"Error opening file"<<std::endl;
            exit(1);
        }
        while(fscanf(fp,"%d%d",&tempS,&tempEOS)>0)
            sInData.push(ReformedPE::CompressSE(tempS,tempEOS));
        fclose(fp);
        /** s_out **/
        fp = fopen(string(pathPrefix
                        + REFORMED_PE_S_OUT_FILE_PATH_PREFIX
                        + TRANS_FILE_TYPE).c_str(),"r");
        if (fp==nullptr){
            std::cout<<"Error opening file"<<std::endl;
            exit(1);
        }
        while(fscanf(fp,"%d%d",&tempS,&tempEOS)>0)
            sOutData.push(ReformedPE::CompressSE(tempS,tempEOS));
        fclose(fp);

        owFullDown = 0;
        xFullRight = 0;
        sFullDDown = 0;

        int clock = 0;
        while(true){
            if (clock %  ruPer == 0){
//                std::cout<<"@clock"<<clock<<std::endl;
                if (pOWFullUp.Data()==0
                && !owInData.empty()){
                    owInUp   = owInData.front();
                    owInData.pop();
                    owPushUp = 1;
                }
                else
                    owPushUp = 0;

                if (pXFullLeft.Data()==0
                && !xInData.empty()){
                    xInLeft   = xInData.front();
                    xInData.pop();
                    xPushLeft = 1;
                }
                else
                    xPushLeft = 0;

                if (pSFullUUp.Data()==0
                && !sInData.empty()){
                    sInUUp   =sInData.front();
                    sInData.pop();
                    sPushUUp = 1;
                }
                else
                    sPushUUp = 0;

                if (pOWPushDown.Data()==1){
                    if (pOWOutDown.Data() != owOutData.front()){
                        assert(false);
                        return false;
                    }
                    else
                        owOutData.pop();
                }
                if (pXPushRight.Data()==1){
                    if (pXOutRight.Data() != xOutData.front()){
                        std::cout<<"## xOut error! ##"<<std::endl;
                        std::cout<<xOutData.size()<<std::endl;
                        this->OutputNotMatch(pXOutRight.Data(),xOutData.front());
                        assert(false);
                        return false;
                    }
                    else
                        xOutData.pop();
                }
                if (pSPushDDown.Data()==1){
                    if (pSOutDDown.Data() != sOutData.front()){
                        std::cout<<"## sOut error! ##"<<std::endl;
                        this->OutputNotMatch(pSOutDDown.Data(),sOutData.front());
                        assert(false);
                        return false;
                    }
                    else
                        sOutData.pop();
                }
            }
            if (owOutData.empty()
             &&  xOutData.empty()
             &&  sOutData.empty())
                break;
            else{
//                if (clock %  ruPer == 0){
//                    std::cout<<"############"<<std::endl;
//                    std::cout<<"owOut left: "<<owOutData.size()<<std::endl;
//                    std::cout<<"owIn  left: "<< owInData.size()<<std::endl;
//                    std::cout<<" xOut left: "<< xOutData.size()<<std::endl;
//                    std::cout<<" xIn  left: "<<  xInData.size()<<std::endl;
//                    std::cout<<" sOut left: "<< sOutData.size()<<std::endl;
//                    std::cout<<" sIn  left: "<<  sInData.size()<<std::endl;
//                    rePE.Print();
//                    getchar();
//                }
            }
            rePE.atPosEdge();
            rePE.Run();
            clock++;
        }

        FILE* analFp = fopen((pathPrefix
                            + ANALYZE_OUT_FILE_PATH
                            + string("_") + String::NumToString( ruPer)
                            + string("-") + String::NumToString(macPer)
                            + TRANS_FILE_TYPE).c_str(),"w+");

        Reg      :: PrintPower(analFp);
        FIFO     :: PrintPower(analFp);
        Adder    :: PrintPower(analFp);
        Multer   :: PrintPower(analFp);
        Comparer :: PrintPower(analFp);
        Reg      :: PrintArea (analFp);
        FIFO     :: PrintArea (analFp);
        Adder    :: PrintArea (analFp);
        Multer   :: PrintArea (analFp);
        Comparer :: PrintArea (analFp);
        fclose(analFp);
        return true;
    }
public:
    ReformedPETest(){
        if (NormalTest(2,5,string("./data/ReformedPE_tb1/")))
            std::cout<<"normal test 1 @2_5 passed"<<std::endl;
        else
            std::cout<<"normal test 1 @2_5 failed"<<std::endl;

        if (NormalTest(2,5,string("./data/ReformedPE_tb2/")))
            std::cout<<"normal test 2 @2_5 passed"<<std::endl;
        else
            std::cout<<"normal test 2 @2_5 passed"<<std::endl;

        if (NormalTest(1,2,string("./data/ReformedPE_tb1/")))
            std::cout<<"normal test 1 @1_2 passed"<<std::endl;
        else
            std::cout<<"normal test 1 @1_2 failed"<<std::endl;

        if (NormalTest(1,2,string("./data/ReformedPE_tb2/")))
            std::cout<<"normal test 2 @1_2 passed"<<std::endl;
        else
            std::cout<<"normal test 2 @1_2 passed"<<std::endl;
        return;
    }
};

class S2EngineTest{
private:
    void IllegalInput(){
        cout<<"Illegal input at RU Array test"<<endl;
        return;
    }
public:
    S2EngineTest(){
//        if (BasicS2EngineTest(16,16,1,4,2000,40.5,string("./data/S2Engine_tb1_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 4-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 4-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,1,3,1500,40.5,string("./data/S2Engine_tb1_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 3-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 3-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,2,5,2500,40.5,string("./data/S2Engine_tb1_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 5-2] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 5-2] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,1,2,1000,40.5,string("./data/S2Engine_tb1_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 2-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 2-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,2,3,1500,40.5,string("./data/S2Engine_tb1_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 3-2] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 3-2] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,1,3,1500,3528,string("./data/S2Engine_tb2_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 3-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 3-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,2,5,2500,3528,string("./data/S2Engine_tb2_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 5-2] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 5-2] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,1,3,1500,3528,string("./data/S2Engine_tb3_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 3-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 3-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,2,5,2500,3528,string("./data/S2Engine_tb3_1_16_16/")))
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 5-2] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x16x16 hard test [@freq ratio 5-2] failed"<<std::endl;
//
//        if (BasicS2EngineTest(32,32,1,3,1500,3528,string("./data/S2Engine_tb4_1_32_32/")))
//            std::cout<<"Basic S2Engine Array 1x32x32 hard test [@freq ratio 3-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x32x32 hard test [@freq ratio 3-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(32,32,2,5,2500,3528,string("./data/S2Engine_tb4_1_32_32/")))
//            std::cout<<"Basic S2Engine Array 1x32x32 hard test [@freq ratio 5-2] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x32x32 hard test [@freq ratio 5-2] failed"<<std::endl;
//
//        if (BasicS2EngineTest(64,32,1,3,1500,3528,string("./data/S2Engine_tb5_1_64_32/")))
//            std::cout<<"Basic S2Engine Array 1x64x32 hard test [@freq ratio 3-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x64x32 hard test [@freq ratio 3-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(64,32,2,5,2500,3528,string("./data/S2Engine_tb5_1_64_32/")))
//            std::cout<<"Basic S2Engine Array 1x64x32 hard test [@freq ratio 5-2] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x64x32 hard test [@freq ratio 5-2] failed"<<std::endl;

//        if (BasicS2EngineTest(256,256,1,3,2100,3528,string("./data/S2Engine_tb6_1_256_256/")))
//            std::cout<<"Basic S2Engine Array 1x256x256 hard test [@freq ratio 3-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x256x256 hard test [@freq ratio 3-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(256,256,2,5,3500,3528,string("./data/S2Engine_tb6_1_256_256/")))
//            std::cout<<"Basic S2Engine Array 1x256x256 hard test [@freq ratio 5-2] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array 1x256x256 hard test [@freq ratio 5-2] failed"<<std::endl;;

//        if (BasicS2EngineTest(16,16,1,3,2100,2.53125,string("./data/S2Engine_tb7_1_16_16/")))
//            std::cout<<"Basic S2Engine Array with new scheduling strategy 1x16x16 hard test [@freq ratio 3-1] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array with new scheduling strategy 1x16x16 hard test [@freq ratio 3-1] failed"<<std::endl;
//
//        if (BasicS2EngineTest(16,16,2,5,3500,2.53125,string("./data/S2Engine_tb7_1_16_16/")))
//            std::cout<<"Basic S2Engine Array with new scheduling strategy 1x16x16 hard test [@freq ratio 5-2] passed"<<std::endl;
//        else
//            std::cout<<"Basic S2Engine Array with new scheduling strategy 1x16x16 hard test [@freq ratio 5-2] failed"<<std::endl;

        if (BasicS2EngineTest(16,16,1,3,2100,3528,string("./data/S2Engine_tb8_1_16_16/")))
            std::cout<<"Basic S2Engine Array with new mapping & scheduling strategy 1x16x16 hard test [@freq ratio 3-1] passed"<<std::endl;
        else
            std::cout<<"Basic S2Engine Array with new mapping & scheduling strategy 1x16x16 hard test [@freq ratio 3-1] failed"<<std::endl;

        if (BasicS2EngineTest(16,16,2,5,3500,3528,string("./data/S2Engine_tb8_1_16_16/")))
            std::cout<<"Basic S2Engine Array with new mapping & scheduling strategy 1x16x16 hard test [@freq ratio 5-2] passed"<<std::endl;
        else
            std::cout<<"Basic S2Engine Array with new mapping & scheduling strategy 1x16x16 hard test [@freq ratio 5-2] failed"<<std::endl;
    }
    bool BasicS2EngineTest(int height  ,int width,
                           int ruPeriod,int pePeriod,
                           int baseFreq,double equalWorkLoad,
                           string pathPrefix){
        S2Engine s2Engine(height,width,1,ruPeriod,pePeriod);

        queue<Object::PortDataType>
            bufPushUpData;

        vector<queue<Object::PortDataType> >
          xelInData( height,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
            wInData(  width,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
            oInData(  width,queue<Object::PortDataType>());
        vector<queue<Object::PortDataType> >
            outData(2*width,queue<Object::PortDataType>());

        uint32_t workLoad = 0;

        for (int i=0;i<1;i++){
            assert(i==0);
            FILE* fp = fopen(string(pathPrefix
                                  + RU_LIN_TRANS_FILE_PATH_PREFIX
                                  + string("0")
                                  + TRANS_FILE_TYPE).c_str(),"r");
            if (fp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            int tempWL,tempEOW;
            while(fscanf(fp,"%d%d",&tempWL,&tempEOW)>0)
                bufPushUpData.push(RU::CompressEWL(tempWL,tempEOW));
            fclose(fp);
        }
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
                    tempLoc,tempEOX;
                assert(dataType==0 || dataType==1);
                if (dataType!=0 && dataType!=1){
                    this->IllegalInput();
                    return false;
                }
                if (dataType == 0){
                    /// command
                    fscanf(fp,"%d",&tempCmd);
                    if (tempCmd<0 || tempCmd>=CE::CtrlNum){
                        assert(false);
                        this->IllegalInput();
                        return false;
                    }
                    xelInData[i].push(CE::Encode(CE::InsTable[tempCmd]));
                }
                else{
                    /// data
                    fscanf(fp,"%d%d%d",&tempVal,&tempLoc,&tempEOX);
                    if (tempLoc<0
                     ||(tempEOX!=0 && tempEOX!=1)){
                        assert(false);
                        this->IllegalInput();
                        return false;
                    }
                    xelInData[i].push(RU::CompressXEXL(tempVal,tempEOX,tempLoc,1));
                }
            }
            fclose(fp);
        }
        for (int h=0;h<2*height+width-1;h++)
            bufPushUpData.push(RU::CompressEWL(0,0));
        bufPushUpData.push(RU::CompressEWL(0,1));
        for (int h=0;h<height;h++){
            xelInData[h].push(CE::Encode(CE::CE_CTRL::BZ));
            xelInData[h].push(CE::Encode(CE::CE_CTRL::BZ));
        }

        for (uint32_t i=1;i<width;i++)
            for (uint32_t j=0;j<i;j++){
                wInData[i].push(0);
                oInData[i].push(0);
            }
        for (uint32_t i=0;i<width;i++){
            FILE *owInfp = fopen(string(pathPrefix
                                      + SA_WIN_TRANS_FILE_PATH_PREFIX
                                      + String::NumToString(i)
                                      + TRANS_FILE_TYPE).c_str(),"r");
            if (owInfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempWeight,tempOutMark;
            while (fscanf(owInfp,"%d%d",&tempWeight,&tempOutMark)>0){
                wInData[i].push(tempWeight);
                oInData[i].push(tempOutMark);
            }
            workLoad += wInData[i].size();
            fclose(owInfp);
        }
        workLoad *= height;
        for (uint32_t i=0;i<width;i++)
            for (uint32_t j=i+1;j<width;j++){
                wInData[i].push(0);
                oInData[i].push(0);
            }
        for (uint32_t i=0;i<width;i++)
            for (uint32_t j=0;j<2 * height + width;j++){
                wInData[i].push(0);
                oInData[i].push(0);
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
                outData[2*i  ].push(tempOut0);
                outData[2*i+1].push(tempOut1);
            }
            fclose(outfp);
        }

        Object::PortDataType bufPushUp,bufInUp;

        vector<Object::PortDataType> xelInList   (height);
        vector<Object::PortDataType> xelEmptyList(height);
        vector<Object::PortDataType> wInList     (width );
        vector<Object::PortDataType> oInList     (width );

        /** signal ports **/
        Object::Port pPEAct(1,Object::PortDirection::IN);
        Object::Port pRUAct(1,Object::PortDirection::IN);
        /** connect to RU array **/
        /** -----    up   ----- **/
        /// the up ports of reorder units
        Object::Port pBufInUp  (1+RU::WL_BIT_WIDTH,
                                  Object::PortDirection::OUT);  /** BufInUp   1+WL_BIT  in  **/
        Object::Port pBufPushUp(1,Object::PortDirection::OUT);  /** BufPushUp    1-bit  in  **/
        Object::Port pBufFullUp(1,Object::PortDirection::IN );  /** BufFullUp    1-bit  out **/
        /** -----   left  ----- **/
        vector<Object::Port> pXELInList   (height,Object::Port(2+RU:: X_BIT_WIDTH+RU::XL_BIT_WIDTH,
                                                                 Object::PortDirection::OUT)); /** XELIn 2+X_BIT+XL_BIT  in  **/
        vector<Object::Port> pXELPopList  (height,Object::Port(1,Object::PortDirection::IN )); /** XELPop         1-bit  out **/
        vector<Object::Port> pXELEmptyList(height,Object::Port(1,Object::PortDirection::OUT)); /** XELEmpty       1-bit  in  **/

        /** connect to PE array **/
        /** -----    up   ----- **/
        vector<Object::Port> pWInList(width,Object::Port( 8,Object::PortDirection::OUT));/**  WIn  8-bit in  **/
        vector<Object::Port> pOInList(width,Object::Port( 1,Object::PortDirection::OUT));/**  OIn  1-bit in  **/
        /** -----   left  ----- **/
        vector<Object::Port> pSEnOList(2*width,Object::Port( 1,Object::PortDirection::IN));
        vector<Object::Port> pSOutList(2*width,Object::Port(24,Object::PortDirection::IN));
        vector<Object::Port> pFOutList(2*width,Object::Port( 1,Object::PortDirection::IN));


        /** signal ports **/
        s2Engine.ConnectTo(pPEAct,s2Engine.PEAct);
        s2Engine.ConnectTo(pRUAct,s2Engine.RUAct);
        /** -----  up  ----- **/
        pBufInUp  .Clone(bufInUp  );
        pBufPushUp.Clone(bufPushUp);
        s2Engine.ConnectTo(s2Engine.BufInUp  ,pBufInUp  );/** BufInUp   1+WL_BIT  in  **/
        s2Engine.ConnectTo(s2Engine.BufPushUp,pBufPushUp);/** BufPushUp    1-bit  in  **/
        s2Engine.ConnectTo(pBufFullUp,s2Engine.BufFullUp);/** BufFullUp    1-bit  in  **/
        for (uint32_t i=0;i<  width;i++){
            pWInList[i].Clone(wInList[i]);
            pOInList[i].Clone(oInList[i]);
            s2Engine.ConnectTo(s2Engine.WInList[i],pWInList[i]);
            s2Engine.ConnectTo(s2Engine.OInList[i],pOInList[i]);
        }
        /** ----- ddown ----- **/
        for (uint32_t i=0;i<2*width;i++){
            s2Engine.ConnectTo(pSEnOList[i],s2Engine.SEnOList[i]);
            s2Engine.ConnectTo(pSOutList[i],s2Engine.SOutList[i]);
            s2Engine.ConnectTo(pFOutList[i],s2Engine.FOutList[i]);
        }
        /** ----- left ----- **/
        for (int i=0;i<height;i++){
            pXELInList   [i].Clone(xelInList   [i]);
            pXELEmptyList[i].Clone(xelEmptyList[i]);
            s2Engine.ConnectTo(s2Engine.XELInList   [i],pXELInList   [i]);
            s2Engine.ConnectTo(s2Engine.XELEmptyList[i],pXELEmptyList[i]);
            s2Engine.ConnectTo(pXELPopList[i],s2Engine.XELPopList[i]);
        }

        s2Engine.InnerConnect();
        assert(s2Engine.Connected());

        /** initialization **/
        /** ----- signal ports ----- **/
        bufPushUp = 0;
        bufInUp   = 233;
        for (int h=0;h<height;h++){
            /** ----- CE left ----- **/
            xelInList[h]  = 233;
            xelEmptyList[h] = 1;
        }
        for (int w=0;w<width ;w++){
            /** ------ PE up ------ **/
            wInList[w] = wInData[w].front();
            oInList[w] = oInData[w].front();
        }

        uint32_t clock = 0;
        uint32_t delay = 0;
        uint64_t totalLen = wInData[0].size();

        while (true){
            if (clock % 100000 == 0){
                #ifdef PRINT_PROCESS
                std::cout<<"@clock"<<clock
                         <<" "<<totalLen-wInData[0].size()
                         <<"/"<<totalLen<<std::endl;
                #endif // PRINT_PROCESS
//                std::cout<<bufPushUpData.size()<<std::endl;
//                for (int i=0;i<height;i++)
//                    std::cout<<xelInData[i].size()<<std::endl;
            }
            clock++;

            s2Engine.atPosEdge();
            s2Engine.Run();

            bool tempRUEnd = true;
            if (pRUAct.Data()==1){
                /** ----- up ----- **/
                if (!bufPushUpData.empty()){
                    tempRUEnd   = false;
                    if (pBufFullUp.Data()==0){
                        bufPushUp = 1;
                        bufInUp   = bufPushUpData.front();
                        bufPushUpData.pop();
                    }
                    else{
                        bufPushUp = 0;
                        bufInUp   = 233;
                    }
                }
                else{
                    bufPushUp = 0;
                    bufInUp   = 233;
                }
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
                        tempRUEnd = false;
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
            }
            else
                tempRUEnd = false;

            bool tempOutputEnd = true;
            if (pPEAct.Data()==1){
                for (uint32_t w=0;w<width;w++){
                    wInData[w].pop();
                    oInData[w].pop();
                    wInList[w] = wInData[w].front();
                    oInList[w] = oInData[w].front();
                }
                for (uint32_t w=0;w<2*width;w++){
                    if (pSEnOList[w].Data()==1){
                        if (outData[w].empty()){
                            if (pSOutList[w].Data()!=0){
                                assert(false);
                                return false;
                            }
                        }
                        else{
                            tempOutputEnd = false;
                            if (pSOutList[w].SignedData()
                               != outData[w].front()){
                                assert(false);
                                return false;
                            }
                            outData[w].pop();
                        }
                    }
                    else{
                        if (!outData[w].empty())
                            tempOutputEnd = false;
                    }
                }
            }
            else
                tempOutputEnd = false;
            if (tempOutputEnd)
                break;
            if (tempRUEnd){
                if (delay<(2*height+width)*pePeriod)
                    delay++;
                else
                    break;
            }
        }

        std::cout<<"total clock used      :"<<clock   <<std::endl;
        std::cout<<"total actual workload :"<<workLoad<<std::endl;
        std::cout<<"  actual   speed(Tops):"<<((2.0*     workLoad)/clock*baseFreq)/1000.0/1000.0<<std::endl;
        std::cout<<"equivalent speed(Tops):"<<((1.0*equalWorkLoad)/clock*baseFreq)<<std::endl;
        for (int i=0;i<2*width;i++)
            if (!outData[i].empty())
                return false;
        return true;
    }
};

class RUArrayTest{
private:
    void IllegalInput(){
        cout<<"Illegal input at RU Array test"<<endl;
        return;
    }
public:
    RUArrayTest(){
        if (BaseRUArrayTest(16,16,string("./data/ru_tb1_16_1/")))
            std::cout<<"Basic RU Array 16x1 easy test passed"<<std::endl;
        else
            std::cout<<"Basic RU Array 16x1 easy test failed"<<std::endl;

        if (BaseRUArrayTest(16,16,string("./data/ru_tb2_16_1/")))
            std::cout<<"Basic RU Array 16x1 hard test passed"<<std::endl;
        else
            std::cout<<"Basic RU Array 16x1 hard test failed"<<std::endl;
    }
    bool BaseRUArrayTest(
            int ceSize,
            int ruSize,
            string pathPrefix
        ){
        assert(ruSize==ceSize);
        assert(ceSize%ruSize==0);

        RUArray ruArray(ceSize,ruSize);

        queue<Object::PortDataType>
            bufPushUpData;

        vector<queue<Object::PortDataType> >
               xelInData(ceSize,queue<Object::PortDataType>()),
               ruOutData(ruSize,queue<Object::PortDataType>());

        for (int i=0;i*ceSize<ruSize;i++){
            assert(i==0);
            FILE* fp = fopen(string(pathPrefix
                                  + RU_LIN_TRANS_FILE_PATH_PREFIX
                                  + string("0")
                                  + TRANS_FILE_TYPE).c_str(),"r");
            int tempWL,tempEOW;
            while(fscanf(fp,"%d%d",&tempWL,&tempEOW)>0)
                bufPushUpData.push(RU::CompressEWL(tempWL,tempEOW));
            fclose(fp);
        }
        for (int i=0;i<ceSize;i++){
            FILE* fp = fopen(string(pathPrefix
                                  + RU_XIN_TRANS_FILE_PATH_PREFIX
                                  + String::NumToString(i)
                                  + string("_MF")
                                  + TRANS_FILE_TYPE).c_str(),"r");
            int dataType;
            while (fscanf(fp,"%d",&dataType)>0){
                int tempCmd,tempVal,
                    tempLoc,tempEOX;
                assert(dataType==0 || dataType==1);
                if (dataType!=0 && dataType!=1){
                    this->IllegalInput();
                    return false;
                }
                if (dataType == 0){
                    /// command
                    fscanf(fp,"%d",&tempCmd);
                    if (tempCmd<0 || tempCmd>=CE::CtrlNum){
                        assert(false);
                        this->IllegalInput();
                        return false;
                    }
                    xelInData[i].push(CE::Encode(CE::InsTable[tempCmd]));
                }
                else{
                    /// data
                    fscanf(fp,"%d%d%d",&tempVal,&tempLoc,&tempEOX);
                    if (tempLoc<0
                     ||(tempEOX!=0 && tempEOX!=1)){
                        assert(false);
                        this->IllegalInput();
                        return false;
                    }
                    xelInData[i].push(RU::CompressXEXL(tempVal,tempEOX,tempLoc,1));
                }
            }
        }
        for (int i=0;i<ruSize;i++){
            FILE* fp = fopen(string(pathPrefix
                                  + RU_XOUT_TRANS_FILE_PATH_PREFIX
                                  + String::NumToString(i)
                                  + TRANS_FILE_TYPE).c_str(),"r");
            int tempX;
            while(fscanf(fp,"%d",&tempX)>0)
                ruOutData[i].push(tempX);
            fclose(fp);
        }

        /** ---------- up -------- **/
        /// the up ports of reorder units
        Object::Port pBufPushUp(1,Object::PortDirection::OUT);  /** BufPushUp    1-bit  in  **/
        Object::Port pBufInUp  (1
               + RU::WL_BIT_WIDTH,Object::PortDirection::OUT);  /** BufInUp   1+WL_BIT  in  **/
        Object::Port pBufFullUp(1,Object::PortDirection::IN );  /** BufFullUp    1-bit  out **/
        /** --------- down ------- **/
        /// none
        /** -------- left -------- **/
        /// CE's left ports
        vector<Object::Port> pXELInList(ceSize,
        Object::Port(2+RU:: X_BIT_WIDTH+
                       RU::XL_BIT_WIDTH,
                       Object::PortDirection::OUT)); /** XELIn 2+X_BIT+XL_BIT  in  **/
        vector<Object::Port> pXELPopList(ceSize,
        Object::Port(1,Object::PortDirection::IN )); /** XELPop         1-bit  out **/
        vector<Object::Port> pXELEmptyList(ceSize,
        Object::Port(1,Object::PortDirection::OUT)); /** XELEmpty       1-bit  in  **/
        /** -------- right ------- **/
        vector<Object::Port> pOOutList(ceSize,
        Object::Port(RU::X_BIT_WIDTH,
                       Object::PortDirection::IN ));   /** OOut   X_BIT   out **/
        vector<Object::Port> pOPopList(ceSize,
        Object::Port(1,Object::PortDirection::OUT));   /** OPop   1-bit   in  **/
        vector<Object::Port> pOEmptyList(ceSize,
        Object::Port(1,Object::PortDirection::IN ));   /** OEmpty 1-bit   out **/

        Object::PortDataType
            bufPushUp,
            bufInUp;

        vector<Object::PortDataType>
               xelInList(ceSize),
            xelEmptyList(ceSize),
                oPopList(ceSize);

        /** ---------- up -------- **/
        /// the up ports of reorder units
        pBufPushUp.Clone(bufPushUp);
          pBufInUp.Clone(  bufInUp);
        for (int i=0;i<ceSize;i++){
            /** -------- left -------- **/
            /// CE's left ports
               pXELInList[i].Clone(   xelInList[i]);
            pXELEmptyList[i].Clone(xelEmptyList[i]);
            /** -------- right ------- **/
                pOPopList[i].Clone(    oPopList[i]);
        }

        /** ---------- up -------- **/
        /// the up ports of reorder units
        ruArray.ConnectTo(ruArray.BufPushUp,pBufPushUp);/** BufPushUp    1-bit  in  **/
        ruArray.ConnectTo(ruArray.BufInUp  ,pBufInUp  );/** BufInUp   1+WL_BIT  in  **/
        /** --------- down ------- **/
        /// none
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<ceSize;i++){
            ruArray.ConnectTo(ruArray.XELInList   [i],pXELInList   [i]);
            ruArray.ConnectTo(ruArray.XELEmptyList[i],pXELEmptyList[i]);
        }
        /** -------- right ------- **/
        for (int i=0;i<ceSize;i++){
            ruArray.ConnectTo(ruArray.OPopList    [i],pOPopList    [i]);
        }

        /** ---------- up -------- **/
        /// the up ports of reorder units
        ruArray.ConnectTo(pBufFullUp,ruArray.BufFullUp);/** BufFullUp    1-bit  in  **/
        /** --------- down ------- **/
        /// none
        /** -------- left -------- **/
        /// CE's left ports
        for (int i=0;i<ceSize;i++){
            ruArray.ConnectTo(pXELPopList[i],ruArray.XELPopList[i]);
        }
        /** -------- right ------- **/
        for (int i=0;i<ceSize;i++){
            ruArray.ConnectTo(pOOutList  [i],ruArray.OOutList  [i]);
            ruArray.ConnectTo(pOEmptyList[i],ruArray.OEmptyList[i]);
        }

        ruArray.InnerConnect();
        assert(ruArray.Connected());

        /** initialization **/
        bufPushUp = 0;
        bufInUp   = 233;

        for (int i=0;i<ceSize;i++){
            /** ----- left ----- **/
            xelInList[i]  = 233;
            xelEmptyList[i] = 1;
        }
        for (int i=0;i<ruSize;i++){
            /** ----- right ----- **/
            oPopList[i] = 0;
        }

        bufPushUpData.push(RU::CompressEWL(0,1));
        bufPushUpData.push(RU::CompressEWL(0,1));
        for (int i=0;i<ceSize;i++){
            xelInData[i].push(CE::Encode(CE::CE_CTRL::BZ));
            xelInData[i].push(CE::Encode(CE::CE_CTRL::BZ));
        }

        int outputNum = ruOutData[0].size();

        int clock = 0;
        while (true){
            if (clock%5000 == 0){
                cout<<"@clock"<<clock<<endl;
            }
            clock++;
            ruArray.atPosEdge();
            ruArray.Run();

            bool tempEnd = true;
            /** ----- up ----- **/
            if (!bufPushUpData.empty()){
                tempEnd   = false;
                if (pBufFullUp.Data()==0){
                    bufPushUp = 1;
                    bufInUp   = bufPushUpData.front();
                    bufPushUpData.pop();
                }
                else{
                    bufPushUp = 0;
                    bufInUp   = 233;
                }
            }
            else{
                bufPushUp = 0;
                bufInUp   = 233;
            }
            for (int i=0;i<ceSize;i++){
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
                    tempEnd = false;
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
            for (int i=0;i<ruSize;i++){
                /** ----- right ----- **/
                if (pOEmptyList[i].Data()==0){
                    /// pop
                    if (ruOutData[i].empty()){
                        if (pOOutList[i].Data()!=0){
                            assert(false);
                            return false;
                        }
                    }
                    else if (ruOutData[i].front()!=pOOutList[i].SignedData()){
                        assert(false);
                        return false;
                    }
                    else
                        ruOutData[i].pop();
                    oPopList[i] = 1;
                }
                else
                    oPopList[i] = 0;
                if (!ruOutData[i].empty())
                    tempEnd = false;
            }
            if (tempEnd)
                break;
        }
        std::cout<<"total clock used   :"<<clock<<std::endl;
        std::cout<<"total output number:"<<outputNum<<std::endl;
        std::cout<<"average clock ratio:"<<clock*1.0/outputNum<<std::endl;
        for (int i=0;i<ruSize;i++)
            if (!ruOutData[i].empty())
                return false;
        return true;
    }
};

class CETest{
private:
    inline void PushRABData  ( /// read from asyn FIFO, broadcast
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={}
//        Clear(bufInDownList);
      ///         xelInList []={}
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::RAB)); ///  0
                  xelInList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
                  xelInList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
                  xelInList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
                  xelInList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
                  xelInList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
                  xelInList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
                  xelInList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
     /** ========================= **/
      ///      bufOutUpList []={}
//         Clear(bufOutUpList);
      ///          oOutList []={}
//             Clear(oOutList);
                   oOutList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
                   oOutList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
                   oOutList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
                   oOutList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
                   oOutList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
                   oOutList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
                   oOutList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
        return;
    }
    inline void PushRAWData  ( /// read from asyn FIFO, write to sync FIFO
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={}
//        Clear(bufInDownList);
      ///         xelInList []={
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::RAW)); ///  0
                  xelInList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
                  xelInList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
                  xelInList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
                  xelInList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
                  xelInList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
                  xelInList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
                  xelInList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
     /** ========================= **/
      ///      bufOutUpList []={
//         Clear(bufOutUpList);
               bufOutUpList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
               bufOutUpList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
               bufOutUpList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
               bufOutUpList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
               bufOutUpList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
               bufOutUpList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
               bufOutUpList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///          oOutList []={}
//             Clear(oOutList);
        return;
    }
    inline void PushRAWBData ( /// read from asyn FIFO, write to sync FIFO, broadcast
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={},
//        Clear(bufInDownList);
      ///         xelInList []={
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::RAWB));///  0
                  xelInList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
                  xelInList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
                  xelInList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
                  xelInList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
                  xelInList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
                  xelInList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
                  xelInList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///      bufOutUpList []={
//         Clear(bufOutUpList);
               bufOutUpList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
               bufOutUpList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
               bufOutUpList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
               bufOutUpList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
               bufOutUpList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
               bufOutUpList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
               bufOutUpList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///          oOutList []={
//             Clear(oOutList);
                   oOutList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
                   oOutList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
                   oOutList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
                   oOutList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
                   oOutList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
                   oOutList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
                   oOutList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
        return;
    }
    inline void PushRAWBZData( /// read from asyn FIFO, write to sync FIFO, broadcast zero
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={}
//        Clear(bufInDownList);
      ///         xelInList []={
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::RAWBZ));///  0
                  xelInList.push(RU::CompressXEXL( 1,0, 2,1));        ///  1
                  xelInList.push(RU::CompressXEXL( 2,0, 5,1));        ///  2
                  xelInList.push(RU::CompressXEXL( 3,0, 7,1));        ///  3
                  xelInList.push(RU::CompressXEXL( 4,0, 9,1));        ///  4
                  xelInList.push(RU::CompressXEXL( 5,0,12,1));        ///  5
                  xelInList.push(RU::CompressXEXL( 6,0,15,1));        ///  6
                  xelInList.push(RU::CompressXEXL( 7,1,18,1));        ///  7
      ///      bufOutUpList []={
//         Clear(bufOutUpList);
               bufOutUpList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
               bufOutUpList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
               bufOutUpList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
               bufOutUpList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
               bufOutUpList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
               bufOutUpList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
               bufOutUpList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///          oOutList []={
//             Clear(oOutList);
                   oOutList.push(RU::CompressXEXL( 0,1, 0,1));       ///  1
        return;
    }
    inline void PushRSBData  ( /// read from sync FIFO, broadcast
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={
//        Clear(bufInDownList);
              bufInDownList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
              bufInDownList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
              bufInDownList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
              bufInDownList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
              bufInDownList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
              bufInDownList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
              bufInDownList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///         xelInList []={
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::RSB)); ///  0
      ///      bufOutUpList []={}
//         Clear(bufOutUpList);
      ///          oOutList []={
//             Clear(oOutList);
                   oOutList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
                   oOutList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
                   oOutList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
                   oOutList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
                   oOutList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
                   oOutList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
                   oOutList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
        return;
    }
    inline void PushRSBZData ( /// read from sync FIFO, broadcast zero
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={
//        Clear(bufInDownList);
              bufInDownList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
              bufInDownList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
              bufInDownList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
              bufInDownList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
              bufInDownList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
              bufInDownList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
              bufInDownList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///         xelInList []={
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::RSBZ)); ///  0
      ///      bufOutUpList []={}
//         Clear(bufOutUpList);
      ///          oOutList []={
//             Clear(oOutList);
                   oOutList.push(RU::CompressXEXL( 0,1, 0,1));       ///  1
        return;
    }
    inline void PushRSWBData ( /// read from sync FIFO, write to sync FIFO, broadcast
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={
//        Clear(bufInDownList);
              bufInDownList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
              bufInDownList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
              bufInDownList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
              bufInDownList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
              bufInDownList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
              bufInDownList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
              bufInDownList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///         xelInList []={
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::RSWB)); ///  0
      ///      bufOutUpList []={
//         Clear(bufOutUpList);
               bufOutUpList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
               bufOutUpList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
               bufOutUpList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
               bufOutUpList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
               bufOutUpList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
               bufOutUpList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
               bufOutUpList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///          oOutList
//             Clear(oOutList);
                   oOutList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
                   oOutList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
                   oOutList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
                   oOutList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
                   oOutList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
                   oOutList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
                   oOutList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
        return;
    }
    inline void PushRSWBZData( /// read from sync FIFO, write to sync FIFO, broadcast zero
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={
//        Clear(bufInDownList);
              bufInDownList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
              bufInDownList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
              bufInDownList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
              bufInDownList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
              bufInDownList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
              bufInDownList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
              bufInDownList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///         xelInList []={
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::RSWBZ)); ///  0
      ///      bufOutUpList []={
//         Clear(bufOutUpList);
               bufOutUpList.push(RU::CompressXEXL( 1,0, 2,1));       ///  1
               bufOutUpList.push(RU::CompressXEXL( 2,0, 5,1));       ///  2
               bufOutUpList.push(RU::CompressXEXL( 3,0, 7,1));       ///  3
               bufOutUpList.push(RU::CompressXEXL( 4,0, 9,1));       ///  4
               bufOutUpList.push(RU::CompressXEXL( 5,0,12,1));       ///  5
               bufOutUpList.push(RU::CompressXEXL( 6,0,15,1));       ///  6
               bufOutUpList.push(RU::CompressXEXL( 7,1,18,1));       ///  7
      ///          oOutList []={
//             Clear(oOutList);
                   oOutList.push(RU::CompressXEXL( 0,1, 0,1));       ///  1
        return;
    }
    inline void PushBZData   ( /// broadcast zero
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList){
      ///     bufInDownList []={}
//        Clear(bufInDownList);
      ///         xelInList []={
//            Clear(xelInList);
                  xelInList.push(CE::Encode(CE::CE_CTRL::BZ)); ///  0
      ///      bufOutUpList []={}
//         Clear(bufOutUpList);
      ///          oOutList []={
//             Clear(oOutList);
                   oOutList.push(RU::CompressXEXL( 0,1, 0,1));      ///  0
        return;
    }

public:
    CETest(){
        if (this->BasicCETestRAB()){
            std::cout<<"### basic single CE [RAB  ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [RAB  ] test failed ---"<<std::endl;
        }
        if (this->BasicCETestRAW()){
            std::cout<<"### basic single CE [RAW  ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [RAW  ] test failed ---"<<std::endl;
        }
        if (this->BasicCETestRAWB()){
            std::cout<<"### basic single CE [RAWB ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [RAWB ] test failed ---"<<std::endl;
        }
        if (this->BasicCETestRAWBZ()){
            std::cout<<"### basic single CE [RAWBZ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [RAWBZ] test failed ---"<<std::endl;
        }
        if (this->BasicCETestRSB()){
            std::cout<<"### basic single CE [RSB  ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [RSB  ] test failed ---"<<std::endl;
        }
        if (this->BasicCETestRSBZ()){
            std::cout<<"### basic single CE [RSBZ ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [RSBZ ] test failed ---"<<std::endl;
        }
        if (this->BasicCETestRSWB()){
            std::cout<<"### basic single CE [RSWB ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [RSWB ] test failed ---"<<std::endl;
        }
        if (this->BasicCETestRSWBZ()){
            std::cout<<"### basic single CE [RSWBZ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [RSWBZ] test failed ---"<<std::endl;
        }
        if (this->BasicCETestBZ()){
            std::cout<<"### basic single CE [BZ   ] test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE [BZ   ] test failed ---"<<std::endl;
        }
        if (this->BasicCEMixTest(5000000)){
            std::cout<<"### basic single CE   MIX   test passed ###"<<std::endl;
        }
        else{
            std::cout<<"--- basic single CE   MIX   test failed ---"<<std::endl;
        }
        return;
    }

    bool BasicCETest(
          queue<Object::PortDataType>& bufInDownList,
          queue<Object::PortDataType>&     xelInList,
     /** ========================= **/
          queue<Object::PortDataType>&  bufOutUpList,
          queue<Object::PortDataType>&      oOutList
        ){
        CE ce;
        /** ---------- up -------- **/
        Object::Port pBufPopUp    (1,Object::PortDirection::OUT);/** BufPopUp              1-bit   in  **/
        Object::Port pBufOutUp    (2
                  + RU:: X_BIT_WIDTH
                  + RU::XL_BIT_WIDTH,Object::PortDirection::IN );/** BufOutUp     2+X_BIT+XL_BIT  out  **/
        Object::Port pBufEmptyUp  (1,Object::PortDirection::IN );/** BufEmptyUp            1-bit  out  **/
        /** --------- down ------- **/
        Object::Port pBufPopDown  (1,Object::PortDirection::IN );/** BufPopDown            1-bit  out **/
        Object::Port pBufInDown   (2
                  + RU:: X_BIT_WIDTH
                  + RU::XL_BIT_WIDTH,Object::PortDirection::OUT);/** BufInDown    2+X_BIT+XL_BIT  in  **/
        Object::Port pBufEmptyDown(1,Object::PortDirection::OUT);/** BufEmptyDown          1-bit  in  **/
        /** --------- left ------- **/
        Object::Port pXELIn   (2
              + RU:: X_BIT_WIDTH
              + RU::XL_BIT_WIDTH,Object::PortDirection::OUT);/** XELIn    2+X_BIT+XL_BIT in  **/
        Object::Port pXELPop  (1,Object::PortDirection::IN );/** XELPop            1-bit out **/
        Object::Port pXELEmpty(1,Object::PortDirection::OUT);/** XELEmpty          1-bit in  **/
        /** -------- right ------- **/
        Object::Port pOOut    (2
              + RU:: X_BIT_WIDTH
              + RU::XL_BIT_WIDTH,Object::PortDirection::IN );/** OOut     2+X_BIT+XL_BIT out **/
        Object::Port pOPush   (1,Object::PortDirection::IN );/** OPush             1-bit out **/
        Object::Port pOFull   (1,Object::PortDirection::OUT);/** OFull             1-bit in  **/

        Object::PortDataType
            bufPopUp,     /**          1-bit  in  **/
            bufInDown,    /** 2+X_BIT+XL_BIT  in  **/
            bufEmptyDown, /**          1-bit  in  **/
            xelIn,        /** 2+X_BIT+XL_BIT  in  **/
            xelEmpty,     /**          1-bit  in  **/
            oFull;        /**          1-bit  in  **/

        /** ---------- up -------- **/
        pBufPopUp    .Clone(bufPopUp     );
        /** --------- down ------- **/
        pBufInDown   .Clone(bufInDown    );
        pBufEmptyDown.Clone(bufEmptyDown );
        /** --------- left ------- **/
        pXELIn       .Clone(xelIn        );
        pXELEmpty    .Clone(xelEmpty     );
        /** -------- right ------- **/
        pOFull       .Clone(oFull        );

        /** ---------- up -------- **/
        ce.ConnectTo(CE::BufPopUp    ,pBufPopUp    );
        /** --------- down ------- **/
        ce.ConnectTo(CE::BufInDown   ,pBufInDown   );
        ce.ConnectTo(CE::BufEmptyDown,pBufEmptyDown);
        /** --------- left ------- **/
        ce.ConnectTo(CE::XELIn       ,pXELIn       );
        ce.ConnectTo(CE::XELEmpty    ,pXELEmpty    );
        /** -------- right ------- **/
        ce.ConnectTo(CE::OFull       ,pOFull       );

        /** ---------- up -------- **/
        ce.ConnectTo(pBufOutUp  ,CE::BufOutUp  );
        ce.ConnectTo(pBufEmptyUp,CE::BufEmptyUp);
        /** --------- down ------- **/
        ce.ConnectTo(pBufPopDown,CE::BufPopDown);
        /** --------- left ------- **/
        ce.ConnectTo(pXELPop    ,CE::XELPop    );
        /** -------- right ------- **/
        ce.ConnectTo(pOOut      ,CE::OOut      );
        ce.ConnectTo(pOPush     ,CE::OPush     );

        ce.InnerConnect();
        assert(ce. InputConnected());
        assert(ce.OutputConnected());
        assert(ce. InnerConnected());
        assert(ce.      Connected());

//        int cycle=0;
        while (!bufInDownList.empty()
            || !    xelInList.empty()){
//            std::cout<<"@cycle"<<cycle
//                <<" left remains:"<<xelInList.size()
//                <<" down remains:"<<bufInDownList.size()
//                <<std::endl;
//            cycle++;
            /** ----- left ----- **/
            if (xelInList.empty()){
                if (pXELPop.Data()==1){
                    assert(false);
                    return false;
                }
                xelIn  = 233;
                xelEmpty = 1;
            }
            else{
                if (pXELPop.Data()==1){
                    xelInList.pop();
                }
                if (xelInList.empty()){
                    xelIn  = 233;
                    xelEmpty = 1;
                }
                else{
                    xelIn = xelInList.front();/** 2+X_BIT+XL_BIT  in  **/
                    xelEmpty = 0;
                }
            }
            /** ----- down ----- **/
            if (bufInDownList.empty()){
                if (pBufPopDown.Data()==1){
                    assert(false);
                    return false;
                }
                bufInDown  = 233;
                bufEmptyDown = 1;
            }
            else{
                if (pBufPopDown.Data()==1){
                    bufInDownList.pop();
                }
                if (bufInDownList.empty()){
                    bufInDown =  233;
                    bufEmptyDown = 1;
                }
                else{
                    bufInDown = bufInDownList.front();/** 2+X_BIT+XL_BIT  in  **/
                    bufEmptyDown = 0;
                }
            }
            /** ----- up ----- **/
            if (pBufEmptyUp.Data()==0){
                /// not empty
                if (bufOutUpList.empty()){
                    assert(false);
                    return false;
                }
                if (pBufOutUp.Data()!=bufOutUpList.front()){
                    assert(false);
                    return false;
                }
                else
                    bufOutUpList.pop();
                bufPopUp = 1;
            }
            else{
                /// empty
                bufPopUp = 0;
            }
            /** ----- right ----- **/
            if (pOPush.Data()==1){
                /// push
                if (oOutList.empty()){
                    assert(false);
                    return false;
                }
                if (pOOut.Data()!=oOutList.front()){
                    assert(false);
                    return false;
                }
                else
                    oOutList.pop();
            }
            oFull = 0;

            ce.atPosEdge();
            ce.Run();
//            ce.Print();
        }
        /** ----- left ----- **/
        if (xelInList.empty()){
            if (pXELPop.Data()==1){
                assert(false);
                return false;
            }
            xelIn  = 233;
            xelEmpty = 1;
        }
        else{
            if (pXELPop.Data()==1){
                xelInList.pop();
            }
            xelIn = xelInList.front();/** 2+X_BIT+XL_BIT  in  **/
            if (xelInList.empty())
                xelEmpty = 1;
            else
                xelEmpty = 0;
        }
        /** ----- down ----- **/
        if (bufInDownList.empty()){
            if (pBufPopDown.Data()==1){
                assert(false);
                return false;
            }
            bufInDown  = 233;
            bufEmptyDown = 1;
        }
        else{
            if (pBufPopDown.Data()==1){
                bufInDownList.pop();
            }
            bufInDown = bufInDownList.front();/** 2+X_BIT+XL_BIT  in  **/
            if (bufInDownList.empty())
                bufEmptyDown = 1;
            else
                bufEmptyDown = 0;
        }
        /** ----- up ----- **/
        if (pBufEmptyUp.Data()==0){
            /// not empty
            if (bufOutUpList.empty()){
                assert(false);
                return false;
            }
            if (pBufOutUp.Data()!=bufOutUpList.front()){
                assert(false);
                return false;
            }
            else
                bufOutUpList.pop();
            bufPopUp = 1;
        }
        else{
            /// empty
            bufPopUp = 0;
        }
        /** ----- right ----- **/
        if (pOPush.Data()==1){
            /// push
            if (oOutList.empty()){
                assert(false);
                return false;
            }
            if (pOOut.Data()!=oOutList.front()){
                assert(false);
                return false;
            }
            else
                oOutList.pop();
        }
        oFull = 0;

        if (!xelInList    .empty()
         || !bufInDownList.empty()
         || !bufOutUpList .empty()
         || !oOutList     .empty()){
            assert(bufInDownList.empty()
            &&         xelInList.empty());
            if (!bufOutUpList.empty()){
                std::cout<<"bufOutUpList is not empty"<<std::endl;
                std::cout<<"remains data:"<<std::endl;
                while(!    bufOutUpList.empty()){
               std::cout<< bufOutUpList.front()<<std::endl;
                           bufOutUpList.pop();
                }
            }
            if (!oOutList    .empty()){
                std::cout<<"oOutList is not empty"<<std::endl;
                std::cout<<"remains data:"<<std::endl;
                while(!   oOutList.empty()){
               std::cout<<oOutList.front()<<std::endl;
                          oOutList.pop();
                }
            }
            assert(false);
            return false;
        }
        return true;
    }

    inline bool BasicCETestRAB(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushRABData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCETestRAW(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushRAWData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCETestRAWB(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushRAWBData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCETestRAWBZ(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushRAWBZData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCETestRSB(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushRSBData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCETestRSBZ(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushRSBZData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCETestRSWB(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushRSWBData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCETestRSWBZ(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushRSWBZData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCETestBZ(){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        this->PushBZData(
            bufInDownList,
                xelInList,
        /** ============== **/
             bufOutUpList,
                 oOutList
        );
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
    inline bool BasicCEMixTest(int number){
        queue<Object::PortDataType> bufInDownList;
        queue<Object::PortDataType>     xelInList;
     /** ========================= **/
        queue<Object::PortDataType>  bufOutUpList;
        queue<Object::PortDataType>      oOutList;

        for (int i=0;i<number;i++){
            switch(rand()%9){
            case 0:
                this->PushRABData  (bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            case 1:
                this->PushRAWData  (bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            case 2:
                this->PushRAWBData (bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            case 3:
                this->PushRAWBZData(bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            case 4:
                this->PushRSBData  (bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            case 5:
                this->PushRSBZData (bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            case 6:
                this->PushRSWBData (bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            case 7:
                this->PushRSWBZData(bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            case 8:
                this->PushBZData   (bufInDownList,xelInList,bufOutUpList,oOutList);
                break;
            }
        }
        return
            this->BasicCETest(
                bufInDownList,
                    xelInList,
            /** ============== **/
                 bufOutUpList,
                     oOutList
            );
    }
};

class RUTest{
public:
    RUTest(){
        this->BasicRUTest();
        std::cout<<"basic single RU test passed"<<std::endl;
        return;
    }
    void BasicRUTest(){
        RU ru;
        /** ---------- up -------- **/
        Object::Port pBufPushUp  (1,Object::PortDirection::OUT);/** BufPushUp    1-bit  in  **/
        Object::Port pBufInUp    (1
                 + RU::WL_BIT_WIDTH,Object::PortDirection::OUT);/** BufInUp   1+WL_BIT  in  **/
        Object::Port pBufFullUp  (1,Object::PortDirection::IN );/** BufFullUp    1-bit out **/
        /** --------- down ------- **/
        Object::Port pBufPushDown(1,Object::PortDirection::IN );/** BufPushDown  1-bit  out **/
        Object::Port pBufInDown  (1
                 + RU::WL_BIT_WIDTH,Object::PortDirection::IN );/** BufInDown 1+WL_BIT  out **/
        Object::Port pBufFullDown(1,Object::PortDirection::OUT);/** BufFullDown  1-bit  in  **/
        /** --------- left ------- **/
        Object::Port pXELIn  (2
             + RU:: X_BIT_WIDTH
             + RU::XL_BIT_WIDTH,Object::PortDirection::OUT);/** XELIn    2+X_BIT+XL_BIT in  **/
        Object::Port pXELPush(1,Object::PortDirection::OUT);/** XELPush  1-bit          in  **/
        Object::Port pXELFull(1,Object::PortDirection::IN );/** XELFull  1-bit          out **/
        /** -------- right ------- **/
        Object::Port pOOut   (
               RU:: X_BIT_WIDTH,Object::PortDirection::IN );/** OOut   X_BIT   out **/
        Object::Port pOPop   (1,Object::PortDirection::OUT);/** OPop   1-bit   in  **/
        Object::Port pOEmpty (1,Object::PortDirection::IN );/** OEmpty 1-bit   out **/

        Object::PortDataType
            bufPushUp,  /**          1-bit  in  **/
            bufInUp,    /**       1+WL_BIT  in  **/
            bufFullDown,/**          1-bit  in  **/
            xelIn,      /** 2+X_BIT+XL_BIT  in  **/
            xelPush,    /**          1-bit  in  **/
            oPop;       /**          1-bit  in  **/

        pBufPushUp  .Clone(bufPushUp  );
        pBufInUp    .Clone(bufInUp    );
        pBufFullDown.Clone(bufFullDown);
        pXELIn      .Clone(xelIn  );
        pXELPush    .Clone(xelPush);
        pOPop       .Clone(oPop   );

        Object::PortDataType

        /************************************************
         ** w:                                         **
         ** position:   3   8  13  14  24  25  35      **
         **    eow  :   0   0   0   0   0   0   1      **
         ** position:   1   2   5  13  35  38  63      **
         **    eow  :   0   0   0   0   0   0   1      **
         **                                            **
         ** x:                                         **
         ** position:   0   1   2  13  24  35  46  63  **
         **   value : 122 211 233 100  99  52  13  87  **
         **    eox  :   0   0   0   0   0   0   0   1  **
         ** position:   3   8  13  25  27              **
         **   value : 251 102 201   9   7              **
         **    eox  :   0   0   0   0   1              **
         **                                            **
         **  output :   0   0 100   0  99   0  52      **
         **             0   0   0 201   0   0   0      **
         ************************************************/

        ///                   0   1   2   3   4   5   6   7   8   9  10  11  12  13
          bufPushUpList []={  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
             wlInUpList []={  3,  8, 13, 14, 24, 25, 35,  1,  2,  5, 13, 35, 38, 63},
            eowInUpList []={  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  1},
        bufFullDownList []={  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
                xInList []={122,211,233,100, 99, 52, 13, 87,251,102,201,  9,  7,  0},
              eoxInList []={  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  1,  0},
               xlInList []={  0,  1,  2, 13, 24, 35, 46, 63,  3,  8, 13, 25, 27,  0},
               xvInList []={  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0},
            xelPushList []={  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0},
///               oPopList []={},
        /** ============================== **/
          bufFullUpList []={  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
        bufPushDownList []={  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
          bufWLDownList []={  3,  8, 13, 14, 24, 25, 35,  1,  2,  5, 13, 35, 38, 63},
         bufEOWDownList []={  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  1},
            xelFullList []={  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
               oOutList []={  0,  0,100,  0, 99,  0, 52,  0,  0,  0,201,  0,  0,  0};
///             oEmptyList []={};

        ru.ConnectTo(RU::BufPushUp  ,pBufPushUp  );
        ru.ConnectTo(RU::BufInUp    ,pBufInUp    );
        ru.ConnectTo(RU::BufFullDown,pBufFullDown);
        ru.ConnectTo(RU::XELIn      ,pXELIn      );
        ru.ConnectTo(RU::XELPush    ,pXELPush    );
        ru.ConnectTo(RU::OPop       ,pOPop       );

        ru.ConnectTo(pBufFullUp  ,RU::BufFullUp  );
        ru.ConnectTo(pBufPushDown,RU::BufPushDown);
        ru.ConnectTo(pBufInDown  ,RU::BufInDown  );
        ru.ConnectTo(pXELFull    ,RU::XELFull    );
        ru.ConnectTo(pOOut       ,RU::OOut       );
        ru.ConnectTo(pOEmpty     ,RU::OEmpty     );

        ru.InnerConnect();
        assert(ru. InputConnected());
        assert(ru.OutputConnected());
        assert(ru. InnerConnected());
        assert(ru.      Connected());

        for (int i=0;i<14;i++){
            bufPushUp   =   bufPushUpList[i];
            bufFullDown = bufFullDownList[i];
            bufInUp     = RU::CompressEWL(
                             wlInUpList[i],
                            eowInUpList[i]
                          );
            xelIn       = RU::CompressXEXL(
                                xInList[i],
                              eoxInList[i],
                               xlInList[i],
                               xvInList[i]
                          );
            xelPush     = xelPushList[i];
            oPop        = 0;

            ru.atPosEdge();
            ru.Run();
//            std::cout<<"================="<<std::endl;
//            std::cout<<" after cycle  "<<i<<std::endl;
//            std::cout<<"================="<<std::endl;
//            ru.Print();

//            std::cout<<"[BufFullUp  ]"<<pBufFullUp  .Data()<<std::endl;
//            std::cout<<"[BufPushDown]"<<pBufPushDown.Data()<<std::endl;
//            std::cout<<"[BufInDown  ]"<<pBufInDown  .Data()<<std::endl;
//            std::cout<<"[XELFull    ]"<<pXELFull    .Data()<<std::endl;

            assert(pBufFullUp  .Data() ==   bufFullUpList[i]
                && pBufPushDown.Data() == bufPushDownList[i]
                && pBufInDown  .Data() == RU::CompressEWL(
                                            bufWLDownList[i],
                                           bufEOWDownList[i])
                && pXELFull    .Data() ==     xelFullList[i]
            );
        }
        bufPushUp   = 1;
        bufFullDown = 0;
        bufInUp     = 0;
        xelIn       = 0;
        xelPush     = 1;
        oPop        = 0;
        for (int i=0;i<10;i++){
            ru.atPosEdge();
            ru.Run();
//            ru.Print();
        }
        bufPushUp   = 0;
        bufFullDown = 1;
        bufInUp     = 0;
        xelIn       = 0;
        xelPush     = 0;
        oPop        = 0;
        for (int i=0;i<10;i++){
            ru.atPosEdge();
            ru.Run();
//            ru.Print();
        }

        bufPushUp   = 0;
        bufFullDown = 0;
        bufInUp     = 0;
        xelIn       = 0;
        xelPush     = 0;
        oPop        = 1;
        for (int i=0;i<13;i++){
//            std::cout<<"####################"<<std::endl;
//            std::cout<<"## output serial:"<<i<<std::endl;
//            std::cout<<"####################"<<std::endl;
            assert(pBufFullUp  .Data() == 0
                && pBufPushDown.Data() == 0
                && pBufInDown  .Data() == 0
                && pXELFull    .Data() == 0
                && pOOut       .Data() == oOutList[i]
                && pOEmpty     .Data() == 0
            );
            ru.atPosEdge();
            ru.Run();
        }
        assert(pBufFullUp  .Data() == 0
            && pBufPushDown.Data() == 0
            && pBufInDown  .Data() == 0
            && pXELFull    .Data() == 0
            && pOOut       .Data() == oOutList[13]
            && pOEmpty     .Data() == 1
        );
        return;
    }
};

class SynPEArrayTest{
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
    SynPEArrayTest(){
        if (BasicTest(  4,  4,string("./data/tb1_4_4/")))
            std::cout<<"basic synchronized PE array @  4_4   test passed"<<std::endl;
        else
            std::cout<<"basic synchronized PE array @  4_4   test failed"<<std::endl;

        if (BasicTest( 16, 16,string("./data/tb2_16_16/")))
            std::cout<<"basic synchronized PE array @ 16_16  test passed"<<std::endl;
        else
            std::cout<<"basic synchronized PE array @ 16_16  test failed"<<std::endl;

        if (BasicTest( 64, 32,string("./data/tb3_64_32/")))
            std::cout<<"basic synchronized PE array @ 64_32  test passed"<<std::endl;
        else
            std::cout<<"basic synchronized PE array @ 64_32  test failed"<<std::endl;

        if (BasicTest(256,256,string("./data/tb4_256_256/")))
            std::cout<<"basic synchronized PE array @256_256 test passed"<<std::endl;
        else
            std::cout<<"basic synchronized PE array @256_256 test passed"<<std::endl;
    }


    bool BasicTest(uint32_t width,uint32_t height,string pathPrefix) {

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
            Object::PortDataType tempWeight,tempOutMark;
            while (fscanf(owInfp,"%d%d",&tempWeight,&tempOutMark)>0){
                wInData[i].push_back(tempWeight);
                oInData[i].push_back(tempOutMark);
            }
            fclose(owInfp);
        }
        for (uint32_t i=0;i<height;i++){
            FILE *xInfp = fopen(xInPath[i].c_str(),"r");
            if (xInfp==nullptr){
                std::cout<<"Error opening file"<<std::endl;
                exit(1);
            }
            Object::PortDataType tempFeature;
            while (fscanf(xInfp,"%d",&tempFeature)>0)
                xInData[i].push_back(tempFeature);
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
            if (i%1000==0)
                std::cout<<"@clock"<<i<<"/"<<inputLen<<std::endl;
            for (uint32_t w=0;w<width;w++){
                wInList[w] = wInData[w][i];
                oInList[w] = oInData[w][i];
            }
            for (uint32_t h=0;h<height;h++)
                xInList[h] = xInData[h][i];

            peArray.atPosEdge();
            peArray.Run();

            peArray.PrintW();
            peArray.PrintX();
            peArray.PrintS();
            peArray.PrintP();

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
        return true;
    }
};

class SynPETest{
public:
    SynPETest(){
        this->SinglePETest();
        std::cout<<"single synchronized PE simple test passed"<<std::endl;
        return;
    }

    void SinglePETest(){
        SynPE synPE;

        Object::Port pSEn( 1,Object::PortDirection::OUT);/**  SEn  1-bit in  **/
        Object::Port pSIn(24,Object::PortDirection::OUT);/**  SIn 24-bit in  **/
        Object::Port pFIn( 1,Object::PortDirection::OUT);/**  FIn  1-bit in  **/
        Object::Port pWIn( 8,Object::PortDirection::OUT);/**  WIn  8-bit in  **/
        Object::Port pOIn( 1,Object::PortDirection::OUT);/**  OIn  1-bit in  **/
        Object::Port pXIn( 8,Object::PortDirection::OUT);/**  XIn  8-bit in  **/

        Object::Port pSEnO( 1,Object::PortDirection::IN);/** SEnO  1-bit out **/
        Object::Port pSOut(24,Object::PortDirection::IN);/** SOut 24-bit out **/
        Object::Port pFOut( 1,Object::PortDirection::IN);/** FOut  1-bit out **/
        Object::Port pWOut( 8,Object::PortDirection::IN);/** WOut  8-bit out **/
        Object::Port pOOut( 1,Object::PortDirection::IN);/** OOut  1-bit out **/
        Object::Port pXOut( 8,Object::PortDirection::IN);/** XOut  8-bit out **/

        Object::PortDataType sEn,sIn,fIn,wIn,oIn,xIn;

        pSEn.Clone(sEn);
        pSIn.Clone(sIn);
        pFIn.Clone(fIn);
        pWIn.Clone(wIn);
        pOIn.Clone(oIn);
        pXIn.Clone(xIn);

        Object::PortDataType
        ///                0   1   2   3   4   5   6   7   8   9  10  11  12   13   14  15   16  17   18   19   20   21  22   23   24
        wInList      []={ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,  23,  24, 25,  26, 27,  28,  29,  30,  31, 32,  33,  34},
        oInList      []={  0,  0,  0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0,   0,   0,  0,   1,  0,   0,   0,   0,   1,  0,   0,   1},
        xInList      []={  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13,  14,  15, 16,  17, 18,  19,  20,  21,  22, 23,  24,  25},
        sInList      []={  0,  0,  0,877,888,  0,  0,  0,122,  0,  0,  0,  0,   0,   0,888,   0,  0,   0,   0,   0,   0,  0,   0,   0},
        fInList      []={  0,  0,  0,  1,  1,  0,  0,  0,  1,  0,  0,  0,  0,   0,   0,  1,   0,  0,   0,   0,   0,   0,  0,   0,   0},
        sEnList      []={  0,  0,  0,  1,  1,  0,  0,  0,  1,  0,  0,  0,  0,   0,   0,  1,   0,  0,   0,   0,   0,   0,  0,   0,   0},
        /** ============================== **/
        wOutList     []={ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,  23,  24, 25,  26, 27,  28,  29,  30,  31, 32,  33,  34},
        oOutList     []={  0,  0,  0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  0,   0,   0,  0,   1,  0,   0,   0,   0,   1,  0,   0,   1},
        xOutList     []={  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13,  14,  15, 16,  17, 18,  19,  20,  21,  22, 23,  24,  25},
        sOutList     []={ 10, 32, 68,877,888,280,112,248,122,600,220,472,758,1080,1440,888,2282,486,1018,1598,2228,2910,736,1528,2378},
        fOutList     []={  0,  0,  0,  1,  1,  1,  0,  0,  1,  1,  0,  0,  0,   0,   0,  1,   1,  0,   0,   0,   0,   1,  0,   0,   1},
        sEnOList     []={  0,  0,  0,  1,  1,  1,  0,  0,  1,  1,  0,  0,  0,   0,   0,  1,   1,  0,   0,   0,   0,   1,  0,   0,   1};

        synPE.ConnectTo(SynPE::SEn,pSEn);
        synPE.ConnectTo(SynPE::SIn,pSIn);
        synPE.ConnectTo(SynPE::FIn,pFIn);
        synPE.ConnectTo(SynPE::WIn,pWIn);
        synPE.ConnectTo(SynPE::OIn,pOIn);
        synPE.ConnectTo(SynPE::XIn,pXIn);

        synPE.ConnectTo(pSEnO,SynPE::SEnO);
        synPE.ConnectTo(pSOut,SynPE::SOut);
        synPE.ConnectTo(pFOut,SynPE::FOut);
        synPE.ConnectTo(pWOut,SynPE::WOut);
        synPE.ConnectTo(pOOut,SynPE::OOut);
        synPE.ConnectTo(pXOut,SynPE::XOut);

        synPE.InnerConnect();
        assert(synPE.InputConnected());
        assert(synPE.OutputConnected());
        assert(synPE.InnerConnected());
        assert(synPE.Connected());

        for (int i=0;i<25;i++){
            sEn = sEnList[i];
            sIn = sInList[i];
            fIn = fInList[i];
            wIn = wInList[i];
            oIn = oInList[i];
            xIn = xInList[i];

            synPE.atPosEdge();
            synPE.Run();
            std::cout<<"================="<<std::endl;
            std::cout<<" after cycle  "<<i<<std::endl;
            std::cout<<"================="<<std::endl;
            system("pause");

            assert(pSEnO.Data() ==  sEnOList[i]
                && pSOut.Data() ==  sOutList[i]
                && pFOut.Data() ==  fOutList[i]
                && pWOut.Data() ==  wOutList[i]
                && pOOut.Data() ==  oOutList[i]
                && pXOut.Data() ==  xOutList[i]
            );

        }
        return;
    }
};

class BasicTest{
public:
    BasicTest(){
        this->TestAdder();
        std::cout<<"[Basic Combinational Logic test]  Adder Passed"<<std::endl;
        this->TestMulter();
        std::cout<<"[Basic Combinational Logic test] Multer Passed"<<std::endl;
        this->TestReg();
        std::cout<<"[Basic    Sequential Logic test]     Reg Passed"<<std::endl;
        this->TestMarkReg();
        std::cout<<"[Basic    Sequential Logic test] MarkReg Passed"<<std::endl;
        this->TestCounter();
        std::cout<<"[Basic    Sequential Logic test] Counter Passed"<<std::endl;
        this->TestFIFO();
        std::cout<<"[Basic    Sequential Logic test]    FIFO Passed"<<std::endl;
    }
    void TestAdder(){
        Adder adder(8,8,8,false);
        Object::Port port1(8,Object::PortDirection::OUT);
        Object::Port port2(8,Object::PortDirection::OUT);
        Object::Port port3(8,Object::PortDirection::IN);
        Object::PortDataType data1,data2;
        port1.Clone(data1);
        port2.Clone(data2);
        Adder::PortName adderInput0(Adder::PortName::_PortName::input0);
        Adder::PortName adderInput1(Adder::PortName::_PortName::input1);
        Adder::PortName adderOutput(Adder::PortName::_PortName::output);

        assert(!adder. InputConnected()
            &&  adder.OutputConnected());
        adder.ConnectTo(adderInput0,port1);
        assert(!adder. InputConnected()
            &&  adder.OutputConnected());
        adder.ConnectTo(adderInput1,port2);
        assert(adder.Connected());
        adder.ConnectTo(port3,adderOutput);

        data1 = 1;
        data2 = 2;
        adder.Run();
        assert(port3.Data()==3);

        data1 = 2;
        data2 = 1;
        adder.Run();
        assert(port3.Data()==3);

        data1 = -1;
        data2 =  1;
        adder.Run();
        assert(port3.Data()==0);

        data1 = -2;
        data2 = 255;
        adder.Run();
        assert(port3.Data()==253);

        data1 = 0;
        data2 = 256;
        adder.Run();
        assert(port3.Data()==0);

        data1 = 255;
        data2 = 255;
        adder.Run();
        assert(port3.Data()==254);
        return;
    }
    void TestMulter(){
        Multer multer(8,8,16,false);
        Object::Port port1( 8,Object::PortDirection::OUT);
        Object::Port port2( 8,Object::PortDirection::OUT);
        Object::Port port3(16,Object::PortDirection::IN);
        Object::PortDataType data1,data2;
        port1.Clone(data1);
        port2.Clone(data2);
        Multer::PortName multerInput0(Multer::PortName::_PortName::input0);
        Multer::PortName multerInput1(Multer::PortName::_PortName::input1);
        Multer::PortName multerOutput(Multer::PortName::_PortName::output);


        assert(!multer. InputConnected()
            &&  multer.OutputConnected());
        multer.ConnectTo(multerInput0,port1);
        assert(!multer. InputConnected()
            &&  multer.OutputConnected());
        multer.ConnectTo(multerInput1,port2);
        assert(multer.Connected());
        multer.ConnectTo(port3,multerOutput);

        data1 = 1;
        data2 = 2;
        multer.Run();
        assert(port3.Data()==2);

        data1 = 2;
        data2 = 1;
        multer.Run();
        assert(port3.Data()==2);

        data1 = -1;
        data2 =  1;
        multer.Run();
        assert(port3.Data()== 255);

        data1 = -2;
        data2 = 255;
        multer.Run();
        assert(port3.Data()==64770);

        data1 = 255;
        data2 = 255;
        multer.Run();
        assert(port3.Data()==65025);

        data1 =-255;
        data2 = 255;
        multer.Run();
        assert(port3.Data()==255);
        return;
    }
    void TestReg(){
        Reg reg(8,ClockedObject::ClockEdge::POS_EDGE);
        Object::Port port1(8,Object::PortDirection::OUT);
        Object::Port port2(1,Object::PortDirection::OUT);
        Object::Port port3(8,Object::PortDirection::IN);
        Object::PortDataType in,en;
        port1.Clone(in);
        port2.Clone(en);
        Reg::PortName regIn (Reg::PortName::_PortName::in );
        Reg::PortName regEn (Reg::PortName::_PortName::en );
        Reg::PortName regOut(Reg::PortName::_PortName::out);

        assert(!reg. InputConnected()
            &&  reg.OutputConnected());
        reg.ConnectTo(regIn,port1 );
        assert(!reg. InputConnected()
            &&  reg.OutputConnected());
        reg.ConnectTo(regEn,port2 );
        assert(reg.Connected());
        reg.ConnectTo(port3,regOut);

        in = 255;
        en = 1;
        reg.Run();
        assert(port3.Data()==  0);
        reg.atPosEdge();
        assert(port3.Data()==  0);
        reg.Run();
        assert(port3.Data()==255);

        in = 1;
        en = 0;
        reg.atPosEdge();
        assert(port3.Data()==255);
        reg.Run();
        assert(port3.Data()==255);

        in = 128;
        en = 1;
        reg.atPosEdge();
        assert(port3.Data()==255);
        reg.Run();
        assert(port3.Data()==128);


        Reg reg0(8,ClockedObject::ClockEdge::POS_EDGE,  0);
        Reg reg1(8,ClockedObject::ClockEdge::POS_EDGE,255);
        Object::Port port4(8,Object::PortDirection:: IN);
        Object::Port port5(8,Object::PortDirection:: IN);
        Object::Port port6(1,Object::PortDirection::OUT);
        Object::Port port7(1,Object::PortDirection::OUT);
        Object::PortDataType en0,en1;
        port6.Clone(en0);
        port7.Clone(en1);
        reg0.ConnectTo(regIn,reg1,regOut);
        reg1.ConnectTo(regIn,reg0,regOut);
        reg0.ConnectTo(port4,regOut);
        reg1.ConnectTo(port5,regOut);
        reg0.ConnectTo(regEn,port6 );
        reg1.ConnectTo(regEn,port7 );

        assert(reg0.Connected() && reg1.Connected());

        en0 = 0;
        en1 = 0;
        assert(port4.Data()==  0);
        assert(port5.Data()==255);
        reg0.atPosEdge();
        reg1.atPosEdge();
        assert(port4.Data()==  0);
        assert(port5.Data()==255);
        reg0.Run();
        reg1.Run();
        assert(port4.Data()==  0);
        assert(port5.Data()==255);

        en0 = 1;
        en1 = 1;
        reg0.atPosEdge();
        reg1.atPosEdge();
        assert(port4.Data()==  0);
        assert(port5.Data()==255);
        reg0.Run();
        reg1.Run();
        assert(port4.Data()==255);
        assert(port5.Data()==  0);

        reg1.atPosEdge();
        reg0.atPosEdge();
        assert(port4.Data()==255);
        assert(port5.Data()==  0);
        reg0.Run();
        reg1.Run();
        assert(port4.Data()==  0);
        assert(port5.Data()==255);

        reg0.atPosEdge();
        reg1.atPosEdge();
        assert(port4.Data()==  0);
        assert(port5.Data()==255);
        reg1.Run();
        reg0.Run();
        assert(port4.Data()==255);
        assert(port5.Data()==  0);

        en0 = 0;
        en1 = 1;
        reg0.atPosEdge();
        reg1.atPosEdge();
        assert(port4.Data()==255);
        assert(port5.Data()==  0);
        reg1.Run();
        reg0.Run();
        assert(port4.Data()==255);
        assert(port5.Data()==255);
        return;
    }
    void TestMarkReg(){
        MarkReg markReg(ClockedObject::ClockEdge::POS_EDGE);
        Object::Port port1(1,Object::PortDirection::OUT);
        Object::Port port2(1,Object::PortDirection::OUT);
        Object::Port port3(1,Object::PortDirection::IN);
        Object::PortDataType Set,Clr;
        port1.Clone(Set);
        port2.Clone(Clr);
        MarkReg::PortName markRegSet(MarkReg::PortName::_PortName::Set);
        MarkReg::PortName markRegClr(MarkReg::PortName::_PortName::Clr);
        MarkReg::PortName markRegOut(MarkReg::PortName::_PortName::out);

        assert(!markReg. InputConnected()
            &&  markReg.OutputConnected());
        markReg.ConnectTo(markRegSet,port1);
        assert(!markReg. InputConnected()
            &&  markReg.OutputConnected());
        markReg.ConnectTo(markRegClr,port2);
        assert(markReg.Connected());
        markReg.ConnectTo(port3,markRegOut);

        Set=0;
        Clr=0;
        assert(port3.Data()==0);
        markReg.atPosEdge();
        assert(port3.Data()==0);
        markReg.Run();
        assert(port3.Data()==0);

        Set=1;
        Clr=0;
        assert(port3.Data()==0);
        markReg.atPosEdge();
        assert(port3.Data()==0);
        markReg.Run();
        assert(port3.Data()==1);

        Set=0;
        Clr=1;
        assert(port3.Data()==1);
        markReg.atPosEdge();
        assert(port3.Data()==1);
        markReg.Run();
        assert(port3.Data()==0);
        return;
    }
    void TestCounter(){
        Counter counter(6,ClockedObject::ClockEdge::POS_EDGE);
        Object::Port port1(1,Object::PortDirection::OUT);
        Object::Port port2(1,Object::PortDirection::IN);
        Object::Port port3(1,Object::PortDirection::OUT);
        Object::PortDataType en,rst;
        port1.Clone(en);
        port3.Clone(rst);
        Counter::PortName CounterEn  (Counter::PortName::_PortName::en);
        Counter::PortName CounterDone(Counter::PortName::_PortName::done);
        Counter::PortName CounterRst (Counter::PortName::_PortName::rst);

        assert(!counter. InputConnected()
            &&  counter.OutputConnected());
        counter.ConnectTo(CounterEn,  port1);
        counter.ConnectTo(CounterRst, port3);
        assert(counter.Connected());
        counter.ConnectTo(port2,CounterDone);

        rst= 0;
        en = 0;
        for (int i=0;i<100;i++){
           counter.atPosEdge();
           assert(port2.Data()==0);
           counter.Run();
           assert(port2.Data()==0);
        }

        en = 1;
        counter.atPosEdge();
        assert(port2.Data()==0);
        counter.Run();
        assert(port2.Data()==0);
        en = 0;
        for (int i=1;i<4;i++){
           counter.atPosEdge();
           assert(port2.Data()==0);
           counter.Run();
           assert(port2.Data()==0);
        }
        counter.atPosEdge();
        assert(port2.Data()==0);
        counter.Run();
        assert(port2.Data()==1);
        for (int i=0;i<100;i++){
           counter.atPosEdge();
           assert(port2.Data()==1);
           counter.Run();
           assert(port2.Data()==1);
        }

        en = 1;
        counter.atPosEdge();
        assert(port2.Data()==1);
        counter.Run();
        assert(port2.Data()==0);
        en = 0;
        for (int i=1;i<4;i++){
           counter.atPosEdge();
           assert(port2.Data()==0);
           counter.Run();
           assert(port2.Data()==0);
        }
        counter.atPosEdge();
        assert(port2.Data()==0);
        counter.Run();
        assert(port2.Data()==1);

        en=0;
        counter.atPosEdge();
        assert(port2.Data()==1);
        counter.Run();
        assert(port2.Data()==1);

        rst=1;
        en =0;
        counter.atPosEdge();
        assert(port2.Data()==1);
        counter.Run();
        assert(port2.Data()==0);
        return;
    }
    void TestFIFO(){
        FIFO fifo(8,10,ClockedObject::ClockEdge::POS_EDGE);
        Object::Port port1(8,Object::PortDirection::OUT);
        Object::Port port2(8,Object::PortDirection::IN );
        Object::Port port3(1,Object::PortDirection::OUT);
        Object::Port port4(1,Object::PortDirection::OUT);
        Object::Port port5(1,Object::PortDirection::IN );
        Object::Port port6(1,Object::PortDirection::IN );
        FIFO::PortName FIFOIn   (FIFO::PortName::_PortName::In   );
        FIFO::PortName FIFOOut  (FIFO::PortName::_PortName::Out  );
        FIFO::PortName FIFOPush (FIFO::PortName::_PortName::Push );
        FIFO::PortName FIFOPop  (FIFO::PortName::_PortName::Pop  );
        FIFO::PortName FIFOFull (FIFO::PortName::_PortName::Full );
        FIFO::PortName FIFOEmpty(FIFO::PortName::_PortName::Empty);
        Object::PortDataType in,push,pop;
        port1.Clone(in  );
        port3.Clone(push);
        port4.Clone(pop );
        fifo.ConnectTo(FIFOIn  ,port1    );
        fifo.ConnectTo(port2   ,FIFOOut  );
        fifo.ConnectTo(FIFOPush,port3    );
        fifo.ConnectTo(FIFOPop ,port4    );
        fifo.ConnectTo(port5   ,FIFOFull );
        fifo.ConnectTo(port6   ,FIFOEmpty);

        assert(port2.Data()==0
            && port5.Data()==0
            && port6.Data()==1);

        in = 233;
        push=1;
        pop =0;
        fifo.atPosEdge();
        assert(port2.Data()==0
            && port5.Data()==0
            && port6.Data()==1);
        fifo.Run();
        assert(port2.Data()==233
            && port5.Data()==0
            && port6.Data()==0);

        in = 522;
        push=0;
        pop =0;
        fifo.atPosEdge();
        assert(port2.Data()==233
            && port5.Data()==0
            && port6.Data()==0);
        fifo.Run();
        assert(port2.Data()==233
            && port5.Data()==0
            && port6.Data()==0);

        in = 499;
        push=0;
        pop =1;
        fifo.atPosEdge();
        assert(port2.Data()==233
            && port5.Data()==0
            && port6.Data()==0);
        fifo.Run();
        assert(port2.Data()==233
            && port5.Data()==0
            && port6.Data()==1);

        in = 618;
        push=1;
        pop =0;
        fifo.atPosEdge();
        assert(port2.Data()==233
            && port5.Data()==0
            && port6.Data()==1);
        fifo.Run();
        assert(port2.Data()==106
            && port5.Data()==0
            && port6.Data()==0);

        in = 499;
        push=0;
        pop =1;
        fifo.atPosEdge();
        assert(port2.Data()==106
            && port5.Data()==0
            && port6.Data()==0);
        fifo.Run();
        assert(port2.Data()==106
            && port5.Data()==0
            && port6.Data()==1);

        push=1;
        pop =0;
        for (int i=0;i<9;i++){
            in = i;
            fifo.atPosEdge();
            fifo.Run();
            assert(port2.Data()==0
                && port5.Data()==0
                && port6.Data()==0);
        }

        push=1;
        pop =1;
        for (int i=9;i<100;i++){
            in = i;
            fifo.atPosEdge();
            assert(port2.Data()==i-9
                && port5.Data()==0
                && port6.Data()==0);
            fifo.Run();
            assert(port2.Data()==i-8
                && port5.Data()==0
                && port6.Data()==0);
        }

        in = 100;
        push=1;
        pop =0;
        fifo.atPosEdge();
        fifo.Run();
        assert(port2.Data()==91
            && port5.Data()==1
            && port6.Data()==0);

        push=0;
        pop =1;
        fifo.atPosEdge();
        fifo.Run();
        assert(port2.Data()==92
            && port5.Data()==0
            && port6.Data()==0);
        return;
    }
};

//class PETest{
//public:
//    PETest(){
//             NormalTest();
//        std::cout<<"[Single PE test]       Normal Test Passed"<<std::endl;
////         InputStallTest();
////        std::cout<<"[Single PE test]  Input Stall Test Passed"<<std::endl;
////        OutputStallTest();
////        std::cout<<"[Single PE test] Output Stall Test Passed"<<std::endl;
//    }
//    void NormalTest(){
//        PE pe;
//        /** the relevant port of weight and output-mark **/
//            ///------------- upper side -------------///
//        PE::PortName PEFupSet   (PE::PortName::_PortName::FupSet   );/** FupSet     1-bit in     up-cell **/
//        PE::PortName PEFupOut   (PE::PortName::_PortName::FupOut   );/** FupOut     1-bit out    up-cell **/
//        PE::PortName PEOin      (PE::PortName::_PortName::Oin      );/** Oin        1-bit in     up-cell **/
//        PE::PortName PEWin      (PE::PortName::_PortName::Win      );/** Win        8-bit in     up-cell **/
//        PE::PortName PEOWinEn   (PE::PortName::_PortName::OWinEn   );/** OWinEn     1-bit in     up-cell **/
//            ///------------- under side -------------///
//        PE::PortName PEFdownIn  (PE::PortName::_PortName::FdownIn  );/** FdownIn    1-bit in   down-cell **/
//        PE::PortName PEFdownSet (PE::PortName::_PortName::FdownSet );/** FdownSet   1-bit out  down-cell **/
//        PE::PortName PEOout     (PE::PortName::_PortName::Oout     );/** Oout       1-bit out  down-cell **/
//        PE::PortName PEWout     (PE::PortName::_PortName::Wout     );/** Wout       8-bit out  down-cell **/
//        PE::PortName PEOWoutEn  (PE::PortName::_PortName::OWoutEn  );/** OWoutEn    1-bit out  down-cell **/
//        /** the relevant port of feature **/
//            ///------------- left  side -------------///
//        PE::PortName PEFleftSet (PE::PortName::_PortName::FleftSet );/** FleftSet   1-bit in   left-cell **/
//        PE::PortName PEFleftOut (PE::PortName::_PortName::FleftOut );/** FleftOut   1-bit out  left-cell **/
//        PE::PortName PEXin      (PE::PortName::_PortName::Xin      );/** Xin        8-bit in   left-cell **/
//        PE::PortName PEXinEn    (PE::PortName::_PortName::XinEn    );/** XinEn      8-bit in   left-cell **/
//            ///------------- right side -------------///
//        PE::PortName PEFrightIn (PE::PortName::_PortName::FrightIn );/** FrightIn   1-bit in  right-cell **/
//        PE::PortName PEFrightSet(PE::PortName::_PortName::FrightSet);/** FrightSet  1-bit out right-cell **/
//        PE::PortName PEXout     (PE::PortName::_PortName::Xout     );/** Xout       8-bit out right-cell **/
//        PE::PortName PEXoutEn   (PE::PortName::_PortName::XoutEn   );/** XoutEn     1-bit out right-cell **/
//        /** the relevant port of partial sum **/
//            ///------------   up of up   ------------///
//        PE::PortName PEFupupSet (PE::PortName::_PortName::FupupSet );/** FupupSet   1-bit in   upup-cell **/
//        PE::PortName PEFupupOut (PE::PortName::_PortName::FupupOut );/** FupupOut   1-bit out  upup-cell **/
//        PE::PortName PESin      (PE::PortName::_PortName::Sin      );/** Sin       24-bit in   upup-cell **/
//        PE::PortName PESen      (PE::PortName::_PortName::Sen      );/** Sen        1-bit in   upup-cell **/
//        PE::PortName PEEOFSet   (PE::PortName::_PortName::EOFSet   );/** EOFSet     1-bit in   upup-cell **/
//            ///------------ down of down ------------///
//        PE::PortName PEFddownIn (PE::PortName::_PortName::FddownIn );/** FddownIn   1-bit in  ddown-cell **/
//        PE::PortName PEFddownSet(PE::PortName::_PortName::FddownSet);/** FddownSet  1-bit out ddown-cell **/
//        PE::PortName PESddownOut(PE::PortName::_PortName::SddownOut);/** SddownOut 24-bit out ddown-cell **/
//        PE::PortName PESddownEn (PE::PortName::_PortName::SddownEn );/** SddownEn   1-bit out ddown-cell **/
//        PE::PortName PESddEOFSet(PE::PortName::_PortName::SddEOFSet);/** SddEOFSet  1-bit out ddown-cell **/
//
//
//        /** the relevant port of weight and output-mark **/
//            ///------------- upper side -------------///
//        Object::Port port0 ( 1,Object::PortDirection::OUT);/** FupSet     1-bit in     up-cell **/
//        Object::Port port1 ( 1,Object::PortDirection::IN );/** FupOut     1-bit out    up-cell **/
//        Object::Port port2 ( 1,Object::PortDirection::OUT);/** Oin        1-bit in     up-cell **/
//        Object::Port port3 ( 8,Object::PortDirection::OUT);/** Win        8-bit in     up-cell **/
//        Object::Port port4 ( 1,Object::PortDirection::OUT);/** OWinEn     1-bit in     up-cell **/
//            ///------------- under side -------------///
//        Object::Port port5 ( 1,Object::PortDirection::OUT);/** FdownIn    1-bit in   down-cell **/
//        Object::Port port6 ( 1,Object::PortDirection::IN );/** FdownSet   1-bit out  down-cell **/
//        Object::Port port7 ( 1,Object::PortDirection::IN );/** Oout       1-bit out  down-cell **/
//        Object::Port port8 ( 8,Object::PortDirection::IN );/** Wout       8-bit out  down-cell **/
//        Object::Port port9 ( 1,Object::PortDirection::IN );/** OWoutEn    1-bit out  down-cell **/
//        /** the relevant port of feature **/
//            ///------------- left  side -------------///
//        Object::Port port10( 1,Object::PortDirection::OUT);/** FleftSet   1-bit in   left-cell **/
//        Object::Port port11( 1,Object::PortDirection::IN );/** FleftOut   1-bit out  left-cell **/
//        Object::Port port12( 8,Object::PortDirection::OUT);/** Xin        8-bit in   left-cell **/
//        Object::Port port13( 1,Object::PortDirection::OUT);/** XinEn      8-bit in   left-cell **/
//            ///------------- right side -------------///
//        Object::Port port14( 1,Object::PortDirection::OUT);/** FrightIn   1-bit in  right-cell **/
//        Object::Port port15( 1,Object::PortDirection::IN );/** FrightSet  1-bit out right-cell **/
//        Object::Port port16( 8,Object::PortDirection::IN );/** Xout       8-bit out right-cell **/
//        Object::Port port17( 1,Object::PortDirection::IN );/** XoutEn     1-bit out right-cell **/
//        /** the relevant port of partial sum **/
//            ///------------   up of up   ------------///
//        Object::Port port18( 1,Object::PortDirection::OUT);/** FupupSet   1-bit in   upup-cell **/
//        Object::Port port19( 1,Object::PortDirection::IN );/** FupupOut   1-bit out  upup-cell **/
//        Object::Port port20(24,Object::PortDirection::OUT);/** Sin       24-bit in   upup-cell **/
//        Object::Port port25( 1,Object::PortDirection::OUT);/** Sen        1-bit in   upup-cell **/
//        Object::Port port26( 1,Object::PortDirection::OUT);/** EOFSet     1-bit in   upup-cell **/
//            ///------------ down of down ------------///
//        Object::Port port21( 1,Object::PortDirection::OUT);/** FddownIn   1-bit in  ddown-cell **/
//        Object::Port port22( 1,Object::PortDirection::IN );/** FddownSet  1-bit out ddown-cell **/
//        Object::Port port23(24,Object::PortDirection::IN );/** SddownOut 24-bit out ddown-cell **/
//        Object::Port port24( 1,Object::PortDirection::IN );/** SddownEn   1-bit out ddown-cell **/
//        Object::Port port27( 1,Object::PortDirection::IN );/** SddEOFSet  1-bit out ddown-cell **/
//
//
//        /** the relevant port of weight and output-mark **/
//            ///------------- upper side -------------///
//        Object::PortDataType fUpSet;    /** FupSet     1-bit in     up-cell **/
//        Object::PortDataType oIn;       /** Oin        1-bit in     up-cell **/
//        Object::PortDataType wIn;       /** Win        8-bit in     up-cell **/
//        Object::PortDataType owInEn;    /** OWinEn     1-bit in     up-cell **/
//            ///------------- under side -------------///
//        Object::PortDataType fDownIn ;  /** FdownIn    1-bit in   down-cell **/
//        /** the relevant port of feature **/
//            ///------------- left  side -------------///
//        Object::PortDataType fLeftSet;  /** FleftSet   1-bit in   left-cell **/
//        Object::PortDataType xIn;       /** Xin        8-bit in   left-cell **/
//        Object::PortDataType xInEn;     /** XinEn      8-bit in   left-cell **/
//            ///------------- right side -------------///
//        Object::PortDataType fRightIn;  /** FrightIn   1-bit in  right-cell **/
//        /** the relevant port of partial sum **/
//            ///------------   up of up   ------------///
//        Object::PortDataType fUpUpSet;  /** FupupSet   1-bit in   upup-cell **/
//        Object::PortDataType sIn;       /** Sin       24-bit in   upup-cell **/
//        Object::PortDataType sEn;       /** Sen        1-bit in   upup-cell **/
//        Object::PortDataType eofSet;    /** EOFSet     1-bit in   upup-cell **/
//            ///------------ down of down ------------///
//        Object::PortDataType fDDownIn;  /** FddownIn   1-bit in  ddown-cell **/
//
//
//        port0 .Clone(fUpSet  ); /** FupSet     1-bit in     up-cell **/
//        port2 .Clone(oIn     ); /** Oin        1-bit in     up-cell **/
//        port3 .Clone(wIn     ); /** Win        8-bit in     up-cell **/
//        port4 .Clone(owInEn  ); /** OWinEn     1-bit in     up-cell **/
//            ///------------- under side -------------///
//        port5 .Clone(fDownIn ); /** FdownIn    1-bit in   down-cell **/
//        /** the relevant port of feature **/
//            ///------------- left  side -------------///
//        port10.Clone(fLeftSet); /** FleftSet   1-bit in   left-cell **/
//        port12.Clone(xIn     ); /** Xin        8-bit in   left-cell **/
//        port13.Clone(xInEn   ); /** XinEn      8-bit in   left-cell **/
//            ///------------- right side -------------///
//        port14.Clone(fRightIn); /** FrightIn   1-bit in  right-cell **/
//        /** the relevant port of partial sum **/
//            ///------------   up of up   ------------///
//        port18.Clone(fUpUpSet); /** FupupSet   1-bit in   upup-cell **/
//        port20.Clone(sIn     ); /** Sin       24-bit in   upup-cell **/
//        port25.Clone(sEn     ); /** Sen        1-bit in   upup-cell **/
//        port26.Clone(eofSet  ); /** EOFSet     1-bit in   upup-cell **/
//            ///------------ down of down ------------///
//        port21.Clone(fDDownIn); /** FddownIn   1-bit in  ddown-cell **/
//
//
//        /** the relevant port of weight and output-mark **/
//            ///------------- upper side -------------///
//        pe.ConnectTo(PEFupSet   ,port0      );/** FupSet     1-bit in     up-cell **/
//        pe.ConnectTo(port1      ,PEFupOut   );/** FupOut     1-bit out    up-cell **/
//        pe.ConnectTo(PEOin      ,port2      );/** Oin        1-bit in     up-cell **/
//        pe.ConnectTo(PEWin      ,port3      );/** Win        8-bit in     up-cell **/
//        pe.ConnectTo(PEOWinEn   ,port4      );/** OWinEn     1-bit in     up-cell **/
//            ///------------- under side -------------///
//        pe.ConnectTo(PEFdownIn  ,port5      );/** FdownIn    1-bit in   down-cell **/
//        pe.ConnectTo(port6      ,PEFdownSet );/** FdownSet   1-bit out  down-cell **/
//        pe.ConnectTo(port7      ,PEOout     );/** Oout       1-bit out  down-cell **/
//        pe.ConnectTo(port8      ,PEWout     );/** Wout       8-bit out  down-cell **/
//        pe.ConnectTo(port9      ,PEOWoutEn  );/** OWoutEn    1-bit out  down-cell **/
//        /** the relevant port of feature **/
//            ///------------- left  side -------------///
//        pe.ConnectTo(PEFleftSet ,port10     );/** FleftSet   1-bit in   left-cell **/
//        pe.ConnectTo(port11     ,PEFleftOut );/** FleftOut   1-bit out  left-cell **/
//        pe.ConnectTo(PEXin      ,port12     );/** Xin        8-bit in   left-cell **/
//        pe.ConnectTo(PEXinEn    ,port13     );/** XinEn      8-bit in   left-cell **/
//            ///------------- right side -------------///
//        pe.ConnectTo(PEFrightIn ,port14     );/** FrightIn   1-bit in  right-cell **/
//        pe.ConnectTo(port15     ,PEFrightSet);/** FrightSet  1-bit out right-cell **/
//        pe.ConnectTo(port16     ,PEXout     );/** Xout       8-bit out right-cell **/
//        pe.ConnectTo(port17     ,PEXoutEn   );/** XoutEn     1-bit out right-cell **/
//        /** the relevant port of partial sum **/
//            ///------------   up of up   ------------///
//        pe.ConnectTo(PEFupupSet ,port18     );/** FupupSet   1-bit in   upup-cell **/
//        pe.ConnectTo(port19     ,PEFupupOut );/** FupupOut   1-bit out  upup-cell **/
//        pe.ConnectTo(PESin      ,port20     );/** Sin       24-bit in   upup-cell **/
//        pe.ConnectTo(PESen      ,port25     );/** Sen        1-bit in   upup-cell **/
//        pe.ConnectTo(PEEOFSet   ,port26     );/** EOFSet     1-bit in   upup-cell **/
//            ///------------ down of down ------------///
//        pe.ConnectTo(PEFddownIn ,port21     );/** FddownIn   1-bit in  ddown-cell **/
//        pe.ConnectTo(port22     ,PEFddownSet);/** FddownSet  1-bit out ddown-cell **/
//        pe.ConnectTo(port23     ,PESddownOut);/** SddownOut 24-bit out ddown-cell **/
//        pe.ConnectTo(port24     ,PESddownEn );/** SddownEn   1-bit out ddown-cell **/
//        pe.ConnectTo(port27     ,PESddEOFSet);/** SddEOFSet  1-bit out ddown-cell **/
//
//
//        assert(pe.InputConnected());
////        assert(pe.OutputConnect());
//        assert(pe.OuterConnected());
//        pe.InnerConnect();
//        assert(pe.InnerConnected());
//        assert(pe.InnerConnectedCorrect());
//        assert(pe.Connected());
//
//        Object::PortDataType
//            ///                0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24
//            wInList      []={ 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 10, 10},
//            oInList      []={  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0},
//            fUpSetList   []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            fLeftSetList []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            xInEnList    []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            owInEnList   []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            xInList      []={  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13},
//            fDownInList  []={  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0},
//            fRightInList []={  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0},
//            fUpUpSetList []={  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0},
//            sInList      []={  0,  0,  0,  0,  0,  0,900,  0,122,  0,233,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,499,  0,  0},
//            sEnList      []={  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0},
//            eofList      []={  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0},
//            fDDownInList []={  0,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
//            /** ============================== **/
//            fUpOutList   []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            fDownSetList []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            oOutList     []={  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0},
//            wOutList     []={ 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 10},
//            owOutEnList  []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            fLeftOutList []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            fRightSetList[]={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            xOutList     []={  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13},
//            xOutEnList   []={  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
//            fUpUpOutList []={  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0},
//            fDDownSetList[]={  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  1},
//            sDDownOutList[]={  0, 10, 10, 32, 32, 68,900,120,122,190,233,280,280, 70, 70,158,158,266,266,396,396,550,499,730,730},
//            sDDownEnList []={  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  1},
//            sDDownEOFList[]={  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1};
//
//        assert( port1 .Data() ==  0  /** FupOut     1-bit out    up-cell **/
//            &&  port6 .Data() ==  0  /** FdownSet   1-bit out  down-cell **/
//            &&  port7 .Data() ==  0  /** Oout       1-bit out  down-cell **/
//            &&  port8 .Data() ==  0  /** Wout       8-bit out  down-cell **/
//            &&  port9 .Data() ==  0  /** OWoutEn    1-bit out  down-cell **/
//            &&  port11.Data() ==  0  /** FleftOut   1-bit out  left-cell **/
//            &&  port15.Data() ==  0  /** FrightSet  1-bit out right-cell **/
//            &&  port16.Data() ==  0  /** Xout       8-bit out right-cell **/
//            &&  port17.Data() ==  0  /** XoutEn     1-bit out right-cell **/
//            &&  port19.Data() ==  0  /** FupupOut   1-bit out  upup-cell **/
//            &&  port22.Data() ==  0  /** FddownSet  1-bit out ddown-cell **/
//            &&  port23.Data() ==  0  /** SddownOut 24-bit out ddown-cell **/
//            &&  port24.Data() ==  0  /** SddownEn   1-bit out ddown-cell **/
//            &&  port27.Data() ==  0  /** SddEOFSet  1-bit out ddown-cell **/
//        );
//
//        for (int i=0;i<25;i++){
//            fUpSet   = fUpSetList  [i];
//            oIn      = oInList     [i];
//            wIn      = wInList     [i];
//            owInEn   = owInEnList  [i];
//            fDownIn  = fDownInList [i];
//            fLeftSet = fLeftSetList[i];
//            xIn      = xInList     [i];
//            xInEn    = xInEnList   [i];
//            fRightIn = fRightInList[i];
//            fUpUpSet = fUpUpSetList[i];
//            sIn      = sInList     [i];
//            sEn      = sEnList     [i];
//            fDDownIn = fDDownInList[i];
//            eofSet   = eofList     [i];
//
//            pe.atPosEdge();
//            pe.Run();
//            std::cout<<"================="<<std::endl;
//            std::cout<<" after cycle  "<<i<<std::endl;
//            std::cout<<"================="<<std::endl;
//            pe.Print();
//
//            assert(port1 .Data() ==  fUpOutList   [i] /** FupOut     1-bit out    up-cell **/
//                && port6 .Data() ==  fDownSetList [i] /** FdownSet   1-bit out  down-cell **/
//                && port7 .Data() ==  oOutList     [i] /** Oout       1-bit out  down-cell **/
//                && port8 .Data() ==  wOutList     [i] /** Wout       8-bit out  down-cell **/
//                && port9 .Data() ==  owOutEnList  [i] /** OWoutEn    1-bit out  down-cell **/
//                && port11.Data() ==  fLeftOutList [i] /** FleftOut   1-bit out  left-cell **/
//                && port15.Data() ==  fRightSetList[i] /** FrightSet  1-bit out right-cell **/
//                && port16.Data() ==  xOutList     [i] /** Xout       8-bit out right-cell **/
//                && port17.Data() ==  xOutEnList   [i] /** XoutEn     1-bit out right-cell **/
//                && port19.Data() ==  fUpUpOutList [i] /** FupupOut   1-bit out  upup-cell **/
//                && port22.Data() ==  fDDownSetList[i] /** FddownSet  1-bit out ddown-cell **/
//                && port23.Data() ==  sDDownOutList[i] /** SddownOut 24-bit out ddown-cell **/
//                && port24.Data() ==  sDDownEnList [i] /** SddownEn   1-bit out ddown-cell **/
//                && port27.Data() ==  sDDownEOFList[i] /** SddEOFSet  1-bit out ddown-cell **/
//            );
//
//        }
//
////        Object::PortDataType wInList[]={10,11,12,13,14,15,10,11,12,13,14,15};
////        Object::PortDataType oInList[]={ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1};
////        Object::PortDataType xInList[]={ 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12};
////        /// at posedge 0
////        fUpSet   = 1;
////        oIn      = oInList[0];
////        wIn      = wInList[0];
////        owInEn   = 1;
////        fDownIn  = 0;
////        fLeftSet = 1;
////        xIn      = xInList[0];
////        xInEn    = 1;
////        fRightIn = 0;
////        fUpUpSet = 1;
////        sIn      = 233;
////        sEn      = 1;
////        fDDownIn = 0;
////
////        pe.atPosEdge();
////        pe.Run();
////        std::cout<<"================="<<std::endl;
////        std::cout<<" after cycle  0  "<<std::endl;
////        std::cout<<"================="<<std::endl;
////        pe.Print();
////
////        assert(port1 .Data() ==  1         /** FupOut     1-bit out    up-cell **/
////            && port6 .Data() ==  1         /** FdownSet   1-bit out  down-cell **/
////            && port7 .Data() ==  oInList[0]/** Oout       1-bit out  down-cell **/
////            && port8 .Data() ==  wInList[0]/** Wout       8-bit out  down-cell **/
////            && port9 .Data() ==  1         /** OWoutEn    1-bit out  down-cell **/
////            && port11.Data() ==  1         /** FleftOut   1-bit out  left-cell **/
////            && port15.Data() ==  1         /** FrightSet  1-bit out right-cell **/
////            && port16.Data() ==  xInList[0]/** Xout       8-bit out right-cell **/
////            && port17.Data() ==  1         /** XoutEn     1-bit out right-cell **/
////            && port19.Data() ==  1         /** FupupOut   1-bit out  upup-cell **/
////            && port22.Data() ==  1         /** FddownSet  1-bit out ddown-cell **/
////            && port23.Data() ==  233       /** SddownOut 24-bit out ddown-cell **/
////            && port24.Data() ==  1         /** SddownEn   1-bit out ddown-cell **/
////        );
////
////        /// at posedge 1
////        fUpSet   = 0;
////        oIn      = oInList[1];
////        wIn      = wInList[1];
////        owInEn   = 0;
////        fDownIn  = 0;
////        fLeftSet = 0;
////        xIn      = xInList[1];
////        xInEn    = 0;
////        fRightIn = 0;
////        fUpUpSet = 0;
////        sIn      = 0;
////        fDDownIn = 1;
////
////        pe.atPosEdge();
////        pe.Run();
////        std::cout<<"================="<<std::endl;
////        std::cout<<" after cycle  1  "<<std::endl;
////        std::cout<<"================="<<std::endl;
////        pe.Print();
////
////        assert(port1 .Data() ==  0         /** FupOut     1-bit out    up-cell **/
////            && port6 .Data() ==  0         /** FdownSet   1-bit out  down-cell **/
////            && port7 .Data() ==  oInList[0]/** Oout       1-bit out  down-cell **/
////            && port8 .Data() ==  wInList[0]/** Wout       8-bit out  down-cell **/
////            && port9 .Data() ==  0         /** OWoutEn    1-bit out  down-cell **/
////            && port11.Data() ==  0         /** FleftOut   1-bit out  left-cell **/
////            && port15.Data() ==  0         /** FrightSet  1-bit out right-cell **/
////            && port16.Data() ==  xInList[0]/** Xout       8-bit out right-cell **/
////            && port17.Data() ==  0         /** XoutEn     1-bit out right-cell **/
////            && port19.Data() ==  0         /** FupupOut   1-bit out  upup-cell **/
////            && port22.Data() ==  0         /** FddownSet  1-bit out ddown-cell **/
////            && port23.Data() ==  10        /** SddownOut 24-bit out ddown-cell **/
////            && port24.Data() ==  0         /** SddownEn   1-bit out ddown-cell **/
////        );
////
////        int partialSum = 10;
////
////        for (int i=1;i<5;i++){
////            fUpSet   = 1;
////            oIn      = oInList[i];
////            wIn      = wInList[i];
////            owInEn   = 1;
////            fDownIn  = 0;
////            fLeftSet = 1;
////            xIn      = xInList[i];
////            xInEn    = 1;
////            fRightIn = 0;
////            fUpUpSet = 0;
////            sIn      = 0;
////            fDDownIn = 0;
////
////            pe.atPosEdge();
////            pe.Run();
////            std::cout<<"==================="<<std::endl;
////            std::cout<<" after cycle  "<<2*i<<std::endl;
////            std::cout<<"==================="<<std::endl;
////            pe.Print();
////
////            assert( port1 .Data() ==  1         /** FupOut     1-bit out    up-cell **/
////                 && port6 .Data() ==  1         /** FdownSet   1-bit out  down-cell **/
////                 && port7 .Data() ==  oInList[i]/** Oout       1-bit out  down-cell **/
////                 && port8 .Data() ==  wInList[i]/** Wout       8-bit out  down-cell **/
////                 && port9 .Data() ==  1         /** OWoutEn    1-bit out  down-cell **/
////                 && port11.Data() ==  1         /** FleftOut   1-bit out  left-cell **/
////                 && port15.Data() ==  1         /** FrightSet  1-bit out right-cell **/
////                 && port16.Data() ==  xInList[i]/** Xout       8-bit out right-cell **/
////                 && port17.Data() ==  1         /** XoutEn     1-bit out right-cell **/
////                 && port19.Data() ==  0         /** FupupOut   1-bit out  upup-cell **/
////                 && port22.Data() ==  0         /** FddownSet  1-bit out ddown-cell **/
////                 && port23.Data() ==  partialSum/** SddownOut 24-bit out ddown-cell **/
////                 && port24.Data() ==  0         /** SddownEn   1-bit out ddown-cell **/
////            );
////
////            partialSum += wInList[i]
////                        * xInList[i];
////
////            fUpSet   = 0;
////            oIn      = oInList[i+1];
////            wIn      = wInList[i+1];
////            owInEn   = 0;
////            fDownIn  = 0;
////            fLeftSet = 0;
////            xIn      = xInList[i+1];
////            xInEn    = 0;
////            fRightIn = 0;
////            fUpUpSet = 0;
////            sIn      = 0;
////            fDDownIn = 0;
////
////            pe.atPosEdge();
////            pe.Run();
////            std::cout<<"====================="<<std::endl;
////            std::cout<<" after cycle  "<<2*i+1<<std::endl;
////            std::cout<<"====================="<<std::endl;
////            pe.Print();
////
////            assert( port1 .Data() ==  0         /** FupOut     1-bit out    up-cell **/
////                 && port6 .Data() ==  0         /** FdownSet   1-bit out  down-cell **/
////                 && port7 .Data() ==  oInList[i]/** Oout       1-bit out  down-cell **/
////                 && port8 .Data() ==  wInList[i]/** Wout       8-bit out  down-cell **/
////                 && port9 .Data() ==  0         /** OWoutEn    1-bit out  down-cell **/
////                 && port11.Data() ==  0         /** FleftOut   1-bit out  left-cell **/
////                 && port15.Data() ==  0         /** FrightSet  1-bit out right-cell **/
////                 && port16.Data() ==  xInList[i]/** Xout       8-bit out right-cell **/
////                 && port17.Data() ==  0         /** XoutEn     1-bit out right-cell **/
////                 && port19.Data() ==  0         /** FupupOut   1-bit out  upup-cell **/
////                 && port22.Data() ==  0         /** FddownSet  1-bit out ddown-cell **/
////                 && port23.Data() ==  partialSum/** SddownOut 24-bit out ddown-cell **/
////                 && port24.Data() ==  0         /** SddownEn   1-bit out ddown-cell **/
////            );
////        }
////
////        fUpSet   = 1;
////        oIn      = oInList[5];
////        wIn      = wInList[5];
////        owInEn   = 1;
////        fDownIn  = 0;
////        fLeftSet = 1;
////        xIn      = xInList[5];
////        xInEn    = 1;
////        fRightIn = 0;
////        fUpUpSet = 0;
////        sIn      = 0;
////        fDDownIn = 0;
////
////        pe.atPosEdge();
////        pe.Run();
////        std::cout<<"================"<<std::endl;
////        std::cout<<" after cycle  10"<<std::endl;
////        std::cout<<"================"<<std::endl;
////        pe.Print();
////
////        assert(port1 .Data() ==  1         /** FupOut     1-bit out    up-cell **/
////            && port6 .Data() ==  1         /** FdownSet   1-bit out  down-cell **/
////            && port7 .Data() ==  oInList[5]/** Oout       1-bit out  down-cell **/
////            && port8 .Data() ==  wInList[5]/** Wout       8-bit out  down-cell **/
////            && port9 .Data() ==  1         /** OWoutEn    1-bit out  down-cell **/
////            && port11.Data() ==  1         /** FleftOut   1-bit out  left-cell **/
////            && port15.Data() ==  1         /** FrightSet  1-bit out right-cell **/
////            && port16.Data() ==  xInList[5]/** Xout       8-bit out right-cell **/
////            && port17.Data() ==  1         /** XoutEn     1-bit out right-cell **/
////            && port19.Data() ==  0         /** FupupOut   1-bit out  upup-cell **/
////            && port22.Data() ==  0         /** FddownSet  1-bit out ddown-cell **/
////            && port23.Data() ==  partialSum/** SddownOut 24-bit out ddown-cell **/
////            && port24.Data() ==  0         /** SddownEn   1-bit out ddown-cell **/
////        );
////
////
////        partialSum += wInList[5]
////                    * xInList[5];
////
////        fUpSet   = 0;
////        oIn      = oInList[6];
////        wIn      = wInList[6];
////        owInEn   = 0;
////        fDownIn  = 0;
////        fLeftSet = 0;
////        xIn      = xInList[6];
////        xInEn    = 0;
////        fRightIn = 0;
////        fUpUpSet = 0;
////        sIn      = 0;
////        fDDownIn = 0;
////
////        pe.atPosEdge();
////        pe.Run();
////        std::cout<<"================"<<std::endl;
////        std::cout<<" after cycle  11"<<std::endl;
////        std::cout<<"================"<<std::endl;
////        pe.Print();
////
////        assert(port1 .Data() ==  0         /** FupOut     1-bit out    up-cell **/
////            && port6 .Data() ==  0         /** FdownSet   1-bit out  down-cell **/
////            && port7 .Data() ==  oInList[5]/** Oout       1-bit out  down-cell **/
////            && port8 .Data() ==  wInList[5]/** Wout       8-bit out  down-cell **/
////            && port9 .Data() ==  0         /** OWoutEn    1-bit out  down-cell **/
////            && port11.Data() ==  0         /** FleftOut   1-bit out  left-cell **/
////            && port15.Data() ==  0         /** FrightSet  1-bit out right-cell **/
////            && port16.Data() ==  xInList[5]/** Xout       8-bit out right-cell **/
////            && port17.Data() ==  0         /** XoutEn     1-bit out right-cell **/
////            && port19.Data() ==  0         /** FupupOut   1-bit out  upup-cell **/
////            && port22.Data() ==  0         /** FddownSet  1-bit out ddown-cell **/
////            && port23.Data() ==  partialSum/** SddownOut 24-bit out ddown-cell **/
////            && port24.Data() ==  0         /** SddownEn   1-bit out ddown-cell **/
////        );
////
////        fUpSet   = 1;
////        oIn      = oInList[6];
////        wIn      = wInList[6];
////        owInEn   = 1;
////        fDownIn  = 0;
////        fLeftSet = 1;
////        xIn      = xInList[6];
////        xInEn    = 1;
////        fRightIn = 0;
////        fUpUpSet = 0;
////        sIn      = 0;
////        fDDownIn = 0;
////
////        pe.atPosEdge();
////        pe.Run();
////        std::cout<<"================"<<std::endl;
////        std::cout<<" after cycle  12"<<std::endl;
////        std::cout<<"================"<<std::endl;
////        pe.Print();
////
////        assert(port1 .Data() ==  1         /** FupOut     1-bit out    up-cell **/
////            && port6 .Data() ==  1         /** FdownSet   1-bit out  down-cell **/
////            && port7 .Data() ==  oInList[6]/** Oout       1-bit out  down-cell **/
////            && port8 .Data() ==  wInList[6]/** Wout       8-bit out  down-cell **/
////            && port9 .Data() ==  1         /** OWoutEn    1-bit out  down-cell **/
////            && port11.Data() ==  1         /** FleftOut   1-bit out  left-cell **/
////            && port15.Data() ==  1         /** FrightSet  1-bit out right-cell **/
////            && port16.Data() ==  xInList[6]/** Xout       8-bit out right-cell **/
////            && port17.Data() ==  1         /** XoutEn     1-bit out right-cell **/
////            && port19.Data() ==  0         /** FupupOut   1-bit out  upup-cell **/
////            && port22.Data() ==  1         /** FddownSet  1-bit out ddown-cell **/
////            && port23.Data() ==  partialSum/** SddownOut 24-bit out ddown-cell **/
////            && port24.Data() ==  1         /** SddownEn   1-bit out ddown-cell **/
////        );
////
////        partialSum = wInList[6]
////                   * xInList[6];
////
////        fUpSet   = 0;
////        oIn      = oInList[7];
////        wIn      = wInList[7];
////        owInEn   = 0;
////        fDownIn  = 0;
////        fLeftSet = 0;
////        xIn      = xInList[7];
////        xInEn    = 0;
////        fRightIn = 0;
////        fUpUpSet = 0;
////        sIn      = 0;
////        fDDownIn = 0;
////
////        pe.atPosEdge();
////        pe.Run();
////        std::cout<<"================"<<std::endl;
////        std::cout<<" after cycle  13"<<std::endl;
////        std::cout<<"================"<<std::endl;
////        pe.Print();
////
////        assert(port1 .Data() ==  0         /** FupOut     1-bit out    up-cell **/
////            && port6 .Data() ==  0         /** FdownSet   1-bit out  down-cell **/
////            && port7 .Data() ==  oInList[6]/** Oout       1-bit out  down-cell **/
////            && port8 .Data() ==  wInList[6]/** Wout       8-bit out  down-cell **/
////            && port9 .Data() ==  0         /** OWoutEn    1-bit out  down-cell **/
////            && port11.Data() ==  0         /** FleftOut   1-bit out  left-cell **/
////            && port15.Data() ==  0         /** FrightSet  1-bit out right-cell **/
////            && port16.Data() ==  xInList[6]/** Xout       8-bit out right-cell **/
////            && port17.Data() ==  0         /** XoutEn     1-bit out right-cell **/
////            && port19.Data() ==  0         /** FupupOut   1-bit out  upup-cell **/
////            && port22.Data() ==  0         /** FddownSet  1-bit out ddown-cell **/
////            && port23.Data() ==  partialSum/** SddownOut 24-bit out ddown-cell **/
////            && port24.Data() ==  0         /** SddownEn   1-bit out ddown-cell **/
////        );
//        return;
//    }
//};
#endif //__TEST_TEST_BASE_COMBI_LOGIC_HPP
