#!/usr/bin/env python3

import sys, collections
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import numpy as np
import process_utility as u
from run_fifo import base_iterations

Line = collections.namedtuple("Line", ["style", "color"])

aggregators = {
            "ffiba4": Line("-", "red"),
            "ffiba8": Line("--", "red"),
            "bclassic4": Line("-", "blue"),
            "bclassic8": Line("-", "green"),
            # "nbfinger2":  Line('--',  'red'),
            "bfinger4": Line("--", "blue"),
            "bfinger8": Line("--", "green"),
}

# aggregators = {
#             "ffiba2": Line("-", "red"),
#             "ffiba4": Line("-", "blue"),
#             "ffiba8": Line("-", "green"),
#             "ffiba16": Line("-", "maroon"),
# }

# aggs_sorted = [
#     "ffiba2",
#     "ffiba4",
#     "ffiba8",
#     "ffiba16",
# ] 
            
              

aggs_sorted = [
    "ffiba4",
    "ffiba8",
    "bclassic4",
    "bclassic8",
    "bfinger4",
    "bfinger8",

]

windows = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 
           1*u.KB, 2*u.KB, 4*u.KB, 8*u.KB, 16*u.KB, 32*u.KB, 64*u.KB, 128*u.KB, 256*u.KB, 512*u.KB,
           1*u.MB, 2*u.MB, 4*u.MB
          ]

functions = {"sum": base_iterations,
             "geomean": base_iterations,
             "bloom": base_iterations//2,
            }

X_AXIS = [
    1,
    2,
    2**3,
    2**5,
    2**7,
    2**9,
    2**11,
    2**13,
    2**15,
    2**17,
    2**19,
    2**21,
]

def make_throughput_panel(preamble, functions, mappings, varying, sorted_keys=aggs_sorted, x_ticks=X_AXIS, x_labels=None):
    graph, axes = plt.subplots(1, 3, figsize=(24*0.75, 6*0.75))
    axes[0].set_ylabel("throughput [million items/s]")
    axes[len(axes)//2].set_xlabel(varying)
    for i, ax in enumerate(axes):
        ax.set_title(functions[i])
        ax.set_xscale("log", basex=2)
        if x_labels:
            ax.set_xticks(x_ticks,  x_labels)
        else:
            ax.set_xticks(x_ticks)
        ax.set_xlim(min(x_ticks), max(x_ticks))
        for agg in sorted_keys:
            print(agg)
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

def make_window_size_varying_graph(preamble, func_to_data):
    functions = ["sum", "geomean", "bloom"]
    aggs_array = [func_to_data[f] for f in functions] 
    make_throughput_panel(preamble, functions, aggs_array, varying="window_size")

def main():
    u.create_figures_dir()
    u.set_fonts()

    agg_to_func = collections.defaultdict(lambda: collections.defaultdict(dict))
    for agg in aggregators:
        agg_to_func[agg] = u.read_throughput('fifo', agg, functions)

    make_window_size_varying_graph("fifo", u.func_varying_small_window(agg_to_func))
    # u.make_small_window_varying_graphs('FIFO', 'fifo', 'window size in data items', aggregators, 
    #                                    u.func_varying_small_window(agg_to_func))
if __name__ == "__main__":
    main()
