#!/bin/bash
set -eu

FILE="test_file"
if [ -f "$FILE" ]; then
    echo "File was found! Test passed!"
else 
    echo "File was not found! Test failed!"
    exit 1
fi 

exit 0