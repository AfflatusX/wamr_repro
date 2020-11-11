#!/bin/bash
set -e

cd app && npm run asbuild:optimized && cd ..
wamrc --target=i386 -o wasm_build/app.aot app/build/optimized.wasm
xxd -i wasm_build/app.aot > src/compiled_wasm_app.h
