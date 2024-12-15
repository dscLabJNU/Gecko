#ifndef __CPIX_H__
#define __CPIX_H__

#include <deque>
#include <math.h>
#include <cstring>
#include <iostream>

#ifdef COLLECT_STATS
#define IF_COLLECT_STATS(x) x
#else
#define IF_COLLECT_STATS(x) ;
#endif

#ifdef DEBUG
#define _IFDEBUG(x) x
#else
#define _IFDEBUG(x)
#endif
IF_COLLECT_STATS(static long statsCombineCount = 0);

typedef typename std::chrono::milliseconds timeD;

namespace TimeStampedCPiX{
    using namespace std;

    template<typename binOpFunc>
    class BinaryTree{
    typedef typename binOpFunc::In inT;
    typedef typename binOpFunc::Partial aggT;
    typedef typename binOpFunc::Out outT;
    public:
    BinaryTree(binOpFunc binOp_, aggT identE_ , size_t initial_size)
      : sz(compute_size(initial_size)), q(new aggT[sz]),  cap(compute_size(initial_size)/2),
        frontPtr(0), backPtr(-1),
        binOp(binOp_), identE(identE_)
    {}

    
    void init(){
      frontPtr = 0;
      backPtr = -1;
    }
    
    bool isEmpty(){
      return frontPtr == backPtr+1;
    }

    size_t compute_size(size_t initial_size){
      size_t n = initial_size-1;
      n |= n>> 1;
      n |= n>> 2;
      n |= n>> 4;
      n |= n>> 8;
      n |= n>> 16;
      n |= n>> 32;
      return (n+1)<<1;
    }
    
    void insert(inT v) {
      backPtr = (1+backPtr)%cap;
      q[cap + backPtr] = binOp.lift(v);
    }

    void updateTree(size_t _tcp, aggT v){
      size_t ei = cap+_tcp;
      q[ei] = v;
      while(1) {
          size_t pb=ei/2;
          if (pb < 1) break; // root reached
          q[pb] = binOp.combine(q[2*pb], q[2*pb+1]);
          ei=pb;
      }
    }

    void create_bintree(aggT *pValue, size_t _tcc, size_t _tcp){
      init();
      memcpy(&q[cap], &pValue[_tcc], sizeof(aggT)*_tcp);
      backPtr = _tcp-1;
      propagate();

    }
  
    void evict() {
      assert(sz>0);
      size_t ei = cap+frontPtr;
      q[ei] = identE;
      while(1) {
          size_t pb=ei/2;
          if (pb < 1) break; // root reached
          q[pb] = binOp.combine(q[2*pb], q[2*pb+1]);
          ei=pb;
          }
      ++frontPtr;
    }

    size_t size(){
      return backPtr - frontPtr + 1;
    }
  
    aggT query() {
      return q[1];
    }

    inline
    bool ok(size_t i, size_t bi, size_t fi) {
        return i>=fi && i<=bi;
    }

    inline
    aggT propagate() {
      if (sz==0) return identE;
      size_t fi = cap+frontPtr, bi = cap+backPtr;
      while(1) {
          size_t pb=fi/2;
          size_t pe=bi/2;
          if (pb < 1) break; // root reached
          for (size_t p=pb;p<=pe;++p) {
            size_t lp = 2*p, rp = 2*p+1;
            bool ok_l = ok(lp,bi,fi);
            bool ok_r = ok(rp,bi,fi);
            //p的两个孩子节点均合法 更新p-value
            if (ok_l&&ok_r) q[p] = binOp.combine(q[lp], q[rp]);
            else if (ok_l) q[p] = q[lp];
            else q[p] = q[rp];
            //            std::cerr << "combining: " << left << " + " << right << std::endl;
          }
          fi =pb; bi =pe;
        }
      return q[1];
    }

    private:
    size_t cap, sz;
    size_t frontPtr, backPtr;
    aggT *q;
    binOpFunc binOp;
    aggT identE;
  };

    

    template<typename binOpFunc ,
             typename _timeT>
    class Aggregate {
    public:
        typedef typename binOpFunc::In inT;
        typedef typename binOpFunc::Partial aggT;
        typedef typename binOpFunc::Out outT;
        typedef _timeT timeT;
        
    
    Aggregate(binOpFunc binOp_, aggT identE_, int size_)
      :  _binOp(binOp_) ,
         _identE(identE_) ,
         sz(0) , max_sz(size_),
         k(computeK(size_)) , 
         block(size_/k),
         _cc(0),_cp(0),
         _tcc(0),_tcp(0),
         pValue(new aggT[size_]{}), 
         cValue(new aggT[k]{}),
         gValue(identE_),
         full(false),
         tValue(BinaryTree<binOpFunc>(binOp_, identE_, size_-(k-1)*block))
         {}
    
    Aggregate(binOpFunc binOp_, aggT identE_, timeD size_, timeD slide_)
      :  _binOp(binOp_) ,
         _identE(identE_) ,
         sz(0) , max_sz(computeMaxSz(size_, slide_)),
         k(computeK(max_sz)) , 
         block(max_sz/k),
         _cc(0),_cp(0),
         _tcc(0),_tcp(0),
         slide(slide_),
         slice(slide_),
         pValue(new aggT[max_sz]{}), 
         cValue(new aggT[k]{}),
         gValue(identE_),
         full(false),
         evictCnt(0),
         tValue(BinaryTree<binOpFunc>(binOp_, identE_, max_sz-(k-1)*block))
         {}

    size_t size() { return sz; }

    size_t computeMaxSz(timeD size, timeD slide){
      size_t milliSize = size.count();
      size_t milliSlide = slide.count();
      return milliSize % milliSlide == 0 ? (milliSize / milliSlide) : (milliSize / milliSlide) + 1;
    }

    size_t computeK(size_t size){
      return ceil(sqrt(size/log(10)));
    }


    void insert(timeT const& time, inT const& v){
      if(0 == sz){
        startTime = time;
        currTime = time;
        ++sz;
      }

      aggT lifted = _binOp.lift(v);
      if (time >= currTime)
      {
        currTime = time;
        _cp = (_tcp + (std::chrono::duration_cast<std::chrono::milliseconds>(time - startTime).count() / slice.count())) % max_sz;
        _cc = _cp / block;
        if(_cc == k) _cc = k-1;
        if (_cp < _tcp + tValue.size())
        {
          tValue.updateTree(_cp, lifted);
          return;
        }
        pValue[_cp] = lifted;
        cValue[_cc] = _binOp.combine(cValue[_cc],lifted);
        gValue = _binOp.combine(gValue,lifted);
        if (_cp > 0)
        {
          full = true;
        }

        if(full && tValue.isEmpty()){
        size_t _treeCp;
        _tcc = _tcp / block;
        if(_tcc == k) _tcc = k-1;
        _treeCp = _tcc == k-1 ?  max_sz-(k-1)*block : block;
        cValue[_tcc] = _identE;

        if(0 == _tcp % block){
          tValue.create_bintree(pValue, _tcc*block , _treeCp);
        }else{
          size_t dis = _tcp % block;
          tValue.create_bintree(pValue, _tcc*block + dis, _treeCp - dis);
          for(int i=0; i<dis; i++){
            cValue[_tcc] = _binOp.combine(cValue[_tcc], pValue[_tcc*block + i]);
          }
        }

        gValue = _identE;
          for (size_t i = 0; i < k; i++)
          {
            gValue = _binOp.combine(cValue[i], gValue);

          }
        }
      } else {
        size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - time).count();
        if (diff < max_sz)
        {
          size_t sliceP = (_tcp + (diff / slice.count())) % max_sz;
          insertIntoSlice(sliceP, lifted);
        }
      }
    }

    void insertIntoSlice(size_t sliceP, aggT const& lifted){
      _cc = sliceP/block;
      if(_cc == k) _cc = k-1;
      pValue[sliceP] = lifted;
      cValue[_cc] = _binOp.combine(cValue[_cc],lifted);
      gValue = _binOp.combine(gValue,lifted);
    }

    void insertSlice(aggT const& lifted){
      ++sz;
      _cc = _cp/block;
      if(_cc == k) _cc = k-1;
      pValue[_cp++] = lifted;
      cValue[_cc] = _binOp.combine(cValue[_cc],lifted);
      gValue = _binOp.combine(gValue,lifted);
      
      if(_cp == max_sz){
        full=true;
        _cp=0;
        _cc=0;
      }
    }

    void bulkEvict(timeT const& time){
      if(time < startTime) {
          return;
      }
      while (time > startTime)
      {
        evict();
      }
    }

    timeT youngest() const {
      return currTime;
    }

    timeT oldest() const {
      return startTime;
    }
    

    void evict(){
      // if(sz <1) return;
      // 窗口已满 应建树
      if(full && tValue.isEmpty()){
        size_t _treeCp;
        _tcc = _tcp / block;
        if(_tcc == k) _tcc = k-1;
        _treeCp = _tcc == k-1 ?  max_sz-(k-1)*block : block;
        cValue[_tcc] = _identE;

        if(0 == _tcp % block){
          tValue.create_bintree(pValue, _tcc*block , _treeCp);
        }else{
          size_t dis = _tcp % block;
          tValue.create_bintree(pValue, _tcc*block + dis, _treeCp - dis);
          for(int i=0; i<dis; i++){
            cValue[_tcc] = _binOp.combine(cValue[_tcc], pValue[_tcc*block + i]);
          }
        }
        gValue = _identE;
          for (size_t i = 0; i < k; i++)
          {
            gValue = _binOp.combine(cValue[i], gValue);
          }
      }
      
      if(!tValue.isEmpty()){
        tValue.evict();
        --sz;
        _tcp = (_tcp + 1) % max_sz;
        startTime += slice;
        evictCnt++;
      }
      // cout << "startTime && endTIme："  << youngest() << "  " << oldest() << "  " << oldest() - youngest() << endl; 
    }

    outT query() {
      return _binOp.lower(_binOp.combine(tValue.query(), gValue));
    }
    private:
        size_t sz , max_sz , k ,block;  
        binOpFunc _binOp;
        size_t _cc,_cp;
        size_t _tcc,_tcp;
        timeT startTime;
        timeT currTime;
        timeD slice;
        timeD slide;
        // timeD stepSize;
        aggT _identE;
        bool full;
        aggT *pValue;
        aggT *cValue;  //坐标为checkpoint，存放cValue
        aggT gValue;
        BinaryTree<binOpFunc> tValue;  //指向二叉树
    public:
        size_t evictCnt = 0;
};
 
  
  template <typename timeT, class BinaryFunction , class T>
  Aggregate<BinaryFunction, timeT> make_aggregate(BinaryFunction f, T elem , size_t size) {
    return Aggregate<BinaryFunction, timeT>(f, elem ,size);
  }

  template <typename timeT, class BinaryFunction , class T>
  Aggregate<BinaryFunction, timeT> make_aggregate(BinaryFunction f, T elem, timeD size, timeD slide) {
    return Aggregate<BinaryFunction, timeT>(f, elem, size, slide);
  }

  template <typename BinaryFunction, typename timeT>
  struct MakeAggregate {
    template <typename T>
    Aggregate<BinaryFunction, timeT> operator()(T elem ,size_t size) {
      BinaryFunction f;
      return make_aggregate<timeT, BinaryFunction>(f, elem , size);
    }

    template <typename T>
    Aggregate<BinaryFunction, timeT> operator()(T elem ,timeD size, timeD slide) {
      BinaryFunction f;
      return make_aggregate<timeT, BinaryFunction>(f, elem , size, slide);
    }
  };
}; 



#endif