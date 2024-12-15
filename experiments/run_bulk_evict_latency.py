#!/usr/bin/env python3

import run_utility as u

aggregators = [
                "CPiX",
                "gecko",
                "bfinger4",
                "bfinger8",
                "nbfinger4",
                "nbfinger8",
                "amta",
                "two_stacks_lite",
                # "chunked_two_stacks_lite",
                # "daba_lite",
              ]



base_window_sizes = [4*u.MB]

base_iterations = 5 * u.MILLION

degrees = [0]
# bulk_sizes = [1, 256, 1024, 16384, 64*u.KB, 128*u.KB, 1*u.MB]

# degrees = [1, 4, 8, 64, 256, 1*u.KB, 8*u.KB, 64*u.KB, 128*u.KB, 
#               512*u.KB, 2*u.MB]
              
bulk_sizes = [1*u.KB]


functions = {
             "sum": (base_iterations, base_window_sizes),
             "geomean": (base_iterations, base_window_sizes),
             "bloom": (base_iterations, base_window_sizes),
            }

def main():
    u.run_bulk_latency(aggregators, functions, degrees, bulk_sizes, 'bulk_evict')

if __name__ == "__main__":
    main()
