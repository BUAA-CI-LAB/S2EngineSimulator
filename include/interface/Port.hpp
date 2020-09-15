#ifdef  __INTERFACE_PORT_HPP
#define __INTERFACE_PORT_HPP
/** abandoned **/
#include <atomic>

#include "../../define.hpp"

class DCPort{
private:
    static std::atomic<int> counter(0);

    enum class PORT_DIRECT direct;
    int thisAreaDomain,
        oppoAreaDomain;

    int thisPortID;

    unsigned char bitwidth;

    class DCPort* paired;

public:
    enum class PORT_DIRECT{
        IN,OUT
    };
    DCPort(unsigned char bitwidth,enum class PORT_DIRECT direct):this->paired(paired){
        this->direct = direct;
        this->bitwidth=bitwidth;
        this->thisAreaDomain = UNALLOCED;
        this->oppoAreaDomain = UNALLOCED;
        return;
    }
    void MatchPair(DCPort& ){
    }
    inline void AllocThisDomain(int thisAreaDomain){
        assert(this->thisAreaDomain == UNALLOCED);
        this->thisAreaDomain = thisAreaDomain;
        return;
    }
    inline void AllocOppoDomain(int oppoAreaDomain){
        assert(this->oppoAreaDomain == UNALLOCED);
        this->oppoAreaDomain = oppoAreaDomain;
        return;
    }
    void Send(int value){
        assert(this->direct==OUT);
        if (this->thisAreaDomain == this->oppoAreaDomain){
            assert(false);
        }
        else{
            assert(false);
        }
        return;
    }
};

#endif // __INTERFACE_PORT_HPP
