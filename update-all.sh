#!/bin/sh

#git submodule foreach "(git checkout master; git pull)&"

cd bibletime
git checkout mini
git pull

cd ../sword
git checkout master
git pull

cd ../clucene
git checkout master
git pull

cd ../curl
git checkout master
git pull