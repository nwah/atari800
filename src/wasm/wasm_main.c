#include <stdio.h>
#include <emscripten.h>
#include <string.h>

#include <stdbool.h>

#include "../libatari800/libatari800.h"
#include "../colours.h"
#include "../akey.h"

typedef struct {
	bool basic_enabled;
	char *exe_file;
	char *cart_file;
	char *disk1_file;
	char *disk2_file;
} web_state_t;

const int GRAPHICS_WIDTH = 336;
const int GRAPHICS_HEIGHT = 192;

const int CANVAS_WIDTH = GRAPHICS_WIDTH;
const int CANVAS_HEIGHT = GRAPHICS_HEIGHT;

web_state_t web_state;
input_template_t input;
static unsigned char pixels[CANVAS_WIDTH * CANVAS_HEIGHT * 4];
static unsigned char audio_buffer[800];
static int audio_buffer_len = 0;

unsigned char* get_pixels() {
	return &pixels[0];
}

void stash_pixels() {
	unsigned char *screen = libatari800_get_screen_ptr();
	int x, y;

	screen += 384 * 24 + 24;
	for (y = 0; y < GRAPHICS_HEIGHT; y++) {
		for (x = 0; x < GRAPHICS_WIDTH; x++) {
			unsigned char c = screen[x];

			unsigned char r = Colours_GetR(c);
			unsigned char g = Colours_GetG(c);
			unsigned char b = Colours_GetB(c);

			const unsigned int offset = (GRAPHICS_WIDTH * 4 * y) + x * 4;
			pixels[offset + 0] = r;
			pixels[offset + 1] = g;
			pixels[offset + 2] = b;
			pixels[offset + 3] = 0xFF; // opacity
		}
		screen += 384;
	}
}

unsigned char* get_audio_buffer() {
	return &audio_buffer[0];
}

int get_audio_buffer_len() {
	return audio_buffer_len;
}

void stash_audio_buffer() {
	audio_buffer_len = libatari800_get_sound_buffer_len();
	unsigned char *buffer = libatari800_get_sound_buffer();
	int i;
	for (i = 0; i < audio_buffer_len; i++) audio_buffer[i] = buffer[i];
}

void set_executable_file(char *filename) {
	web_state.exe_file = filename;
}

void set_basic_enabled(bool enabled) {
	web_state.basic_enabled = enabled;
}

void insert_cartridge(char *filename) {
	web_state.cart_file = filename;
	printf("insert_cartridge: %s\n", filename);
	libatari800_insert_cartridge_auto_reboot(filename);
}

void remove_cartridge() {
	web_state.cart_file = NULL;
	libatari800_remove_cartridge_auto_reboot();
}

void insert_cassette(char *filename) {
	// web_state.cart_file = filename;
	printf("insert_cassette: %s\n", filename);
	// libatari800_insert_cassette_auto_reboot(filename);
}

void remove_cassette() {
	web_state.cart_file = NULL;
	libatari800_remove_cartridge_auto_reboot();
}

void mount_disk(int diskno, char *filename, bool read_only) {
	printf("mounting disk %d: %s\n", diskno, filename);
	switch (diskno) {
	case 0:
		web_state.disk1_file = filename;
		break;
	case 1:
		web_state.disk2_file = filename;
		break;
	}
	libatari800_mount_disk_image(diskno, filename, read_only);
}

void unmount_disk(int diskno) {
	switch (diskno) {
	case 0:
		web_state.disk1_file = NULL;
		break;
	case 1:
		web_state.disk2_file = NULL;
		break;
	}
	libatari800_dismount(diskno);
}

void init_default() {
	char *args[] = {
		"atari800",
		"-xl",
		"-ntsc",
		"-audio8",
		"-dsprate",
		"44100",
		"-cart",
		"basic.car",
	};
	libatari800_clear_input_array(&input);
	libatari800_init(sizeof(args) / sizeof(args[0]), args);
}

void init_without_basic() {
	char *args[] = {
		"atari800",
		"-xl",
		"-ntsc",
		"-audio8",
		"-dsprate",
		"44100",
		"-nobasic",
	};
	libatari800_clear_input_array(&input);
	libatari800_remove_cartridge_auto_reboot();
	libatari800_init(7, args);
}

void init_executable(char *filename) {
	libatari800_clear_input_array(&input);
	char *args[] = {
		"atari800",
		"-xl",
		"-ntsc",
		"-audio8",
		"-dsprate",
		"44100",
		"-run",
		filename
	};
	libatari800_init(8, args);
}

void next_frame() {
	libatari800_next_frame(&input);
	stash_pixels();
	stash_audio_buffer();
}

void console_keys(bool start, bool select, bool option) {
	input.start = start;
	input.select = select;
	input.option = option;
}

void joystick(int num, UBYTE joy, UBYTE trig) {
	switch (num) {
	case 0:
		input.joy0 = joy;
		input.trig0 = trig;
		break;
	case 1:
		input.joy1 = joy;
		input.trig1 = trig;
		break;
	case 2:
		input.joy2 = joy;
		input.trig2 = trig;
		break;
	case 3:
		input.joy3 = joy;
		input.trig3 = trig;
		break;
	}
}

void key_press(int keychar, int keycode, bool control, bool shift) {
	input.keychar = keychar;
	input.keycode = keycode;
	input.control = control;
	input.shift = shift;
}

void key_release(int keychar, int keycode, bool control, bool shift) {
	input.keychar = AKEY_NONE;
	input.keycode = keycode;
	input.control = control;
	input.shift = shift;
}

void init_web_state() {
	web_state.basic_enabled = true;
}

int main(int argc, char **argv) {
	init_web_state();	
	return 0;
}
