#!/bin/sh

# Get project stats/complexity

echo Number of files in the current directory: `ls -l | wc -l`
echo Number of directories in the current directory: `ls -l | grep ^d | wc -l`
echo Number of symbolic links in the current directory: `ls -l | grep ^l | wc -l`

# Get the number of lines of code in the current directory. Account for files with spaces
# in their names.

# Run make clean first.
echo Number of lines of code in the current directory: `find . -type f -name "*.*" -print0 | xargs -0 wc -l | grep total | awk '{print $1}'`