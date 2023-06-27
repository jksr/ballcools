#!/bin/bash

#cd $SRC_DIR/src
make
mkdir -p $PREFIX/bin
cp ballcools $PREFIX/bin

