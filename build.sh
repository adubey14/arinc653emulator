#!/bin/bash
source bootstrap.sh

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
