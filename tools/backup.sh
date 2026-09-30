#!/bin/sh
# TatsumiLoader - backup timestamp (jalan dari folder EXTERNAL-MLBB/).
# Exclude: obj/ (regenerable), .git/ (pakai push biasa).
cd "$(dirname "$0")/.." || exit 1
TS=$(date +%Y%m%d-%H%M)
7z a -t7z -m0=lzma2 -mx=1 "../EXTERNAL-MLBB-TatsumiLoader-${TS}.7z" \
  . -xr!'app/src/main/obj' -xr!'.git'
