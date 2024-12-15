#ifndef __GECKO_H__
#define __GECKO_H__

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


typedef typename std::chrono::milliseconds timeD;

namespace TimedStampedGecko{
    using namespace std;
    IF_COLLECT_STATS(static long statsCombineCount = 0L);
    template<typename binOpFunc>
    class BinaryTree{
    typedef typename binOpFunc::In inT;
    typedef typename binOpFunc::Partial aggT;
    typedef typename binOpFunc::Out outT;
    public:
    BinaryTree(binOpFunc binOp_, aggT identE_ , size_t initial_size)
      : sz(compute_size(initial_size)), q(new aggT[sz]),  cap(compute_size(initial_size)/2),
        frontPtr(0), backPtr(-1), resIndex(cap/2), count(0), point(2),
        binOp(binOp_), identE(identE_)
    {}

    
    void init(){
      frontPtr = 0;
      backPtr = -1;
    }
    
    bool isEmpty(){
      return frontPtr == backPtr + 1;
    }

    size_t size(){
      return backPtr - frontPtr + 1;
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

    void create_bintree(aggT *pValue, size_t _tcc, size_t _tcp){
      memcpy(&q[cap], &pValue[_tcc], sizeof(aggT)*_tcp);
      resIndex = cap/2;
      frontPtr = 0;
      backPtr = _tcp-1;
      count = 0;
      point = 2;
      assert(cap > backPtr);
      propagateUp();
      propagateDown();
      // for(int i=0; i<sz;i++){
      //   cout << i << "::  " << q[i] <<endl;
      // }
    }
  
    void evict() {
      
      size_t ei = cap+frontPtr;
      q[ei] = identE;
      int cnt = count;
      while(cnt > 0) {
          --cnt;
          ei/=2;
          q[ei] = binOp.combine(q[2*ei], q[2*ei+1]);
          if(ei == 3) break;
      }
      ei/=2;
      if(ei != 2){
          q[ei] = binOp.combine(q[ei/2], q[2*ei+1]);
      }else{
          q[2] = binOp.combine(q[3], q[2*ei+1]);
      }
      resIndex = ei;
      ++frontPtr;
      if(frontPtr == point) {
        ++count;
        point *= 2;
      }
    }

    void updateTree(size_t _cp, aggT v) {
      
      size_t ei = cap+_cp;
      q[ei] = binOp.combine(q[ei], v);
      int cnt = count;
      while(cnt > 0) {
          --cnt;
          ei/=2;
          q[ei] = binOp.combine(q[2*ei], q[2*ei+1]);
          if(ei == 3) break;
      }
      ei/=2;
      if(ei != 2){
          q[ei] = binOp.combine(q[ei/2], q[2*ei+1]);
      }else{
          q[2] = binOp.combine(q[3], q[2*ei+1]);
      }
    }

    void evict(size_t t) {
      frontPtr += t - 1;
      size_t ei = cap+frontPtr;
      q[ei] = identE;

      size_t tempT = t;
      while (tempT > 1)
      {
        point *= 2;
        tempT /= 2;
        ei /= 2;
        q[ei] = identE;
        if(ei == 3) break;
      }
      ei/=2;
      if(ei != 2){
          q[ei] = binOp.combine(q[ei/2], q[2*ei+1]);
      }else{
          q[2] = binOp.combine(q[3], q[2*ei+1]);
      }

      resIndex = ei;
      ++frontPtr;
      if(frontPtr == point) {
        ++count;
        point *= 2;
      }
    
    }
  
    aggT query() {
      if(frontPtr-1 >=  cap/2){
        return q[3];
      }
      return q[resIndex];
    }

    inline
    bool ok(size_t i, size_t bi, size_t fi) {
        return i>=fi && i<=bi;
    }

    inline
    void propagateUp() {
      // if (sz==0) return identE;
      size_t fi = cap+frontPtr, bi = cap+backPtr;
      while(1) {
          size_t pb=fi/2+1;
          size_t pe=bi/2;
          if (pb < 3) break; // root-right-child reached
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
    }

    inline 
    void propagateDown() {
      size_t fi = 2;
      q[fi] = binOp.combine(q[3],q[5]);
      while(1){
        size_t pb = fi*2;
        if(pb == cap) break;
        q[pb] = binOp.combine(q[pb+1],q[fi]);
        fi = pb;
      }
      q[cap/2] = binOp.combine(q[cap],q[cap+1]);
      q[cap/2] = binOp.combine(q[cap/2],q[cap/4]);
    }

  

    private:
    size_t cap, sz;
    int frontPtr, backPtr;
    aggT *q;
    binOpFunc binOp;
    aggT identE;
    size_t resIndex;
    int count;
    int point;
  };
 
    

    template<typename binOpFunc ,
             typename _timeT >
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
         sliceSize(slice.count()),
         blockSize(block * sliceSize),
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
      // cout << "正在插入，当前时间：" << std::chrono::time_point_cast<std::chrono::hours>

      aggT lifted = _binOp.lift(v);
      if (time >= currTime)
      {
        currTime = time;
        _cp = (_tcp + (size_t)(std::chrono::duration_cast<std::chrono::milliseconds>(time - startTime) / slice)) % max_sz;
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

    void insert(inT const& v){
      ++sz;
      aggT lifted = _binOp.lift(v);
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
    

    void evict(){
      // 窗口已满 应建树
      // if(sz <1) return;
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
            // cout<< i << " cValue::" <<cValue[i] <<endl;
          }
          // cout<< "gValue::" <<gValue <<endl;
      }
      if(!tValue.isEmpty()){
        // cout << "正在逐出:" << tValue.size() << endl;
        tValue.evict();
        // --sz;
        _tcp = (_tcp + 1) % max_sz;
        startTime += slice;
        // cout << evictCnt++ << endl;
      }
    }

    // void bulkEvict(timeT const& time){
    //   if(time < startTime) {
    //       return;
    //   }
    //   while (time > startTime)
    //   {
    //     evict();
    //   }
    // }

    void bulkEvict(timeT const& time) {
        if(time < startTime) {
          return;
        }
        size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(time - startTime).count() / sliceSize;
        
        // 若树中仍有剩余
         if (!tValue.isEmpty() && diff >= tValue.size())
        {
          diff -= tValue.size();
          _tcp = (_tcp + tValue.size()) % max_sz;
          startTime += std::chrono::milliseconds(tValue.size() * sliceSize);
          tValue.init();
        }

        size_t blockNum = diff / block;
        if (blockNum > 0)
        {
          _tcc = _tcp / block;
          _tcc = _tcc == k ? k - 1 : _tcc;
          size_t _ntcc = _tcc + blockNum;
          if (_ntcc >= k)
          {
            fill(cValue + _tcc, cValue + k - 1, _identE);
            _ntcc = _ntcc % k;
            fill(cValue, cValue + _ntcc, _identE);
          } else{
            fill(cValue + _tcc, cValue + _ntcc, _identE);
          }
          startTime += std::chrono::milliseconds(blockNum * blockSize);
          _tcp = (_tcp + blockNum * block) % max_sz;
        }
        
        diff = diff % block;
        if (!tValue.isEmpty())
        {
          
          tValue.evict(diff);
        } else{
          _tcc = _tcp / block;
          if(_tcc == k) _tcc = k-1;
          size_t _treeCp = _tcc == k-1 ?  max_sz-(k-1)*block : block;
          cValue[_tcc] = _identE;
          tValue.create_bintree(pValue, _tcc*block + diff, _treeCp - diff);
          gValue = _identE;
          for (size_t i = 0; i < k; i++)
          {
            gValue = _binOp.combine(cValue[i], gValue);
          }
        }
        startTime += std::chrono::milliseconds(diff * sliceSize);
    }

    timeT youngest() const {
      return currTime;
    }

    timeT oldest() const {
      return startTime;
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
        timeD stepSize;
        size_t sliceSize, blockSize;
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
  Aggregate<BinaryFunction, timeT > make_aggregate(BinaryFunction f, T elem , size_t size) {
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