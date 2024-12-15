#include "benchmark_core.h"

#include "DABA.hpp"
#include "DABALite.hpp"
#include "TwoStacks.hpp"
#include "TwoStacksLite.hpp"
#include "ImplicitTwoStacksLite.hpp"
#include "FlatFIT.hpp"
#include "DynamicFlatFIT.hpp"
#include "ImplicitQueueABA.hpp"
#include "SubtractOnEvict.hpp"
#include "ReCalc.hpp"
#include "Reactive.hpp"
#include "OkasakisQueue.hpp"
#include "Reactive.hpp"
#include "FiBA.hpp"
#include "gecko.hpp"
#include "CPiX.hpp"
#include "DataGenerators.h"
// #include "data_benchmark.cc"

#include<iostream>
typedef uint64_t timestamp;


#define KB 1024
#define MB 1024 * KB
#define MILLION int(1e6)

// #define SECOND  10**3
// #define MINUTE  60*SECOND
// #define HOUR  60*MINUTE

int main(){
    using namespace GECKO;
    // size_t base_window_sizes[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512,
    // 1*KB, 2*KB, 4*KB, 8*KB, 16*KB, 32*KB, 64*KB, 128*KB, 256*KB, 512*KB,
    // 1*MB, 2*MB, 4*MB};
    // size_t base_window_sizes[] = {32, 64, 128, 256, 512,
    // 1*KB, 2*KB, 4*KB, 8*KB, 16*KB, 32*KB, 64*KB, 128*KB, 256*KB, 512*KB,
    // 1*MB, 2*MB, 4*MB};

    size_t base_window_sizes[] = {4*MB};
    size_t degrees[] = {
    1*KB, 2*KB, 4*KB, 8*KB, 16*KB, 32*KB, 64*KB, 128*KB, 256*KB, 512*KB,
    1*MB, 2*MB};
    // size_t degrees[] = {
    // 1*KB};
    // size_t durations[] = {10,
    //          100,
    //          SECOND,
    //          5*SECOND,
    //          10*SECOND,
    //          30*SECOND,
    //          MINUTE,
    //          100*SECOND,
    //          15*MINUTE,
    //          1000*SECOND,
    //          30*MINUTE,
    //          HOUR, 2*HOUR,
    //          10000*SECOND,
    //          3*HOUR, 4*HOUR, 5*HOUR, 6*HOUR};

    // size_t degrees[] = {4*MB};
    size_t base_iterations = 50 * MILLION;
    // size_t base_window_sizes[] = {16};
    // size_t base_iterations = 10;
    size_t bulk_size = 1*KB;
    

    std::string aggregator("gecko");
    // std::string aggregator("bfinger2");
    std::string function("sum");
    std::vector<cycle_duration> latencies;
    

    for(size_t win_sz : base_window_sizes){
        for(size_t degree: degrees) {
            size_t iterations = base_iterations + win_sz;
        Experiment exp(win_sz, iterations, degree, bulk_size, false, latencies);
        // query_call_benchmark<GECKO::MakeAggregate, timestamp>("gecko", "gecko", function, exp, exp.window_size);
        Sum<int> f;
        typedef typename Sum<int>::Partial aggT;

        // Aggregate<Sum<int>,timestamp> agg = GECKO::MakeAggregate<Sum<int>,timestamp>()(0,win_sz);
        // Aggregate<Sum<int>,timestamp,4> agg = MakeAggregate<Sum<int>,timestamp,4>()(0, win_sz);
        // Aggregate<Sum<int>,timestamp,4, finger > agg = MakeAggregate<Sum<int>,timestamp,4,finger>()(0, win_sz);
        // CPIX::Aggregate<Sum<int>,timestamp> rec_agg = CPIX::MakeAggregate<Sum<int>,timestamp>()(0,win_sz);
        
        // query_call_data_benchmark<DataSet, GECKO::MakeAggregate>("gecko", aggregator, function, exp, win_sz, gen, out);
        // query_call_bulk_evict_benchmark<btree::MakeAggregate, timestamp, 4, btree::finger>("bfinger4", aggregator, function, exp);
        // query_call_bulk_evict_benchmark<CPIX::MakeAggregate, timestamp>("CPiX", "CPiX", function, exp, exp.window_size);
        // query_call_bulk_evict_benchmark<GECKO::MakeAggregate, timestamp>("gecko", aggregator, function, exp, exp.window_size);
        
        // query_call_benchmark<recalc::MakeAggregate>("recalc", "recalc", function, exp);
        // query_call_ooo_benchmark<CPIX::MakeAggregate, timestamp>("CPiX", "CPiX", function, exp, exp.window_size);
        // query_call_benchmark<btree::MakeAggregate, timestamp, 2, btree::classic>("bclassic2", "bclassic2", function, exp);
        // query_call_ooo_benchmark<btree::MakeAggregate, timestamp, 2, btree::finger>("bfinger2", "bfinger2", function, exp);
        // query_call_benchmark<btree::MakeAggregate, timestamp, 4, btree::finger>("bfinger4", "bfinger4", function, exp);
        //test for fifo
        
        // typename Aggregate<Sum<int>,timestamp>::outT force_side_effect = typename Aggregate<Sum<int>,timestamp>::outT();
        // for (uint64_t i = 0; i < exp.window_size; ++i) {           
        //     // agg.insert(time++,1 + (i % 101));
        //     agg.insert(1 + (i % 101));
        //     rec_agg.insert(1 + (i % 101));
        // }
        
        // if (agg.size() != exp.window_size) {
        //     exit(2);
        // }
        // // cout << "开始逐出" <<endl;
        // auto start = std::chrono::system_clock::now();
        // for (uint64_t i = exp.window_size; i < exp.iterations; ++i) {
        //     std::atomic_thread_fence(std::memory_order_seq_cst);
        //     agg.evict();
        //     // agg.insert(time++,1 + (i % 101));
        //     agg.insert(1 + (i % 101));
        //     rec_agg.evict();
        //     rec_agg.insert(1 + (i % 101));
        //     if(agg.query() != rec_agg.query()){
        //         cout <<exp.iterations << "  第"<< i-exp.window_size << " 次插入结果错误！" << 1 + (i % 101)  << " "  << agg.query() << " " << rec_agg.query() <<endl;
        //         break;
        //     }
        //     // cout << i <<endl;
        //     // cout << i-exp.window_size << " "  << agg.query() << " " << rec_agg.query() <<endl;
        //     silly_combine(force_side_effect, agg.query());
        //     silly_combine(force_side_effect, rec_agg.query());
        // }
        // std::chrono::duration<double> runtime = std::chrono::system_clock::now() - start;
        // std::cout << "winsz " << win_sz << std::endl;
        // std::cout << "core runtime: " << runtime.count() << std::endl;
        }
        
    }


    
    
    







    // // test for ooo
    // for(size_t win_sz : base_window_sizes){
    //     for(size_t degree : degrees){
    //         size_t iterations = base_iterations + win_sz;
    //         Experiment exp(win_sz, iterations,degree,  false, latencies);
    //         timestamp time= 0;
    //         Aggregate<Sum<int>,timestamp> agg = CPIX::MakeAggregate<Sum<int>,timestamp>()(0,win_sz);
    //         btree::Aggregate<timestamp, 4, btree::finger, Sum<int>> fiba_agg = btree::MakeAggregate<Sum<int>,timestamp, 4, btree::finger>()(0);
    //         typename Aggregate<Sum<int>,timestamp>::outT force_side_effect = typename Aggregate<Sum<int>,timestamp>::outT();
    //         typename Aggregate<Sum<int>,timestamp>::timeT i = 0;

    //         std::cout << "window size " << exp.window_size << ", iterations " << exp.iterations << std::endl;
            
    //             for (i = exp.iterations - exp.ooo_distance; i < exp.iterations; ++i) {
    //                 agg.insert(i, 1 + (i % 101));
    //                 // cout << "插入成功" <<endl;
    //                 fiba_agg.insert(i, 1 + (i % 101));
    //             }
    //             for (i = 0; i < exp.window_size - exp.ooo_distance; ++i) {
    //                 agg.insert(i, 1 + (i % 101));
                    
    //                 fiba_agg.insert(i, 1 + (i % 101));
    //             }
    //             // cout << "插入完成" <<endl;
    //             if (agg.size() != exp.window_size) {
    //                 std::cerr << "window is not exactly full; should be " << exp.window_size << ", but is " << agg.size();
    //                 exit(2);
    //             }

    //             auto start = std::chrono::system_clock::now();

    //             for (i = exp.window_size - exp.ooo_distance; i < exp.iterations - exp.ooo_distance; ++i) {
    //                 std::atomic_thread_fence(std::memory_order_seq_cst);

    //                 agg.evict();
    //                 agg.insert(i, 1 + (i % 101));
    //                 fiba_agg.evict();
    //                 fiba_agg.insert(i, 1 + (i % 101));
    //                 if(agg.query() != fiba_agg.query()){
    //                     cout << "结果错误！" << i % 101  << " "  << agg.query() << " " << fiba_agg.query() <<endl;
    //                     break;
    //                 }
    //                 // silly_combine(force_side_effect, agg.query());
    //             }

    //             std::chrono::duration<double> runtime = std::chrono::system_clock::now() - start;
    //             std::cout << "core runtime: " << runtime.count() << std::endl;
    //             std::cerr << force_side_effect << std::endl;
    //     }
    // }
    
}

