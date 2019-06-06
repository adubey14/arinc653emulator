#!/usr/bin/python
import sys
import string
import time

def printusage():
	print "usage:\n"
	print sys.argv[0]+" <file_with_times> --partition <partition_name> \n"
	print "Or,\n"
	print sys.argv[0]+" <file_with_times> --process <process_name> "
	sys.exit(-1)

if len(sys.argv) < 4:
   printusage()

try:
  filename=sys.argv[1]
  flag=sys.argv[2]
  if flag=="--partition":
     searchStr="START:"
     searchStr2="STOP:"
  else:
   if flag=="--process":
     searchStr="START_PROCESS:"
     searchStr2="END_PROCESS:"
   else:
      printusage()
  pattern=sys.argv[3]
  pidname=sys.argv[1].split('_')[0]

  file1=open(sys.argv[1])
#first part extract the string
  x=[];
  y=[];
  start =1;
  for lines in file1:
      if (searchStr in lines and  pattern in lines) or (searchStr2 in lines and  pattern in lines):
         #print lines
         string1=lines.split('|')[1]
         x.append(float(string1))
         if (searchStr in lines):
             y.append(1)
         else:
             y.append(0)
         start = 0
      if (start==1):
          string1=lines.split('|')[1]
          x.append(float(string1))
          y.append(0);
      start = 0;
  file1.close()
  print x,y
  
  from pylab import *
  for i in range(len(buffer1)):
      y.append(buffer1[i][1])
      x.append(buffer1[i][0])                                                        
  plt.plot(x, y, '*k')
  plt.grid(True)
  plt.title("Active Sequence for "+pattern)
  plt.xlabel("Active ")
  plt.ylabel("Time in micro seconds")
  plt.savefig(pattern+"_"+pidname+".png")


except Exception, e:
        print(e)
        sys.exit(-1)
