#!/usr/bin/python
import sys
import string
import time

def printusage():
	print "usage:\n"
	print sys.argv[0]+" <file_with_times> --partition <partition_name> <timeperiod in seconds> [--silent]\n"
	print "Or,\n"
	print sys.argv[0]+" <file_with_times> --process <process_name> <timeperiod in seconds> [--silent]"
	sys.exit(-1)

if len(sys.argv) < 5:
   printusage()

if len(sys.argv)==6:
	silent=True
else:
	silent=False

try:
  filename=sys.argv[1]
  flag=sys.argv[2]
  if flag=="--partition":
     searchStr="START:"
  else:
   if flag=="--process":
     searchStr="START_PROCESS:"
   else:
      printusage()
  


  pattern=sys.argv[3]
  trigger=float(sys.argv[4])
  pidname=sys.argv[1].split('_')[0]
  
  file1=open(sys.argv[1])
#first part extract the string
  buffer1=[]
  counter=0
  bufferString=[]
  for lines in file1:
      if searchStr in lines and  pattern in lines:
         #print lines
         counter=counter+1
         string1=lines.split('|')[1]
         bufferString.append(string1)
         temp=float(string1)
         buffer1.append (temp)
  file1.close()
  if not silent: 
	print "Found "+str(counter)+" lines with pattern "+searchStr+" "+pattern+" in file "+filename
  if (len(buffer1))==0:
	sys.exit(0)
  for i in range(1,len(buffer1)):
  	buffer1[i] = buffer1[i]-buffer1[0]-float(trigger)*i;
  if not silent: 
	  print "-----------------------------------------------------"
	  print "S.No|%34s|Drift (secs)" %("Time of Message")
	  print "-----------------------------------------------------"
  buffer1[0]=0
  counter=0
  for i in range(len(buffer1)):
        timestr=time.ctime(float(bufferString[i]))
        timelist=timestr.split()
        timelist[3]=timelist[3]+':'+bufferString[i].split('.')[1]
        timestr=str.join(' ',timelist)
	if not silent:
        	print "%4s|%s|%3.9f"% (str(i+1), timestr, buffer1[i])
  	#print i+1,'|', bufferString[i],'|',buffer1[i]
#          str(i+1)+'|'+str(float(times[i]))+'|'+str(float(buffer1[i]))
  if not silent:   
	print "-----------------------------------------------------"
  	print "Finished"

  from pylab import *
  y=[]
  absy=[]
  for i in range(len(buffer1)):
      y.append(buffer1[i]*1000000)
      if i ==0:
         continue
      absy.append(abs(buffer1[i])*1000000)
	
  x =range(len(buffer1))
  z=std(absy)
  av=mean(absy)
  ma=max(absy)
  print "StandardDeviation "+str(z)+" us"+" average "+str(av)+" us"+" max "+str(ma)+" us"+" for "+pattern
  plt.plot(x, y, '*k')
  if not silent: 
	print "plotted"
  #plt.show()
  plt.grid(True)
  plt.title("Drift over time in start of "+pattern)
  plt.xlabel("Sample. Period=%s seconds"%(trigger))
  plt.ylabel("Drift in micro seconds")
  plt.savefig(pattern+"_"+pidname+".png")


except Exception, e:
        print(e)
        sys.exit(-1)
