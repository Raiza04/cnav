#!/bin/bash

set -e
cmake -B build
cmake --build build --clean-first --parallel
