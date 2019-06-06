#!/bin/bash
source bootstrap.sh



cd $CIAO_ROOT
make $@ 

result=$?
    script="ACE make $@"
    if (( $result ))
    then
        echo "$script failed"
        exit 1
    else
        echo "$script OK"
    fi



cd $DDS_ROOT
make $@
result=$?
    script="DDS make $@"
    if (( $result ))
    then
        echo "$script failed"
        exit 1
    else
        echo "$script OK"
    fi
cd $ACM_SRC
make $@

result=$?
    script="ACM make $@"
    if (( $result ))
    then
        echo "$script failed"
        exit 1
    else
        echo "$script OK"
    fi


var="$@"
mresult=`expr match "$var"  ".*clean*"`

if [ "$mresult" == "0" ]; then
make install
else
exit 0
fi

exit 0
