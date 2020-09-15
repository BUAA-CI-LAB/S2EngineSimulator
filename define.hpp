#ifndef __DEFINE_HPP
#define __DEFINE_HPP

//#define NDEBUG
#include <assert.h>


/** runtime info **/
#define PE_ARRAY_STATE_FILE_PATH "./output/PEArrayState.txt"
#define REFORMED_PE_ARRAY_STATE_FILE_PATH "./output/RePEArrayState.txt"
#define PRINT_RUNTIME_INFO_TO_STD
#define PRINT_RUNTIME_INFO_TO_FILE


/** output file path **/

#define PRINT_INFO_TO_STD
#define PRINT_INFO_TO_FILE
#define PRINT_ERROR_TO_STD
#define PRINT_ERROR_TO_FILE
#define PRINT_WARNING_TO_STD
#define PRINT_WARNING_TO_FILE

#define INFO_FILE_PATH    "./output/info.txt"
#define INFO_ERROR_PATH   "./output/error.txt"
#define INFO_WARNING_PATH "./output/warning.txt"

/** some assumption of the hardware behaviours **/
#define REG_WRITE_THE_SAME_VALUE_NOT_CONSUME_POWER
    /** i.e. when writing the same value to register
     *  as it contained, it won't add to writing time
     **/

#define COMBI_LOGIC_HOLD_THE_SAME_VALUE_NOT_CONSUME_POWER

#define MAX_PARAM_SIZE 16/// temporary solution in case of the copy of vector

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class String{
private:

public:
    static inline std::string NumToString(int num){
        std::stringstream ss;
        ss<<num;
        return ss.str();
    }
    static inline std::string NumToString(float num){
        std::stringstream ss;
        ss<<num;
        return ss.str();
    }
    static inline std::string NumToString(uint64_t num){
        std::stringstream ss;
        ss<<num;
        return ss.str();
    }
    static inline std::string NumToString(uint32_t num){
        std::stringstream ss;
        ss<<num;
        return ss.str();
    }
    static inline bool isNum(std::string& str){
        std::stringstream sin(str);
        double d;
        char c;
        if(!(sin >> d))
            return false;
        if (sin >> c)
            return false;
        return true;
    }
};

#endif // __DEFINE_HPP
