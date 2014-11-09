#!/bin/bash
echo ./ix1 -mem "$1 -$2 -$3 $4 $5 | ./ixf  $4 $5"
./ix1 -mem $1 -$2 -$3 $4 $5  | ./ixf $4 $5
