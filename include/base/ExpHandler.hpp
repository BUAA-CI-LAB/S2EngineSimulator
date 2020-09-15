#ifdef __BASE_EXP_HANDLER_HPP
#define __BASE_EXP_HANDLER_HPP

#include <mutex>
#include <string>
#include <stdio.h>
#include <iostream>

#include "../../define.hpp"



class ExpHandler{
private:
    static FILE* fpInfo;   // fopen(INFO_FILE_PATH,"w+"),
    static FILE* fpError;  // fopen(INFO_ERROR_PATH,"w+"),
    static FILE* fpWarning;// fopen(INFO_WARNING_PATH,"w+"),

    static std::mutex m;

public:

    static inline void Info(std::string& info){
        #ifdef PRINT_INFO_TO_STD
        {
            std::lock_guard<std::mutex> lock(m);
            std::cout<<"[INFO]: " + info<<std::endl;
        }
        #endif // PRINT_INFO_TO_STD

        #ifdef PRINT_INFO_TO_FILE
        fprintf(ExpHandler::fpInfo,"[INFO]: %s\n",info.c_str());
        #endif // PRINT_INFO_TO_FILE
    }
    static inline void Error(std::string& error){
        #ifdef PRINT_ERROR_TO_STD
        {
            std::lock_guard<std::mutex> lock(m);
            std::cout<<"[ERROR]: " + error<<std::endl;
        }
        #endif // PRINT_INFO_TO_STD

        #ifdef PRINT_ERROR_TO_FILE
        fprintf(ExpHandler::fpError,"[ERROR]: %s\n",error.c_str());
        #endif // PRINT_ERROR_TO_FILE
    }
    static inline void Warning(std::string& warning){
        #ifdef PRINT_WARNING_TO_STD
        {
            std::lock_guard<std::mutex> lock(m);
            std::cout<<"[WARNING]: " + warning<<std::endl;
        }
        #endif // PRINT_INFO_TO_STD

        #ifdef PRINT_WARNING_TO_FILE
        fprintf(ExpHandler::fpError,"[WARNING]: %s\n",warning.c_str());
        #endif // PRINT_WARNING_TO_FILE
    }
};
#endif // __BASE_EXP_HANDLER_HPP
