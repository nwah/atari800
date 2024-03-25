#!/bin/bash

pushd ../..

# ./autogen.sh
# emconfigure ./configure --target=libatari800
emmake make
popd

cp ../libatari800.a .

# rm -rf ./build
# mkdir build
cp index.html build/index.html
cp keymap.js build/keymap.js

emcc \
	-s WASM=1 \
	-s EXPORTED_FUNCTIONS="[ \
		'_malloc', \
		'_free', \
		'_main', \
		'_init_web_state', \
		'_init_default', \
		'_init_without_basic', \
		'_init_executable', \
		'_next_frame', \
		'_get_pixels', \
		'_get_audio_buffer_len', \
		'_get_audio_buffer', \
		'_joystick', \
		'_console_keys', \
		'_key_press', \
		'_key_release', \
		'_set_executable_file', \
		'_set_basic_enabled', \
		'_insert_cartridge', \
		'_remove_cartridge', \
		'_mount_disk', \
		'_unmount_disk', \
		'_libatari800_warmstart', \
		'_libatari800_coldstart', \
		'_libatari800_reboot_with_file', \
		'_libatari800_get_sound_buffer', \
		'_libatari800_get_sound_buffer_len', \
		'_libatari800_get_sound_buffer_allocated_size', \
		'_libatari800_get_sound_frequency', \
		'_libatari800_get_num_sound_channels', \
		'_libatari800_get_sound_sample_size', \
		'_libatari800_next_frame', \
		'_libatari800_get_screen_ptr', \
		'_libatari800_init', \
		'_libatari800_exit', \
		'_libatari800_clear_input_array']" \
	-s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'FS']" \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s STACK_SIZE=2097152 \
	--pre-js wrapper.js \
	wasm_main.c libatari800.a -o build/libatari800.js \
	--preload-file ./atbasic.car@/basic.car \
	--preload-file ~/Atari/Atari\ DOS\ 2.5\ Original.atr@/dos25.atr \
	--preload-file ~/Atari/ROMs/Asteroids.rom@/Asteroids.rom