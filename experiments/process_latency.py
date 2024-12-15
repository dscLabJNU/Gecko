#!/usr/bin/env python3

import sys
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import matplotlib.lines as plt_lines
import numpy as np
import math
import process_utility as u
import process_bulk_data_latency as pbdl

aggs_sorted = [
            #    "daba", "daba_lite",
            #    "two_stacks", "two_stacks_lite",
            #    "rb_two_stacks_lite", "chunked_two_stacks_lite",
            #    "flatfit",
            #    "bfinger4",
            "ffiba4", "ffiba8",
            # "ffiba8", "ffiba16",
            "bclassic4", "bclassic8", 
            # #"bfinger2", 
            "bfinger4", "bfinger8"
              ]

windows = [4*u.MB]

functions = [
             "sum",
             "geomean",
             "bloom",
            ]

def main():
    u.create_figures_dir()
    u.set_fonts()
    plt.rcParams['xtick.labelsize'] = 12

    preamble = 'latency'

    for f in functions:
        for w in windows:
            data = {}
            exp_name = f + '_w' + str(w)
            window_size = '$2^{' + str(int(math.log(w, 2))) + '}$'
            exp_title = f + ', window ' + window_size
            for agg in aggs_sorted:
                data[agg] = u.read_latency_data('results/' + preamble + '_' + agg + '_' + exp_name + '.txt')
            print(exp_name)
            our_max_y_axis = {
                    "sum": 1e8,
                    "geomean": 1e8,
                    "bloom": 1.1e9,
                }
            our_min_y_axis = {
                "sum": 1e2,
                "geomean": 1e2,
                "bloom": 5e3,

            }
            # u.make_violin_graph(data, aggs_sorted, exp_name, exp_title, preamble, force_bottom=False, 
            #                            use_custom_y={"max_yaxis": our_max_y_axis, "min_yaxis": our_min_y_axis}, grid_line=True)
            pbdl.make_violin_graph(data, aggs_sorted, exp_name, exp_title, preamble, f, force_bottom=False, 
                                       use_custom_y={"max_yaxis": our_max_y_axis, "min_yaxis": our_min_y_axis}, grid_line=True)
if __name__ == "__main__":
    main()
