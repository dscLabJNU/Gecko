#!/usr/bin/env python3

import sys, collections
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import math
import numpy as np
import process_utility as u
from run_ooo import base_iterations

Line = collections.namedtuple('Line', ['style', 'color'])

aggregators = {#"bfinger2":   Line('-',  'red'),
               "ffiba4"  :   Line('-',  'red'),
               "ffiba8"  :   Line('--',  'red'),
               "bclassic4":   Line('-',  'blue'),
               "bclassic8":   Line('-',  'green'),
               #"bclassic2":  Line('--',  'red'),
               "bfinger4":  Line('--',  'blue'),
               "bfinger8":  Line('--',  'green'),
               }
aggs_sorted = [#"bclassic2", 
        "ffiba4", "ffiba8",
        "bclassic4", "bclassic8", 
        #"bfinger2", 
        "bfinger4", "bfinger8"]

# distances = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 
#            1*u.KB, 2*u.KB, 4*u.KB, 8*u.KB, 16*u.KB, 32*u.KB, 64*u.KB, 128*u.KB, 256*u.KB, 512*u.KB,
#            1*u.MB, 2*u.MB, 4*u.MB]

# windows = [4*u.MB] #[1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 
#            #1*u.KB, 2*u.KB, 4*u.KB, 8*u.KB, 16*u.KB, 32*u.KB, 64*u.KB, 128*u.KB, 256*u.KB, 512*u.KB,
#            #1*u.MB, 2*u.MB, 4*u.MB]

# X_AXIS = [
#     1,
#     2,
#     2**3,
#     2**5,
#     2**7,
#     2**9,
#     2**11,
#     2**13,
#     2**15,
#     2**17,
#     2**19,
#     2**21,
# ]

distances = [1*u.KB]
windows = [
           1*u.KB, 2*u.KB, 4*u.KB, 8*u.KB, 16*u.KB, 32*u.KB, 64*u.KB, 128*u.KB, 256*u.KB, 512*u.KB,
           1*u.MB, 2*u.MB, 4*u.MB]
X_AXIS = [
    2**11,
    2**13,
    2**15,
    2**17,
    2**19,
    2**21,
]
functions = {"sum": base_iterations,
             "geomean": base_iterations,
             "bloom": base_iterations//2,
            }



def make_throughput_panel(preamble, functions, mappings, varying, x_ticks=X_AXIS, x_labels=None):
    graph, axes = plt.subplots(1, 3, figsize=(24*0.75, 6*0.75))
    axes[0].set_ylabel("throughput [million items/s]")
    axes[len(axes)//2].set_xlabel(varying)
    for i, ax in enumerate(axes):
        print(i)
        ax.set_title(functions[i])
        ax.set_xscale("log", basex=2)
        if x_labels:
            ax.set_xticks(x_ticks,  x_labels)
        else:
            ax.set_xticks(x_ticks)
        ax.set_xlim(min(x_ticks), max(x_ticks))
        for agg in aggs_sorted:
            
            data = mappings[i][agg]

            if len(data) <= 1:
                return
            x_axis = np.array(sorted(data.keys()))
            throughput = np.array([data[w].avg for w in sorted(data.keys())]) / 1e6

            stddev = np.array([data[w].std for w in sorted(data.keys())]) / 1e6
            ax.errorbar(
                x_axis,
                throughput,
                yerr=stddev,
                label=agg,
                linewidth=2,
                linestyle=aggregators[agg].style,
                color=aggregators[agg].color,
            )
        ax.grid()
        ax.set_ylim(bottom=0)
    lines, labels = axes[0].get_legend_handles_labels()
    margin = 0.04
    graph.subplots_adjust(top=0.8, left=0.0+margin, right=1.0-margin, bottom=0.2)
    graph.legend(
        lines,
        labels,
        frameon=False,
        ncol=len(labels),
        loc="upper center",
    )
    # graph.tight_layout()
    name =  "figures/" + preamble + "_" + varying + "_panel.pdf"
    print(f"Writing to {name}")
    graph.savefig(name)
    print("OKK")
    plt.close("all")


def make_window_varying_graph(preamble, func_to_data):
    ww = None
    for _, windows in func_to_data.items():
        for w, _ in windows.items():
            ww = w
    functions = ["sum", "geomean", "bloom"]
    aggs_array = [func_to_data[f][ww] for f in functions] 
    # for f, distances in func_to_data.items():
    #     for d, aggs in distances.items():
    # make_throughput_panel(preamble, functions, aggs_array, varying="window_size")
    make_throughput_panel(preamble, functions, aggs_array, varying="window size in data items")

def make_distance_varying_graph(preamble, func_to_data):
    ww = None
    for _, windows in func_to_data.items():
        for w, _ in windows.items():
            ww = w
    functions = ["sum", "geomean", "bloom"]
    aggs_array = [func_to_data[f][ww] for f in functions] 
    make_throughput_panel(preamble, functions, aggs_array, varying='out-of-order distance')


def func_varying_window(agg_to_func):
    ret = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict(dict)))
    for agg, func in agg_to_func.items():
        for f, distance in func.items():
            for d, window in distance.items():
                for w, data in window.items():
                    ret[f][d][agg][w] = data
    return ret

def func_varying_distance(agg_to_func):
    ret = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict(dict)))
    for agg, func in agg_to_func.items():
        for f, distance in func.items():
            for d, window in distance.items():
                for w, data in window.items():
                    ret[f][w][agg][d] = data
    return ret

def main():
    u.create_figures_dir()
    u.set_fonts()

    agg_to_func = collections.defaultdict(lambda: collections.defaultdict(dict))
    for agg in aggregators.keys():
        agg_to_func[agg] = u.read_throughput_data('ooo', agg, functions)

    make_window_varying_graph('ooo_window', func_varying_window(agg_to_func))
    # make_distance_varying_graph('ooo_distance', func_varying_distance(agg_to_func))

if __name__ == "__main__":
    main()
