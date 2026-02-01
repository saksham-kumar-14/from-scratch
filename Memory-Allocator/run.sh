#!/bin/bash

gcc main.c lib.c -o run
echo "<< Running test >>"
./run
rm run
