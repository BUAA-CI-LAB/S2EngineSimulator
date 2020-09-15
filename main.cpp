#include "./include/base/Object.hpp"
#include "./include/component/PE.hpp"
#include "./include/component/SynPE.hpp"
#include "./include/resource/BaseCombiLogic.hpp"
#include "./include/test/Test.hpp"
#include "./include/test/SystemTest.hpp"
#include "./include/system/PEArray.hpp"
#include "./include/component/RePE.hpp"

#include <iostream>
using namespace std;

void StringToInt(const std::string& str,uint32_t& number){
    std::stringstream sstr(str);
    sstr>>number;
    return;
}

int main(int argc, char * argv []){
    uint32_t height,width,ruPeriod,macPeriod;
    std::string str1;
    std::string str2;
    std::string str3;
    std::string str4;
    std::string str5;
    switch(argc){
    case 5:
        str1 = argv[1];
        str2 = argv[2];
        str3 = argv[3];
        str4 = argv[4];
        StringToInt(str1,height);
        StringToInt(str2,width);
        StringToInt(str3,ruPeriod);
        StringToInt(str4,macPeriod);
        ReformedPEArrayTest().TestLayer(height,width,ruPeriod,macPeriod);
        break;
    case 6:
        str1 = argv[1];
        str2 = argv[2];
        str3 = argv[3];
        str4 = argv[4];
        str5 = argv[5];
        StringToInt(str1,height);
        StringToInt(str2,width);
        StringToInt(str3,ruPeriod);
        StringToInt(str4,macPeriod);
        ReformedPEArrayTest().TestLayer(height,width,ruPeriod,macPeriod,str5);
        break;
    }
//    SynPETest();
//    SynPEArrayTest();
//    RUTest();
//    CETest();
//    RUArrayTest();
//    S2EngineTest();
//    ReformedPETest();
//    ReformedS2EngineTest().TestLayer("./conv_7/transed_32_32/");/// dead lock
//    ReformedS2EngineTest().TestAlex();
//    ReformedPEArrayTest().TestAlex();
//    ReformedPEArrayTest().TestVGG16();
//    ReMixedS2EngineTest().TestAlex();

//    SynPEArray_SystemTest().TestAlex();
//    SynPEArray_SystemTest().TestVGG16();
    cout<<"## test end ##"<<endl;
    return 0;
}
