#!/bin/bash
# fill names of hidoku instances into the lists for the competition script

path=`pwd`

# all files recursively
find $path/hidokus/ | grep ".txt" > files.list

# all files in the sub directory unsat recursively
find $path/hidokus/unsat/ | grep ".txt" > unsatHidokus.list

