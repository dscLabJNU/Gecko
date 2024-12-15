#!/usr/bin/env python3

import run_utility as u

aggregators = [
    # "daba",
    # "daba_lite",
    # "rb_daba_lite",
    # "two_stacks",
    "ffiba2",
                # "ffiba4",
                # "ffiba8",
                "ffiba16",
    # "ffiba4",
    # "ffiba8",
    # "two_stacks_lite",
    # # "rb_two_stacks_lite",
    # # "chunked_two_stacks_lite",
    # "flatfit",
    # "bclassic4",
    # "bfinger4",
    # "bclassic8",
    # "bfinger8",
]

base_window_size = 4*u.MB

base_iterations = 10 * u.MILLION

functions = {
                "sum": (base_iterations, base_window_size),
                "geomean": (base_iterations, base_window_size),
                "bloom": (base_iterations//2, base_window_size),
            }

def main():
    u.run_fifo_latency(aggregators, functions)

if __name__ == "__main__":
    main()
