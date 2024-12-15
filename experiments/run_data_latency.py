#!/usr/bin/env python3

import run_utility as u

aggregators = [
    # "daba",
    # "daba_lite",
    # "rb_daba_lite",
    # "two_stacks",
    # "two_stacks_lite",
    # "rb_two_stacks_lite",
    # "chunked_two_stacks_lite",
    # "flatfit",
    # "bclassic4",
    # "bfinger4",
    "ffiba4",
    "ffiba8",
    "bclassic2",
    "bclassic4",
    "bclassic8",
    "bfinger2",
    "bfinger4",
    "bfinger8",
]

# time is in milliseconds
# we want to go up to a 6 hour window
# SECOND = 10**3
# MINUTE = 60*SECOND
# HOUR = 60*MINUTE
# durations = [10,
#              10*MINUTE,
#              819*SECOND,
#              6*HOUR
#             ]
DAY = 60 * 60 * 24 # one day is 60 seconds * 60 minutes * 24 hours
durations = [DAY//4, DAY//2, 1*DAY, 2*DAY, 4*DAY, 8*DAY, 12*DAY, 16*DAY, 24*DAY, 32*DAY]

data_sets = {
                "bike": "../experiments/data/citibike_Aug2Dec.csv",
            }

functions = [
             "sum",
             "geomean",
             "bloom",
            #  "relvar"
            ]

def main():
    u.run_data(aggregators, functions, durations, data_sets, 'data', 'latency', 1)

if __name__ == "__main__":
    main()
