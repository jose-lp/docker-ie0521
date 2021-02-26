#!/bin/sh

#Script for concatenate avi files into one, should be run
#on the dir containing the avi files to join.
mencoder -oac copy -ovc copy -idx -o output.avi *.avi
