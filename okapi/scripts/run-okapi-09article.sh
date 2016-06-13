#!/bin/bash

source /home/okapi/environmentSettings.bshrc

cd $OKAPI_BINDIR

./convert_runtime 09article < /home/okapi/input/Chem_para_1000.exch 

./ix1 -mem 500 -delfinal -doclens 09article 0 | ./ixf 09article 0

cd $OKAPI_JAVABIN
java searching

