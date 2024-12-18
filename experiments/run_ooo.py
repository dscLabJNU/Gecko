#!/usr/bin/env python3

import run_utility as u

aggregators = [
                # "bclassic2",
                "ffiba2",
                "ffiba4",
                "ffiba8",
                "ffiba16",
                # "ffiba8",
                # "bclassic4",
                # "bclassic8",
                # # "bfinger2",
                # "bfinger4",
                # "bfinger8",
                
                #"bfinger16",
                #"bfinger32",
              ]

# degrees = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 
#            1*u.KB, 2*u.KB, 4*u.KB, 8*u.KB, 16*u.KB, 32*u.KB, 64*u.KB, 128*u.KB, 256*u.KB, 512*u.KB,
#            1*u.MB, 2*u.MB, 4*u.MB]

degrees = [1*u.KB]

# base_window_sizes = [4*u.MB]
base_window_sizes = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 
                    1*u.KB, 2*u.KB, 4*u.KB, 8*u.KB, 16*u.KB, 32*u.KB, 64*u.KB, 128*u.KB, 256*u.KB, 512*u.KB,
                    1*u.MB, 2*u.MB, 4*u.MB]

base_iterations = 100 * u.MILLION

functions = { 
             "sum": (base_iterations, base_window_sizes),
             "geomean": (base_iterations, base_window_sizes),
             "bloom": (base_iterations/100, base_window_sizes),
            }

def main():
    u.run_ooo(aggregators, functions, degrees, 'ooo')

if __name__ == "__main__":
    main()
