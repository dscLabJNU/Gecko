#!/usr/bin/env python3

import run_utility as u

full_aggregators = [
                "CPIX",
                "gecko",
#                "bfinger2",
                "bfinger4",
                "bfinger8",
#                "nbfinger2",
                "nbfinger4",
                "nbfinger8",
                "amta",
#                "two_stacks_lite",
                "chunked_two_stacks_lite",
                "daba_lite",
              ]
ooo_aggregators = [
                "CPIX",
                "gecko",
#                "bfinger2",
                "bfinger4",
                "bfinger8",
#                "nbfinger2",
                "nbfinger4",
                "nbfinger8",
              ]

fifo_degrees = [0]
ooo_degrees = [2, 8, 32, 128, 512, 
           2*u.KB, 8*u.KB, 32*u.KB, 128*u.KB, 512*u.KB,
           2*u.MB, ]

base_window_sizes = [4*u.MB]

base_iterations = 50 * u.MILLION

fifo_bulk_sizes = [1, 4, 16, 64, 256,
             1*u.KB, 4*u.KB, 16*u.KB, 64*u.KB, 256*u.KB,
             1*u.MB, 2*u.MB]

ooo_bulk_sizes = [1, 1*u.KB, 64*u.KB, 2*u.MB]

functions = { 
             "sum": (base_iterations, base_window_sizes),
             "geomean": (base_iterations, base_window_sizes),
             "bloom": (base_iterations//2, base_window_sizes),
            }

def main():
    u.run_bulk(full_aggregators, functions, fifo_degrees, fifo_bulk_sizes, 'bulk_evict_insert', filemode='wt')
    u.run_bulk(ooo_aggregators, functions, ooo_degrees, ooo_bulk_sizes, 'bulk_evict_insert', filemode='at+')

if __name__ == "__main__":
    main()
