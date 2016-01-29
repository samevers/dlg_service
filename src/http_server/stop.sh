#!/bin/bash

runpath=`pwd`
kill -9 `ps -ef | grep $runpath | grep lt-httpserver | awk '{print $2}'`
