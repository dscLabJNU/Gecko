#include "benchmark_core.h"
#include "DataGenerators.h"

#include "gecko.hpp"
#include "CPiX.hpp"

#include<iostream>

#define SECOND  1000
#define MINUTE  60*SECOND
#define HOUR  60*MINUTE

#define KB 1024
#define MB 1024 * KB
#define MILLION int(1e6)

int main(){
    using namespace GECKO;
    
    size_t durations[] = {
            // 10,
            //  100,
            //  SECOND,
             5*SECOND,
             10*SECOND,
             30*SECOND,
             MINUTE,
             100*SECOND,
             15*MINUTE,
             1000*SECOND,
             30*MINUTE,
             HOUR, 2*HOUR,
             10000*SECOND,
             3*HOUR, 4*HOUR, 5*HOUR, 6*HOUR};

    std::string data_file = "../../experiments/data/201812-citibike-tripdata.csv";
    FileDataGenerator<CitiBikeCsv> gen(data_file, CitiBikeCsv::skip_lines());
    gen.load();

    std::vector<cycle_duration> latencies;
    std::vector<uint32_t> evictions;
    bool latency = false;

    size_t size = sizeof(durations) / sizeof(*durations);
    for (size_t i = 0; i < size; i++)
    {
        size_t window_duration = durations[i];
        std::string result_file = "test_gecko_bulk_data_.csv";
        std::ofstream out;
        if (!latency) { // open & write the header tag for throughput experiments
            out.open(result_file, std::ios::app);
            if (!out) {
                std::cerr << "could not open " + result_file << std::endl;
                throw std::invalid_argument(result_file);
            }
            out << "CPIX" << "," << window_duration;
        }
        using SumOp = Sum<CitiBikeCsv,int,int>;
        
        DataExperiment exp(std::chrono::milliseconds(window_duration), latency, latencies, evictions);
        std::chrono::milliseconds slice = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(1));
        cout << exp.window_duration.count() << endl;
        // MakeAggregate<SumOp, typename CitiBikeCsv::timestamp>()(SumOp::identity, exp.window_duration, slice);
        // query_call_bulk_data_benchmark<CitiBikeCsv, CPIX::MakeAggregate>("CPIX", "CPIX", "sum", exp, exp.window_duration, gen, out);
        query_call_bulk_data_benchmark<CitiBikeCsv, GECKO::MakeAggregate>("gecko", "gecko", "bloom", exp, exp.window_duration, gen, out);
    }
}