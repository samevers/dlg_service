#!/bin/bash

./stop.sh
ulimit -c unlimited
nohup ./httpserver -f http.cfg -k HTTP ../../log >std 2>err &
