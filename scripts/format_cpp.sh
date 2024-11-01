#!/bin/bash

set -e

find ./applications/ -iname "*.cpp" -o -iname "*.h" -o -iname "*.hpp" -o -iname "*.cc" -o -iname "*.hh" | xargs git-clang-format
find ./libraries/ -iname "*.cpp" -o -iname "*.h" -o -iname "*.hpp" -o -iname "*.cc" -o -iname "*.hh" | xargs git-clang-format
