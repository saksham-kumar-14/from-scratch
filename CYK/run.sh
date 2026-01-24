#!/bin/bash

ocamlc main.ml -o run
./run
rm -rf run main.cmi main.cmo
