#!/bin/bash

set -e

find ./applications/ -iname "*.cpp" -o -iname "*.h" -o -iname "*.hpp" -o -iname "*.cc" -o -iname "*.hh" | xargs clang-format -i || echo "ran clang-format on applications"
find ./libraries/ -iname "*.cpp" -o -iname "*.h" -o -iname "*.hpp" -o -iname "*.cc" -o -iname "*.hh" | xargs clang-format -i || echo "ran clang-format on libraries"
