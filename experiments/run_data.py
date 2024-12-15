#!/usr/bin/env python3

import run_utility as u

aggregators = [
    "ffiba4",
    "ffiba8",
    # "daba",
    # "daba_lite",
    # "rb_daba_lite",
    # "two_stacks",
    # "two_stacks_lite",
    # "ffiba",
    # "rb_two_stacks_lite",
    # "chunked_two_stacks_lite",
    # "flatfit",
    # "bclassic4",
    # "bclassic8",
    # "bfinger4",
    # "bfinger8",
]

# time is in milliseconds
# we want to go up to a 6 hour window
# SECOND = 10**3
# MINUTE = 60*SECOND
# HOUR = 60*MINUTE
# durations = [10,
#              100,
#              SECOND,
#              5*SECOND,
#              10*SECOND,
#              30*SECOND,
#              MINUTE,
#              100*SECOND,
#              15*MINUTE,
#              1000*SECOND,
#              30*MINUTE,
#              HOUR, 2*HOUR,
#              10000*SECOND,
#              3*HOUR, 4*HOUR, 5*HOUR, 6*HOUR]

DAY = 60 * 60 * 24 # one day is 60 seconds * 60 minutes * 24 hours
durations = [DAY//4, DAY//2, 1*DAY, 2*DAY, 4*DAY, 8*DAY, 12*DAY, 16*DAY, 24*DAY, 32*DAY]

data_sets = {
                # "mfgdebs": "../experiments/data/DEBS2012-cleaned-v3.txt",
                # "bike": "../experiments/data/test.csv",
              "bike": "../experiments/data/citibike_Aug2Dec.csv",
            }

functions = [
             "sum",
             "geomean",
             "bloom",
            #  "relvar"
            ]

def main():
    u.run_data(aggregators, functions, durations, data_sets, 'data', latency='', sample_size=5)

if __name__ == "__main__":
    main()
