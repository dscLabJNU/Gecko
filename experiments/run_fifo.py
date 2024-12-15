#!/usr/bin/env python3

import run_utility as u

aggregators = [
    # "ffiba2",
    # "ffiba4",
    # "ffiba8",
    # "ffiba16",
    # "daba",
    # "daba_lite",
    # "rb_daba_lite",
    # "two_stacks",
    # "two_stacks_lite",
    # "rb_two_stacks_lite",
    # "chunked_two_stacks_lite",
    # "flatfit",
    # "bclassic4",
    # "nbclassic8",
    # "bfinger4",
    # "bfinger8",
    # "bclassic4",
    "bclassic8",
    
]

base_window_sizes = [
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512,
    1*u.KB, 2*u.KB, 4*u.KB, 8*u.KB, 16*u.KB, 32*u.KB, 64*u.KB, 128*u.KB, 256*u.KB, 512*u.KB,
    1*u.MB, 2*u.MB, 4*u.MB
]

base_iterations = 200 * u.MILLION

functions = {
    "sum": (base_iterations, base_window_sizes),
    "geomean": (base_iterations, base_window_sizes),
    "bloom": (base_iterations//2, base_window_sizes),
}

def main():
    u.run_fifo(aggregators, functions, 'fifo')

if __name__ == "__main__":
    main()
