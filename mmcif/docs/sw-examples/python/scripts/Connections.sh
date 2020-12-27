#!/bin/bash

chimeraPath="/path/to/chimera"

if [ $# -ne 1 ]
then 
    echo "Usage: ./Connections.sh /path/to/file.cif"
    exit
fi

ABS=$(readlink -f "$1")

filename=$(basename "$ABS")
filename=${filename%.*}

pathname=$ABS
pathname=${pathname%/*}

python Connections.py $ABS

$chimeraPath "$pathname/$filename.com"
