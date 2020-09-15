#ifndef __COMPONENT_REFORMED_PE_HPP
#define __COMPONENT_REFORMED_PE_HPP

#include "../../define.hpp"
#include "../base/Object.hpp"
#include "../resource/BaseSequLogic.hpp"
#include "../resource/BaseCombiLogic.hpp"

class ReformedPE: public ClockedObject{
public:
    static constexpr const uint8_t
           VALID_BIT_WIDTH =  1,///the bit to mark whether this data is valid
          OFFSET_BIT_WIDTH =  6;///group number == 64
    static constexpr const uint8_t
             OUT_BIT_WIDTH =  1,
               W_BIT_WIDTH =  8,
              WE_BIT_WIDTH =  1;/// End-Of-Block
    static constexpr const uint8_t
               X_BIT_WIDTH =  8,
              XE_BIT_WIDTH =  1;/// End-Of-Block
    static constexpr const uint8_t
               S_BIT_WIDTH = 24,
              SE_BIT_WIDTH =  1;/// End-Of-Forwarding

    static constexpr const uint8_t
              wFIFO_LEN = 1,
              xFIFO_LEN = 1,
            owxFIFO_LEN = 1,
              sFIFO_LEN = 1;

    /** Compress methods **/
    static inline PortDataType CompressOWVEO(
            PortDataType out,
            PortDataType weight,
            PortDataType eow,
            PortDataType offset
        ){
        assert(((out    & ((Object::extend[   OUT_BIT_WIDTH])<<1))==0)
            && ((weight & ((Object::extend[     W_BIT_WIDTH])<<1))==0)
            && ((eow    & ((Object::extend[    WE_BIT_WIDTH])<<1))==0)
            && ((offset & ((Object::extend[OFFSET_BIT_WIDTH])<<1))==0));
        return (weight <<(OFFSET_BIT_WIDTH + WE_BIT_WIDTH + OUT_BIT_WIDTH))
             + (out    <<(OFFSET_BIT_WIDTH + WE_BIT_WIDTH))
             + (eow    <<(OFFSET_BIT_WIDTH))
             + (offset);
    }
    static inline PortDataType CompressXVEO(
            PortDataType x,
            PortDataType eox,
            PortDataType offset
        ){
        assert((((x& Object::mask[X_BIT_WIDTH])
                         & ((Object::extend[     X_BIT_WIDTH])<<1))==0)
             && ((eox    & ((Object::extend[    XE_BIT_WIDTH])<<1))==0)
             && ((offset & ((Object::extend[OFFSET_BIT_WIDTH])<<1))==0));
        return ((x & Object::mask[X_BIT_WIDTH])
                      <<(OFFSET_BIT_WIDTH + XE_BIT_WIDTH))
              +(eox   <<(OFFSET_BIT_WIDTH))
              +(offset);
    }
    static inline PortDataType CompressOWX(
            PortDataType ow,
            PortDataType x
        ){
        assert(((ow & ((Object::extend[OUT_BIT_WIDTH + W_BIT_WIDTH])<<1))==0)
            && ((x  & ((Object::extend[  X_BIT_WIDTH])<<1))==0));
        return (ow<<(X_BIT_WIDTH))
             +  x;
    }
    static inline PortDataType CompressOWX(
            PortDataType o,
            PortDataType w,
            PortDataType x
        ){
        assert(((o  & ((Object::extend[OUT_BIT_WIDTH])<<1))==0)
            && ((w  & ((Object::extend[  W_BIT_WIDTH])<<1))==0)
            && ((x  & ((Object::extend[  X_BIT_WIDTH])<<1))==0));
        return (o<<(X_BIT_WIDTH + W_BIT_WIDTH))
             + (w<<(X_BIT_WIDTH))
             +  x;
    }
    static inline PortDataType CompressSE(
            PortDataType s,
            PortDataType eof
        ){
        assert(((s   & ((Object::extend[ S_BIT_WIDTH])<<1))==0)
            && ((eof & ((Object::extend[SE_BIT_WIDTH])<<1))==0));
        return (s<<(SE_BIT_WIDTH))
             +  eof;
    }
    static inline PortDataType CompressWEO(
            PortDataType oeo,
            PortDataType valid
        ){
        assert(((oeo   & ((Object::extend[  OUT_BIT_WIDTH
                                      +      WE_BIT_WIDTH
                                      +  OFFSET_BIT_WIDTH])<<1))==0)
            && ((valid & ((Object::extend[VALID_BIT_WIDTH])<<1))==0));
        return (oeo<<(VALID_BIT_WIDTH))
              + valid;
    }
    static inline PortDataType CompressXEO(
            PortDataType xeo,
            PortDataType valid
        ){
        assert(((xeo   & ((Object::extend[   XE_BIT_WIDTH
                                      +  OFFSET_BIT_WIDTH])<<1))==0)
            && ((valid & ((Object::extend[VALID_BIT_WIDTH])<<1))==0));
        return (xeo<<(VALID_BIT_WIDTH))
              + valid;
    }

    /**  Decompress methods **/
    /** - Decompress owveo - **/
    static inline PortDataType DecompressOWVEO_W     (PortDataType owveo){
        assert((owveo & ((Object::extend[  W_BIT_WIDTH
                                    +    OUT_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo >> (OFFSET_BIT_WIDTH + WE_BIT_WIDTH + OUT_BIT_WIDTH))
             & Object::mask[W_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWVEO_Out   (PortDataType owveo){
        assert((owveo & ((Object::extend[  W_BIT_WIDTH
                                    +    OUT_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo >> (OFFSET_BIT_WIDTH + WE_BIT_WIDTH))
               & Object::mask[OUT_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWVEO_EOW   (PortDataType owveo){
        assert((owveo & ((Object::extend[  W_BIT_WIDTH
                                    +    OUT_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo >> (OFFSET_BIT_WIDTH))
               & Object::mask[WE_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWVEO_Offset(PortDataType owveo){
        assert((owveo & ((Object::extend[  W_BIT_WIDTH
                                    +    OUT_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo)
               & Object::mask[OFFSET_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWVEO_OEO (PortDataType owveo){
        assert((owveo & ((Object::extend[  W_BIT_WIDTH
                                    +    OUT_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo)
               & Object::mask[OUT_BIT_WIDTH
                         +     WE_BIT_WIDTH
                         + OFFSET_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWVEO_OW    (PortDataType owveo){
        assert((owveo & ((Object::extend[  W_BIT_WIDTH
                                    +    OUT_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo >> (OFFSET_BIT_WIDTH + WE_BIT_WIDTH))
             & Object::mask[W_BIT_WIDTH + OUT_BIT_WIDTH];
    }
    /** - Decompress xveo - **/
    static inline PortDataType DecompressXVEO_X     (PortDataType xexl){
        assert((xexl & ((Object::extend[     X_BIT_WIDTH
                                      +     XE_BIT_WIDTH
                                      + OFFSET_BIT_WIDTH])<<1))==0);
        return (xexl>>(OFFSET_BIT_WIDTH + XE_BIT_WIDTH))
               & Object::mask[X_BIT_WIDTH];
    }
    static inline PortDataType DecompressXVEO_EOX   (PortDataType xexl){
        assert((xexl & ((Object::extend[     X_BIT_WIDTH
                                      +     XE_BIT_WIDTH
                                      + OFFSET_BIT_WIDTH])<<1))==0);
        return (xexl>>(OFFSET_BIT_WIDTH))
               & Object::mask[XE_BIT_WIDTH];
    }
    static inline PortDataType DecompressXVEO_Offset(PortDataType xexl){
        assert((xexl & ((Object::extend[     X_BIT_WIDTH
                                      +     XE_BIT_WIDTH
                                      + OFFSET_BIT_WIDTH])<<1))==0);
        return (xexl & Object::mask[OFFSET_BIT_WIDTH]);
    }
    static inline PortDataType DecompressXVEO_EO    (PortDataType xexl){
        assert((xexl & ((Object::extend[     X_BIT_WIDTH
                                      +     XE_BIT_WIDTH
                                      + OFFSET_BIT_WIDTH])<<1))==0);
        return (xexl & Object::mask[XE_BIT_WIDTH
                              + OFFSET_BIT_WIDTH]);
    }
    /** - Decompress ow - **/
    static inline PortDataType DecompressOW_W (PortDataType ow){
        assert((ow & ((Object::extend[  W_BIT_WIDTH
                                    + OUT_BIT_WIDTH])<<1))==0);
        return (ow>>(OUT_BIT_WIDTH)) & Object::mask[W_BIT_WIDTH];
    }
    static inline PortDataType DecompressOW_O (PortDataType ow){
        assert((ow & ((Object::extend[  W_BIT_WIDTH
                                    + OUT_BIT_WIDTH])<<1))==0);
        return (ow & Object::mask[OUT_BIT_WIDTH]);
    }
    /** - Decompress se - **/
    static inline PortDataType DecompressSE_S (PortDataType se){
        assert((se & ((Object::extend[SE_BIT_WIDTH
                                    +  S_BIT_WIDTH])<<1))==0);
        return (se>>(SE_BIT_WIDTH)) & Object::mask[S_BIT_WIDTH];
    }
    static inline PortDataType DecompressSE_E (PortDataType se){
        assert((se & ((Object::extend[SE_BIT_WIDTH
                                    +  S_BIT_WIDTH])<<1))==0);
        return (se & Object::mask[SE_BIT_WIDTH]);
    }
    /** - Decompress owx - **/
    static inline PortDataType DecompressOWX_OW(PortDataType owx){
        assert((owx & ((Object::extend[  W_BIT_WIDTH
                                     + OUT_BIT_WIDTH
                                     +   X_BIT_WIDTH])<<1))==0);
        return (owx>>(X_BIT_WIDTH)) & Object::mask[W_BIT_WIDTH + OUT_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWX_X (PortDataType owx){
        assert((owx & ((Object::extend[  W_BIT_WIDTH
                                     + OUT_BIT_WIDTH
                                     +   X_BIT_WIDTH])<<1))==0);
        return (owx & Object::mask[X_BIT_WIDTH]);
    }
    /** - Decompress weo - **/
    static inline PortDataType DecompressWEO_Out  (PortDataType weo){
        assert((weo & ((Object::extend[OUT_BIT_WIDTH
                                  +     WE_BIT_WIDTH
                                  + OFFSET_BIT_WIDTH
                                  +  VALID_BIT_WIDTH])<<1))==0);
        return ((weo>>(    WE_BIT_WIDTH
                    +  OFFSET_BIT_WIDTH
                    +   VALID_BIT_WIDTH)) & Object::mask[OUT_BIT_WIDTH]);
    }
    static inline PortDataType DecompressWEO_EOW  (PortDataType weo){
        assert((weo & ((Object::extend[OUT_BIT_WIDTH
                                  +     WE_BIT_WIDTH
                                  + OFFSET_BIT_WIDTH
                                  +  VALID_BIT_WIDTH])<<1))==0);
        return ((weo>>(OFFSET_BIT_WIDTH
                    +   VALID_BIT_WIDTH)) & Object::mask[WE_BIT_WIDTH]);
    }
    static inline PortDataType DecompressWEO_Off  (PortDataType weo){
        assert((weo & ((Object::extend[OUT_BIT_WIDTH
                                  +     WE_BIT_WIDTH
                                  + OFFSET_BIT_WIDTH
                                  +  VALID_BIT_WIDTH])<<1))==0);
        return ((weo>>(VALID_BIT_WIDTH)) & Object::mask[OFFSET_BIT_WIDTH]);
    }
    static inline PortDataType DecompressWEO_Valid(PortDataType weo){
        assert((weo & ((Object::extend[OUT_BIT_WIDTH
                                  +     WE_BIT_WIDTH
                                  + OFFSET_BIT_WIDTH
                                  +  VALID_BIT_WIDTH])<<1))==0);
        return  (weo & Object::mask[ VALID_BIT_WIDTH]);
    }
    /** - Decompress xeo - **/
    static inline PortDataType DecompressXEO_Off  (PortDataType xeo){
        assert ((xeo & ((Object::extend[ XE_BIT_WIDTH
                                   + OFFSET_BIT_WIDTH
                                   +  VALID_BIT_WIDTH])<<1))==0);
        return ((xeo>>(OFFSET_BIT_WIDTH
                    +   VALID_BIT_WIDTH)) & Object::mask[XE_BIT_WIDTH]);
    }
    static inline PortDataType DecompressXEO_EOX  (PortDataType xeo){
        assert ((xeo & ((Object::extend[ XE_BIT_WIDTH
                                   + OFFSET_BIT_WIDTH
                                   +  VALID_BIT_WIDTH])<<1))==0);
        return ((xeo>>( VALID_BIT_WIDTH)) & Object::mask[OFFSET_BIT_WIDTH]);
    }
    static inline PortDataType DecompressXEO_Valid(PortDataType xeo){
        assert ((xeo & ((Object::extend[ XE_BIT_WIDTH
                                   + OFFSET_BIT_WIDTH
                                   +  VALID_BIT_WIDTH])<<1))==0);
        return  (xeo & Object::mask[ VALID_BIT_WIDTH]);
    }

    typedef uint32_t ClockFeqType;
    typedef uint32_t CycleCountType;

private:
    enum class RU_STATE{
        W_EQUAL_X,
        W_LESS_X ,
        X_LESS_W ,
        W_END    , ///       w  reach the end of group
        X_END    , ///       x  reach the end of group
        WX_END   , /// both w&x reach the end of group
        NOT_READY
    };

    /** MAC state **/
    bool hasRecEOF;
    bool hasRecOut;
    bool hasSentParSum;

    FIFO owxFIFO,/**       O+W+X-bit **/
       owveoFIFO,/** O+W+V+Off+E-bit **/
        xveoFIFO,/**   X+V+Off+E-bit **/
           sFIFO;/**         S+E-bit **/

    Reg ///owveoReg0, /** O+W+V+Off+E-bit **/
        ///owveoReg1, /** O+W+V+Off+E-bit **/
        /// xveoReg0, /**   X+V+Off+E-bit **/
        /// xveoReg1, /**   X+V+Off+E-bit **/
           owReg , /**         O+W-bit **/
            xReg , /**           X-bit **/
          ParSum , /**           S-bit **/
          weoReg , /**     O-Off-E-bit **/
          xeoReg ; /**       Off-E-bit **/

    Port ///pOWVEOReg0Out,   /** O+V+W+Off+E-bit **/
         ///pOWVEOReg1Out,   /** O+V+W+Off+E-bit **/
         pOWVEOFIFOEmpty, /**           1-bit **/
         pOWVEOFIFOOut,   /** O+V+W+Off+E-bit **/
         /// pXVEOReg0Out,   /**   X+V+Off+E-bit **/
         /// pXVEOReg1Out,   /**   X+V+Off+E-bit **/
          pXVEOFIFOEmpty, /**           1-bit **/
          pXVEOFIFOOut,   /**   X+V+Off+E-bit **/
           pOWXFIFOOut,   /**       O+W+X-bit **/
           pOWXFIFOFull,  /**           1-bit **/
           pOWXFIFOEmpty, /**           1-bit **/
             pSFIFOOut,   /**         S+E-bit **/
             pSFIFOEmpty, /**           1-bit **/
             pOWRegOut,   /**         O+W-bit **/
             pAdderOut,   /**           S-bit **/
            pParSumOut,   /**           S-bit **/
            pMulterOut,   /**         X+W-bit **/
            pWEORegOut,   /**     O-Off-E-bit **/
            pXEORegOut;   /**       Off-E-bit **/

     /** --- inner control --- **/
    Port ///pOWVEOReg0En,
         ///pOWVEOReg1En,
         /// pXVEOReg0En,
         /// pXVEOReg1En,
            pOWXRegEn,
             pOWRegIn,
              pWRegOut,
              pXRegIn,
            pParSumIn,
            pParSumEn,
         pOWVEOFIFOPop,
          pXVEOFIFOPop,
           pOWXFIFOIn,
           pOWXFIFOPush,
           pOWXFIFOPop,
             pSFIFOPop,
            pWEORegIn,   /**     O-Off-E-bit **/
            pWEORegEn,   /**           1-bit **/
            pXEORegIn,   /**       Off-E-bit **/
            pXEORegEn;   /**           1-bit **/

    PortDataType
    /** --- RU inner control --- **/
        owveoFIFOPop,
         xveoFIFOPop,
        ///owveoReg0En,
        ///owveoReg1En,
        /// xveoReg0En,
        /// xveoReg1En,
          owxFIFOPush,
         _owxFIFOPush,
    /** --- RU inner data --- **/
           weoRegIn, /** O-Off-E-bit **/
           weoRegEn, /**       1-bit **/
           xeoRegIn, /**   Off-E-bit **/
           xeoRegEn, /**       1-bit **/
          owxFIFOIn, /** O+W+X-bit **/
    /** --- MAC inner control --- **/
          owxFIFOPop,
         _owxFIFOPop,
            sFIFOPop,
           owxRegEn,
           ParSumEn,
    /** --- MAC inner data --- **/
            owRegIn, /**   O+W-bit **/
             wRegOut,/**     W-bit **/
             xRegIn, /**     X-bit **/
           ParSumIn, /**     S-bit **/
    /** --- outward port --- **/
          owPushDown,
           xPushRight,
           sPushDDown,
             sInDDown;

    Adder  adder;
    Multer multer;

    const ClockFeqType ruPer,
                       macPer;
    const bool receiveS;
    CycleCountType clock;

    /**    RU action   **/
    /** action handles **/
    inline void Stuck(){
        this->owveoFIFOPop = 0; /** owveoFIFO    1-bit **/
    /// this->owveoReg0En  = 0; /** owveoReg0    1-bit **/
    /// this->owveoReg1En  = 0; /** owveoReg1    1-bit **/
        this-> xveoFIFOPop  = 0; /**  xveoFIFO    1-bit **/
    /// this-> xveoReg0En   = 0; /**  xveoReg0    1-bit **/
    /// this-> xveoReg1En   = 0; /**  xveoReg1    1-bit **/
        this->_owxFIFOPush  = 0; /**   owxFIFO    1-bit **/
        this-> owPushDown   = 0; /** down PE      1-bit **/
        this-> xPushRight   = 0; /**  right PE    1-bit **/
        return;
    }
    inline void WPush(){
        this->owveoFIFOPop = 1; /** owveoFIFO    1-bit **/
    /// this->owveoReg0En  = 1; /** owveoReg0    1-bit **/
    /// this->owveoReg1En  = 1; /** owveoReg1    1-bit **/
        this->owPushDown   = 1; /** down PE      1-bit **/
    /// if (ReformedPE::DecompressOWVEO_Out(this->pOWVEOReg1Out.Data()) == 1)
        if (ReformedPE::DecompressWEO_Out(this->pWEORegOut.Data())==1)
            this->hasRecOut = true;
        return;
    }
    inline void WNotPush(){
        this->owveoFIFOPop = 0; /** owveoFIFO    1-bit **/
    /// this->owveoReg0En  = 0; /** owveoReg0    1-bit **/
    /// this->owveoReg1En  = 0; /** owveoReg1    1-bit **/
        this->owPushDown   = 0; /** down PE      1-bit **/
        return;
    }
    inline void XPush(){
        this->xveoFIFOPop  = 1; /**  xveoFIFO    1-bit **/
    /// this->xveoReg0En   = 1; /**  xveoReg0    1-bit **/
    /// this->xveoReg1En   = 1; /**  xveoReg1    1-bit **/
        this->xPushRight   = 1; /**  right PE    1-bit **/
        return;
    }
    inline void XNotPush(){
        this->xveoFIFOPop  = 0; /**  xveoFIFO    1-bit **/
    /// this->xveoReg0En   = 0; /**  xveoReg0    1-bit **/
    /// this->xveoReg1En   = 0; /**  xveoReg1    1-bit **/
        this->xPushRight   = 0; /**  right PE    1-bit **/
        return;
    }
    inline void Output(){
        this->_owxFIFOPush = 1;
        return;
    }
    inline void NotOutput(){
        this->_owxFIFOPush = 0;
        return;
    }
    inline void OutputOut(){
        this-> owxFIFOIn   = ReformedPE::CompressOWX(1,0,0);
        this->_owxFIFOPush = 1;
        return;
    }
    /** current states handles **/
    inline bool owveoFIFOIsEmpty() const{
        return (this->pOWVEOFIFOEmpty.Data()==1);
    }
    inline bool  xveoFIFOIsEmpty() const{
        return (this-> pXVEOFIFOEmpty.Data()==1);
    }
    inline bool   owxFIFOIsFull () const{
        return (this->   pOWXFIFOFull.Data()==1);
    }
    inline bool owveoFIFODownIsFull() const{
        return (this->ports[ReformedPE::OWFullDown.ToIndex()].Data()==1);
    }
    inline bool weoRegNotValid() const{
        return (ReformedPE::DecompressWEO_Valid(this->pWEORegOut.Data())==0);
    }
    inline bool xeoRegNotValid() const{
        return (ReformedPE::DecompressXEO_Valid(this->pXEORegOut.Data())==0);
    }
    inline bool owIsOut() const{
        return (ReformedPE::DecompressWEO_Out(this->pWEORegOut.Data()));
    }
    inline bool  xveoFIFORightIsFull() const{
        return (this->ports[ReformedPE::XFullRight.ToIndex()].Data()==1);
    }
//    /**   ewl register 0  **/
//    inline PortDataType wlReg0() const{
//        return  ReformedPE::DecompressOWVEO_Offset(this->pOWVEOReg0Out.Data());
//    }
//    inline bool eowReg0() const{
//        return  ReformedPE::DecompressOWVEO_EOW(this->pOWVEOReg0Out.Data());
//    }
//    /**   ewl register 1  **/
//    inline PortDataType wlReg1() const{
//        return  ReformedPE::DecompressOWVEO_Offset(this->pOWVEOReg1Out.Data());
//    }
//    inline bool eowReg1() const{
//        return  ReformedPE::DecompressOWVEO_EOW(this->pOWVEOReg1Out.Data()==1);
//    }
//    inline bool outReg1() const{
//        return  ReformedPE::DecompressOWVEO_Out(this->pOWVEOReg1Out.Data()==1);
//    }
//    /**  xexl register 0  **/
//    inline PortDataType  xReg0() const{
//        return  ReformedPE::DecompressXVEO_X     (this->pXVEOReg0Out.Data());
//    }
//    inline PortDataType xlReg0() const{
//        return  ReformedPE::DecompressXVEO_Offset(this->pXVEOReg0Out.Data());
//    }
//    inline bool eoxReg0() const{
//        return  ReformedPE::DecompressXVEO_EOX   (this->pXVEOReg0Out.Data());
//    }
//    /**  xexl register 1  **/
//    inline PortDataType  xReg1() const{
//        return  ReformedPE::DecompressXVEO_X     (this->pXVEOReg1Out.Data());
//    }
//    inline PortDataType xlReg1() const{
//        return  ReformedPE::DecompressXVEO_Offset(this->pXVEOReg1Out.Data());
//    }
//    inline bool   eoxReg1() const{
//        return  ReformedPE::DecompressXVEO_EOX   (this->pXVEOReg1Out.Data());
//    }
    inline bool wIsEnd() const{
        return ReformedPE::DecompressWEO_EOW(this->pWEORegOut.Data())==1;
    }
    inline bool xIsEnd() const{
        return ReformedPE::DecompressWEO_EOW(this->pXEORegOut.Data())==1;
    }
    inline PortDataType wOff() const{
        return ReformedPE::DecompressWEO_Off(this->pWEORegOut.Data());
    }
    inline PortDataType xOff() const{
        return ReformedPE::DecompressXEO_Off(this->pXEORegOut.Data());
    }

    /**   MAC handle **/
    /** action handles **/
    inline void NotForward(){
        this->sInDDown   = 0;
        this->sFIFOPop   = 0;
        this->sPushDDown = 0;
        return;
    }
    inline void ForwardRecSum(){
        this->sInDDown   = this->CompressSE(this->DecompressSE_S(this->pSFIFOOut.Data()),0);
        this->sFIFOPop   = 1;
        this->sPushDDown = 1;
        if (this->DecompressSE_E(this->pSFIFOOut.Data())==1)
            this->hasRecEOF = true;
        return;
    }
    inline void ForwardParSum(){
        assert(false);
        this->sInDDown      = this->CompressSE(this->pParSumOut.Data(),1);
        this->sFIFOPop      = 0;
        this->sPushDDown    = 1;
        this->hasRecEOF     = false;
        this->hasSentParSum = true;
        return;
    }
    inline void ForwardAddOut(){
        this->sInDDown      = this->CompressSE(this->pAdderOut.Data(),1);
        this->sFIFOPop      = 0;
        this->sPushDDown    = 1;
        this->hasRecEOF     = false;
        this->hasSentParSum = true;
        return;
    }
    inline void owxRegWrite(){
        this-> owxRegEn   = 1;
        this->_owxFIFOPop = 1;
        return;
    }
    inline void owxRegNotWrite(){
        this-> owxRegEn   = 0;
        this->_owxFIFOPop = 0;
        return;
    }
    inline void ParSumNotWrite(){
        this->ParSumEn = 0;
        this->ParSumIn = 0;
        return;
    }
    inline void ParSumWriteAddOut(){
        this->ParSumEn = 1;
        this->ParSumIn = this->pAdderOut.SignedData();
        return;
    }
    inline void ParSumWriteZero(){
        this->ParSumEn = 1;
        this->ParSumIn = 0;
        return;
    }
    inline void ParSumWriteMultOut(){
        assert(false);
        this->ParSumEn = 1;
        this->ParSumIn = this->pMulterOut.SignedData();
        return;
    }
    /** current state handles **/
    inline bool owxFIFOIsEmpty() const{
        return this->pOWXFIFOEmpty.Data()==1;
    }
    inline bool owNotOut()    const{
        return ((ReformedPE::DecompressOW_O(this->pOWRegOut.Data())==0)
             || (this->hasSentParSum));
    }
    inline bool NeedForward() const{
        return (!this->hasRecEOF)
            && (this->pSFIFOEmpty.Data() == 0)
            &&   this->receiveS;
    }
    inline bool  EndForward() const{
        return ( this->hasRecEOF)
            || (!this->receiveS );
    }
    inline bool  CanForward() const{
        return (this->ports[ReformedPE::SFullDDown.ToIndex()].Data()==0);
    }
    inline bool  CanForwardRec() const{
        return (this->ports[ReformedPE::SFullDDown.ToIndex()].Data()==0)
            && (this->pSFIFOEmpty.Data() == 0);
    }

    /** for state transfer **/
    inline enum RU_STATE NowState() const{
        if (this->owveoFIFOIsEmpty()
        ||  this-> xveoFIFOIsEmpty()
        ||  this->  weoRegNotValid()
        ||  this->  xeoRegNotValid())
            return RU_STATE::NOT_READY;

        if (this->wOff()<this->xOff()){
            if (this->wIsEnd()){
                if (this->xIsEnd())
                    return RU_STATE::WX_END;
                else
                    return RU_STATE:: W_END;
            }
            else
                return RU_STATE::W_LESS_X;
        }
        else if (this->xOff()<this->wOff()){
            if (this->xIsEnd()){
                if (this->wIsEnd())
                    return RU_STATE::WX_END;
                else
                    return RU_STATE:: X_END;
            }
            else
                return RU_STATE::X_LESS_W;
        }
        else
            return RU_STATE::W_EQUAL_X;

        assert(false);
        return RU_STATE::NOT_READY;
    }

//    inline void SetNextState(
//        PortDataType wl,bool eow,
//        PortDataType xl,bool eox){
//        assert(false);
//        if (this->ruState == RU_STATE::WL_OX_END
//         || this->ruState == RU_STATE::XL_OX_END
//         || this->ruState == RU_STATE::WL_END
//         || this->ruState == RU_STATE::XL_END){
//            if(eow && eox){
//                if (this->ruState == RU_STATE::XL_OX_END
//                 || this->ruState == RU_STATE::XL_END){
//                    if (wl == xl)
//                        this->ruState = RU_STATE::WX_OX_END;
//                    else
//                        this->ruState = RU_STATE::WX_END;
//                }
//                else
//                    this->ruState = RU_STATE::WX_END;
//            }
//            else
//            if (this->ruState == RU_STATE::WL_OX_END
//             || this->ruState == RU_STATE::WL_END){
//                this->ruState  = RU_STATE::WL_END;
//            }
//            else
//                this->ruState  = RU_STATE::XL_END;
//        }
//        else{
//            if (wl == xl){
//                if (!eow &&!eox)
//                    this->ruState = RU_STATE::WL_EQUAL_XL;
//                else if (!eow && eox)
//                    this->ruState = RU_STATE::XL_OX_END;
//                else if ( eow &&!eox)
//                    this->ruState = RU_STATE::WL_OX_END;
//                else
//                    this->ruState = RU_STATE::WX_OX_END;
//            }
//            else if (wl < xl){
//                if (!eow)
//                    this->ruState = RU_STATE::WL_LESS_XL;
//                else if ( eow &&!eox)
//                    this->ruState = RU_STATE::WL_END;
//                else
//                    this->ruState = RU_STATE::WX_END;
//            }
//            else{///wl > xl
//                if (!eox)
//                    this->ruState = RU_STATE::WL_BIGGER_XL;
//                else if (!eow && eox)
//                    this->ruState = RU_STATE::XL_END;
//                else
//                    this->ruState = RU_STATE::WX_END;
//            }
//        }
//        return;
//    }

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::Number());
        /** ---------- up -------- **/
        this->ports.push_back(Port(OUT_BIT_WIDTH
                              +      W_BIT_WIDTH
                              +  VALID_BIT_WIDTH
                              + OFFSET_BIT_WIDTH
                              +     WE_BIT_WIDTH,
                                      PortDirection::IN ));/** OWInUp   O+W+V+Off+E-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** OWPushUp           1-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** OWFullUp           1-bit out **/
        /** --------- down ------- **/
        this->ports.push_back(Port(OUT_BIT_WIDTH
                              +      W_BIT_WIDTH
                              +  VALID_BIT_WIDTH
                              + OFFSET_BIT_WIDTH
                              +     WE_BIT_WIDTH,
                                      PortDirection::OUT));/** OWOutDown  O+W+V+Off+E-bit out **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** OWPushDown           1-bit out **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** OWFullDown           1-bit in  **/
        /** -------- left -------- **/
        this->ports.push_back(Port(  X_BIT_WIDTH
                              +  VALID_BIT_WIDTH
                              + OFFSET_BIT_WIDTH
                              +     XE_BIT_WIDTH,
                                      PortDirection::IN ));/** XInLeft  X+V+Off+E-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** XPushLeft        1-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** XFullLeft        1-bit out **/
        /** -------- right ------- **/
        this->ports.push_back(Port(  X_BIT_WIDTH
                              +  VALID_BIT_WIDTH
                              + OFFSET_BIT_WIDTH
                              +     XE_BIT_WIDTH,
                                      PortDirection::OUT));/** XOutRight   X+V+Off+E-bit out **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** XPushRight          1-bit out **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** XFullRight          1-bit in  **/
        /** -------- up-up ------- **/
        this->ports.push_back(Port(S_BIT_WIDTH
                                + SE_BIT_WIDTH,
                                      PortDirection::IN ));/** SInUUp   S+E-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** SPushUUp   1-bit in  **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** SFullUUp   1-bit out **/
        /** ------ down-down ----- **/
        this->ports.push_back(Port(S_BIT_WIDTH
                                + SE_BIT_WIDTH,
                                      PortDirection::OUT));/** SOutDDown  S+E-bit out **/
        this->ports.push_back(Port( 1,PortDirection::OUT));/** SPushDDown   1-bit out **/
        this->ports.push_back(Port( 1,PortDirection::IN ));/** SFullDDown   1-bit in  **/
        assert(this->ports.size()==PortName::Number());
        return;
    }

    inline void OutputConnect() override{
        /** ---------- up -------- **/
        this->owveoFIFO.Clone(this->ports[ReformedPE::OWFullUp .ToIndex()],FIFO::Full);
        /** --------- down ------- **/
        this->owveoFIFO.Clone(this->ports[ReformedPE::OWOutDown.ToIndex()],FIFO::Out );
        this->ports[ReformedPE::OWPushDown.ToIndex()].Clone(this->owPushDown);
        /** -------- left -------- **/
        this-> xveoFIFO.Clone(this->ports[ReformedPE::XFullLeft.ToIndex()],FIFO::Full);
        /** -------- right ------- **/
        this-> xveoFIFO.Clone(this->ports[ReformedPE::XOutRight.ToIndex()],FIFO::Out );
        this->ports[ReformedPE::XPushRight.ToIndex()].Clone(this->xPushRight);
        /** -------- up-up ------- **/
        this->    sFIFO.Clone(this->ports[ReformedPE:: SFullUUp.ToIndex()],FIFO::Full);
        /** ------ down-down ----- **/
        this->ports[ReformedPE:: SOutDDown.ToIndex()].Clone(this->  sInDDown);
        this->ports[ReformedPE::SPushDDown.ToIndex()].Clone(this->sPushDDown);
        assert(this->OutputConnected());
        return;
    }

public:
    class PortName:public BasePortName{
    public:
        enum class _PortName:uint8_t{
        /** ---------- up -------- **/
          OWInUp     =  0, /**  O+W+V+Off+E-bit in  **/
          OWPushUp   =  1, /**            1-bit in  **/
          OWFullUp   =  2, /**            1-bit out **/
        /** --------- down ------- **/
          OWOutDown  =  3, /**  O+W+V+Off+E-bit out **/
          OWPushDown =  4, /**            1-bit out **/
          OWFullDown =  5, /**            1-bit in  **/
        /** -------- left -------- **/
          XInLeft    =  6, /**    X+V+Off+E-bit in  **/
          XPushLeft  =  7, /**            1-bit in  **/
          XFullLeft  =  8, /**            1-bit out **/
        /** -------- right ------- **/
          XOutRight  =  9, /**    X+V+Off+E-bit out **/
          XPushRight = 10, /**            1-bit out **/
          XFullRight = 11, /**            1-bit in  **/
        /** -------- up-up ------- **/
          SInUUp     = 12, /**        S+E-bit in  **/
          SPushUUp   = 13, /**          1-bit in  **/
          SFullUUp   = 14, /**          1-bit out **/
        /** ------ down-down ----- **/
          SOutDDown  = 15, /**        S+E-bit out **/
          SPushDDown = 16, /**          1-bit out **/
          SFullDDown = 17, /**          1-bit in  **/
          /************************/
          PortNumber
        };
    private:
        enum _PortName portName;
    public:
        PortName(enum _PortName _portName):portName(_portName){
            assert(_portName != _PortName::PortNumber);
            return;
        }
        inline PortNumType ToIndex() const override{
            return static_cast<PortNumType>(this->portName);
        }
        static inline PortNumType Number(){
            return static_cast<PortNumType>(_PortName::PortNumber);
        }
        static inline PortNumType ToIndex(enum _PortName portName){
            assert(portName != _PortName::PortNumber);
            return static_cast<PortNumType>(portName);
        }
    };

    /** ---------- up -------- **/
    static const ReformedPE::PortName   OWInUp; /**  O+W+V+Off+E-bit in  **/
    static const ReformedPE::PortName OWPushUp; /**            1-bit in  **/
    static const ReformedPE::PortName OWFullUp; /**            1-bit out **/
    /** --------- down ------- **/
    static const ReformedPE::PortName  OWOutDown; /**  O+W+V+Off+E-bit out **/
    static const ReformedPE::PortName OWPushDown; /**            1-bit out **/
    static const ReformedPE::PortName OWFullDown; /**            1-bit in  **/
    /** -------- left -------- **/
    static const ReformedPE::PortName   XInLeft; /**    X+V+Off+E-bit in  **/
    static const ReformedPE::PortName XPushLeft; /**            1-bit in  **/
    static const ReformedPE::PortName XFullLeft; /**            1-bit out **/
    /** -------- right ------- **/
    static const ReformedPE::PortName  XOutRight; /**    X+V+Off+E-bit out **/
    static const ReformedPE::PortName XPushRight; /**            1-bit out **/
    static const ReformedPE::PortName XFullRight; /**            1-bit in  **/
    /** -------- up-up ------- **/
    static const ReformedPE::PortName   SInUUp; /**        S+E-bit in  **/
    static const ReformedPE::PortName SPushUUp; /**          1-bit in  **/
    static const ReformedPE::PortName SFullUUp; /**          1-bit out **/
    /** -------- down-down ------- **/
    static const ReformedPE::PortName  SOutDDown; /**        S+E-bit out **/
    static const ReformedPE::PortName SPushDDown; /**          1-bit out **/
    static const ReformedPE::PortName SFullDDown; /**          1-bit in  **/

    ReformedPE(
        ClockFeqType  ruPer,
        ClockFeqType macPer,
        bool receiveS,
        /** --- RU inner control --- **/
        PortDataType owveoFIFOPop = 0,
        PortDataType  xveoFIFOPop = 0,
        PortDataType owveoReg0En  = 0,
        PortDataType owveoReg1En  = 0,
        PortDataType  xveoReg0En  = 0,
        PortDataType  xveoReg1En  = 0,
        PortDataType  owxFIFOPush = 0,
        PortDataType _owxFIFOPush = 0,
        /** --- RU inner data --- **/
        PortDataType  owxFIFOIn  = 0, /** O+W+X-bit **/
        /** --- MAC inner control --- **/
        PortDataType  owxFIFOPop = 0,
        PortDataType _owxFIFOPop = 0,
        PortDataType    sFIFOPop = 0,
        PortDataType   owxRegEn  = 0,
        PortDataType   ParSumEn  = 0,
        /** --- MAC inner data --- **/
        PortDataType  owRegIn  = 0,/**   O+W-bit **/
        PortDataType   wRegOut = 0,/**     W-bit **/
        PortDataType   xRegIn  = 0,/**     X-bit **/
        PortDataType ParSumIn  = 0,/**     S-bit **/
        /** --- outward port --- **/
        PortDataType  owPushDown  = 0,
        PortDataType   xPushRight = 0,
        PortDataType   sPushDDown = 0,
        PortDataType     sInDDown = 0,
        /** --- MAC state --- **/
        bool hasRecEOF     = false,
        bool hasRecOut     = false,
        bool hasSentParSum = false
        ):
//          owveoReg0(OUT_BIT_WIDTH
//               +      W_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     WE_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/** O+V+W+Off+E-bit **/
//          owveoReg1(OUT_BIT_WIDTH
//               +      W_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     WE_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/** O+W+V+Off+E-bit **/
//           xveoReg0(  X_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     XE_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**   X+V+Off+E-bit **/
//           xveoReg1(  X_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     XE_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**   X+V+Off+E-bit **/
             owReg (OUT_BIT_WIDTH
               +      W_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**       O+W-bit **/
              xReg (  X_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**         X-bit **/
            ParSum (  S_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**         S-bit **/
            weoReg (OUT_BIT_WIDTH
               +     WE_BIT_WIDTH
               + OFFSET_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**   O+Off+E-bit **/
            xeoReg ( XE_BIT_WIDTH
               + OFFSET_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**     Off+E-bit **/
            owxFIFO(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +      X_BIT_WIDTH,owxFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**     O+W+X-bit **/
          owveoFIFO(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +  VALID_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     WE_BIT_WIDTH,  wFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/** O+W+V+Off+E-bit **/
           xveoFIFO(  X_BIT_WIDTH
               +  VALID_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     XE_BIT_WIDTH,  xFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**   X+V+Off+E-bit **/
              sFIFO(  S_BIT_WIDTH
               +     SE_BIT_WIDTH,  sFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**       S+E-bit **/
//      pOWVEOReg0Out(OUT_BIT_WIDTH
//               +      W_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     WE_BIT_WIDTH,Object::PortDirection::IN ),/** O+W+V+Off+E-bit **/
//      pOWVEOReg1Out(OUT_BIT_WIDTH
//               +      W_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     WE_BIT_WIDTH,Object::PortDirection::IN ),/** O+W+V+Off+E-bit **/
    pOWVEOFIFOEmpty(      1      ,Object::PortDirection::IN ),/**           1-bit **/
    pOWVEOFIFOOut  (OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +  VALID_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     WE_BIT_WIDTH,Object::PortDirection::IN ),/** O+W+V+Off+E-bit **/
    pXVEOFIFOOut   (  X_BIT_WIDTH
               +  VALID_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     XE_BIT_WIDTH,Object::PortDirection::IN ),/**   X+V+Off+E-bit **/
//       pXVEOReg0Out(  X_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               +     XE_BIT_WIDTH,Object::PortDirection::IN ),/** X+V+Off+E-bit **/
//       pXVEOReg1Out(  X_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               +     XE_BIT_WIDTH,Object::PortDirection::IN ),/** X+V+Off+E-bit **/
     pXVEOFIFOEmpty(      1      ,Object::PortDirection::IN ),/**         1-bit **/
        pOWXFIFOOut(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +      X_BIT_WIDTH,Object::PortDirection::IN ),/**     O+W+X-bit **/
       pOWXFIFOFull(      1      ,Object::PortDirection::IN ),/**         1-bit **/
      pOWXFIFOEmpty(      1      ,Object::PortDirection::IN ),/**         1-bit **/
          pSFIFOOut(  S_BIT_WIDTH
               +     SE_BIT_WIDTH,Object::PortDirection::IN ),/**       S+E-bit **/
        pSFIFOEmpty(      1      ,Object::PortDirection::IN ),/**         1-bit **/
          pOWRegOut(OUT_BIT_WIDTH
               +      W_BIT_WIDTH,Object::PortDirection::IN ),/**       O+W-bit **/
         pWEORegOut(OUT_BIT_WIDTH
               +     WE_BIT_WIDTH
               + OFFSET_BIT_WIDTH,Object::PortDirection::IN ),/**   O+Off+E-bit **/
         pXEORegOut( XE_BIT_WIDTH
               + OFFSET_BIT_WIDTH,Object::PortDirection::IN ),/**     Off+E-bit **/
          pAdderOut(  S_BIT_WIDTH,Object::PortDirection::IN ),/**         S-bit **/
         pParSumOut(  S_BIT_WIDTH,Object::PortDirection::IN ),/**         S-bit **/
         pMulterOut(  W_BIT_WIDTH
               +      X_BIT_WIDTH,Object::PortDirection::IN ),/**       W+X-bit **/
     /** --- inner control --- **/
//       pOWVEOReg0En  (1,Object::PortDirection::OUT),/**     1-bit **/
//       pOWVEOReg1En  (1,Object::PortDirection::OUT),/**     1-bit **/
//        pXVEOReg0En  (1,Object::PortDirection::OUT),/**     1-bit **/
//        pXVEOReg1En  (1,Object::PortDirection::OUT),/**     1-bit **/
          pOWXRegEn  (1,Object::PortDirection::OUT),/**     1-bit **/
          pParSumEn  (1,Object::PortDirection::OUT),/**     1-bit **/
       pOWVEOFIFOPop (1,Object::PortDirection::OUT),/**     1-bit **/
        pXVEOFIFOPop (1,Object::PortDirection::OUT),/**     1-bit **/
         pOWXFIFOPush(1,Object::PortDirection::OUT),/**     1-bit **/
         pOWXFIFOPop (1,Object::PortDirection::OUT),/**     1-bit **/
           pSFIFOPop (1,Object::PortDirection::OUT),/**     1-bit **/
          pWEORegEn  (1,Object::PortDirection::OUT),/**   O+Off+E-bit **/
          pXEORegEn  (1,Object::PortDirection::OUT),/**     Off+E-bit **/
           pOWRegIn  (OUT_BIT_WIDTH
                 +      W_BIT_WIDTH,Object::PortDirection::OUT),/**   O+W-bit **/
            pWRegOut (  W_BIT_WIDTH,Object::PortDirection::OUT),/**     W-bit **/
            pXRegIn  (  X_BIT_WIDTH,Object::PortDirection::OUT),/**     X-bit **/
          pParSumIn  (  S_BIT_WIDTH,Object::PortDirection::OUT),/**     S-bit **/
         pOWXFIFOIn  (OUT_BIT_WIDTH
                 +      W_BIT_WIDTH
                 +      X_BIT_WIDTH,Object::PortDirection::OUT),/** O+W+X-bit **/
          pWEORegIn  (OUT_BIT_WIDTH
                 +     WE_BIT_WIDTH
                 + OFFSET_BIT_WIDTH,Object::PortDirection::OUT),/**   O+Off+E-bit **/
          pXEORegIn  ( XE_BIT_WIDTH
                 + OFFSET_BIT_WIDTH,Object::PortDirection::OUT),/**     Off+E-bit **/
             adder (W_BIT_WIDTH+X_BIT_WIDTH,S_BIT_WIDTH,S_BIT_WIDTH,true),
            multer (W_BIT_WIDTH,X_BIT_WIDTH,W_BIT_WIDTH+X_BIT_WIDTH,true),
             ruPer ( ruPer),
            macPer (macPer),
           receiveS(receiveS){
        this->clock   = 0;
        this->hasRecEOF     = hasRecEOF;
        this->hasRecOut     = hasRecOut;
        this->hasSentParSum = hasSentParSum;
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        /** --- RU inner control --- **/
        this->owveoFIFOPop = owveoFIFOPop;
        this-> xveoFIFOPop =  xveoFIFOPop;
//        this->owveoReg0En  = owveoReg0En ;
//        this->owveoReg1En  = owveoReg1En ;
//        this-> xveoReg0En  =  xveoReg0En ;
//        this-> xveoReg1En  =  xveoReg1En ;
        this-> owxFIFOPush =  owxFIFOPush;
        this->_owxFIFOPush = _owxFIFOPush;
        /** --- RU inner data --- **/
        this->owxFIFOIn    = owxFIFOIn;/** O+W+X-bit **/
        /** --- MAC inner control --- **/
        this-> owxFIFOPop  = owxFIFOPop;
        this->_owxFIFOPop  =_owxFIFOPop;
        this->   sFIFOPop  =   sFIFOPop;
        this->  owxRegEn   =  owxRegEn ;
        this->  ParSumEn   =  ParSumEn ;
        /** --- MAC inner data --- **/
        this-> owRegIn  =  owRegIn ;/**   O+W-bit **/
        this->  wRegOut =   wRegOut;/**     W-bit **/
        this->  xRegIn  =   xRegIn ;/**     X-bit **/
        this->ParSumIn  = ParSumIn ;/**     S-bit **/
        /** --- outward port --- **/
        this-> owPushDown  = owPushDown ;
        this->  xPushRight =  xPushRight;
        this->  sPushDDown =  sPushDDown;
        this->    sInDDown =    sInDDown;
        /**************************
         ** initialize the ports **
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }

    ReformedPE(const ReformedPE& rePE
        ):
//          owveoReg0(OUT_BIT_WIDTH
//               +      W_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     WE_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/** O+V+W+Off+E-bit **/
//          owveoReg1(OUT_BIT_WIDTH
//               +      W_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     WE_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/** O+W+V+Off+E-bit **/
//           xveoReg0(  X_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     XE_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**   X+V+Off+E-bit **/
//           xveoReg1(  X_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     XE_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**   X+V+Off+E-bit **/
             owReg (OUT_BIT_WIDTH
               +      W_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**       O+W-bit **/
              xReg (  X_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**         X-bit **/
            ParSum (  S_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**         S-bit **/
            weoReg (OUT_BIT_WIDTH
               +     WE_BIT_WIDTH
               + OFFSET_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**   O+Off+E-bit **/
            xeoReg ( XE_BIT_WIDTH
               + OFFSET_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**     Off+E-bit **/
            owxFIFO(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +      X_BIT_WIDTH,owxFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**     O+W+X-bit **/
          owveoFIFO(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +  VALID_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     WE_BIT_WIDTH,  wFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/** O+W+V+Off+E-bit **/
           xveoFIFO(  X_BIT_WIDTH
               +  VALID_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     XE_BIT_WIDTH,  xFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**   X+V+Off+E-bit **/
              sFIFO(  S_BIT_WIDTH
               +     SE_BIT_WIDTH,  sFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**       S+E-bit **/
//      pOWVEOReg0Out(OUT_BIT_WIDTH
//               +      W_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     WE_BIT_WIDTH,Object::PortDirection::IN ),/** O+W+V+Off+E-bit **/
//      pOWVEOReg1Out(OUT_BIT_WIDTH
//               +      W_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +     WE_BIT_WIDTH,Object::PortDirection::IN ),/** O+W+V+Off+E-bit **/
    pOWVEOFIFOEmpty(      1      ,Object::PortDirection::IN ),/**           1-bit **/
    pOWVEOFIFOOut  (OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +  VALID_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     WE_BIT_WIDTH,Object::PortDirection::IN ),/** O+W+V+Off+E-bit **/
    pXVEOFIFOOut   (  X_BIT_WIDTH
               +  VALID_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     XE_BIT_WIDTH,Object::PortDirection::IN ),/**   X+V+Off+E-bit **/
//       pXVEOReg0Out(  X_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               +     XE_BIT_WIDTH,Object::PortDirection::IN ),/** X+V+Off+E-bit **/
//       pXVEOReg1Out(  X_BIT_WIDTH
//               + OFFSET_BIT_WIDTH
//               +  VALID_BIT_WIDTH
//               +     XE_BIT_WIDTH,Object::PortDirection::IN ),/** X+V+Off+E-bit **/
     pXVEOFIFOEmpty(      1      ,Object::PortDirection::IN ),/**         1-bit **/
        pOWXFIFOOut(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +      X_BIT_WIDTH,Object::PortDirection::IN ),/**     O+W+X-bit **/
       pOWXFIFOFull(      1      ,Object::PortDirection::IN ),/**         1-bit **/
      pOWXFIFOEmpty(      1      ,Object::PortDirection::IN ),/**         1-bit **/
          pSFIFOOut(  S_BIT_WIDTH
               +     SE_BIT_WIDTH,Object::PortDirection::IN ),/**       S+E-bit **/
        pSFIFOEmpty(      1      ,Object::PortDirection::IN ),/**         1-bit **/
          pOWRegOut(OUT_BIT_WIDTH
               +      W_BIT_WIDTH,Object::PortDirection::IN ),/**       O+W-bit **/
         pWEORegOut(OUT_BIT_WIDTH
               +     WE_BIT_WIDTH
               + OFFSET_BIT_WIDTH,Object::PortDirection::IN ),/**   O+Off+E-bit **/
         pXEORegOut( XE_BIT_WIDTH
               + OFFSET_BIT_WIDTH,Object::PortDirection::IN ),/**     Off+E-bit **/
          pAdderOut(  S_BIT_WIDTH,Object::PortDirection::IN ),/**         S-bit **/
         pParSumOut(  S_BIT_WIDTH,Object::PortDirection::IN ),/**         S-bit **/
         pMulterOut(  W_BIT_WIDTH
               +      X_BIT_WIDTH,Object::PortDirection::IN ),/**       W+X-bit **/
     /** --- inner control --- **/
//       pOWVEOReg0En  (1,Object::PortDirection::OUT),/**     1-bit **/
//       pOWVEOReg1En  (1,Object::PortDirection::OUT),/**     1-bit **/
//        pXVEOReg0En  (1,Object::PortDirection::OUT),/**     1-bit **/
//        pXVEOReg1En  (1,Object::PortDirection::OUT),/**     1-bit **/
          pOWXRegEn  (1,Object::PortDirection::OUT),/**     1-bit **/
          pParSumEn  (1,Object::PortDirection::OUT),/**     1-bit **/
       pOWVEOFIFOPop (1,Object::PortDirection::OUT),/**     1-bit **/
        pXVEOFIFOPop (1,Object::PortDirection::OUT),/**     1-bit **/
         pOWXFIFOPush(1,Object::PortDirection::OUT),/**     1-bit **/
         pOWXFIFOPop (1,Object::PortDirection::OUT),/**     1-bit **/
           pSFIFOPop (1,Object::PortDirection::OUT),/**     1-bit **/
          pWEORegEn  (1,Object::PortDirection::OUT),/**   O+Off+E-bit **/
          pXEORegEn  (1,Object::PortDirection::OUT),/**     Off+E-bit **/
           pOWRegIn  (OUT_BIT_WIDTH
                 +      W_BIT_WIDTH,Object::PortDirection::OUT),/**   O+W-bit **/
            pWRegOut (  W_BIT_WIDTH,Object::PortDirection::OUT),/**     W-bit **/
            pXRegIn  (  X_BIT_WIDTH,Object::PortDirection::OUT),/**     X-bit **/
          pParSumIn  (  S_BIT_WIDTH,Object::PortDirection::OUT),/**     S-bit **/
         pOWXFIFOIn  (OUT_BIT_WIDTH
                 +      W_BIT_WIDTH
                 +      X_BIT_WIDTH,Object::PortDirection::OUT),/** O+W+X-bit **/
          pWEORegIn  (OUT_BIT_WIDTH
                 +     WE_BIT_WIDTH
                 + OFFSET_BIT_WIDTH,Object::PortDirection::OUT),/**   O+Off+E-bit **/
          pXEORegIn  ( XE_BIT_WIDTH
                 + OFFSET_BIT_WIDTH,Object::PortDirection::OUT),/**     Off+E-bit **/
             adder (W_BIT_WIDTH+X_BIT_WIDTH,S_BIT_WIDTH,S_BIT_WIDTH,true),
            multer (W_BIT_WIDTH,X_BIT_WIDTH,W_BIT_WIDTH+X_BIT_WIDTH,true),
             ruPer ( ruPer),
            macPer (macPer),
           receiveS(receiveS){
        this->clock   = 0;
        this->hasRecEOF     = rePE.hasRecEOF;
        this->hasRecOut     = rePE.hasRecOut;
        this->hasSentParSum = rePE.hasSentParSum;
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        /** --- RU inner control --- **/
        this->owveoFIFOPop = rePE. owveoFIFOPop;
        this-> xveoFIFOPop = rePE.  xveoFIFOPop;
//        this->owveoReg0En  = rePE. owveoReg0En ;
//        this->owveoReg1En  = rePE. owveoReg1En ;
//        this-> xveoReg0En  = rePE.  xveoReg0En ;
//        this-> xveoReg1En  = rePE.  xveoReg1En ;
        this-> owxFIFOPush = rePE.  owxFIFOPush;
        this->_owxFIFOPush = rePE. _owxFIFOPush;
        /** --- RU inner data --- **/
        this->owxFIFOIn    = rePE.owxFIFOIn;/** O+W+X-bit **/
        /** --- MAC inner control --- **/
        this-> owxFIFOPop  = rePE. owxFIFOPop;
        this->_owxFIFOPop  = rePE._owxFIFOPop;
        this->   sFIFOPop  = rePE.   sFIFOPop;
        this->  owxRegEn   = rePE.  owxRegEn ;
        this->  ParSumEn   = rePE.  ParSumEn ;
        /** --- MAC inner data --- **/
        this-> owRegIn  = rePE. owRegIn ;/**   O+W-bit **/
        this->  wRegOut = rePE.  wRegOut;/**     W-bit **/
        this->  xRegIn  = rePE.  xRegIn ;/**     X-bit **/
        this->ParSumIn  = rePE.ParSumIn ;/**     S-bit **/
        /** --- outward port --- **/
        this-> owPushDown  = rePE.owPushDown ;
        this->  xPushRight = rePE. xPushRight;
        this->  sPushDDown = rePE. sPushDDown;
        this->    sInDDown = rePE.   sInDDown;
        /**************************
         ** initialize the ports **
         **************************/
        this->InitializePorts();
        /*******************************************
         ** connect output ports with inner logic **
         *******************************************/
        this->OutputConnect();
        return;
    }

    inline bool OutputConnectedCorrect() const override{
        return
        /** ---------- up -------- **/
            this->owveoFIFO.SameTo(this->ports[ReformedPE::OWFullUp .ToIndex()],FIFO::Full)
        /** --------- down ------- **/
         && this->owveoFIFO.SameTo(this->ports[ReformedPE::OWOutDown.ToIndex()],FIFO::Out )
         && this->ports[ReformedPE::OWPushDown.ToIndex()].SameTo(this->owPushDown)
        /** -------- left -------- **/
         && this-> xveoFIFO.SameTo(this->ports[ReformedPE::XFullLeft.ToIndex()],FIFO::Full)
        /** -------- right ------- **/
         && this-> xveoFIFO.SameTo(this->ports[ReformedPE::XOutRight.ToIndex()],FIFO::Out )
         && this->ports[ReformedPE::XPushRight.ToIndex()].SameTo(this->xPushRight)
        /** -------- up-up ------- **/
         && this->    sFIFO.SameTo(this->ports[ReformedPE:: SFullUUp.ToIndex()],FIFO::Full)
        /** ------ down-down ----- **/
         && this->ports[ReformedPE:: SOutDDown.ToIndex()].SameTo(this->  sInDDown)
         && this->ports[ReformedPE::SPushDDown.ToIndex()].SameTo(this->sPushDDown);
    }

    inline void InnerConnect() override{
        if (!this->InputConnected()){
            for (int i=0;i<this->ports.size();i++)
                if (this->ports[i].IsInput()&&(!this->ports[i].Connected()))
                    std::cout<<i<<std::endl;
            getchar();
        }
        assert(this->InputConnected());
        /*****************
         ** stray ports **
         *****************/
//        this->pOWVEOReg0En  .Clone(this->owveoReg0En  );
//        this->pOWVEOReg1En  .Clone(this->owveoReg1En  );
//        this-> pXVEOReg0En  .Clone(this-> xveoReg0En  );
//        this-> pXVEOReg1En  .Clone(this-> xveoReg1En  );
        this->   pOWXRegEn  .Clone(this->   owxRegEn  );
        this->    pOWRegIn  .Clone(this->    owRegIn  );
        this->     pXRegIn  .Clone(this->     xRegIn  );
        this->   pParSumIn  .Clone(this->   ParSumIn  );
        this->   pParSumEn  .Clone(this->   ParSumEn  );
        this->pOWVEOFIFOPop .Clone(this->owveoFIFOPop );
        this-> pXVEOFIFOPop .Clone(this-> xveoFIFOPop );
        this->  pOWXFIFOIn  .Clone(this->  owxFIFOIn  );
        this->  pOWXFIFOPush.Clone(this->  owxFIFOPush);
        this->  pOWXFIFOPop .Clone(this->  owxFIFOPop );
        this->    pSFIFOPop .Clone(this->    sFIFOPop );
        this->     pWRegOut .Clone(this->     wRegOut );
        this->   pXEORegIn  .Clone(this->   xeoRegIn  );
        this->   pXEORegEn  .Clone(this->   xeoRegEn  );
        this->   pWEORegIn  .Clone(this->   weoRegIn  );
        this->   pWEORegEn  .Clone(this->   weoRegEn  );
        /** --- Reg --- **/
//        this->owveoReg0.ConnectTo(Reg::in,this-> owveoFIFO,FIFO::Out);
//        this->owveoReg0.ConnectTo(Reg::en,this->pOWVEOReg0En);
//        this->owveoReg0.ConnectTo(this->pOWVEOReg0Out,Reg::out);
//        this->owveoReg0.InnerConnect();
//
//        this->owveoReg1.ConnectTo(Reg::in,this-> owveoReg0,Reg::out);
//        this->owveoReg1.ConnectTo(Reg::en,this->pOWVEOReg1En);
//        this->owveoReg1.ConnectTo(this->pOWVEOReg1Out,Reg::out);
//        this->owveoReg1.InnerConnect();
//
//        this->xveoReg0.ConnectTo(Reg::in,this-> xveoFIFO,FIFO::Out);
//        this->xveoReg0.ConnectTo(Reg::en,this->pXVEOReg0En);
//        this->xveoReg0.ConnectTo(this->pXVEOReg0Out,Reg::out);
//        this->xveoReg0.InnerConnect();
//
//        this->xveoReg1.ConnectTo(Reg::in,this-> xveoReg0,Reg::out);
//        this->xveoReg1.ConnectTo(Reg::en,this->pXVEOReg1En);
//        this->xveoReg1.ConnectTo(this->pXVEOReg1Out,Reg::out);
//        this->xveoReg1.InnerConnect();

        this->owReg.ConnectTo(Reg::in,this-> pOWRegIn);
        this->owReg.ConnectTo(Reg::en,this->pOWXRegEn);
        this->owReg.ConnectTo(this->pOWRegOut,Reg::out);
        this->owReg.InnerConnect();

        this->xReg.ConnectTo(Reg::in,this->  pXRegIn);
        this->xReg.ConnectTo(Reg::en,this->pOWXRegEn);
        this->xReg.InnerConnect();

        this->ParSum.ConnectTo(Reg::in,this->pParSumIn);
        this->ParSum.ConnectTo(Reg::en,this->pParSumEn);
        this->ParSum.ConnectTo(this->pParSumOut,Reg::out);
        this->ParSum.InnerConnect();

        this->weoReg.ConnectTo(this->pWEORegOut,Reg::out);
        this->weoReg.ConnectTo(Reg::in ,this->pWEORegIn );
        this->weoReg.ConnectTo(Reg::en ,this->pWEORegEn );

        this->xeoReg.ConnectTo(this->pXEORegOut,Reg::out);
        this->xeoReg.ConnectTo(Reg::in ,this->pXEORegIn );
        this->xeoReg.ConnectTo(Reg::en ,this->pXEORegEn );

        /** --- FIFO --- **/
        this->owveoFIFO.Clone    (FIFO::In  ,this->ports[ReformedPE::OWInUp  .ToIndex()]);
        this->owveoFIFO.Clone    (FIFO::Push,this->ports[ReformedPE::OWPushUp.ToIndex()]);
        this->owveoFIFO.ConnectTo(FIFO::Pop ,this->pOWVEOFIFOPop);
        this->owveoFIFO.ConnectTo(this->pOWVEOFIFOEmpty,FIFO::Empty);
        this->owveoFIFO.ConnectTo(this->pOWVEOFIFOOut  ,FIFO::Out  );
        this->owveoFIFO.InnerConnect();

        this->xveoFIFO.Clone    (FIFO::In  ,this->ports[ReformedPE::XInLeft  .ToIndex()]);
        this->xveoFIFO.Clone    (FIFO::Push,this->ports[ReformedPE::XPushLeft.ToIndex()]);
        this->xveoFIFO.ConnectTo(FIFO::Pop ,this->pXVEOFIFOPop);
        this->xveoFIFO.ConnectTo(this->pXVEOFIFOEmpty,FIFO::Empty);
        this->xveoFIFO.ConnectTo(this->pXVEOFIFOOut  ,FIFO::Out  );
        this->xveoFIFO.InnerConnect();

        this->owxFIFO.ConnectTo(FIFO::In  ,this->pOWXFIFOIn  );
        this->owxFIFO.ConnectTo(FIFO::Push,this->pOWXFIFOPush);
        this->owxFIFO.ConnectTo(FIFO::Pop ,this->pOWXFIFOPop );
        this->owxFIFO.ConnectTo(this->pOWXFIFOOut  ,FIFO::Out  );
        this->owxFIFO.ConnectTo(this->pOWXFIFOFull ,FIFO::Full );
        this->owxFIFO.ConnectTo(this->pOWXFIFOEmpty,FIFO::Empty);
        this->owxFIFO.InnerConnect();

        this->sFIFO.Clone    (FIFO::In  ,this->ports[ReformedPE::SInUUp  .ToIndex()]);
        this->sFIFO.Clone    (FIFO::Push,this->ports[ReformedPE::SPushUUp.ToIndex()]);
        this->sFIFO.ConnectTo(FIFO::Pop ,this->pSFIFOPop   );
        this->sFIFO.ConnectTo(this->pSFIFOOut  ,FIFO::Out  );
        this->sFIFO.ConnectTo(this->pSFIFOEmpty,FIFO::Empty);
        this->sFIFO.InnerConnect();

        /** --- Multer --- **/
        this->multer.ConnectTo(Multer::in0,this->pWRegOut);
        this->multer.ConnectTo(Multer::in1,this->xReg,Reg::out);
        this->multer.ConnectTo(this->pMulterOut,Multer::out);
        this->multer.InnerConnect();

        /** --- Adder --- **/
        this->adder.ConnectTo( Adder::in0,this->multer,Multer::out);
        this->adder.ConnectTo( Adder::in1,this->ParSum,Reg   ::out);
        this->adder.ConnectTo(this->pAdderOut,Adder::out);
        this->adder.InnerConnect();

        assert(this->InnerConnected());
        return;
    }

    bool InnerConnectedCorrect() const override{
        return
        /*****************
         ** stray ports **
         *****************/
//            this->pOWVEOReg0En  .SameTo(this->owveoReg0En  )
//         && this->pOWVEOReg1En  .SameTo(this->owveoReg1En  )
//         && this-> pXVEOReg0En  .SameTo(this-> xveoReg0En  )
//         && this-> pXVEOReg1En  .SameTo(this-> xveoReg1En  )
            this->   pOWXRegEn  .SameTo(this->   owxRegEn  )
         && this->    pOWRegIn  .SameTo(this->    owRegIn  )
         && this->     pXRegIn  .SameTo(this->     xRegIn  )
         && this->   pParSumEn  .SameTo(this->   ParSumEn  )
         && this->pOWVEOFIFOPop .SameTo(this->owveoFIFOPop )
         && this-> pXVEOFIFOPop .SameTo(this-> xveoFIFOPop )
         && this->  pOWXFIFOIn  .SameTo(this->  owxFIFOIn  )
         && this->  pOWXFIFOPush.SameTo(this->  owxFIFOPush)
         && this->  pOWXFIFOPop .SameTo(this->  owxFIFOPop )
         && this->    pSFIFOPop .SameTo(this->    sFIFOPop )
         && this->     pWRegOut .SameTo(this->     wRegOut )
        /** --- Reg --- **/
//         && this->owveoReg0.ConnectedTo(Reg::in,this-> owveoFIFO,FIFO::Out)
//         && this->owveoReg0.ConnectedTo(Reg::en,this->pOWVEOReg0En)
//         && this->owveoReg0.ConnectedTo(this->pOWVEOReg0Out,Reg::out)
//         && this->owveoReg0.InnerConnectedCorrect()
//
//         && this->owveoReg1.ConnectedTo(Reg::in,this-> owveoReg0,Reg::out)
//         && this->owveoReg1.ConnectedTo(Reg::en,this->pOWVEOReg1En)
//         && this->owveoReg1.ConnectedTo(this->pOWVEOReg1Out,Reg::out)
//         && this->owveoReg1.InnerConnectedCorrect()
//
//         && this->xveoReg0.ConnectedTo(Reg::in,this-> xveoFIFO,FIFO::Out)
//         && this->xveoReg0.ConnectedTo(Reg::en,this->pXVEOReg0En)
//         && this->xveoReg0.ConnectedTo(this->pXVEOReg0Out,Reg::out)
//         && this->xveoReg0.InnerConnectedCorrect()
//
//         && this->xveoReg1.ConnectedTo(Reg::in,this-> xveoReg0,Reg::out)
//         && this->xveoReg1.ConnectedTo(Reg::en,this->pXVEOReg1En)
//         && this->xveoReg1.ConnectedTo(this->pXVEOReg1Out,Reg::out)
//         && this->xveoReg1.InnerConnectedCorrect()

         && this->owReg.ConnectedTo(Reg::in,this-> pOWRegIn)
         && this->owReg.ConnectedTo(Reg::en,this->pOWXRegEn)
         && this->owReg.ConnectedTo(this->pOWRegOut,Reg::out)
         && this->owReg.InnerConnectedCorrect()

         && this->xReg.ConnectedTo(Reg::in,this->  pXRegIn)
         && this->xReg.ConnectedTo(Reg::en,this->pOWXRegEn)
         && this->xReg.InnerConnectedCorrect()

         && this->ParSum.ConnectedTo(Reg::in,this->adder,Adder::out)
         && this->ParSum.ConnectedTo(Reg::en,this->pParSumEn)
         && this->ParSum.ConnectedTo(this->pParSumOut,Reg::out)
         && this->ParSum.InnerConnectedCorrect()

        /** --- FIFO --- **/
         && this->owveoFIFO.SameTo     (FIFO::In  ,this->ports[ReformedPE::OWInUp  .ToIndex()])
         && this->owveoFIFO.SameTo     (FIFO::Push,this->ports[ReformedPE::OWPushUp.ToIndex()])
         && this->owveoFIFO.ConnectedTo(FIFO::Pop ,this->pOWVEOFIFOPop)
         && this->owveoFIFO.ConnectedTo(this->pOWVEOFIFOEmpty,FIFO::Empty)
         && this->owveoFIFO.InnerConnectedCorrect()

         && this->xveoFIFO.SameTo     (FIFO::In  ,this->ports[ReformedPE::XInLeft  .ToIndex()])
         && this->xveoFIFO.SameTo     (FIFO::Push,this->ports[ReformedPE::XPushLeft.ToIndex()])
         && this->xveoFIFO.ConnectedTo(FIFO::Pop ,this->pXVEOFIFOPop)
         && this->xveoFIFO.ConnectedTo(this->pXVEOFIFOEmpty,FIFO::Empty)
         && this->xveoFIFO.InnerConnectedCorrect()

         && this->owxFIFO.ConnectedTo(FIFO::In  ,this->pOWXFIFOIn  )
         && this->owxFIFO.ConnectedTo(FIFO::Push,this->pOWXFIFOPush)
         && this->owxFIFO.ConnectedTo(FIFO::Pop ,this->pOWXFIFOPop )
         && this->owxFIFO.ConnectedTo(this->pOWXFIFOOut  ,FIFO::Out  )
         && this->owxFIFO.ConnectedTo(this->pOWXFIFOFull ,FIFO::Full )
         && this->owxFIFO.ConnectedTo(this->pOWXFIFOEmpty,FIFO::Empty)
         && this->owxFIFO.InnerConnectedCorrect()

         && this->sFIFO.SameTo     (FIFO::In  ,this->ports[ReformedPE::SInUUp  .ToIndex()])
         && this->sFIFO.SameTo     (FIFO::Push,this->ports[ReformedPE::SPushUUp.ToIndex()])
         && this->sFIFO.ConnectedTo(FIFO::Pop ,this->pSFIFOPop   )
         && this->sFIFO.ConnectedTo(this->pSFIFOOut  ,FIFO::Out  )
         && this->sFIFO.ConnectedTo(this->pSFIFOEmpty,FIFO::Empty)
         && this->sFIFO.InnerConnectedCorrect()

        /** --- Multer --- **/
         && this->multer.ConnectedTo(Multer::in0,this->pWRegOut)
         && this->multer.ConnectedTo(Multer::in1,this->xReg,Reg::out)
         && this->multer.ConnectedTo(this->pMulterOut,Multer::out)
         && this->multer.InnerConnectedCorrect()

        /** --- Adder --- **/
         && this->adder.ConnectedTo( Adder::in0,this->multer,Multer::out)
         && this->adder.ConnectedTo( Adder::in1,this->ParSum,Reg   ::out)
         && this->adder.ConnectedTo(this->pAdderOut,Adder::out)
         && this->adder.InnerConnectedCorrect();
    }

    inline bool atPosEdge() override{
        if (this->clock % this->macPer == 0)
            this->owxFIFOPop = this->_owxFIFOPop;
        else
            this->owxFIFOPop = 0;

        if (this->clock % this->ruPer == 0)
            this->owxFIFOPush = this->_owxFIFOPush;
        else
            this->owxFIFOPush = 0;
//            this->owxFIFOPop = this->_owxFIFOPop;

        if (this->clock % this->ruPer == 0){
            this->  owxFIFO.atPosEdge();
            this->owveoFIFO.atPosEdge();
            this-> xveoFIFO.atPosEdge();
            this->    sFIFO.atPosEdge();
            this->   weoReg.atPosEdge();
            this->   xeoReg.atPosEdge();
//            owveoReg0.atPosEdge();/** O+W+Off+E-bit **/
//            owveoReg1.atPosEdge();/** O+W+Off+E-bit **/
//             xveoReg0.atPosEdge();/**   X+Off+E-bit **/
//             xveoReg1.atPosEdge();/**   X+Off+E-bit **/
        }

        if (this->clock % this->macPer == 0){
            if (this->clock % this->ruPer != 0)
                owxFIFO.atPosEdge();
               owReg .atPosEdge();/**       O+W-bit **/
                xReg .atPosEdge();/**         X-bit **/
              ParSum .atPosEdge();/**         S-bit **/
        }
        return true;
    }

    inline bool atNegEdge() override{
        assert(false);
        return false;
    }

    inline bool InnerConnected() const override{
        return  this->  owxFIFO .Connected() /**       O+W+X-bit **/
             && this->owveoFIFO .Connected() /** O+W+V+Off+E-bit **/
             && this-> xveoFIFO .Connected() /**   X+V+Off+E-bit **/
             && this->    sFIFO .Connected() /**         S+E-bit **/
//             && this-> owveoReg0.Connected() /** O+W+V+Off+E-bit **/
//             && this-> owveoReg1.Connected() /** O+W+V+Off+E-bit **/
//             && this->  xveoReg0.Connected() /**   X+V+Off+E-bit **/
//             && this->  xveoReg1.Connected() /**   X+V+Off+E-bit **/
             && this->    owReg .Connected() /**         O+W-bit **/
             && this->     xReg .Connected() /**           X-bit **/
             && this->    weoReg.Connected()
             && this->    xeoReg.Connected()
             && this->   ParSum .Connected();/**           S-bit **/
    }

    inline bool Run() override{
        if (this->clock % this->ruPer == 0){
            this->    sFIFO.Run();
            this->  owxFIFO.Run();
            this->owRegIn = ReformedPE::DecompressOWX_OW(this->pOWXFIFOOut.Data());
            this-> xRegIn = ReformedPE::DecompressOWX_X (this->pOWXFIFOOut.Data());
            this->owveoFIFO.Run();
            this-> xveoFIFO.Run();
            this->weoRegIn = ReformedPE::CompressWEO(
                                ReformedPE::DecompressOWVEO_OEO(this->pOWVEOFIFOOut.Data()),
                                (1-this->pOWVEOFIFOEmpty.Data()));
            this->xeoRegIn = ReformedPE::CompressXEO(
                                ReformedPE:: DecompressXVEO_EO (this-> pXVEOFIFOOut.Data()),
                                (1-this-> pXVEOFIFOEmpty.Data()));
            this->weoReg.Run();
            this->xeoReg.Run();
            this->owxFIFOIn = ReformedPE::CompressOWX(
                    ReformedPE::DecompressOWVEO_OW(this->pOWVEOFIFOOut.Data()),
                    ReformedPE::DecompressXVEO_X  (this-> pXVEOFIFOOut.Data())
                );

            /***************************
             *   control logic of RU   *
             ***************************/
            switch(this->NowState()){
            case RU_STATE::W_EQUAL_X:
                if(!this->owveoFIFODownIsFull()
                && !this-> xveoFIFORightIsFull()
                && !this->  owxFIFOIsFull()){
                    this-> WPush();
                    this-> XPush();
                    this->Output();
                }
                else
                    this->Stuck();
                break;
            case RU_STATE::X_LESS_W:
                if(!this->xveoFIFORightIsFull()){
                    this-> WNotPush();
                    this->    XPush();
                    this->NotOutput();
                }
                else
                    this->Stuck();
                break;
            case RU_STATE::W_LESS_X:
                if (this->owIsOut()){
                    if(!this->owveoFIFODownIsFull()
                    && !this->owxFIFOIsFull()){
                        this->    WPush();
                        this-> XNotPush();
                        this->OutputOut();
                    }
                    else
                        this->Stuck();
                }
                else{
                    if(!this->owveoFIFODownIsFull()){
                        this->    WPush();
                        this-> XNotPush();
                        this->NotOutput();
                    }
                    else
                        this->Stuck();
                }
                break;
            case RU_STATE::X_END:
                /**  X reached end-of-block and is used
                 **  i.e.
                 **     X reached end-of-block while the
                 **     offset of X is less than offset of W
                 **/
                if(!this->owveoFIFODownIsFull()){
                    assert(!this->owIsOut());
                    this->    WPush();
                    this-> XNotPush();
                    this->NotOutput();
                }
                else
                    this->Stuck();
                break;
            case RU_STATE::W_END:
                /**  W reached end-of-block and is used
                 **  i.e.
                 **     W reached end-of-block while the
                 **     offset of W is less than offset of X
                 **/
                if(!this->xveoFIFORightIsFull()){
                    this-> WNotPush();
                    this->    XPush();
                    this->NotOutput();
                }
                else
                    this->Stuck();
                break;
            case RU_STATE::WX_END:
                /**  both X and W reached end-of-block and are both used
                 **/
                if (this->owIsOut()){
                    if(!this->owveoFIFODownIsFull()
                    && !this->xveoFIFORightIsFull()
                    && !this->owxFIFOIsFull()){
                        this->    WPush();
                        this->    XPush();
                        this->OutputOut();
                    }
                    else
                        this->Stuck();
                }
                else{
                    if(!this->owveoFIFODownIsFull()
                    && !this->xveoFIFORightIsFull()){
                        this->    WPush();
                        this->    XPush();
                        this->NotOutput();
                    }
                    else
                        this->Stuck();
                }
                break;
            case RU_STATE::NOT_READY:
                this->Stuck();
                break;
            default:
                assert(false);
            }

            if (!this->owxFIFOIsEmpty()){
                if (ReformedPE::DecompressOW_O(this->pOWRegOut.Data())==0)
                    this->hasSentParSum = false;
                if (this->owNotOut()){
                    if (this->NeedForward()){
                        if (this->CanForwardRec())
                            this->ForwardRecSum();
                        else
                            this->NotForward();
                    }
                    else
                        this->NotForward();
                    if (ReformedPE::DecompressOW_O(this->pOWRegOut.Data())==0)
                        this->ParSumWriteAddOut();
                    else
                        this->ParSumWriteZero();
                    this->owxRegWrite();
                }
                else{
                    if (this->EndForward()){
                        if (this->CanForward()){
                            this->ForwardAddOut();
                            this->ParSumWriteZero();
                            this->owxRegWrite();
                        }
                        else{
                            this->NotForward();
                            this->ParSumNotWrite();
                            this->owxRegNotWrite();
                        }
                    }
                    else{
                        if (this->CanForwardRec())
                            this->ForwardRecSum();
                        else
                            this->NotForward();
                        this->ParSumNotWrite();
                        this->owxRegNotWrite();
                    }
                }
            }
            else{
                if (this->NeedForward()){
                    if (this->CanForward())
                        this->ForwardRecSum();
                    else
                        this->NotForward();
                }
                else
                    this->NotForward();
                this->ParSumNotWrite();
                this->owxRegNotWrite();
            }
        }
        if (this->clock % this->macPer == 0){
             owReg .Run(); /** O+W-bit **/
            this-> wRegOut= ReformedPE::DecompressOW_W  (this->pOWRegOut  .Data());
              xReg .Run(); /**   X-bit **/
            ParSum .Run(); /**   S-bit **/
             multer.Run();
              adder.Run();
        }
        this->clock++;
        return false;
    }

    inline void PrintX (FILE* fp) const{
        this->xReg.PrintTo(fp);
        return;
    }
    inline void PrintOW(FILE* fp) const{
        this->owReg.PrintTo(fp);
        return;
    }
    inline void PrintP(FILE* fp) const{
        this->ParSum.PrintTo(fp);
        return;
    }
    inline void PrintFIFOState() const{
        Port pOWFullUp(1,PortDirection::IN);
        this->owveoFIFO.ConnectTo(pOWFullUp,FIFO::Full);
        if (pOWFullUp.Data()==1)
            std::cout<<"(F,";
        else if (this->owveoFIFOIsEmpty())
            std::cout<<"(E,";
        else
            std::cout<<"(0,";

        Port pXFullLeft(1,PortDirection::IN);
        this->xveoFIFO.ConnectTo(pXFullLeft,FIFO::Full);
        if (pXFullLeft.Data()==1)
            std::cout<<"F)";
        else if (this->xveoFIFOIsEmpty())
            std::cout<<"E)";
        else
            std::cout<<"0)";

//        if (this->pOWXFIFOFull.Data()==1)
//            std::cout<<"F)";
//        else if (this->pOWXFIFOEmpty.Data())
//            std::cout<<"E)";
//        else
//            std::cout<<"0)";
        return;
    }
    inline void Print(){
        /**   up  **/
        Port pOWFullUp(1,PortDirection::IN);
        this->owveoFIFO.ConnectTo(pOWFullUp,FIFO::Full);
        std::cout<<"OWInUp  : "<<this->ports[ReformedPE::  OWInUp.ToIndex()].Data()<<std::endl;
        std::cout<<"OWPushUp: "<<this->ports[ReformedPE::OWPushUp.ToIndex()].Data()<<std::endl;
        std::cout<<"OWFullUp: "<<pOWFullUp.Data()<<std::endl;
        /**  down **/
        Port pOWOutDown(OUT_BIT_WIDTH
                   +      W_BIT_WIDTH
                   +  VALID_BIT_WIDTH
                   + OFFSET_BIT_WIDTH
                   +     XE_BIT_WIDTH,PortDirection::IN);
        this->owveoFIFO.ConnectTo(pOWOutDown,FIFO::Out);
        std::cout<<"OWOutDown : "<<pOWOutDown.Data()<<std::endl;
        std::cout<<"OWPushDown: "<<this->owPushDown<<std::endl;
        std::cout<<"OWFullDown: "<<this->ports[ReformedPE::OWFullDown.ToIndex()].Data()<<std::endl;
        /**  left **/
        Port pXFullLeft(1,PortDirection::IN);
        this->xveoFIFO.ConnectTo(pXFullLeft,FIFO::Full);
        std::cout<<"XInLeft  : "<<this->ports[ReformedPE::  XInLeft.ToIndex()].Data()<<std::endl;
        std::cout<<"XPushLeft: "<<this->ports[ReformedPE::XPushLeft.ToIndex()].Data()<<std::endl;
        std::cout<<"XFullLeft: "<<pXFullLeft.Data()<<std::endl;
        /** right **/
        Port pXOutRight( X_BIT_WIDTH
                  +  VALID_BIT_WIDTH
                  + OFFSET_BIT_WIDTH
                  +     XE_BIT_WIDTH,PortDirection::IN);
        this->xveoFIFO.ConnectTo(pXOutRight,FIFO::Out);
        std::cout<<"XOutRight : "<<pXOutRight.Data()<<std::endl;
        std::cout<<"XPushRight: "<<this->xPushRight<<std::endl;
        std::cout<<"XFullRight: "<<this->ports[ReformedPE::XFullRight.ToIndex()].Data()<<std::endl;
        /**  upup  **/
        Port pSFullUUp(1,PortDirection::IN);
        this->sFIFO.ConnectTo(pSFullUUp,FIFO::Full);
        std::cout<<"  SInUpUp : "<<this->ports[ReformedPE::  SInUUp.ToIndex()].Data()<<std::endl;
        std::cout<<"SPushUpUp : "<<this->ports[ReformedPE::SPushUUp.ToIndex()].Data()<<std::endl;
        std::cout<<"SFullUpUp : "<<pSFullUUp.Data()<<std::endl;
        /** ddown **/
        std::cout<<" SOutDDown: "<<this->sInDDown<<std::endl;
        std::cout<<"SPushDDown: "<<this->sPushDDown<<std::endl;
        std::cout<<"SFullDDown: "<<this->ports[ReformedPE::SFullDDown.ToIndex()].Data()<<std::endl;

        std::cout<<"##  owReg ##"<<std::endl;
        this->owReg .Print();
        std::cout<<"##   xReg ##"<<std::endl;
        this->xReg  .Print();
        std::cout<<"## ParSum ##"<<std::endl;
        this->ParSum.Print();
//        std::cout<<"## owveoReg0 ##"<<std::endl;
//        this->owveoReg0.Print();
//        std::cout<<"## owveoReg1 ##"<<std::endl;
//        this->owveoReg1.Print();
//        std::cout<<"## xveoReg0 ##"<<std::endl;
//        this->xveoReg0.Print();
//        std::cout<<"## xveoReg1 ##"<<std::endl;
//        this->xveoReg1.Print();
        std::cout<<"## owveoFIFO ##"<<std::endl;
        this->owveoFIFO.Print();
        std::cout<<"##  xveoFIFO ##"<<std::endl;
        this-> xveoFIFO.Print();
        std::cout<<"##     sFIFO ##"<<std::endl;
        this->    sFIFO.Print();
        std::cout<<"##   owxFIFO ##"<<std::endl;
        this->  owxFIFO.Print();
        std::cout<<"##   _owxFIFOPop   ##"<<std::endl;
        std::cout<<this->_owxFIFOPop<<std::endl;
        std::cout<<"##   _owxFIFOPush   ##"<<std::endl;
        std::cout<<this->_owxFIFOPush<<std::endl;
        std::cout<<"##    hasRecOut    ##"<<std::endl;
        std::cout<<(this->hasRecOut?1:0)<<std::endl;
        std::cout<<"## Adder ##"<<std::endl;
        this->adder.Print();
        std::cout<<"## Multer ##"<<std::endl;
        this->multer.Print();
    }
};

#endif // __COMPONENT_REFORMED_PE_HPP

//            switch(this->ruState){
//            case RU_STATE::WX_END:
//                if (this->outReg1()){
//                    if(!this->owveoFIFOIsEmpty()
//                    && !this->owveoFIFODownIsFull()
//                    && !this-> xveoFIFOIsEmpty()
//                    && !this-> xveoFIFORightIsFull()
//                    && !this->  owxFIFOIsFull()){
//                        this->    WPush();
//                        this->    XPush();
//                        this->OutputOut();
//                        this->SetNextState(
//                            this-> wlReg0(),
//                            this->eowReg0(),
//                            this-> xlReg0(),
//                            this->eoxReg0()
//                        );
//                    }
//                    else
//                         this->Stuck();
//                }
//                else{
//                    if(!this->owveoFIFOIsEmpty()
//                    && !this->owveoFIFODownIsFull()
//                    && !this-> xveoFIFOIsEmpty()
//                    && !this-> xveoFIFORightIsFull()){
//                        this->    WPush();
//                        this->    XPush();
//                        this->NotOutput();
//                        this->SetNextState(
//                            this-> wlReg0(),
//                            this->eowReg0(),
//                            this-> xlReg0(),
//                            this->eoxReg0()
//                        );
//                    }
//                    else
//                         this->Stuck();
//                }
//                break;
//            case RU_STATE::WX_OX_END:
//                if(!this->owveoFIFOIsEmpty()
//                && !this-> xveoFIFOIsEmpty()
//                && !this->owveoFIFODownIsFull()
//                && !this-> xveoFIFORightIsFull()
//                && !this->  owxFIFOIsFull ()){
//                    this-> WPush();
//                    this-> XPush();
//                    this->Output();
//                    this->SetNextState(
//                        this-> wlReg0(),
//                        this->eowReg0(),
//                        this-> xlReg0(),
//                        this->eoxReg0()
//                    );
//                }
//                else
//                     this->Stuck();
//                break;
//            case RU_STATE::WL_LESS_XL:
//                if(!this->owveoFIFOIsEmpty()
//                && !this->owveoFIFODownIsFull()){
//                    this->    WPush();
//                    this-> XNotPush();
//                    this->NotOutput();
//                    this->SetNextState(
//                        this-> wlReg0(),
//                        this->eowReg0(),
//                        this-> xlReg1(),
//                        this->eoxReg1()
//                    );
//                }
//                else
//                     this->Stuck();
//                break;
//            case RU_STATE::WL_EQUAL_XL:
//                if(!this->owveoFIFOIsEmpty()
//                && !this-> xveoFIFOIsEmpty()
//                && !this->owveoFIFODownIsFull()
//                && !this-> xveoFIFORightIsFull()
//                && !this->  owxFIFOIsFull ()){
//                    this-> WPush();
//                    this-> XPush();
//                    this->Output();
//                    this->SetNextState(
//                        this-> wlReg0(),
//                        this->eowReg0(),
//                        this-> xlReg0(),
//                        this->eoxReg0()
//                    );
//                }
//                else
//                    this->Stuck();
//                break;
//            case RU_STATE::WL_BIGGER_XL:
//                if(!this->xveoFIFOIsEmpty()
//                && !this->xveoFIFORightIsFull()){
//                    this-> WNotPush();
//                    this->    XPush();
//                    this->NotOutput();
//                    this->SetNextState(
//                        this-> wlReg1(),
//                        this->eowReg1(),
//                        this-> xlReg0(),
//                        this->eoxReg0()
//                    );
//                }
//                else
//                    this->Stuck();
//                break;
//            case RU_STATE::WL_END:
//                if(!this->xveoFIFOIsEmpty()
//                && !this->xveoFIFORightIsFull()){
//                    this-> WNotPush();
//                    this->    XPush();
//                    this->NotOutput();
//                    this->SetNextState(
//                        this-> wlReg1(),
//                        this->eowReg1(),
//                        this-> xlReg0(),
//                        this->eoxReg0()
//                    );
//                }
//                else
//                     this->Stuck();
//                break;
//            case RU_STATE::WL_OX_END:
//                if(!this->xveoFIFOIsEmpty()
//                && !this->xveoFIFORightIsFull()
//                && !this-> owxFIFOIsFull ()){
//                    this->WNotPush();
//                    this->   XPush();
//                    this->  Output();
//                    this->SetNextState(
//                        this-> wlReg1(),
//                        this->eowReg1(),
//                        this-> xlReg0(),
//                        this->eoxReg0()
//                    );
//                }
//                else
//                     this->Stuck();
//                break;
//            case RU_STATE::XL_END:/// x reach the end of group
//                if(!this->owveoFIFOIsEmpty()
//                && !this->owveoFIFODownIsFull()){
//                    this->    WPush();
//                    this-> XNotPush();
//                    this->NotOutput();
//                    this->SetNextState(
//                        this-> wlReg0(),
//                        this->eowReg0(),
//                        this-> xlReg1(),
//                        this->eoxReg1()
//                    );
//                }
//                else
//                    this->Stuck();
//                break;
//            case RU_STATE::XL_OX_END:/// x reach the end of group
//                if(!this->owveoFIFOIsEmpty()
//                && !this->  owxFIFOIsFull ()
//                && !this->owveoFIFODownIsFull()){
//                    this->   WPush();
//                    this->XNotPush();
//                    this->  Output();
//                    this->SetNextState(
//                        this-> wlReg0(),
//                        this->eowReg0(),
//                        this-> xlReg1(),
//                        this->eoxReg1()
//                    );
//                }
//                else
//                    this->Stuck();
//                break;
//            default:
//                assert(false);
//            }
