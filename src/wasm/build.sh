#!/bin/bash

pushd ../..

./autogen.sh
emconfigure ./configure --target=libatari800
emmake make
popd

rm -rf build
mkdir -p build

cp ../libatari800.a .
emcc \
	-s WASM=1 \
	-s INVOKE_RUN=1 \
	-s EXPORTED_FUNCTIONS="['_main', '_getPixels','_getSoundBuffer','_getSoundBufferLen','_getSoundBufferAllocatedSize','_getSoundFrequency','_getNumSoundChannels','_getSoundSampleSize']" \
	-s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s STACK_SIZE=2097152 \
	--pre-js libatari800.js \
	wasm_main.c libatari800.a -o build/index.html \
	--preload-file ./atbasic.car@/basic.car \
	--preload-file /Users/nwah/Atari/tnfs/bonkem.atr@/bonkem.atr \
	--preload-file /Users/nwah/Atari/tnfs/pop_a.atr@/pop_a.atr \
	--preload-file /Users/nwah/Atari/tnfs/Binary-Parasite.xex@/Binary-Parasite.xex
	
cd build && python3 -m http.server