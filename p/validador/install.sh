#!/bin/sh

sudo apt update
sudo apt upgrade

sudo apt -y --allow-unauthenticated install libyaml-cpp-dev libyaml-doc

#Debian puede que no use la nueva version del api de libyaml-cpp, si falla compilacion mejor instalar como modulo desde codigo fuente
