#!/usr/bin/python
import sys
import string
import time
import pickle
import os.path

def printusage():
	print "usage:\n"
	print sys.argv[0]+"<num_iterations_to_plot> <output_file.m>  <input_files_with_times> \n"
	sys.exit(-1)

if len(sys.argv) < 4:
   printusage()

def UniqueProcessName(line, prefix):
  result = line
  if ('|START_PROCESS:' in line) or ('|END_PROCESS:' in line):
    lineSplit = line.split(': ')
    name = lineSplit.pop()
    name = ' ' + prefix + '_' + name
    lineSplit.append(name)
    result = ': '.join(lineSplit)
  return result

try:
  filename=sys.argv[1]
  searchPart1="START:"
  searchPart2="STOP:"
  searchProc1="START_PROCESS:"
  searchProc2="END_PROCESS:"
  searchHyperperiod="HYPERPERIOD="
  
#first part extract the string
  x={};
  y={};
  xstart =0;
  ystart =0;
  start = 1;
  procPartitions = {};
  lastState = {};
  partName={}
  indexVal = {}
  indexCounter=1;
  xrangemin=-3;
  xrangemax=0;
  executionTimes={};
  executionInterval={};
  cet={};
  hyperperiod_val=0.0;
  max_time=0;

  all_lines= [];
  scaling_factor = int(sys.argv[1]);
  if (scaling_factor <=0):
          print " Wrong num iterations to plot. Quitting !!"
          sys.exit(-1);
          
  for i in range(3,len(sys.argv)):
      file1=open(sys.argv[i]);
      if (not file1):
              print ' File not found : ' + sys.argv[i]
              print ' Quitting! ';
              sys.exit(-1);
      for line in file1:
          # get partition name from filename
          partition_name = os.path.basename(sys.argv[i])
          hyphen_pos = partition_name.find('-')
          if hyphen_pos > 0:
            partition_name = partition_name[:hyphen_pos]
          string1 = UniqueProcessName(line, partition_name)
          string1 = string1.split("|");
          if (len(string1)<2):
                  continue;
          all_lines.append(string1);
  all_lines.sort(key=lambda temp: float(temp[1])); 
              
              

  
  for silly in all_lines:
          lines = string.join(silly,'|');
          if searchHyperperiod in lines:
                  strings=lines.split('|')
                  string_part=lines.split('=')
                  floatstr=string_part[len(string_part)-1]
                  hyperperiod_val=float(floatstr);
                  #print hyperperiod_val;
                  max_time=scaling_factor*hyperperiod_val;
                                 
                        
          if (searchPart1 in lines) or (searchPart2 in lines)or(searchProc1 in lines) or (searchProc2 in lines):
             #print lines
             strings=lines.split('|')
             string1 = strings[1];
             string0 = strings[0];
             string2 = lines.split(':');
             name = string2[len(string2)-1].strip();
             partitionname = name;
             
             if (searchProc1 in lines) or (searchProc2 in lines):
                 if (':' in string0):
                     partitionname=string0.split(':')[1];
                 
                
                
                 
             if (start==1):
                 xstart=float(string1);

             if (not x.has_key(name)):
                 indexVal[name]=indexCounter;
                 indexCounter = indexCounter+1;
                 x[name]=[0];
                 y[name]=[ystart];
                 executionTimes[name]='';
                 executionInterval[name]=[];
                 lastState[name] = 0;
                 if (name != partitionname):
                     procPartitions[partitionname].append(name);
                     partName[name] = partitionname;
                 else:
                     procPartitions[partitionname]=[];
                     
             if (start ==1):
                 start = 0;
                 continue;

             curtime = float(string1);
             curtimestr = string1;
             timediff = float(string1)-xstart;
             if (xrangemin <0 or timediff<xrangemin):
                xrangemin=timediff
             if(timediff>xrangemax):
                xrangemax=timediff                      
             x[name].append(timediff);
             x[name].append(timediff);

##             if (searchProc1 in lines or searchProc2 in lines):
##                     if name not in executionTimes.keys():
##                             executionTimes[name]=[];
##
##             if searchProc1 in lines:
##                     cet[name]=float(string1);
##
##             if  searchProc2 in lines:
##                      executionTimes[name].append(float(string1)- cet[name]);                  
                     
           

             if (searchPart2 in lines):
                 for name1 in procPartitions[partitionname]:
                     if (lastState[name1]== 1):
                         y[name1].append(indexVal[name1]+1);
                         y[name1].append(indexVal[name1]+0.5);
                         x[name1].append(timediff);
                         x[name1].append(timediff);
                         #val0 = executionInterval[name][0]+(curtime-executionInterval[name][1]);
                         executionInterval[name].append(curtimestr);
                         
             if (searchPart1 in lines):
                 for name1 in procPartitions[partitionname]:
                     if (lastState[name1]== 1):
                         y[name1].append(indexVal[name1]+0.5);
                         y[name1].append(indexVal[name1]+1);
                         x[name1].append(timediff);
                         x[name1].append(timediff);
                         #val0 = executionInterval[name][0];
                         #executionInterval[name]=(val0,curtime);
                         executionInterval[name].append(curtimestr);
                         
             if (searchPart1 in lines)or(searchProc1 in lines):
                 y[name].append(indexVal[name]+0)
                 y[name].append(indexVal[name]+1)
                 lastState[name]=1;
                 executionInterval[name]=[curtimestr];
             elif (searchPart2 in lines)or(searchProc2 in lines):
                 y[name].append(indexVal[name]+1)
                 y[name].append(indexVal[name]+0)
                 lastState[name]=0;
                 executionInterval[name].append(curtimestr);
                 if (len(executionInterval[name]) > 0):
                         executionInterval[name].reverse();
                         val='';
                         sign='-';
                         for val1 in executionInterval[name]:
                                 if (val != ''):
                                         val += sign;
                                         if (sign =='-'):
                                                 sign = '+';
                                         else:
                                                 sign = '-';
                                 val += val1;
                         if (executionTimes[name]  != ''):
                                 executionTimes[name] += ',';
                         executionTimes[name] += val;
                         
                         
                 
          if (start==1):
              strings=lines.split('|');
              if (len(strings) < 2):
                continue;
              string1 = strings[1];
              xstart = float(string1);
              start = 0;
      

##  #print x,y
##  output = open(sys.argv[1], 'wb')
##  pickle.dump((x,y), output)
##  output.close()
  output_file = open(sys.argv[2],"w")
#  import matplotlib
#  matplotlib.use('PDF')
#  from pylab import *
  count = 0

  output_file.write('fid = fopen(\'%s\', \'w\');'%(sys.argv[2]+'.times'))
  for key in executionTimes.keys():
      if len(executionTimes[key]) < 1:
         continue
      output_file.write('%'+key+'Execution Times \n');
      output_file.write(key+'Times = [' +executionTimes[key]+'];\n');
      output_file.write(key+'AvgT= mean('+key+'Times);\n');
      output_file.write('fprintf(fid,\'%sAverage=%%f seconds\\n\',%sAvgT);\n'%(key,key));
      output_file.write(key+'WCET= max('+key+'Times);\n');
      output_file.write('fprintf(fid,\'%sWCET=%%f seconds\\n\',%sWCET);\n'%(key,key));
      output_file.write(key+'MinT= min('+key+'Times);\n');
      output_file.write('fprintf(fid,\'%sMinT=%%f seconds\\n\',%sMinT);\n'%(key,key));
      output_file.write(key+'SigmaT= std('+key+'Times);\n');
      output_file.write('fprintf(fid,\'%sSigma=%%f seconds\\n\',%sSigmaT);\n'%(key,key));
      
      
  output_file.write('fclose(fid)')        

  for keyx in x.keys():
      output_file.write('%'+keyx+'\n');
      output_file.write('label'+str(count)+' = \'' + keyx+'\';\n');
      output_file.write('x'+str(count)+' = ' +str(x[keyx][1:]) +';\n');
      output_file.write('y'+str(count)+' = ' +str(y[keyx][1:]) +';\n');
      
      
      count=count+1;
      #plt.plot(x[keyx][1:], y[keyx][1:], 'b')
      #plt.grid(True)
      #plt.title("Active Sequence for "+keyx)
      #plt.xlabel("Active ")
      #plt.ylabel("Time in micro seconds")
      #plt.savefig(keyx+".pdf")
  count=0;
  numplots=len(x.keys())
  plotkeys=[]
  for keyx in x.keys():
	if 'MM' in keyx or 'HM' in keyx or 'DUMMY_PROCESS' in keyx:
		numplots=numplots-1	
	#else:
	#	plotkeys.append(keyx)
		
  plotcounter=1
  count=0  
  for keyx in x.keys():
	if 'MM' in keyx or 'HM' in keyx or 'DUMMY_PROCESS' in keyx:
		count=count+1
		continue
	label=keyx.replace('_',' ')
	output_file.write('%% Plot for %s\n'%(keyx))
	str1='temp_d=y%d-y%d(1);'%(count,count)
	output_file.write(str1+'\n')	
	str1='plot(x%d,temp_d),  set(gca, \'YTick\',[0 0.5 1 1.5]),set(gca, \'YTickLabel\',{\'stop\',\'suspend\',\'running\',\' \'})'%(count)
	output_file.write(str1+'\n')
	output_file.write('hold \n');
	output_file.write('plot(0,-0.5,\'.\'); \n');
	output_file.write('plot(0,1.5,\'.\'); \n');	
        str1='plot(x%d,y%d-y%d(1)),  set(gca, \'YTick\',[0 0.5 1]),set(gca, \'YTickLabel\',{\'stop\',\'suspend\',\'running\'})'%(count,count,count)	
	#str1='plot(x%d,y%d-y%d(1)),  set(gca, \'YTick\',[0 0.5 1]),set(gca, \'YTickLabel\',{\'stop\',\'suspend\',\'running\'})'%(count,count,count)
	#output_file.write(str1+'\n')
	#str1='axis([%f,%f,0,1.5]), set(gca, \'XTick\',[%f:%f:%f]), grid on'%(xrangemin,xrangemax,xrangemin,scaling_factor,xrangemax)
	str1='axis([%f,%f,-0.5,1.5]), grid on'%(xrangemin,max_time)
	output_file.write(str1+'\n')

	if keyx not in executionTimes.keys():
                output_file.write('title(\'%s\');\n'%(label));
        else:
                if len(executionTimes[keyx]) >= 1:
                        output_file.write('str1=sprintf(\'max=%%f.avg=%%f.min=%%f.std=%%f sec\',%sWCET,%sAvgT,%sMinT,%sSigmaT);\n'%(keyx,keyx,keyx,keyx));
                        output_file.write('str2=sprintf(\'%%s\\n.%%s\',\'%s\',str1);\n'%(label));

                        output_file.write('title(str2);\n');
                else:
                        output_file.write('title(\'%s\');\n'%(label));
                        
        output_file.write('axis image; \n')
        str1='print -dpdf \'%s.pdf\';'%(keyx)
	#str1='saveas(gcf,\'%s\',\'fig\');'%(keyx)
	output_file.write(str1+'\n')
	#str1='saveas(gcf,\'%s\',\'png\');'%(keyx)
	#output_file.write(str1+'\n')
	
	output_file.write('close();\n');

	
	count=count+1
	plotcounter=plotcounter+1
	
 
  output_file.close();


except Exception, e:
        print(e)
        sys.exit(-1)
