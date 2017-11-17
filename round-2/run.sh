#!/bin/bash
set -e

PATH="$(sugoi target -b):${PATH}"
OUTPUT="saves/save-$(date +%Y-%m-%dT%H:%M:%S).txt"

mkdir -p saves
LAST=$(find saves -name 'save-*.txt' -type f | sort -n | tail -n1)
xonix "${LAST}" | tee "${OUTPUT}"
