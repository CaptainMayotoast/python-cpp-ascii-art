#!/bin/bash

set -e

find /build/libraries -name 'meson.build' -print0 | xargs -0 -I {} muon fmt -i {}
find /build/applications -name 'meson.build' -print0 | xargs -0 -I {} muon fmt -i {}
