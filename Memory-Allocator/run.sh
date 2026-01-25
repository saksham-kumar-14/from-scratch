#!/bin/bash

gcc main.c lib.c -o run -Wno-deprecated-declarations
echo "<< Running test >>"
./run
rm run
