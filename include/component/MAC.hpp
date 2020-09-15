#ifndef __COMPONENT_MAC_HPP
#define __COMPONENT_MAC_HPP

#include "../base/Object.hpp"
#include "../resource/BaseCombiLogic.hpp"

class MAC: public Object{
    /** d = a * b + c
     *
     *      a: input0
     *      b: input1
     *      c: input2
     *      d: output
     **/
private:
    Multer multer;
    Adder  adder;

public:
    enum class PortName{
        input0 = 0,
        input1 = 1,
        input2 = 2,
        output = 3,
      /*************/
        PortNumber
    };
    MAC(PortBitWidthType input0BW,
        PortBitWidthType input1BW,
        PortBitWidthType input2BW,
        PortBitWidthType adderOBW,
        PortBitWidthType outputBW,
        PortDataType multerOut=0,
        PortDataType adderOut =0)
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        :multer(input0BW,input1BW,adderOBW,multerOut)
        ,adder( adderOBW,input2BW,outputBW, adderOut){

        /**************************
         ** initialize the ports **
         **************************/
        this->ports.reserve(MAC::PortNumber);
        this->ports.emplace_back(input0BW, INPUT);
        this->ports.emplace_back(input1BW, INPUT);
        this->ports.emplace_back(input2BW, INPUT);
        this->ports.emplace_back(outputBW,OUTPUT);
        assert(this->ports.size()==MAC::PortNumber);

        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->adder.Clone(this->ports[MAC::PortName::output],
                                    Adder::PortName::output);
        assert(this->OutputConnected());
        return;
    }
    bool Run() override{
        this->multer.Run();
        return this->adder.Run();
    }

    inline void ConnectTo(MAC::PortName port,const Port& externalPort}){
        this->IndexConnectTo(port,externalPort);
        return;
    }
    inline void Clone(MAC::PortName port,const Port& externalPort}){
        this->IndexCloneTo(port,externalPort);
        return;
    }
    inline void InnerConnect() override{
        assert(this->OuterConnected());
        this->multer.Clone(Multer::PortName::input0,
                  this->ports[MAC::PortName::input0]);
        this->multer.Clone(Multer::PortName::input1,
                  this->ports[MAC::PortName::input1]);
        this->adder.Clone(Adder::PortName::input0,
                this->ports[MAC::PortName::input2]);

        PortNumType serial = this->adder.GetPortSerial(Adder::PortName::input1);

        this->multer.ConnectTo(serial,this->adder,
                        Multer::PortName::output);
        return;
    }
    inline bool Connected(){
        return this->InnerConnected()
            && this->OuterConnected();
    }
};

#endif // __COMPONENT_MAC_HPP
