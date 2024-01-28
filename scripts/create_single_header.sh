#!/bin/sh

set -e

cargo install cpp-amalgamate

rm -rf build/single_header/
mkdir -p build/single_header/libdbc

files=$(find src -name "*.cpp")
files="${files} $(find include -name "*.hpp")"

cpp-amalgamate -d include -d build/_deps/fastfloat-src/include ${files} -o build/single_header/libdbc/libdbc.hpp