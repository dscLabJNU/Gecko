#!/usr/bin/env python3

import process_utility as u
import sys, collections


aggregators = [
               u.Aggregator("daba", '-', 'black'),
               u.Aggregator("daba_lite", '--', 'black'),
               u.Aggregator("two_stacks", '-', 'red'),
               u.Aggregator("two_stacks_lite", '--', 'red'),
               u.Aggregator("rb_two_stacks_lite", '-', 'darkorchid'),
               u.Aggregator("chunked_two_stacks_lite", '--', 'darkorchid'),
               u.Aggregator("flatfit", '-', 'blue'),
               u.Aggregator("bfinger4", '-', 'green'),
              ]

windows = [
           1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 
           1*u.KB, 2*u.KB, 4*u.KB, 8*u.KB, 16*u.KB, 32*u.KB, 64*u.KB, 128*u.KB, 256*u.KB, 512*u.KB,
           1*u.MB, 2*u.MB, 4*u.MB
          ]

functions = [ "sum",
             "geomean",
             "bloom",
            #  "relvar", 
            ]

def main():
    u.create_figures_dir()
    u.set_fonts()

    agg_to_func = collections.defaultdict(lambda: collections.defaultdict(dict))
    for agg in aggregators:
        agg_to_func[agg.name] = u.read_mfgdebs_throughput(agg.name, functions)

    u.make_mfgdebs_small_window_varying_graphs('DEBS 2012', 'window size in seconds', aggregators, 
                                       u.func_varying_small_window(agg_to_func))
if __name__ == "__main__":
    main()
