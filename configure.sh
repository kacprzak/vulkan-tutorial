#!/usr/bin/env bash
set -euo pipefail

BUILD_TYPE=${1:-release}
BUILD_DIR=build/$BUILD_TYPE

mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake -GNinja ../.. -DCMAKE_BUILD_TYPE=$BUILD_TYPE
