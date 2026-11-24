#!/bin/bash
gcc -o game *.c -lncurses -Wall -Wextra 2>&1
STATUS=$?
if [ $STATUS -eq 0 ]; then
    git push origin master
fi
exit $STATUS
