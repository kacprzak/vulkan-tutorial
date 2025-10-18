#!/usr/bin/env bash
set -euo pipefail

BUILD_TYPE=${1:-release}
BUILD_DIR=build/$BUILD_TYPE

meson $BUILD_DIR --buildtype $BUILD_TYPE
