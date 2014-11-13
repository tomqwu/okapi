#!/bin/bash
echo ./convert_runtime -c "$BSS_PARMPATH/ $1 < $OKAPI_PARSE/$2.exch"

./convert_runtime "$1" < "$OKAPI_PARSE/$2.exch"
