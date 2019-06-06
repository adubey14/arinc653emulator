#!/bin/bash
source bootstrap.sh
cd $CIAO_ROOT
$ACE_ROOT/bin/mwc.pl -type gnuace CIAO_TAO.mwc
result=$?
    script="generate ace makefiles"
    if (( $result ))
    then
        echo "$script failed"
        exit 1
    else
        echo "$script OK"
    fi
cd $DDS_ROOT
$ACE_ROOT/bin/mwc.pl -type gnuace DDS_no_tests.mwc
result=$?
    script="generate dds makefiles"
    if (( $result ))
    then
        echo "$script failed"
        exit 1
    else
        echo "$script OK"
    fi


cd $ACM_SRC
./autogen.sh --prefix=$ACM_ROOT --enable-logging-fatal CXXFLAGS=-O2

result=$?
    script="generate acm makefiles"
    if (( $result ))
    then
        echo "$script failed"
        exit 1
    else
        echo "$script OK"
    fi

