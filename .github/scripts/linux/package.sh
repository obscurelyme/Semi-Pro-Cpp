#!/bin/sh

mkdir release &&
  cp ./build/main ./release/ultra-cosmo-invaders &&
    cp -r ./assets ./release/assets &&
  tar vcfz ultra-cosmo-invaders-linux.tar.gz ./release
