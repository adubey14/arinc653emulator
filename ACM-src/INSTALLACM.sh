#!/bin/bash
if [ $# -ne 1 ]
then
    echo "Error in $0 - Invalid Argument Count"
    echo "Syntax: $0 install_dir_path"
    exit
fi

LOG_FILE=`pwd`/INSTALL`date +%d%b%y`.log
INST_DIR=$1
touch $LOG_FILE

tail -f $LOG_FILE &
tailpid=$!
date >$LOG_FILE
echo "INSTALLING TO $INST_DIR" >>$LOG_FILE
echo "LOGGING TO $LOG_FILE"     >>$LOG_FILE

echo "./autogen.sh --enable-logging-hmevent --prefix=$INST_DIR --enable-corba " >>$LOG_FILE 2>&1
./autogen.sh --enable-logging-hmevent --prefix=$INST_DIR --enable-corba >>$LOG_FILE 2>&1
make -j4 >>$LOG_FILE 2>&1
make install >>$LOG_FILE 2>&1
echo "#------ Finished Installation. Check $LOG_FILE for errors--------"  

kill $tailpid













