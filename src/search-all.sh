#!/bin/bash
# Search snapshots for lines containing provided snapshot number

if [ -z "$1" ]; then
    echo "Usage: ./search-all snapshot_number"

else
    grep "snapshot:  $1" records/*
    grep "snapshot: $1" records/*

fi

