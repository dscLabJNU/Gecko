#!/usr/bin/env python

import run_utility as u

aggregators = [
                "ffiba4",
                "ffiba8",
                "bclassic2",
                "bclassic4",
                "bclassic8",
                "bfinger2",
                "bfinger4",
                "bfinger8",
                #"bfinger16",
                #"bfinger32",
                #"finger64",
                #"finger128",
                #"23tree",
              ]

base_window_sizes = [4*u.MB]
# base_window_sizes = [1, 4, 16, 64, 256,
#                     1*u.KB, 4*u.KB, 16*u.KB, 64*u.KB, 256*u.KB,
#                     1*u.MB, 4*u.MB]

base_iterations = 1 * u.MILLION

functions = {
             "sum": (base_iterations, base_window_sizes),
             "geomean": (base_iterations, base_window_sizes),
             "bloom": (base_iterations//2, base_window_sizes),
            }

def main():
    u.run_ooo_latency(aggregators, functions, 'ooo')

if __name__ == "__main__":
    main()
