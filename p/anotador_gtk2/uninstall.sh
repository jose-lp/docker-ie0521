#!/bin/sh

sudo apt -y  purge libyaml-cpp-dev libyaml-doc 

sudo rm /usr/local/lib/libyaml-cpp.*
sudo rm -r /usr/local/include/yaml-cpp
sudo rm -r /usr/local/lib/cmake/yaml-cpp/
