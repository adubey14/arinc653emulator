EXPECTED_ARGS=2

if [ $# -ne $EXPECTED_ARGS ]
then
  echo "Usage: `basename $0` {ExecutableName} {MM_PATH}"
  exit -1
fi
MM_PATH=$2
ExecutableName=$1
testcfg=testConfiguration.cfg
HYPERVal=4
echo "MAXITERATIONS=2"> $testcfg || exit 1
echo "HYPERPERIOD = $HYPERVal">> $testcfg || exit 1
echo "PARTITION_NAME = part1">> $testcfg || exit 1
echo "part1_EXECUTABLE = $ExecutableName">> $testcfg || exit 1
echo "part1_SCHEDULE=0,$HYPERVal">> $testcfg || exit 1

sudo $MM_PATH $testcfg

