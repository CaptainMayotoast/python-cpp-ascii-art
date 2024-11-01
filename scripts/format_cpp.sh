#!/bin/bash

set -e

find . -type f $ -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.cc" -o -name "*.hh" $ -exec clang-format -i {} +
