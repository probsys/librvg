#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt

df_ran = pd.read_csv('bounds.ran',
  delimiter=' ',
  header=None,
  names=['sampler','lo','hi','runtime'],
  dtype={'sampler':str, 'lo':float, 'hi':float, 'runtime':float}
  )

df_cdf = pd.read_csv(
  'bounds.cdf', delimiter=' ',
  header=None,
  names=['sampler','c_lo','c_hi','s_lo','s_hi', 'e_lo', 'e_hi', 'runtime'],
  dtype={'sampler':str, 'c_lo':float, 'c_hi':float, 's_lo':float, 's_hi':float, 'e_lo': float, 'e_hi':float, 'runtime':float},
  )

df_ran['sampler'] = df_ran.sampler.str.replace('rng, ', '').str.replace('gsl_ran_', '')
df_cdf['sampler'] = df_cdf.sampler.replace('rng, ', '').str.replace('gsl_cdf_', '')
assert (df_ran.sampler == df_cdf.sampler).all()

for (i, r1), (j, r2) in zip(df_ran.iterrows(), df_cdf.iterrows()):
    fig, ax = plt.subplots(figsize=(10,8), tight_layout=True)
    config = {'color': 'k', 'marker': 's', 'linewidth': 2}

    ax.plot([r1.lo, r1.hi], [3,3], **config)
    ax.plot([r2.c_lo, r2.c_hi], [2,2], **config)
    ax.plot([r2.s_lo, r2.s_hi], [1,1], **config)
    ax.plot([r2.e_lo, r2.e_hi], [0,0], **config)

    ax.text(r1.lo, 3.1, '%e'%(r1.lo), ha='center')
    ax.text(r1.hi, 3.1, '%e'%(r1.hi), ha='center')

    for i, k in enumerate(['e', 's', 'c']):
      kl, kh = f'{k}_lo', f'{k}_hi'
      ax.text(r2[kl], i+.1, '%e'%(r2[kl],), ha='center')
      ax.text(r2[kh], i+.1, '%e'%(r2[kh],), ha='center')

    ax.set_title(r1.sampler, fontsize=16, y=1.1)
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    ax.spines['left'].set_visible(False)
    if any(x <= 0. for x in [r1.lo, r2.c_lo, r2.s_lo]):
      # In bounds.c add code for the following:
      # - In this case show a dot at zero and then find positive/negative
      # intervals separately, with symlog set to very small linear region,
      # so that the next positive value shows up.
      # - Add the median, using GSL.
      ax.set_xscale('symlog')
    else:
      ax.set_xscale('log')
    ax.set_yticks(
      ticks=[0, 1, 2, 3],
      labels=[
        'DDF\n(%s sec)' % (r2.runtime),
        'SF\n(%s sec)' % (r2.runtime),
        'CDF\n(%s sec)' % (r2.runtime),
        'GSL\n(%s sec)' % (r1.runtime),
        ],
      fontsize=14, fontweight='bold')
    ax.tick_params(axis='y', width=0)

    fname = '%s.png' % (r1.sampler,)
    fig.savefig(fname, dpi=150)
    print(fname)
