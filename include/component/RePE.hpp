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
          OFFSET_BIT_WIDTH =  4;///group number == 16
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
              wFIFO_LEN = -1,
              xFIFO_LEN = -1,
            owxFIFO_LEN = -1,
              sFIFO_LEN =  1;

    /** Compress methods **/
    static inline PortDataType CompressOWEO(
            PortDataType out,
            PortDataType weight,
            PortDataType eow,
            PortDataType offset
        ){
        assert(((out    & ((Object::extend[   OUT_BIT_WIDTH])<<1))==0)
            && ((weight & ((Object::extend[     W_BIT_WIDTH])<<1))==0)
            && ((eow    & ((Object::extend[    WE_BIT_WIDTH])<<1))==0)
            && ((offset & ((Object::extend[OFFSET_BIT_WIDTH])<<1))==0));
        return (out    <<(OFFSET_BIT_WIDTH + WE_BIT_WIDTH + W_BIT_WIDTH))
             + (weight <<(OFFSET_BIT_WIDTH + WE_BIT_WIDTH))
             + (eow    <<(OFFSET_BIT_WIDTH))
             + (offset);
    }
    static inline PortDataType CompressXEO(
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
    static inline PortDataType CompressOWEOV(
            PortDataType oweo,
            PortDataType valid
        ){
        assert(((oweo  & ((Object::extend[  OUT_BIT_WIDTH
                                      +       W_BIT_WIDTH
                                      +      WE_BIT_WIDTH
                                      +  OFFSET_BIT_WIDTH])<<1))==0)
            && ((valid & ((Object::extend[VALID_BIT_WIDTH])<<1))==0));
        return (oweo<<(VALID_BIT_WIDTH))
              + valid;
    }
    static inline PortDataType CompressXEOV(
            PortDataType xeo,
            PortDataType valid
        ){
        assert(((xeo   & ((Object::extend[    X_BIT_WIDTH
                                      +      XE_BIT_WIDTH
                                      +  OFFSET_BIT_WIDTH])<<1))==0)
            && ((valid & ((Object::extend[VALID_BIT_WIDTH])<<1))==0));
        return (xeo<<(VALID_BIT_WIDTH))
              + valid;
    }

    /**  Decompress methods **/
    /** - Decompress oweo - **/
    static inline PortDataType DecompressOWEO_Out   (PortDataType owveo){
        assert((owveo & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo >> (OFFSET_BIT_WIDTH + WE_BIT_WIDTH + W_BIT_WIDTH))
               & Object::mask[OUT_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWEO_W     (PortDataType owveo){
        assert((owveo & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo >> (OFFSET_BIT_WIDTH + WE_BIT_WIDTH))
             & Object::mask[W_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWEO_EOW   (PortDataType owveo){
        assert((owveo & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo >> (OFFSET_BIT_WIDTH))
               & Object::mask[WE_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWEO_Offset(PortDataType owveo){
        assert((owveo & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo)
               & Object::mask[OFFSET_BIT_WIDTH];
    }
    static inline PortDataType DecompressOWEO_OW    (PortDataType owveo){
        assert((owveo & ((Object::extend[  W_BIT_WIDTH
                                    +    OUT_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH])<<1))==0);
        return (owveo >> (OFFSET_BIT_WIDTH + WE_BIT_WIDTH))
             & Object::mask[W_BIT_WIDTH + OUT_BIT_WIDTH];
    }
    /** - Decompress xeo - **/
    static inline PortDataType DecompressXEO_X     (PortDataType xexl){
        assert((xexl & ((Object::extend[     X_BIT_WIDTH
                                      +     XE_BIT_WIDTH
                                      + OFFSET_BIT_WIDTH])<<1))==0);
        return (xexl>>(OFFSET_BIT_WIDTH + XE_BIT_WIDTH))
               & Object::mask[X_BIT_WIDTH];
    }
    static inline PortDataType DecompressXEO_EOX   (PortDataType xexl){
        assert((xexl & ((Object::extend[     X_BIT_WIDTH
                                      +     XE_BIT_WIDTH
                                      + OFFSET_BIT_WIDTH])<<1))==0);
        return (xexl>>(OFFSET_BIT_WIDTH))
               & Object::mask[XE_BIT_WIDTH];
    }
    static inline PortDataType DecompressXEO_Offset(PortDataType xexl){
        assert((xexl & ((Object::extend[     X_BIT_WIDTH
                                      +     XE_BIT_WIDTH
                                      + OFFSET_BIT_WIDTH])<<1))==0);
        return (xexl & Object::mask[OFFSET_BIT_WIDTH]);
    }
    static inline PortDataType DecompressXEO_EO    (PortDataType xexl){
        assert((xexl & ((Object::extend[     X_BIT_WIDTH
                                      +     XE_BIT_WIDTH
                                      + OFFSET_BIT_WIDTH])<<1))==0);
        return (xexl & Object::mask[XE_BIT_WIDTH
                              + OFFSET_BIT_WIDTH]);
    }
    /** - Decompress ow - **/
    static inline PortDataType DecompressOW_O (PortDataType ow){
        assert((ow & ((Object::extend[  W_BIT_WIDTH
                                    + OUT_BIT_WIDTH])<<1))==0);
        return (ow>>(W_BIT_WIDTH)) & Object::mask[OUT_BIT_WIDTH];
    }
    static inline PortDataType DecompressOW_W (PortDataType ow){
        assert((ow & ((Object::extend[  W_BIT_WIDTH
                                    + OUT_BIT_WIDTH])<<1))==0);
        return (ow & Object::mask[W_BIT_WIDTH]);
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
    /** - Decompress oweov - **/
    static inline PortDataType DecompressOWEOV_OW   (PortDataType oweov){
        assert((oweov & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return ((oweov>>( WE_BIT_WIDTH
                   +  OFFSET_BIT_WIDTH
                   +   VALID_BIT_WIDTH)) & Object::mask[W_BIT_WIDTH
                                                   +  OUT_BIT_WIDTH]);
    }
    static inline PortDataType DecompressOWEOV_Out (PortDataType oweov){
        assert((oweov & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return ((oweov>>(   W_BIT_WIDTH
                    +      WE_BIT_WIDTH
                    +  OFFSET_BIT_WIDTH
                    +   VALID_BIT_WIDTH)) & Object::mask[OUT_BIT_WIDTH]);
    }
    static inline PortDataType DecompressOWEOV_W  (PortDataType oweov){
        assert((oweov & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return ((oweov>>(    WE_BIT_WIDTH
                      +  OFFSET_BIT_WIDTH
                      +   VALID_BIT_WIDTH)) & Object::mask[W_BIT_WIDTH]);
    }
    static inline PortDataType DecompressOWEOV_EOW  (PortDataType oweov){
        assert((oweov & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return ((oweov>>(OFFSET_BIT_WIDTH
                      +   VALID_BIT_WIDTH)) & Object::mask[WE_BIT_WIDTH]);
    }
    static inline PortDataType DecompressOWEOV_Off  (PortDataType oweov){
        assert((oweov & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return ((oweov>>(VALID_BIT_WIDTH)) & Object::mask[OFFSET_BIT_WIDTH]);
    }
    static inline PortDataType DecompressOWEOV_Valid(PortDataType oweov){
        assert((oweov & ((Object::extend[OUT_BIT_WIDTH
                                    +      W_BIT_WIDTH
                                    +     WE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return  (oweov & Object::mask[ VALID_BIT_WIDTH]);
    }
    /** - Decompress xeov - **/
    static inline PortDataType DecompressXEOV_X    (PortDataType xeov){
        assert ((xeov & ((Object::extend[  X_BIT_WIDTH
                                    +     XE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return ((xeov>>(    XE_BIT_WIDTH
                     +  OFFSET_BIT_WIDTH
                     +   VALID_BIT_WIDTH)) & Object::mask[X_BIT_WIDTH]);
    }
    static inline PortDataType DecompressXEOV_EOX  (PortDataType xeov){
        assert ((xeov & ((Object::extend[  X_BIT_WIDTH
                                    +     XE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return ((xeov>>(OFFSET_BIT_WIDTH
                     +   VALID_BIT_WIDTH)) & Object::mask[XE_BIT_WIDTH]);
    }
    static inline PortDataType DecompressXEOV_Off  (PortDataType xeov){
        assert ((xeov & ((Object::extend[  X_BIT_WIDTH
                                    +     XE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return ((xeov>>( VALID_BIT_WIDTH)) & Object::mask[OFFSET_BIT_WIDTH]);
    }
    static inline PortDataType DecompressXEOV_Valid(PortDataType xeov){
        assert ((xeov & ((Object::extend[  X_BIT_WIDTH
                                    +     XE_BIT_WIDTH
                                    + OFFSET_BIT_WIDTH
                                    +  VALID_BIT_WIDTH])<<1))==0);
        return  (xeov & Object::mask[ VALID_BIT_WIDTH]);
    }

    typedef uint32_t ClockFeqType;
    typedef uint32_t CycleCountType;

private:
    enum class RU_STATE{
        W_EQUAL_X,
        W_EQUAL_X_WE,
        W_EQUAL_X_XE,
        W_LESS_X ,
        X_LESS_W ,
        W_END    , ///       w  reach the end of group
        X_END    , ///       x  reach the end of group
        WX_END   , /// both w&x reach the end of group
        NOT_READY
    };

    /** RU state **/
    bool lastStuck;

    /** MAC state **/
    bool hasRecEOF,
         hasRecOut;
    bool hasSentParSum;


    FIFO owxFIFO,/**       O+W+X-bit **/
       oweovFIFO,/**   O+W+Off+E-bit **/
        xeovFIFO,/**     X+Off+E-bit **/
           sFIFO;/**         S+E-bit **/

    Reg  owReg  , /**         O+W-bit **/
          xReg  , /**           X-bit **/
        ParSum  , /**           S-bit **/
        oweovReg, /** W+O+E+Off+V-bit **/
         xeovReg; /**   X+E+Off+V-bit **/

    Comparer comp;

    Port pOWEOVFIFOEmpty, /**           1-bit **/
         pOWEOVFIFOOut,   /**   O+W+Off+E-bit **/
          pXVEOFIFOEmpty, /**           1-bit **/
          pXVEOFIFOOut,   /**     X+Off+E-bit **/
           pOWXFIFOOut,   /**       O+W+X-bit **/
           pOWXFIFOFull,  /**           1-bit **/
           pOWXFIFOEmpty, /**           1-bit **/
             pSFIFOOut,   /**         S+E-bit **/
             pSFIFOEmpty, /**           1-bit **/
             pOWRegOut,   /**         O+W-bit **/
             pAdderOut,   /**           S-bit **/
            pParSumOut,   /**           S-bit **/
              pWOffOut,   /**         Off-bit **/
              pXOffOut,   /**         Off-bit **/
            pMulterOut,   /**         X+W-bit **/
          pOWEOVRegOut,   /** W+O+Off+E+V-bit **/
           pXEOVRegOut;   /**   X+Off+E+V-bit **/

     /** --- inner control --- **/
    Port    pOWXRegEn,
             pOWRegIn,
              pWRegOut,
              pXRegIn,
            pParSumIn,
            pParSumEn,
         pOWEOVFIFOPop,
          pXVEOFIFOPop,
           pOWXFIFOIn,
           pOWXFIFOPush,
           pOWXFIFOPop,
              pCompOut,
             pSFIFOPop,
          pOWEOVRegIn,  /**     O-Off-E-bit **/
          pOWEOVRegEn,  /**           1-bit **/
           pXEOVRegIn,   /**       Off-E-bit **/
           pXEOVRegEn;   /**           1-bit **/

    PortDataType
    /** --- RU inner control --- **/
        oweovFIFOPop,
         xeovFIFOPop,
          owxFIFOPush,
         _owxFIFOPush,
    /** --- RU inner data --- **/
           oweovRegIn, /** O-Off-E-bit **/
           oweovRegEn, /**       1-bit **/
            xeovRegIn, /**   Off-E-bit **/
            xeovRegEn, /**       1-bit **/
            owxFIFOIn, /**   O+W+X-bit **/
               wOffOut,/**     Off-bit **/
               xOffOut,/**     Off-bit **/
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

    const ClockFeqType  ruPer,
                       macPer;
    const bool receiveS;

    CycleCountType clock;

    /**    RU action   **/
    /** action handles **/
    inline void Stuck(){
        this->oweovFIFOPop = 0; /** oweovFIFO    1-bit **/
        this->oweovRegEn   = 0;
        this->owPushDown   = 0; /** down PE      1-bit **/

        this-> xeovFIFOPop = 0; /**  xeovFIFO    1-bit **/
        this-> xeovRegEn   = 0;
        this-> xPushRight  = 0; /**  right PE    1-bit **/

        this->_owxFIFOPush = 0; /**   owxFIFO    1-bit **/

        this->lastStuck = true;
        return;
    }
    inline void WPush(){
        this->oweovRegEn   = 1;
        this->oweovFIFOPop = 1; /** oweovFIFO    1-bit **/
        this->owPushDown   = 1; /** down PE      1-bit **/
        this->oweovReg.ReadBits(W_BIT_WIDTH
                             + WE_BIT_WIDTH);
        return;
    }
    inline void WNotPush(){
        this->oweovRegEn   = 0;
        this->oweovFIFOPop = 0; /** oweovFIFO    1-bit **/
        this->owPushDown   = 0; /** down PE      1-bit **/
        return;
    }
    inline void XPush(){
        this->xeovRegEn   = 1;
        this->xeovFIFOPop = 1; /**  xeovFIFO    1-bit **/
        this->xPushRight  = 1; /**  right PE    1-bit **/
        this->xeovReg.ReadBits(X_BIT_WIDTH
                            + XE_BIT_WIDTH);
        return;
    }
    inline void XNotPush(){
        this->xeovRegEn   = 0;
        this->xeovFIFOPop = 0; /**  xeovFIFO    1-bit **/
        this->xPushRight  = 0; /**  right PE    1-bit **/
        return;
    }
    inline void Output(){
        this->_owxFIFOPush =
          (this->oweovRegValid()
         &&this-> xeovRegValid())?1:0;
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
    inline bool oweovFIFOIsEmpty() const{
        return (this->pOWEOVFIFOEmpty.Data()==1);
    }
    inline bool  xeovFIFOIsEmpty() const{
        return (this-> pXVEOFIFOEmpty.Data()==1);
    }
    inline bool   owxFIFOIsFull () const{
        return (this->   pOWXFIFOFull.Data()==1);
    }
    inline bool oweovFIFODownIsFull() const{
        return (this->ports[ReformedPE::OWFullDown.ToIndex()].Data()==1);
    }
    inline bool oweovRegNotValid() const{
        return (ReformedPE::DecompressOWEOV_Valid(this->pOWEOVRegOut.Data())==0);
    }
    inline bool oweovRegValid() const{
        return (ReformedPE::DecompressOWEOV_Valid(this->pOWEOVRegOut.Data())==1);
    }
    inline bool xeovRegNotValid() const{
        return (ReformedPE:: DecompressXEOV_Valid(this-> pXEOVRegOut.Data())==0);
    }
    inline bool xeovRegValid() const{
        return (ReformedPE:: DecompressXEOV_Valid(this-> pXEOVRegOut.Data())==1);
    }
    inline bool oweovIsOut() const{
        return (ReformedPE::DecompressOWEOV_Out(this->pOWEOVRegOut.Data()));
    }
    inline bool  xeovFIFORightIsFull() const{
        return (this->ports[ReformedPE::XFullRight.ToIndex()].Data()==1);
    }

    inline bool wIsEnd() const{
        return (ReformedPE::DecompressOWEOV_EOW(this->pOWEOVRegOut.Data())==1);
    }
    inline PortDataType wOff() const{
        return  ReformedPE::DecompressOWEOV_Off(this->pOWEOVRegOut.Data());
    }
    inline bool xIsEnd() const{
        return (ReformedPE:: DecompressXEOV_EOX(this-> pXEOVRegOut.Data())==1);
    }
    inline PortDataType xOff() const{
        return  ReformedPE:: DecompressXEOV_Off(this-> pXEOVRegOut.Data());
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
        this->pSFIFOOut.Read();
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
            && ( this->pSFIFOEmpty.Data() == 0)
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

    /** state **/
    inline enum RU_STATE NowState() const{
        switch(this->pCompOut.Data()){
        case Comparer::In0_Less_In1:
            ///this->wOff()<this->xOff()
            if (this->wIsEnd()){
                if (this->xIsEnd())
                    return RU_STATE::WX_END;
                else
                    return RU_STATE:: W_END;
            }
            else
                return RU_STATE::W_LESS_X;
            break;
        case Comparer::In1_Less_In0:
            ///this->xOff()<this->wOff()
            if (this->xIsEnd()){
                if (this->wIsEnd())
                    return RU_STATE::WX_END;
                else
                    return RU_STATE:: X_END;
            }
            else
                return RU_STATE::X_LESS_W;
            break;
        case Comparer::In0_Equal_In1:
            /// w equal x
            if ( this->xIsEnd() &&!this->wIsEnd())
                return RU_STATE::W_EQUAL_X_XE;
            else if (!this->xIsEnd() && this->wIsEnd())
                return RU_STATE::W_EQUAL_X_WE;
            else
                return RU_STATE::W_EQUAL_X;
            break;
        default:
            assert(false);
        }

        assert(false);
        return RU_STATE::NOT_READY;
    }

    inline void InitializePorts() override{
        this->ports.clear();
        this->ports.reserve(PortName::Number());
        /** ---------- up -------- **/
        this->ports.emplace_back(OUT_BIT_WIDTH
                            +      W_BIT_WIDTH
                            + OFFSET_BIT_WIDTH
                            +     WE_BIT_WIDTH,
                                    PortDirection::IN );/** OWInUp   O+W+V+Off+E-bit in  **/
        this->ports.emplace_back( 1,PortDirection::IN );/** OWPushUp           1-bit in  **/
        this->ports.emplace_back( 1,PortDirection::OUT);/** OWFullUp           1-bit out **/
        /** --------- down ------- **/
        this->ports.emplace_back(OUT_BIT_WIDTH
                            +      W_BIT_WIDTH
                            + OFFSET_BIT_WIDTH
                            +     WE_BIT_WIDTH,
                                    PortDirection::OUT);/** OWOutDown  O+W+V+Off+E-bit out **/
        this->ports.emplace_back( 1,PortDirection::OUT);/** OWPushDown           1-bit out **/
        this->ports.emplace_back( 1,PortDirection::IN );/** OWFullDown           1-bit in  **/
        /** -------- left -------- **/
        this->ports.emplace_back(  X_BIT_WIDTH
                              + OFFSET_BIT_WIDTH
                              +     XE_BIT_WIDTH,
                                    PortDirection::IN );/** XInLeft  X+V+Off+E-bit in  **/
        this->ports.emplace_back( 1,PortDirection::IN );/** XPushLeft        1-bit in  **/
        this->ports.emplace_back( 1,PortDirection::OUT);/** XFullLeft        1-bit out **/
        /** -------- right ------- **/
        this->ports.emplace_back(  X_BIT_WIDTH
                            + OFFSET_BIT_WIDTH
                            +     XE_BIT_WIDTH,
                                    PortDirection::OUT);/** XOutRight   X+V+Off+E-bit out **/
        this->ports.emplace_back( 1,PortDirection::OUT);/** XPushRight          1-bit out **/
        this->ports.emplace_back( 1,PortDirection::IN );/** XFullRight          1-bit in  **/
        /** -------- up-up ------- **/
        this->ports.emplace_back(S_BIT_WIDTH
                              + SE_BIT_WIDTH,
                                    PortDirection::IN );/** SInUUp   S+E-bit in  **/
        this->ports.emplace_back( 1,PortDirection::IN );/** SPushUUp   1-bit in  **/
        this->ports.emplace_back( 1,PortDirection::OUT);/** SFullUUp   1-bit out **/
        /** ------ down-down ----- **/
        this->ports.emplace_back(S_BIT_WIDTH
                              + SE_BIT_WIDTH,
                                    PortDirection::OUT);/** SOutDDown  S+E-bit out **/
        this->ports.emplace_back( 1,PortDirection::OUT);/** SPushDDown   1-bit out **/
        this->ports.emplace_back( 1,PortDirection::IN );/** SFullDDown   1-bit in  **/
        assert(this->ports.size()==PortName::Number());
        return;
    }

    inline void OutputConnect() override{
        /** ---------- up -------- **/
        this->oweovFIFO.Clone(this->ports[ReformedPE::OWFullUp .ToIndex()],FIFO::Full);
        /** --------- down ------- **/
        this->oweovFIFO.Clone(this->ports[ReformedPE::OWOutDown.ToIndex()],FIFO::Out );
        this->ports[ReformedPE::OWPushDown.ToIndex()].Clone(this->owPushDown);
        /** -------- left -------- **/
        this-> xeovFIFO.Clone(this->ports[ReformedPE::XFullLeft.ToIndex()],FIFO::Full);
        /** -------- right ------- **/
        this-> xeovFIFO.Clone(this->ports[ReformedPE::XOutRight.ToIndex()],FIFO::Out );
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
    static const ReformedPE::PortName     OWInUp; /**  O+W+V+Off+E-bit in  **/
    static const ReformedPE::PortName   OWPushUp; /**            1-bit in  **/
    static const ReformedPE::PortName   OWFullUp; /**            1-bit out **/
    /** --------- down ------- **/
    static const ReformedPE::PortName  OWOutDown; /**  O+W+V+Off+E-bit out **/
    static const ReformedPE::PortName OWPushDown; /**            1-bit out **/
    static const ReformedPE::PortName OWFullDown; /**            1-bit in  **/
    /** -------- left -------- **/
    static const ReformedPE::PortName    XInLeft; /**    X+V+Off+E-bit in  **/
    static const ReformedPE::PortName  XPushLeft; /**            1-bit in  **/
    static const ReformedPE::PortName  XFullLeft; /**            1-bit out **/
    /** -------- right ------- **/
    static const ReformedPE::PortName  XOutRight; /**    X+V+Off+E-bit out **/
    static const ReformedPE::PortName XPushRight; /**            1-bit out **/
    static const ReformedPE::PortName XFullRight; /**            1-bit in  **/
    /** -------- up-up ------- **/
    static const ReformedPE::PortName     SInUUp; /**        S+E-bit in  **/
    static const ReformedPE::PortName   SPushUUp; /**          1-bit in  **/
    static const ReformedPE::PortName   SFullUUp; /**          1-bit out **/
    /** -------- down-down ------- **/
    static const ReformedPE::PortName  SOutDDown; /**        S+E-bit out **/
    static const ReformedPE::PortName SPushDDown; /**          1-bit out **/
    static const ReformedPE::PortName SFullDDown; /**          1-bit in  **/

private:
    ReformedPE &operator=(const ReformedPE&);
//    ReformedPE(const ReformedPE&);

public:
    ReformedPE(const ReformedPE& rePE):
           owReg (OUT_BIT_WIDTH
               +      W_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**       O+W-bit **/
              xReg (  X_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**         X-bit **/
            ParSum (  S_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**         S-bit **/
          oweovReg (  W_BIT_WIDTH
               +    OUT_BIT_WIDTH
               +     WE_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +  VALID_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**   O+Off+E-bit **/
           xeovReg (  X_BIT_WIDTH
               +     XE_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +  VALID_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**     Off+E-bit **/
            owxFIFO(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +      X_BIT_WIDTH,owxFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**     O+W+X-bit **/
          oweovFIFO(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     WE_BIT_WIDTH,  wFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/** O+W+Off+E-bit **/
           xeovFIFO(  X_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     XE_BIT_WIDTH,  xFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**   X+Off+E-bit **/
              sFIFO(  S_BIT_WIDTH
               +     SE_BIT_WIDTH,  sFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**       S+E-bit **/
    pOWEOVFIFOEmpty(      1      ,Object::PortDirection::IN ),/**           1-bit **/
    pOWEOVFIFOOut  (OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     WE_BIT_WIDTH,Object::PortDirection::IN ),/** O+W+Off+E-bit **/
    pXVEOFIFOOut   (  X_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     XE_BIT_WIDTH,Object::PortDirection::IN ),/**   X+Off+E-bit **/
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
         pOWEOVRegOut(W_BIT_WIDTH
               +    OUT_BIT_WIDTH
               +     WE_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +  VALID_BIT_WIDTH,Object::PortDirection::IN ), /** W+O+E+Off+V-bit **/
        pXEOVRegOut(  X_BIT_WIDTH
               +     XE_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +  VALID_BIT_WIDTH,Object::PortDirection::IN ),/**   X+E+Off+V-bit **/
          pAdderOut(  S_BIT_WIDTH,Object::PortDirection::IN ),/**         S-bit **/
         pParSumOut(  S_BIT_WIDTH,Object::PortDirection::IN ),/**         S-bit **/
         pMulterOut(  W_BIT_WIDTH
               +      X_BIT_WIDTH,Object::PortDirection::IN ),/**       W+X-bit **/
     /** --- inner control --- **/
          pOWXRegEn  (1,Object::PortDirection::OUT),/** 1-bit **/
          pParSumEn  (1,Object::PortDirection::OUT),/** 1-bit **/
       pOWEOVFIFOPop (1,Object::PortDirection::OUT),/** 1-bit **/
        pXVEOFIFOPop (1,Object::PortDirection::OUT),/** 1-bit **/
         pOWXFIFOPush(1,Object::PortDirection::OUT),/** 1-bit **/
         pOWXFIFOPop (1,Object::PortDirection::OUT),/** 1-bit **/
           pSFIFOPop (1,Object::PortDirection::OUT),/** 1-bit **/
         pOWEOVRegEn (1,Object::PortDirection::OUT),/** 1-bit **/
          pXEOVRegEn (1,Object::PortDirection::OUT),/** 1-bit **/
             pCompOut(2,Object::PortDirection::IN ),/** 1-bit **/
           pOWRegIn  (OUT_BIT_WIDTH
                 +      W_BIT_WIDTH,Object::PortDirection::OUT),/**   O+W-bit **/
            pWRegOut (  W_BIT_WIDTH,Object::PortDirection::OUT),/**     W-bit **/
            pXRegIn  (  X_BIT_WIDTH,Object::PortDirection::OUT),/**     X-bit **/
          pParSumIn  (  S_BIT_WIDTH,Object::PortDirection::OUT),/**     S-bit **/
          pWOffOut(OFFSET_BIT_WIDTH,Object::PortDirection::OUT),/**   Off-bit **/
          pXOffOut(OFFSET_BIT_WIDTH,Object::PortDirection::OUT),/**   Off-bit **/
         pOWXFIFOIn  (OUT_BIT_WIDTH
                 +      W_BIT_WIDTH
                 +      X_BIT_WIDTH,Object::PortDirection::OUT),/** O+W+X-bit **/
          pOWEOVRegIn  (W_BIT_WIDTH
                 +    OUT_BIT_WIDTH
                 +     WE_BIT_WIDTH
                 + OFFSET_BIT_WIDTH
                 +  VALID_BIT_WIDTH,Object::PortDirection::OUT),/** W+O+E+Off+V-bit **/
          pXEOVRegIn   (X_BIT_WIDTH
                 +     XE_BIT_WIDTH
                 + OFFSET_BIT_WIDTH
                 +  VALID_BIT_WIDTH,Object::PortDirection::OUT),/**   X+E+Off+V-bit **/
             adder (W_BIT_WIDTH+X_BIT_WIDTH,S_BIT_WIDTH,S_BIT_WIDTH,true),
            multer (W_BIT_WIDTH,X_BIT_WIDTH,W_BIT_WIDTH+X_BIT_WIDTH,true),
              comp (OFFSET_BIT_WIDTH),
             ruPer (rePE. ruPer),
            macPer (rePE.macPer),
           receiveS(rePE.receiveS){
        this->clock   = 0;
        this->lastStuck     = rePE.lastStuck;
        this->hasRecEOF     = rePE.hasRecEOF;
        this->hasRecOut     = rePE.hasRecOut;
        this->hasSentParSum = rePE.hasSentParSum;
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        /** --- RU inner control --- **/
        this->oweovFIFOPop  = rePE.oweovFIFOPop;
        this-> xeovFIFOPop  = rePE. xeovFIFOPop;
        this->  owxFIFOPush = rePE.  owxFIFOPush;
        this-> _owxFIFOPush = rePE. _owxFIFOPush;
        /** --- RU inner data --- **/
        this->oweovRegIn = rePE.oweovRegIn; /** W+O+E+Off+V-bit **/
        this->oweovRegEn = rePE.oweovRegEn; /**           1-bit **/
        this-> xeovRegIn = rePE. xeovRegIn; /**   X+E+Off+V-bit **/
        this-> xeovRegEn = rePE. xeovRegEn; /**           1-bit **/
        this-> owxFIFOIn = rePE. owxFIFOIn; /**       W+O+X-bit **/
        this->   wOffOut = rePE.   wOffOut; /**         Off-bit **/
        this->   xOffOut = rePE.   xOffOut; /**         Off-bit **/
        /** --- MAC inner control --- **/
        this-> owxFIFOPop =rePE. owxFIFOPop;
        this->_owxFIFOPop =rePE._owxFIFOPop;
        this->   sFIFOPop =rePE.   sFIFOPop;
        this->  owxRegEn  =rePE.  owxRegEn ;
        this->  ParSumEn  =rePE.  ParSumEn ;
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

    ReformedPE(
        ClockFeqType  ruPer,
        ClockFeqType macPer,
        bool receiveS,
        /** --- RU inner control --- **/
        PortDataType oweovFIFOPop = 0,
        PortDataType  xeovFIFOPop = 0,
        PortDataType  owxFIFOPush = 0,
        PortDataType _owxFIFOPush = 0,
        /** --- RU inner data --- **/
        PortDataType  oweovRegIn = 0, /** W+O+E+Off+V-bit **/
        PortDataType  oweovRegEn = 0, /**           1-bit **/
        PortDataType   xeovRegIn = 0, /**   X+E+Off+V-bit **/
        PortDataType   xeovRegEn = 0, /**           1-bit **/
        PortDataType   owxFIFOIn = 0, /**       W+O+X-bit **/
        PortDataType     wOffOut = 0, /**         Off-bit **/
        PortDataType     xOffOut = 0, /**         Off-bit **/
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
        /** ---  RU state --- **/
        bool lastStuck     = false,
        /** --- MAC state --- **/
        bool hasRecEOF     = false,
        bool hasRecOut     = false,
        bool hasSentParSum = false
        ):   owReg (OUT_BIT_WIDTH
               +      W_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**       O+W-bit **/
              xReg (  X_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**         X-bit **/
            ParSum (  S_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**         S-bit **/
          oweovReg (  W_BIT_WIDTH
               +    OUT_BIT_WIDTH
               +     WE_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +  VALID_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**   O+Off+E-bit **/
           xeovReg (  X_BIT_WIDTH
               +     XE_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +  VALID_BIT_WIDTH,ClockedObject::ClockEdge::POS_EDGE),/**     Off+E-bit **/
            owxFIFO(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               +      X_BIT_WIDTH,owxFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**     O+W+X-bit **/
          oweovFIFO(OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     WE_BIT_WIDTH,  wFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/** O+W+Off+E-bit **/
           xeovFIFO(  X_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     XE_BIT_WIDTH,  xFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**   X+Off+E-bit **/
              sFIFO(  S_BIT_WIDTH
               +     SE_BIT_WIDTH,  sFIFO_LEN,ClockedObject::ClockEdge::POS_EDGE),/**       S+E-bit **/
    pOWEOVFIFOEmpty(      1      ,Object::PortDirection::IN ),/**           1-bit **/
    pOWEOVFIFOOut  (OUT_BIT_WIDTH
               +      W_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     WE_BIT_WIDTH,Object::PortDirection::IN ),/** O+W+Off+E-bit **/
    pXVEOFIFOOut   (  X_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +     XE_BIT_WIDTH,Object::PortDirection::IN ),/**   X+Off+E-bit **/
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
         pOWEOVRegOut(W_BIT_WIDTH
               +    OUT_BIT_WIDTH
               +     WE_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +  VALID_BIT_WIDTH,Object::PortDirection::IN ), /** W+O+E+Off+V-bit **/
        pXEOVRegOut(  X_BIT_WIDTH
               +     XE_BIT_WIDTH
               + OFFSET_BIT_WIDTH
               +  VALID_BIT_WIDTH,Object::PortDirection::IN ),/**   X+E+Off+V-bit **/
          pAdderOut(  S_BIT_WIDTH,Object::PortDirection::IN ),/**         S-bit **/
         pParSumOut(  S_BIT_WIDTH,Object::PortDirection::IN ),/**         S-bit **/
         pMulterOut(  W_BIT_WIDTH
               +      X_BIT_WIDTH,Object::PortDirection::IN ),/**       W+X-bit **/
     /** --- inner control --- **/
          pOWXRegEn  (1,Object::PortDirection::OUT),/** 1-bit **/
          pParSumEn  (1,Object::PortDirection::OUT),/** 1-bit **/
       pOWEOVFIFOPop (1,Object::PortDirection::OUT),/** 1-bit **/
        pXVEOFIFOPop (1,Object::PortDirection::OUT),/** 1-bit **/
         pOWXFIFOPush(1,Object::PortDirection::OUT),/** 1-bit **/
         pOWXFIFOPop (1,Object::PortDirection::OUT),/** 1-bit **/
           pSFIFOPop (1,Object::PortDirection::OUT),/** 1-bit **/
         pOWEOVRegEn (1,Object::PortDirection::OUT),/** 1-bit **/
          pXEOVRegEn (1,Object::PortDirection::OUT),/** 1-bit **/
             pCompOut(2,Object::PortDirection::IN ),/** 1-bit **/
           pOWRegIn  (OUT_BIT_WIDTH
                 +      W_BIT_WIDTH,Object::PortDirection::OUT),/**   O+W-bit **/
            pWRegOut (  W_BIT_WIDTH,Object::PortDirection::OUT),/**     W-bit **/
            pXRegIn  (  X_BIT_WIDTH,Object::PortDirection::OUT),/**     X-bit **/
          pParSumIn  (  S_BIT_WIDTH,Object::PortDirection::OUT),/**     S-bit **/
          pWOffOut(OFFSET_BIT_WIDTH,Object::PortDirection::OUT),/**   Off-bit **/
          pXOffOut(OFFSET_BIT_WIDTH,Object::PortDirection::OUT),/**   Off-bit **/
         pOWXFIFOIn  (OUT_BIT_WIDTH
                 +      W_BIT_WIDTH
                 +      X_BIT_WIDTH,Object::PortDirection::OUT),/** O+W+X-bit **/
          pOWEOVRegIn  (W_BIT_WIDTH
                 +    OUT_BIT_WIDTH
                 +     WE_BIT_WIDTH
                 + OFFSET_BIT_WIDTH
                 +  VALID_BIT_WIDTH,Object::PortDirection::OUT),/** W+O+E+Off+V-bit **/
          pXEOVRegIn   (X_BIT_WIDTH
                 +     XE_BIT_WIDTH
                 + OFFSET_BIT_WIDTH
                 +  VALID_BIT_WIDTH,Object::PortDirection::OUT),/**   X+E+Off+V-bit **/
             adder (W_BIT_WIDTH+X_BIT_WIDTH,S_BIT_WIDTH,S_BIT_WIDTH,true),
            multer (W_BIT_WIDTH,X_BIT_WIDTH,W_BIT_WIDTH+X_BIT_WIDTH,true),
             ruPer ( ruPer),
            macPer (macPer),
              comp (OFFSET_BIT_WIDTH),
           receiveS(receiveS){
        this->clock   = 0;
        this->lastStuck     = lastStuck;
        this->hasRecEOF     = hasRecEOF;
        this->hasRecOut     = hasRecOut;
        this->hasSentParSum = hasSentParSum;
        /********************************************************
         ** initialize the data in case of restoring the scene **
         ********************************************************/
        /** --- RU inner control --- **/
        this->oweovFIFOPop  = oweovFIFOPop;
        this-> xeovFIFOPop  =  xeovFIFOPop;
        this->  owxFIFOPush =   owxFIFOPush;
        this-> _owxFIFOPush =  _owxFIFOPush;
        /** --- RU inner data --- **/
        this->oweovRegIn = oweovRegIn; /** W+O+E+Off+V-bit **/
        this->oweovRegEn = oweovRegEn; /**           1-bit **/
        this-> xeovRegIn =  xeovRegIn; /**   X+E+Off+V-bit **/
        this-> xeovRegEn =  xeovRegEn; /**           1-bit **/
        this-> owxFIFOIn =  owxFIFOIn; /**       W+O+X-bit **/
        this->   wOffOut =    wOffOut; /**         Off-bit **/
        this->   xOffOut =    xOffOut; /**         Off-bit **/
        /** --- MAC inner control --- **/
        this-> owxFIFOPop = owxFIFOPop;
        this->_owxFIFOPop =_owxFIFOPop;
        this->   sFIFOPop =   sFIFOPop;
        this->  owxRegEn  =  owxRegEn ;
        this->  ParSumEn  =  ParSumEn ;
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

    inline bool OutputConnectedCorrect() const override{
        return
        /** ---------- up -------- **/
            this->oweovFIFO.SameTo(this->ports[ReformedPE::OWFullUp .ToIndex()],FIFO::Full)
        /** --------- down ------- **/
         && this->oweovFIFO.SameTo(this->ports[ReformedPE::OWOutDown.ToIndex()],FIFO::Out )
         && this->ports[ReformedPE::OWPushDown.ToIndex()].SameTo(this->owPushDown)
        /** -------- left -------- **/
         && this-> xeovFIFO.SameTo(this->ports[ReformedPE::XFullLeft.ToIndex()],FIFO::Full)
        /** -------- right ------- **/
         && this-> xeovFIFO.SameTo(this->ports[ReformedPE::XOutRight.ToIndex()],FIFO::Out )
         && this->ports[ReformedPE::XPushRight.ToIndex()].SameTo(this->xPushRight)
        /** -------- up-up ------- **/
         && this->    sFIFO.SameTo(this->ports[ReformedPE:: SFullUUp.ToIndex()],FIFO::Full)
        /** ------ down-down ----- **/
         && this->ports[ReformedPE:: SOutDDown.ToIndex()].SameTo(this->  sInDDown)
         && this->ports[ReformedPE::SPushDDown.ToIndex()].SameTo(this->sPushDDown);
    }

    inline void InnerConnect() override{
        assert(this->InputConnected());
        /*****************
         ** stray ports **
         *****************/
        this->   pOWXRegEn  .Clone(this->   owxRegEn  );
        this->    pOWRegIn  .Clone(this->    owRegIn  );
        this->     pXRegIn  .Clone(this->     xRegIn  );
        this->   pParSumIn  .Clone(this->   ParSumIn  );
        this->   pParSumEn  .Clone(this->   ParSumEn  );
        this->pOWEOVFIFOPop .Clone(this->oweovFIFOPop );
        this-> pXVEOFIFOPop .Clone(this-> xeovFIFOPop );
        this->  pOWXFIFOIn  .Clone(this->  owxFIFOIn  );
        this->  pOWXFIFOPush.Clone(this->  owxFIFOPush);
        this->  pOWXFIFOPop .Clone(this->  owxFIFOPop );
        this->     pWOffOut .Clone(this->     wOffOut );
        this->     pXOffOut .Clone(this->     xOffOut );
        this->    pSFIFOPop .Clone(this->    sFIFOPop );
        this->     pWRegOut .Clone(this->     wRegOut );
        this->  pXEOVRegIn  .Clone(this->  xeovRegIn  );
        this->  pXEOVRegEn  .Clone(this->  xeovRegEn  );
        this-> pOWEOVRegIn  .Clone(this-> oweovRegIn  );
        this-> pOWEOVRegEn  .Clone(this-> oweovRegEn  );

        /** --- Reg --- **/
        this->owReg.ConnectTo(Reg::in,this-> pOWRegIn);
        this->owReg.ConnectTo(Reg::en,this->pOWXRegEn);
        this->owReg.ConnectTo(this->pOWRegOut,Reg::out);
        this->owReg.InnerConnect();

        this->xReg.ConnectTo(Reg::in,this->  pXRegIn);
        this->xReg.ConnectTo(Reg::en,this->pOWXRegEn);
        this->xReg.InnerConnect();

        this->ParSum.ConnectTo(Reg::in ,this->pParSumIn );
        this->ParSum.ConnectTo(Reg::en ,this->pParSumEn );
        this->ParSum.ConnectTo(this->pParSumOut,Reg::out);
        this->ParSum.InnerConnect();

        this->oweovReg.ConnectTo(this->pOWEOVRegOut,Reg::out);
        this->oweovReg.ConnectTo(Reg::in ,this->pOWEOVRegIn );
        this->oweovReg.ConnectTo(Reg::en ,this->pOWEOVRegEn );
        this->oweovReg.InnerConnect();

        this->xeovReg.ConnectTo(this->pXEOVRegOut,Reg::out);
        this->xeovReg.ConnectTo(Reg::in ,this->pXEOVRegIn );
        this->xeovReg.ConnectTo(Reg::en ,this->pXEOVRegEn );
        this->xeovReg.InnerConnect();

        /** --- FIFO --- **/
        this->oweovFIFO.Clone    (FIFO::In  ,this->ports[ReformedPE::OWInUp  .ToIndex()]);
        this->oweovFIFO.Clone    (FIFO::Push,this->ports[ReformedPE::OWPushUp.ToIndex()]);
        this->oweovFIFO.ConnectTo(FIFO::Pop ,this->pOWEOVFIFOPop);
        this->oweovFIFO.ConnectTo(this->pOWEOVFIFOEmpty,FIFO::Empty);
        this->oweovFIFO.ConnectTo(this->pOWEOVFIFOOut  ,FIFO::Out  );
        this->oweovFIFO.InnerConnect();

        this->xeovFIFO.Clone    (FIFO::In  ,this->ports[ReformedPE::XInLeft  .ToIndex()]);
        this->xeovFIFO.Clone    (FIFO::Push,this->ports[ReformedPE::XPushLeft.ToIndex()]);
        this->xeovFIFO.ConnectTo(FIFO::Pop ,this->pXVEOFIFOPop);
        this->xeovFIFO.ConnectTo(this->pXVEOFIFOEmpty,FIFO::Empty);
        this->xeovFIFO.ConnectTo(this->pXVEOFIFOOut  ,FIFO::Out  );
        this->xeovFIFO.InnerConnect();

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

        /** --- Comparer --- **/
        this->comp.ConnectTo(Comparer::in0,this->pWOffOut);
        this->comp.ConnectTo(Comparer::in1,this->pXOffOut);
        this->comp.ConnectTo(this->pCompOut,Comparer::out);

        assert(this->InnerConnected());
        return;
    }

    bool InnerConnectedCorrect() const override{
        return
        /*****************
         ** stray ports **
         *****************/
            this->   pOWXRegEn  .SameTo(this->   owxRegEn  )
         && this->    pOWRegIn  .SameTo(this->    owRegIn  )
         && this->     pXRegIn  .SameTo(this->     xRegIn  )
         && this->   pParSumIn  .SameTo(this->   ParSumIn  )
         && this->   pParSumEn  .SameTo(this->   ParSumEn  )
         && this->pOWEOVFIFOPop .SameTo(this->oweovFIFOPop )
         && this-> pXVEOFIFOPop .SameTo(this-> xeovFIFOPop )
         && this->  pOWXFIFOIn  .SameTo(this->  owxFIFOIn  )
         && this->  pOWXFIFOPush.SameTo(this->  owxFIFOPush)
         && this->  pOWXFIFOPop .SameTo(this->  owxFIFOPop )
         && this->    pSFIFOPop .SameTo(this->    sFIFOPop )
         && this->     pWRegOut .SameTo(this->     wRegOut )
         && this->  pXEOVRegIn  .SameTo(this->  xeovRegIn  )
         && this->  pXEOVRegEn  .SameTo(this->  xeovRegEn  )
         && this-> pOWEOVRegIn  .SameTo(this-> oweovRegIn  )
         && this-> pOWEOVRegEn  .SameTo(this-> oweovRegEn  )
        /** --- Reg --- **/
         && this->owReg.ConnectedTo(Reg::in,this-> pOWRegIn)
         && this->owReg.ConnectedTo(Reg::en,this->pOWXRegEn)
         && this->owReg.ConnectedTo(this->pOWRegOut,Reg::out)
         && this->owReg.InnerConnected()

         && this->xReg.ConnectedTo(Reg::in,this->  pXRegIn)
         && this->xReg.ConnectedTo(Reg::en,this->pOWXRegEn)
         && this->xReg.InnerConnected()

         && this->ParSum.ConnectedTo(Reg::in,this->pParSumIn)
         && this->ParSum.ConnectedTo(Reg::en,this->pParSumEn)
         && this->ParSum.ConnectedTo(this->pParSumOut,Reg::out)
         && this->ParSum.InnerConnected()

         && this->oweovReg.ConnectedTo(this->pOWEOVRegOut,Reg::out)
         && this->oweovReg.ConnectedTo(Reg::in ,this->pOWEOVRegIn )
         && this->oweovReg.ConnectedTo(Reg::en ,this->pOWEOVRegEn )
         && this->oweovReg.InnerConnected()

         && this->xeovReg.ConnectedTo(this->pXEOVRegOut,Reg::out)
         && this->xeovReg.ConnectedTo(Reg::in ,this->pXEOVRegIn )
         && this->xeovReg.ConnectedTo(Reg::en ,this->pXEOVRegEn )
         && this->xeovReg.InnerConnected()

        /** --- FIFO --- **/
         && this->oweovFIFO.SameTo     (FIFO::In  ,this->ports[ReformedPE::OWInUp  .ToIndex()])
         && this->oweovFIFO.SameTo     (FIFO::Push,this->ports[ReformedPE::OWPushUp.ToIndex()])
         && this->oweovFIFO.ConnectedTo(FIFO::Pop ,this->pOWEOVFIFOPop)
         && this->oweovFIFO.ConnectedTo(this->pOWEOVFIFOEmpty,FIFO::Empty)
         && this->oweovFIFO.ConnectedTo(this->pOWEOVFIFOOut  ,FIFO::Out  )
         && this->oweovFIFO.InnerConnected()

         && this->xeovFIFO.SameTo     (FIFO::In  ,this->ports[ReformedPE::XInLeft  .ToIndex()])
         && this->xeovFIFO.SameTo     (FIFO::Push,this->ports[ReformedPE::XPushLeft.ToIndex()])
         && this->xeovFIFO.ConnectedTo(FIFO::Pop ,this->pXVEOFIFOPop)
         && this->xeovFIFO.ConnectedTo(this->pXVEOFIFOEmpty,FIFO::Empty)
         && this->xeovFIFO.ConnectedTo(this->pXVEOFIFOOut  ,FIFO::Out  )
         && this->xeovFIFO.InnerConnected()

         && this->owxFIFO.ConnectedTo(FIFO::In  ,this->pOWXFIFOIn  )
         && this->owxFIFO.ConnectedTo(FIFO::Push,this->pOWXFIFOPush)
         && this->owxFIFO.ConnectedTo(FIFO::Pop ,this->pOWXFIFOPop )
         && this->owxFIFO.ConnectedTo(this->pOWXFIFOOut  ,FIFO::Out  )
         && this->owxFIFO.ConnectedTo(this->pOWXFIFOFull ,FIFO::Full )
         && this->owxFIFO.ConnectedTo(this->pOWXFIFOEmpty,FIFO::Empty)
         && this->owxFIFO.InnerConnected()

         && this->sFIFO.SameTo     (FIFO::In  ,this->ports[ReformedPE::SInUUp  .ToIndex()])
         && this->sFIFO.SameTo     (FIFO::Push,this->ports[ReformedPE::SPushUUp.ToIndex()])
         && this->sFIFO.ConnectedTo(FIFO::Pop ,this->pSFIFOPop   )
         && this->sFIFO.ConnectedTo(this->pSFIFOOut  ,FIFO::Out  )
         && this->sFIFO.ConnectedTo(this->pSFIFOEmpty,FIFO::Empty)
         && this->sFIFO.InnerConnected()

        /** --- Multer --- **/
         && this->multer.ConnectedTo(Multer::in0,this->pWRegOut)
         && this->multer.ConnectedTo(Multer::in1,this->xReg,Reg::out)
         && this->multer.ConnectedTo(this->pMulterOut,Multer::out)
         && this->multer.InnerConnected()

        /** --- Adder --- **/
         && this->adder.ConnectedTo( Adder::in0,this->multer,Multer::out)
         && this->adder.ConnectedTo( Adder::in1,this->ParSum,Reg   ::out)
         && this->adder.ConnectedTo(this->pAdderOut,Adder::out)
         && this->adder.InnerConnected();
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

        if (this->clock % this->ruPer == 0){
            /// the read time has been calculated by the following FIFO
//            if (this->oweovRegEn == 1)
//                this->pOWEOVFIFOOut.Read();
//            if (this-> xeovRegEn == 1)
//                this-> pXVEOFIFOOut.Read();
            this->  owxFIFO.atPosEdge();
            this->oweovFIFO.atPosEdge();
            this-> xeovFIFO.atPosEdge();
            this->    sFIFO.atPosEdge();
            this-> oweovReg.atPosEdge();
            this->  xeovReg.atPosEdge();
        }

        if (this->clock % this->macPer == 0){
            if (this->clock % this->ruPer != 0)
                this->owxFIFO.atPosEdge();
            if (this-> owxFIFOPop == 1)
                this->pOWXFIFOOut.Read();
            if (ReformedPE::DecompressOW_O(this->pOWRegOut.Data())==1
             && ReformedPE::DecompressOW_O(this->owRegIn)==1
             && this->owxRegEn == 1)
                this->hasSentParSum = false;
            this-> owReg.atPosEdge();/**       O+W-bit **/
            this->  xReg.atPosEdge();/**         X-bit **/
            this->ParSum.atPosEdge();/**         S-bit **/
        }
        return true;
    }

    inline bool atNegEdge() override{
        assert(false);
        return false;
    }

    inline bool InnerConnected() const override{
        return  this->  owxFIFO.Connected() /**       O+W+X-bit **/
             && this->oweovFIFO.Connected() /** O+W+V+Off+E-bit **/
             && this-> xeovFIFO.Connected() /**   X+V+Off+E-bit **/
             && this->    sFIFO.Connected() /**         S+E-bit **/
             && this->oweovReg .Connected() /**         O+W-bit **/
             && this-> xeovReg .Connected() /**         O+W-bit **/
             && this->   owReg .Connected() /**         O+W-bit **/
             && this->    xReg .Connected() /**           X-bit **/
             && this->oweovReg .Connected() /**   O+V+Off+E-bit **/
             && this-> xeovReg .Connected() /**     V+Off+E-bit **/
             && this->  ParSum .Connected();/**           S-bit **/
    }

    inline bool Run() override{
        if (this->clock % this->ruPer == 0){
            this->    sFIFO.Run();
            this->  owxFIFO.Run();
            this->owRegIn = ReformedPE::DecompressOWX_OW(this->pOWXFIFOOut.Data());
            this-> xRegIn = ReformedPE::DecompressOWX_X (this->pOWXFIFOOut.Data());
            this->oweovFIFO.Run();
            this-> xeovFIFO.Run();
            this->oweovRegIn = ReformedPE::CompressOWEOV(
                                   this->pOWEOVFIFOOut.Data(),
                                (1-this->pOWEOVFIFOEmpty.Data()));
            this-> xeovRegIn = ReformedPE::CompressXEOV(
                                   this->pXVEOFIFOOut.Data(),
                                (1-this->pXVEOFIFOEmpty.Data()));
            this->oweovReg.Run();
            this-> xeovReg.Run();
            this->owxFIFOIn = ReformedPE::CompressOWX(
                    ReformedPE::DecompressOWEOV_OW(this->pOWEOVRegOut.Data()),
                    ReformedPE:: DecompressXEOV_X (this-> pXEOVRegOut.Data())
                );

            if (!this->lastStuck){
                this->wOffOut
                    = ReformedPE::DecompressOWEOV_Off(this->pOWEOVRegOut.Data());
                this->xOffOut
                    = ReformedPE:: DecompressXEOV_Off(this-> pXEOVRegOut.Data());
                this->oweovReg.ReadBits(OFFSET_BIT_WIDTH
                                      +     WE_BIT_WIDTH);
                this-> xeovReg.ReadBits(OFFSET_BIT_WIDTH
                                      +     XE_BIT_WIDTH);
                this->comp.Run();
            }

            this->lastStuck = false;

            /***************************
             *   control logic of RU   *
             ***************************/
            switch(this->NowState()){
            case RU_STATE::W_EQUAL_X:
                if(!this->oweovFIFODownIsFull()
                && !this->oweovFIFOIsEmpty()
                && !this-> xeovFIFORightIsFull()
                && !this-> xeovFIFOIsEmpty()
                && !this->  owxFIFOIsFull()){
                    this-> WPush();
                    this-> XPush();
                    this->Output();
                    this->hasRecOut = false;
                }
                else
                    this->Stuck();
                break;
            case RU_STATE::W_EQUAL_X_WE:
                if(!this->xeovFIFORightIsFull()
                && !this->xeovFIFOIsEmpty()
                && !this-> owxFIFOIsFull()){
                    this-> WNotPush();
                    this->    XPush();
                    this->   Output();
                    this->hasRecOut = true;
                }
                else
                    this->Stuck();
                break;
            case RU_STATE::W_EQUAL_X_XE:
                if(!this->oweovFIFODownIsFull()
                && !this->oweovFIFOIsEmpty()
                && !this-> owxFIFOIsFull()){
                    this->    WPush();
                    this-> XNotPush();
                    this->   Output();
                    this->hasRecOut = false;
                }
                else
                    this->Stuck();
                break;
            case RU_STATE::X_LESS_W:
                if(!this->xeovFIFORightIsFull()
                && !this->xeovFIFOIsEmpty()){
                    this-> WNotPush();
                    this->    XPush();
                    this->NotOutput();
                }
                else
                    this->Stuck();
                break;
            case RU_STATE::W_LESS_X:
                if (this->oweovIsOut()){
                    assert(false);
                    if(!this->oweovFIFODownIsFull()
                    && !this->oweovFIFOIsEmpty()
                    && !this->owxFIFOIsFull()){
                        this->    WPush();
                        this-> XNotPush();
                        this->OutputOut();
                        this->hasRecOut = false;
                    }
                    else
                        this->Stuck();
                }
                else{
                    if(!this->oweovFIFODownIsFull()
                    && !this->oweovFIFOIsEmpty()){
                        this->    WPush();
                        this-> XNotPush();
                        this->NotOutput();
                        this->hasRecOut = false;
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
                if(!this->oweovFIFODownIsFull()
                && !this->oweovFIFOIsEmpty()){
                    assert(!this->oweovIsOut());
                    this->    WPush();
                    this-> XNotPush();
                    this->NotOutput();
                    this->hasRecOut = false;
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
                if(!this->xeovFIFORightIsFull()
                && !this->xeovFIFOIsEmpty()){
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
                if (this->oweovIsOut()
                 &&!this->hasRecOut){
                    if(!this->oweovFIFODownIsFull()
                    && !this->oweovFIFOIsEmpty()
                    && !this->xeovFIFORightIsFull()
                    && !this->xeovFIFOIsEmpty()
                    && !this->owxFIFOIsFull()){
                        this->    WPush();
                        this->    XPush();
                        this->OutputOut();
                        this->hasRecOut = false;
                    }
                    else
                        this->Stuck();
                }
                else{
                    if(!this->oweovFIFODownIsFull()
                    && !this->oweovFIFOIsEmpty()
                    && !this->xeovFIFORightIsFull()
                    && !this->xeovFIFOIsEmpty()){
                        this->    WPush();
                        this->    XPush();
                        this->NotOutput();
                        this->hasRecOut = false;
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

            this->owReg.ReadBits(ReformedPE::OUT_BIT_WIDTH);
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
                else if(this->EndForward()){
                    if (this->CanForward() && !this->owNotOut())
                        this->ForwardAddOut();
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
            if (this->owxRegEn == 1)
                this->owReg.ReadBits(ReformedPE::W_BIT_WIDTH);
            this->wRegOut = ReformedPE::DecompressOW_W(this->pOWRegOut  .Data());
              xReg .Run(); /**   X-bit **/
            ParSum .Run(); /**   S-bit **/
             multer.Run();
              adder.Run();
        }

        if (this->clock % this->ruPer == 0){
            if ((!this->owNotOut())
             && ( this-> owxFIFOIsEmpty())
             && ( this->_owxFIFOPush == 0)
             &&   this->oweovRegValid()
             &&   this-> xeovRegValid()){
                this-> owxFIFOIn   = 0;
                this->_owxFIFOPush = 1;
            }
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
        this->oweovFIFO.ConnectTo(pOWFullUp,FIFO::Full);
        if (pOWFullUp.Data()==1)
            std::cout<<"(F,";
        else if (this->oweovFIFOIsEmpty())
            std::cout<<"(E,";
        else
            std::cout<<"(0,";

        Port pXFullLeft(1,PortDirection::IN);
        this->xeovFIFO.ConnectTo(pXFullLeft,FIFO::Full);
        if (pXFullLeft.Data()==1)
            std::cout<<"F,";
        else if (this->xeovFIFOIsEmpty())
            std::cout<<"E,";
        else
            std::cout<<"0,";

        if (this->pOWXFIFOFull.Data()==1)
            std::cout<<"F)";
        else if (this->pOWXFIFOEmpty.Data())
            std::cout<<"E)";
        else
            std::cout<<"0)";

        std::cout<<"#"<<setw(2)<<this->wOff()<<","<<this->wIsEnd()<<"_"<<setw(2)<<this->xOff()<<","<<this->xIsEnd()<<"#";
        return;
    }
    inline void Print(){
        /**   up  **/
        Port pOWFullUp(1,PortDirection::IN);
        this->oweovFIFO.ConnectTo(pOWFullUp,FIFO::Full);
        std::cout<<"OWInUp  : "<<this->ports[ReformedPE::  OWInUp.ToIndex()].Data()<<std::endl;
        std::cout<<"OWPushUp: "<<this->ports[ReformedPE::OWPushUp.ToIndex()].Data()<<std::endl;
        std::cout<<"OWFullUp: "<<pOWFullUp.Data()<<std::endl;
        /**  down **/
        Port pOWOutDown(OUT_BIT_WIDTH
                   +      W_BIT_WIDTH
                   + OFFSET_BIT_WIDTH
                   +     XE_BIT_WIDTH,PortDirection::IN);
        this->oweovFIFO.ConnectTo(pOWOutDown,FIFO::Out);
        std::cout<<"OWOutDown : "<<pOWOutDown.Data()<<std::endl;
        std::cout<<"OWPushDown: "<<this->owPushDown<<std::endl;
        std::cout<<"OWFullDown: "<<this->ports[ReformedPE::OWFullDown.ToIndex()].Data()<<std::endl;
        /**  left **/
        Port pXFullLeft(1,PortDirection::IN);
        this->xeovFIFO.ConnectTo(pXFullLeft,FIFO::Full);
        std::cout<<"XInLeft  : "<<this->ports[ReformedPE::  XInLeft.ToIndex()].Data()<<std::endl;
        std::cout<<"XPushLeft: "<<this->ports[ReformedPE::XPushLeft.ToIndex()].Data()<<std::endl;
        std::cout<<"XFullLeft: "<<pXFullLeft.Data()<<std::endl;
        /** right **/
        Port pXOutRight( X_BIT_WIDTH
                  + OFFSET_BIT_WIDTH
                  +     XE_BIT_WIDTH,PortDirection::IN);
        this->xeovFIFO.ConnectTo(pXOutRight,FIFO::Out);
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

        /** Offset **/
        std::cout<<" weo     : "<<ReformedPE::DecompressOWEOV_Off  (this->pOWEOVRegOut.Data())
                                <<","
                                <<ReformedPE::DecompressOWEOV_EOW  (this->pOWEOVRegOut.Data())
                                <<","
                                <<ReformedPE::DecompressOWEOV_Out  (this->pOWEOVRegOut.Data())
                                <<","
                                <<ReformedPE::DecompressOWEOV_Valid(this->pOWEOVRegOut.Data())
                                <<std::endl;
        std::cout<<" wveoFIFO: "<<ReformedPE::DecompressOWEO_Offset(this->pOWEOVFIFOOut.Data())
                                <<","
                                <<ReformedPE::DecompressOWEO_EOW   (this->pOWEOVFIFOOut.Data())
                                <<","
                                <<ReformedPE::DecompressOWEO_Out   (this->pOWEOVFIFOOut.Data())
                                <<std::endl;
        std::cout<<" xeo     : "<<ReformedPE::DecompressXEOV_Off  (this->pXEOVRegOut.Data())
                                <<","
                                <<ReformedPE::DecompressXEOV_EOX  (this->pXEOVRegOut.Data())
                                <<","
                                <<ReformedPE::DecompressXEOV_Valid(this->pXEOVRegOut.Data())
                                <<std::endl;
        std::cout<<" xeoFIFO : "<<ReformedPE::DecompressXEO_Offset(this->pXVEOFIFOOut.Data())
                                <<","
                                <<ReformedPE::DecompressXEO_EOX   (this->pXVEOFIFOOut.Data())
                                <<std::endl;

        std::cout<<"## oweovFIFO ##"<<std::endl;
        this->oweovFIFO.Print();
        std::cout<<"##  xeovFIFO ##"<<std::endl;
        this-> xeovFIFO.Print();
        std::cout<<"## oweovReg ##"<<std::endl;
        this->oweovReg.Print();
        std::cout<<"## xeovReg ##"<<std::endl;
        this->xeovReg.Print();
        std::cout<<"##   owxFIFO ##"<<std::endl;
        this->  owxFIFO.Print();
        std::cout<<"##  owReg ##"<<std::endl;
        this->owReg .Print();
        std::cout<<"##   xReg ##"<<std::endl;
        this->xReg  .Print();
        std::cout<<"## ParSum ##"<<std::endl;
        this->ParSum.Print();
        std::cout<<"##     sFIFO ##"<<std::endl;
        this->    sFIFO.Print();
        std::cout<<"##   _owxFIFOPop   ##"<<std::endl;
        std::cout<<this->_owxFIFOPop<<std::endl;
        std::cout<<"##   _owxFIFOPush   ##"<<std::endl;
        std::cout<<this->_owxFIFOPush<<std::endl;
        std::cout<<"## Adder ##"<<std::endl;
        this->adder.Print();
        std::cout<<"## Multer ##"<<std::endl;
        this->multer.Print();
    }
};

#endif // __COMPONENT_REFORMED_PE_HPP
