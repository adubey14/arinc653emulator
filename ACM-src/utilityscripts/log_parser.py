#!/usr/bin/python
# Copyright (c) Vanderbilt University, 2005
# ALL RIGHTS RESERVED
# Vanderbilt University disclaims all warranties with regard to this
# software, including all implied warranties of merchantability
# and fitness.  In no event shall Vanderbilt University be liable for
# any special, indirect or consequential damages or any damages
# whatsoever resulting from loss of use, data or profits, whether
# in an action of contract, negligence or other tortious action,
# arising out of or in connection with the use or performance of
# this software.


# 
# This utility reads a log file generated from MICO-ARINC experiment
# generates an input file (.elog) for the Sequence Chart Viewer in
# omnetpp.

# Status
# 2009/11/13 - Works for the specific examples in trunk/November2009AviationSafetyDemo

import sys
import string
import locale

class _static:
    def __init__(self,f):
        self.__call__ = f

# Records the information pertaining to every event i.e. line in the log file
# Parses the event (line) and converts it to a form that can be output to elog format

# The different types of elog messages that are generated include
# MC - Create Module - This is based on the following log informations
# START:MM --> creates module for  Module Manager
# CREATE_PARTITION --> creates module for Partition
# CREATE_PROCESS --> creates module for Process
# When a CREATE_PARTITION or process is marked as (BK), the process is for BookKeeping purposes.
# By default, Bookkeeping processes are not listed.
# If a BK is followed by the name of a process, then all activities related to the
# Bookkeeping process  are transferred to the other process.
# e.g
# CREATE_PROCESS(BK->ACTUALPROCESSNAME):PROCESS_NAME

# Events properties are set by reading the log file.
# The last entry in any line (the one that appears after a ":") corresponds to the module
# where the event is occuring


# Causal Relationship is established between Events by trying to relate each event to 
# past and future events. Whenever a causal edge is drawn, a message is displayed on the edge.

# Corresponding to every START_PARTITION, STOP_PARTITION call two events with the same
# time stamp are generated in the MM and the Parition. The MM Event is set as the Causal
# event of the Partition event. The Message start or stop is printed on the edge connecting
# the events.

# Corresponding to every START_PROCESS  call two events with the same
# time stamp are generated in the Partition and the Process. The Partition Event is set as the Causal
# event of the Process event. The Message start or stop is printed on the edge connecting
# the events.

# The start and stop event in every process are related by a Hoop Message to indicate
# the active region of the process/module.

# The HME events are related to one another through the causal edge.

IgnoreProcesses = ["NAV_PROC_DATA_IN_TRIGGER","ORB_PROCESS_Partition2","ORB_PROCESS_Partition1","U_NavDisplay_getGPSData"];
IgnoreStart = ["NAV_PROC_DATA_IN"];


class LogEvent:
    #holds the processes/modules, for creation commands and module Id assignment
    processes={};
    #holds the current invoke event to relate to future start event
    InvokeEvents=[];
    #holds the set of HM Events
    #assumption - one fault at a time
    #hence HMEvent cleared after every INJECT message
    HMEvents=[];
    #DEADLINE VIOLATION DETECTED
    DVDetected  = 0;
    #Start events for each module -> to generate the causal edge for End events
    StartEvent ={}
    #Publish event -> to generate the causal edge for Consume event
    PublishEvent= ""
    #To generate gates and cc for sh --> start to end of a process
    ModuleWithStartToEndConn={};

    #consturctor called by the parser with the following args
    # line -> contents of the line
    # startTime -> for the simulation ( to compute the relative time for current event)
    # eventID -> last assigned event ID
    # copy -> Flag that indicates if this event is a copy of an existing event
    #         This flag is needed to generate extra events for the same line
    def __init__(self, line, startTime, eventID,copy=0):
        # The copy of the line in the logfile that is used to generate the event        
        self.line = line 
        # Remove the pid portion of each line
        content = line[line.find(":")+1:];
        # Split the string based on "|"         
        contents = string.split(content,"|");
        # first entry is the source of the message (MM/Partx)        
        self.source = string.strip(contents[0]);
        # second entry is time (absolute time)
        self.absTime = locale.atof(contents[1]);
        # third entry is type -> TRC,HME etc. We are interested if the message is HME
        self.type =  string.strip(contents[2]);
        # rest of the entry is description
        self.desc = string.strip(contents[3]);

        # holds the assigned/generated event  id 
        self.eventID = 1;
        # relative time from start
        self.relTime = 0;
        # action performed by the event
        self.action = "";
        # further qualification on the action
        self.actionInfo = "";
        # module where the event occured
        self.module = "";
        # the generated string message for the elog
        self.strValue = "";
        # generated id of the module where the event occured 
        self.moduleID =-1;
        # holds the message for the future event (this message appears on the causal edge)
        self.msg = "";
        # message id -> this is the id of the message for the current event
        # This id is set to the causal event id
        self.msgID = -1;
        # ID of the causal event for the current event
        self.ce =-1;
        # This extra event needs to be generated during the end_process call following 
        # invoke_process->start_process
        self.returnEvent = "";
        #To ignore this event or not.
        self.ignore = 0;
        # store the start event (if end_process)
        self.startEvent =0;
        # Method to parse and build information
        self.buildInfo(startTime,eventID,copy);
        


    #buildInfo uses the contents of self.desc to populate the contents of the event
    def buildInfo(self,startTime,eventID,copy):
        
        if (self.desc == "START: MM"):
            #create command for Module Manager
            self.action = "CREATE";
            self.module = "MM";
        elif (self.desc.find(":") != -1):
            #desc_string = action:module_name
            details = self.desc.split(":");
            self.action = string.strip(details[0]);#action
            self.module = string.strip(details[1]);

            # sometimes the action has additional information
            # action_string = action->action_info
            actions = self.action.split("->");
            self.action = string.strip(actions[0]);
            # if action contains addition information populate self.actioninfo
            if (len(actions)==2):
                self.actionInfo = string.strip(actions[1]);
            # convert all CREATE_* messages to CREATE
            if ((self.action == "CREATE_PROCESS") or (self.action == "CREATE_PARTITION")):
                self.action = "CREATE";
        else:
            # assign the entire description to the action if module information is not present
            self.action = self.desc;
        

        if (self.action == "CREATE"):
            #handle create events
            #store the module_name (MM,PARTX,Process_Name) and assign it an id
            #This id is the module id to be used in the elog file
            LogEvent.processes[self.module] = len(LogEvent.processes)+2;
            # store the module Id in the event
            self.moduleID = LogEvent.processes[self.module];
            
        else:
            # for non-create events
            # action event ids are generated
            self.eventID = eventID +1;
            # search for module name and assign the module id
            # else use the source (MM/PARTX) as the module and assign its module id
            if (self.module in LogEvent.processes.keys()):
                self.moduleID = LogEvent.processes[self.module];
            elif (self.source in LogEvent.processes.keys()):
                self.module = self.source;
                self.moduleID = LogEvent.processes[self.module];
                

        # compute relative-time for non-create events
        if (self.action != "CREATE"):
            self.relTime = self.absTime - startTime;

        # if the event is a copy return without further processing
        if (copy !=0):
            return;

        # if the event is a HME event store it in HMEvents
        if (self.type == "HME"):
            LogEvent.HMEvents.append(self);
            # check for deadline violation detected based on desc containing the "hard deadline violation" message
            if ((self.desc.find("INJECT")!=-1) and (self.desc.find("DEADLINE") != -1)):
                LogEvent.DVDetected = 1;
            if (self.action.find("REPONSE RESTART") !=-1):
                LogEvent.InvokeEvents.append(self);
            return;
        
            
        # if the action is an invoke_process or invoke_method event
        # store it in the InvokeEvents for future reference
        if ((self.action == "INVOKE_PROCESS")or(self.action=="INVOKE_METHOD")):
            LogEvent.InvokeEvents.append(self);

        
        
        # if the action is start_process or end_process
        # check if invoke messages need to be generated
        # also record the start event, to send a message to end event
        if ((self.action == "START_PROCESS") or (self.action == "END_PROCESS") ):
            self.genInvokeEventMsg(startTime);
            self.recordStartEvent();#record the start event, to send a message to end event

        #record the publish event, to send a message to consume event        
        if ((self.action == "PUBLISH") or (self.action == "CONSUME") or (self.action == "START_CONSUME")):
            self.recordPublishToConsumeConn();

        if (self.action == "START_CONSUME") :
            self.recordStartEvent();#record the start event, to send a message to end event

        
            

  

    # This method is used to ensure that the start calls for the module is issued from the correct module
    # By default, this is issued by the appropriate Partition Manager
    # But during invoke/restart calls it is issued by module that issued the invoke/restart command
    def genInvokeEventMsg(self,startTime):
        if (len(LogEvent.InvokeEvents)==0):
            return;

        msg = "BS id x1 tid x1 eid x1 etid x1 c cMessage n ";
        msgES = "\nES t ";
        msgES += str(self.relTime);
        msgES += "\n";

        # in the current log format, this invoke process is issued by the Bookkeeping process
        # this is useful command to have when an aperiodic process is invoked by another module
        # rather than the partition
        if (LogEvent.InvokeEvents[0].action == "INVOKE_PROCESS"):
            # an appropriate message is created for the action so that the message can appear in the
            # causal edge
            msg = string.replace(msg, "x1", str(LogEvent.InvokeEvents[0].eventID));
            msg += " START";
            msg += msgES;
            LogEvent.InvokeEvents[0].msg = msg;
            self.ce = LogEvent.InvokeEvents[0].eventID;
            self.msgID = LogEvent.InvokeEvents[0].eventID;
            LogEvent.InvokeEvents = [];# create the asynchronous process-start event
        elif ((self.action == "START_PROCESS") and (LogEvent.InvokeEvents[0].action == "INVOKE_METHOD")):
        # This handles the invoke_method synchronous method calls
            msg = string.replace(msg, "x1", str(LogEvent.InvokeEvents[0].eventID));
            msg += " INVOKE";
            msg += msgES;
            LogEvent.InvokeEvents[0].msg = msg;
            self.ce = LogEvent.InvokeEvents[0].eventID;
            self.msgID = LogEvent.InvokeEvents[0].eventID; 
        elif ((self.action == "END_PROCESS")and (LogEvent.InvokeEvents[0].action == "INVOKE_METHOD")):
        # This handles the return for synchronous method calls
        # An extra return event is generated and stored in this situation.
            msg = string.replace(msg, "x1", str(self.eventID));
            msg += " RETURN";
            msg += msgES;
            self.msg = msg;
            self.returnEvent = LogEvent(self.line, startTime, self.eventID+1,1);
            self.returnEvent.module = LogEvent.InvokeEvents[0].module;
            self.returnEvent.moduleID = LogEvent.InvokeEvents[0].moduleID;
            self.returnEvent.ce = self.eventID;
            self.returnEvent.msgID = self.eventID;
            LogEvent.InvokeEvents = [];#clear the invoke event
        elif ((self.action == "START_PROCESS") and (LogEvent.InvokeEvents[0].action == "REPONSE RESTART")):
            # This handles the restart call issued by another process, here HM
            msg = string.replace(msg, "x1", str(LogEvent.InvokeEvents[0].eventID));
            msg += " RESTART";
            msg += msgES;
            LogEvent.InvokeEvents[0].msg = msg;
            self.ce = LogEvent.InvokeEvents[0].eventID;
            self.msgID = LogEvent.InvokeEvents[0].eventID;
            LogEvent.InvokeEvents = []; #clear the restart event
        return;
    
    # record the start event
    def recordStartEvent(self):
        if ((self.action == "START_PROCESS") or (self.action == "START_CONSUME")):
            LogEvent.StartEvent[self.moduleID] = self;
        elif (self.action == "END_PROCESS"):
            if ((self.moduleID in LogEvent.StartEvent.keys())and (LogEvent.StartEvent[self.moduleID] !=-1)):
                self.startEvent = LogEvent.StartEvent[self.moduleID];
                if (self.moduleID not in LogEvent.ModuleWithStartToEndConn.keys()):
                    LogEvent.ModuleWithStartToEndConn[self.moduleID]="";
                
    # produce the start to end conn
    def recordStartToEndConn(self):
        if ((self.action == "END_PROCESS") and (self.startEvent !=0)):
            self.ce = -1;
            self.msgID = -1;
            msg = "\nBS id x1 tid x1 eid x1 etid x1 c cMessage n \"\" pe ";
            msg += str(self.startEvent.eventID);
            x1= 1;
            if (self.startEvent.msgID != -1):
                x1 = str(self.startEvent.msgID);
            msg = string.replace(msg, "x1", str(x1));
            msg += "\nSH sm ";
            msg += str(self.moduleID);
            msg += " sg ";
            msg += str(self.moduleID);
            msg += "\nES t ";
            msg += str(self.relTime);
            msg += "\n";
            self.msg += msg;
                
    # produce the publish to consume conn
    def recordPublishToConsumeConn(self):
        if (self.action == "PUBLISH"):
            LogEvent.PublishEvent = self;
        elif ((self.action == "CONSUME") or (self.action == "START_CONSUME")):
            if (LogEvent.PublishEvent==""):
                return;
            else:
                self.ce = LogEvent.PublishEvent.eventID;
                self.msgID = LogEvent.PublishEvent.eventID;
                msg = "BS id x1 tid x1 eid x1 etid x1 c cMessage n ";
                msg = string.replace(msg, "x1", str(LogEvent.PublishEvent.eventID));
                msg += "\"";
                msg += " PUBLISH->";
                msg += self.action;
                msg += "\"";
                msg += "\nES t ";
                msg += str(self.relTime);
                msg += "\n";
                LogEvent.PublishEvent.msg = msg;
                LogEvent.PublishEvent = "";
                
            
            
        

    # This method is used to generate the HME messages
    def genHMEMsg(self, pastEvent):
        # do not generate a message where a message is already present
        # this should be present for "RESPONSE RESTART" after hard deadline violation
##        if (self.msg != ""):
##            return;

        # do not generate the first edge from INJECT event to DETECT event
        # if the fault is not a deadline violation
        if (pastEvent.action.find("INJECT") != -1):
            if (LogEvent.DVDetected == 0 ):
                return;

                        
        msg = "BS id x1 tid x1 eid x1 etid x1 c cMessage n ";
        msg = string.replace(msg, "x1", str(pastEvent.eventID));
        msg += "\"";

        #Display the past event action (source of the edge) for all cases except when
        #the source is a "Recieve" message.
        if (pastEvent.action.find("RECEIVE") == -1):
            msg += pastEvent.action;
        else:
            msg += self.action;
            
        msg += "\"";
##        msg += "->";
##        msg += self.action;
        msg += "\nES t ";
        msg += str(self.relTime);
        msg += "\n";
        pastEvent.msg += msg;
        self.ce = pastEvent.eventID;
        self.msgID = pastEvent.eventID;
        return;
        
        
        
            
        
            
    # generate start message
    def generateStartMsg(self):
        self.msg = "BS id x1 tid x1 eid x1 etid x1 c cMessage n ";
        self.msg = string.replace(self.msg, "x1", str(self.eventID));
        action = string.replace(self.action,"_PROCESS","");
        self.msg += action;
        #self.msg += "_";
        #self.msg +=  self.module;
        self.msg += "\nES t ";
        self.msg += str(self.relTime);
        self.msg += "\n";
        

    # post processing to generate the correct extra event for
    # start,stop partitions
    # start processes
    def postProcessing(self,startTime):
        newLogEvent = 0;

        # return with return event in cases of synchronous method end call        
        if (self.returnEvent != ""):
            return self.returnEvent;

        # if the causal event is already set (for invoke,restart)
        # return with null event
        if (self.msgID != -1):
            return newLogEvent;
        # return with null event for all HME events
        if (self.type == "HME"):
            if ((self.desc.find("INJECT") == -1) or (LogEvent.DVDetected ==1)):
                return newLogEvent;
            else:
                return self.generateInjectEvent(startTime);
        

        # return the extra event to start & stop partitions from MM and
        # start processes (where reqd) from partition
        if ((self.source == "MM") or (self.action == "START_PROCESS")):#or(self.action == "END_PROCESS")):
            #self.eventID = self.eventID +1;
            newLogEvent = LogEvent(self.line, startTime, self.eventID-1,1);
            newLogEvent.generateStartMsg();
            newLogEvent.module = self.source;
            newLogEvent.moduleID = LogEvent.processes[self.source];
            self.eventID = self.eventID +1;
            self.ce = newLogEvent.eventID;
            self.msgID = self.ce;
        return newLogEvent;

    # for self-loop generation in inject for non-deadline violation cases
    def generateInjectEvent(self,startTime):
        newLogEvent = LogEvent(self.line, startTime, self.eventID,1);
        newLogEvent.generateStartMsg();
        newLogEvent.ce = self.eventID;
        newLogEvent.msgID = self.eventID;
        msg = "BS id x1 tid x1 eid x1 etid x1 c cMessage n ";
        msg +="\"";
        msg += self.action;
        msg += " -> ";
        msg += self.actionInfo;
        msg +="\"";
        
        msg += "\nES t ";
        msg += str(self.relTime);
        msg += "\n";
        self.msg = string.replace(msg,"x1",str(self.eventID));
        return newLogEvent;

    def constructGateAndCCInfo(self):
        str_ = "";
        if (self.moduleID not in LogEvent.ModuleWithStartToEndConn.keys()):
            return str_;
        str_= '''
CC sm id sg id dm id dg 0
''';
        str_ = string.replace(str_,"id",str(self.moduleID));
        LogEvent.ModuleWithStartToEndConn[self.moduleID]=str_;
        
        str_ = '''
GC m id g 0 n in o 0
GC m id g id n out o 1
''';
        str_ = string.replace(str_,"id",str(self.moduleID));
        
        return str_;
        
    # create the module creation string            
    def constructCreateStr(self):
        str_ = self.constructGateAndCCInfo();
        str_ += "MC id ";
        str_ += str(LogEvent.processes[self.module]);
        str_ += " c ";
        str_ += self.module;
        str_ += " t ";
        str_ += self.module;
        str_ += " pid 1 ";
        str_ += " n ";
        str_ += self.module;
        self.strValue = str_;
        return self.strValue;

    # create the event string
    def constructEventStr(self):
        str_ = "E # ";
        str_ += str(self.eventID);
        str_ += " t ";
        str_ += str(self.relTime);
        str_ += " m ";
        if (self.module in LogEvent.processes.keys()):
            str_ += str(LogEvent.processes[self.module]);
        else:
            #print 'module id  not found in ' + self.line
            str_ += "-1"
        if (self.ce != -1):
            str_ +=" ce ";
            str_ +=str(self.ce);
            str_ += " msg ";
            if (self.msgID == -1):
                str_ += str(self.ce); #message id  ( for the message on the incoming edge) is set to causal event's id
            else:
                str_ += str(self.msgID); 
        else:
            str_ += " msg -1";
            str_ += " // ";
            str_ += self.action;
        if ((self.action == "END_PROCESS") and (self.startEvent !=0)):
            self.recordStartToEndConn();
        # add the action as a comment. 
        str_ += "\n- ";
        str_ += self.action;
        if (self.msg != ""):
            str_+="\n";
            str_+=self.msg;
        self.strValue = str_;
        return self.strValue;


    # returns the string for the elog file
    # invokes the correct string creation method
    # based on the activity -> module creation or event 
    def toString(self):
        if (self.strValue != ""):
            return self.strValue;
        if (self.action == "CREATE"):
            return self.constructCreateStr();
        return self.constructEventStr();
            

#global variables            
LogEvent.processes={};# store a dictionary of module name and module id
LogEvent.InvokeEvents = [];# store the last invoke event
LogEvent.HMEvents=[]; # store the last set of hm events
LogEvent.DVDetected = 0;# store whether deadline violation was detected
LogEvent.StartEvent={}; # store the start_process event on a per module basis (key module id)
LogEvent.PublishEvent = "";#store publish event to generate causal edge to Consume Event
LogEvent.ModuleWithStartToEndConn={} # to generate gc and cc for sh connections between start and end process


# parser class
# parses the log file and generates the elog file
class LogParser:
    
    # Constructor accepts logfile name and output (.elog) file name
    def __init__(self, logFile, outputFile):
        self.logFileName = logFile;
        self.outputFile = outputFile;
        self.startTime = -1;
        self.eventID = 1;
        self.CreateContents=[];
        self.EventContents={};
        #reads the log file
        self.readLog();
        # updates hme data
        self.updateHME();
        # outputs logfile
        self.output();
        
    #for output
    #create the header which is the compound module create
    def createOutputHeader(self):
        header = "MC id 1 c cCompoundModule t GPSAssembly n GPSAssembly cm 1\n";
        return header;

    #for output
    #creates header after module creation and before events are started
    #specifies version number
    #command to initialize all modules
    def createEventHeader (self, num):
        header = '''
SB v 1024 rid GPSAssembly-0-20080819-12:11:11-16382\n
E # 0 t 0 m 1 msg -1
MB sm 1 tm 2 m initialize
BS id 1 tid 1 eid 1 etid 1 c cMessage n StartMM
ES t 0.1
ME
''';
        #initialize calls for all modules
        for i in range(3,num+2):
            module = str(i);
            header +="MB sm 1 tm ";
            header += module;
            header += " m initialize \n";
            header += "ME\n";
        header +="E # 1 t 0  m 2 ce 0 msg 1\n";

        return header;

        

    # for output
    # generates output file
    def output(self):
        try:
            # create output file            
            output_file = open(self.outputFile,"w")
            # first header (compound module creation i.e. parent of all modules)
            header1 = self.createOutputHeader();
            output_file.write(header1);
            # module creation strings
            for i in self.CreateContents:
                str = i.toString();
                output_file.write(str);
                output_file.write("\n");
            #writing the cc part.
            endkeys = LogEvent.ModuleWithStartToEndConn.keys();
            endkeys.sort();
            for i in endkeys:
                output_file.write(LogEvent.ModuleWithStartToEndConn[i]);
                
                
            #version number +
            #module initialization commands
            header2 = self.createEventHeader(len(LogEvent.processes));
            output_file.write(header2);        
            y = self.EventContents.keys();
            y.sort();
            #event commands
            for i in y:
                str = self.EventContents[i].toString();
                output_file.write(str);
                output_file.write("\n");
            output_file.write("SE");
            output_file.close();
        except IOError :
            print "log_parser.py : I/O error!  to open file "+ self.outputFile

    # updates the sequence of HME events
    # to tie them with a causal edge
    def updateHME(self):
        #print " num hme " + str(len(LogEvent.HMEvents))
        if (len(LogEvent.HMEvents)==0):
            return;
        for i in range(0,len(LogEvent.HMEvents)):
            if (LogEvent.HMEvents[i].action != "INJECT"):
                LogEvent.HMEvents[i].genHMEMsg(LogEvent.HMEvents[i-1]);
            

    #handles each new line in the log file
    def processEvent(self,line):
        logEvent = LogEvent(line,self.startTime,self.eventID);
        # store create messages
        if (logEvent.action == "CREATE"):
            self.CreateContents.append(logEvent);
        else:
        # handle event  messages
            # check for extra event
            extraEvent = logEvent.postProcessing(self.startTime);
            # if extra event
            if (extraEvent != 0):
                # store extra event
                self.EventContents[extraEvent.eventID] = extraEvent;
                # update event id to max of the two event ids
                self.eventID = max([logEvent.eventID,extraEvent.eventID]);
            else:
            # no extra event, set the event ID                        
                self.eventID = logEvent.eventID;

            # store the new event                    
            self.EventContents[logEvent.eventID] =logEvent;

        # update the start time after the first line (START:MM)
        if (self.startTime == -1):
            self.startTime = logEvent.absTime;
        return logEvent;

    #This is required for the case wherein deadline violation is detected and forceful stop happens
    def generateEndEvent(self,line,oldLogEvent):
        newline = string.replace(line,"HME","TRC");
        pos1 = newline.find("PROCESS")+len("PROCESS")
        pos2 = newline.find("forcefully");
        initPart = newline[:pos1-len("PROCESS")]
        module =string.strip(newline[pos1+1:pos2]);
        outline = initPart;
        outline += "END_PROCESS:";
        outline += module;
        logEvent = self.processEvent(outline);
        newoutline = string.replace(outline,"END_PROCESS","STOP_PROCESS");
        logEvent = self.processEvent(newoutline);
        logEvent.ce = oldLogEvent.eventID;
        #print " new event id " + str(logEvent.eventID) + " old log event id "+ str(oldLogEvent.eventID)
        logEvent.msgID =1;
        msg = "BS id 1 tid 1 eid 1 etid 1 c cMessage n STOP";
        msg += "\nES t ";
        msg += str(logEvent.relTime);
        msg += "\n";
        oldLogEvent.msg = msg;
        
        
        
        
        
   # This method checks if the line needs to be ignored
   # because the associated process need not be output in elog
    def checkForIgnoreProceses(self,line):
        for i in IgnoreProcesses:
            if (line.find(i)!= -1):
                return 1;
        for i in IgnoreStart:
            if (line.find(i)!= -1):
                if (line.find("START_PROCESS")!=-1):
                    return 1;
        return 0;
    
        
        
        
    # reads the log file
    # invokes the LogEvent constructor for each line of the log file
    def readLog(self):
        try:
            log_file = open(self.logFileName, 'r')
            self.eventID = 1;#initial event id
            self.startTime = -1;# initial start time
            for i in log_file.readlines():
                if (i == ""):
                    continue
                if (i[-1] == "\n"):
                    i = i[:-1]
                if (i == ""):
                    continue
                if (self.checkForIgnoreProceses(i)):
                    continue;
                logEvent = self.processEvent(i);
                #This is required for the case wherein deadline violation is detected and forceful stop happens
                if (LogEvent.DVDetected == 1):
                    if (logEvent.action.find("forcefully stopped") !=-1):
                        self.generateEndEvent(i,logEvent);
                

                
        except IOError :
            print "log_parser.py : I/O error! Unable to open file "+ self.logFileName
            


def main():
    if (len(sys.argv) != 3):
        print "\nUsage: LogParser.py <LogFileName> <OutputFileName>"
        sys.exit(1)
    log = LogParser(sys.argv[1], sys.argv[2])
    
if __name__ == '__main__':
    main()
