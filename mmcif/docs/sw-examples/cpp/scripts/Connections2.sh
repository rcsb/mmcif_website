#!/bin/bash

chimeraPath="/path/to/chimera"

if [ $# -ne 1 ]
then 
    echo "Usage: ./Connections2.sh /path/to/file.cif"
    exit
fi

ABS=$(readlink -f "$1")

filename=$(basename "$ABS")
filename=${filename%.*}

pathname=$ABS
pathname=${pathname%/*}

./Connections2 $ABS

$chimeraPath "$pathname/$filename.com"

