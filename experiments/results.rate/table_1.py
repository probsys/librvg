#!/usr/bin/env python3

import os
import re
import sys

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def load_file(f):
    df = pd.read_csv(f,
        delimiter=' ',
        header=None,
        names=['method', 'n', 'time', 'flips'],
        dtype={'method': str, 'n': int, 'time': float, 'flips': int}
        )
    df.method = df.method.str.replace('generate_', '')
    df = df[~df.method.str.contains('_ext')]
    df.loc[df.method.str.contains('gsl_ran'),'method'] = 'gsl'
    df['distribution'] = f
    return df

df = pd.concat([load_file(f) for f in os.listdir() if '@' in f])
df['bits/variate'] = df.flips / df.n
df['variates/sec'] = df.n / df.time
df = df[['distribution', 'method', 'bits/variate', 'variates/sec']]
df = df.sort_values(by=['distribution','method'])
print(df.to_latex())
