#!/usr/bin/python
import sys
import string
import time
import pickle

def printusage():
	print "usage:\n"
	print sys.argv[0]+"<input_file>\n"
	sys.exit(-1)

if len(sys.argv) < 2:
   printusage()

try:
  pkl_file = open(sys.argv[1], 'rb')
  data1 = pickle.load(pkl_file)
  x=data1[0];
  y=data1[1];
  pkl_file.close()
  
  
  from pylab import *
  for keyx in x.keys():
      plt.plot(x[keyx], y[keyx], '*k')
      plt.grid(True)
      plt.title("Active Sequence for "+keyx)
      plt.xlabel("Active ")
      plt.ylabel("Time in micro seconds")
      plt.savefig(keyx+"_"+pidname+".pdf")


except Exception, e:
        print(e)
        sys.exit(-1)
