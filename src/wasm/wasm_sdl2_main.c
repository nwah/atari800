#include <stdio.h>
#include <SDL.h>
#include <emscripten.h>

#include <stdbool.h>

#include "../libatari800/libatari800.h"
#include "../colours.h"
#include "../akey.h"

const int GRAPHICS_WIDTH = 336;
const int GRAPHICS_HEIGHT = 192;

const int CANVAS_WIDTH = GRAPHICS_WIDTH;
const int CANVAS_HEIGHT = GRAPHICS_HEIGHT;

SDL_Window *window;
SDL_Renderer *renderer;

int ascii2scan(int code)
{

	int keycode;

	switch (code)
	{
	case 0x01:
		keycode = AKEY_CTRL_a;
		break;
	case 0x02:
		keycode = AKEY_CTRL_b;
		break;
	case 0x03:
		keycode = AKEY_CTRL_c;
		break;
	case 0x04:
		keycode = AKEY_CTRL_d;
		break;
	case 0x05:
		keycode = AKEY_CTRL_E;
		break;
	case 0x06:
		keycode = AKEY_CTRL_F;
		break;
	case 0x07:
		keycode = AKEY_CTRL_G;
		break;

	/*
							case 0x08 :
								keycode = AKEY_CTRL_H;
								break;
		*/
	/* TAB - see case '\t' :
							case 0x09 :
								keycode = AKEY_CTRL_I;
								break;
		*/
	case 0x0a:
		keycode = AKEY_CTRL_J;
		break;
	case 0x0b:
		keycode = AKEY_CTRL_K;
		break;
	case 0x0c:
		keycode = AKEY_CTRL_L;
		break;

	/*
							case 0x0d :
								keycode = AKEY_CTRL_M;
								break;
		*/
	case 0x0e:
		keycode = AKEY_CTRL_N;
		break;
	case 0x0f:
		keycode = AKEY_CTRL_O;
		break;
	case 0x10:
		keycode = AKEY_CTRL_P;
		break;
	case 0x11:
		keycode = AKEY_CTRL_Q;
		break;
	case 0x12:
		keycode = AKEY_CTRL_R;
		break;
	case 0x13:
		keycode = AKEY_CTRL_S;
		break;
	case 0x14:
		keycode = AKEY_CTRL_T;
		break;
	case 0x15:
		keycode = AKEY_CTRL_U;
		break;
	case 0x16:
		keycode = AKEY_CTRL_V;
		break;
	case 0x17:
		keycode = AKEY_CTRL_W;
		break;
	case 0x18:
		keycode = AKEY_CTRL_X;
		break;
	case 0x19:
		keycode = AKEY_CTRL_Y;
		break;
	case 0x1a:
		keycode = AKEY_CTRL_Z;
		break;
	case 8:
		keycode = AKEY_BACKSPACE;
		break;
	case 13:
		keycode = AKEY_RETURN;
		break;
	case 0x1b:
		keycode = AKEY_ESCAPE;
		break;
	case '0':
		keycode = AKEY_0;
		break;
	case '1':
		keycode = AKEY_1;
		break;
	case '2':
		keycode = AKEY_2;
		break;
	case '3':
		keycode = AKEY_3;
		break;
	case '4':
		keycode = AKEY_4;
		break;
	case '5':
		keycode = AKEY_5;
		break;
	case '6':
		keycode = AKEY_6;
		break;
	case '7':
		keycode = AKEY_7;
		break;
	case '8':
		keycode = AKEY_8;
		break;
	case '9':
		keycode = AKEY_9;
		break;
	case 'A':
	case 'a':
		keycode = AKEY_a;
		break;
	case 'B':
	case 'b':
		keycode = AKEY_b;
		break;
	case 'C':
	case 'c':
		keycode = AKEY_c;
		break;
	case 'D':
	case 'd':
		keycode = AKEY_d;
		break;
	case 'E':
	case 'e':
		keycode = AKEY_e;
		break;
	case 'F':
	case 'f':
		keycode = AKEY_f;
		break;
	case 'G':
	case 'g':
		keycode = AKEY_g;
		break;
	case 'H':
	case 'h':
		keycode = AKEY_h;
		break;
	case 'I':
	case 'i':
		keycode = AKEY_i;
		break;
	case 'J':
	case 'j':
		keycode = AKEY_j;
		break;
	case 'K':
	case 'k':
		keycode = AKEY_k;
		break;
	case 'L':
	case 'l':
		keycode = AKEY_l;
		break;
	case 'M':
	case 'm':
		keycode = AKEY_m;
		break;
	case 'N':
	case 'n':
		keycode = AKEY_n;
		break;
	case 'O':
	case 'o':
		keycode = AKEY_o;
		break;
	case 'P':
	case 'p':
		keycode = AKEY_p;
		break;
	case 'Q':
	case 'q':
		keycode = AKEY_q;
		break;
	case 'R':
	case 'r':
		keycode = AKEY_r;
		break;
	case 'S':
	case 's':
		keycode = AKEY_s;
		break;
	case 'T':
	case 't':
		keycode = AKEY_t;
		break;
	case 'U':
	case 'u':
		keycode = AKEY_u;
		break;
	case 'V':
	case 'v':
		keycode = AKEY_v;
		break;
	case 'W':
	case 'w':
		keycode = AKEY_w;
		break;
	case 'X':
	case 'x':
		keycode = AKEY_x;
		break;
	case 'Y':
	case 'y':
		keycode = AKEY_y;
		break;
	case 'Z':
	case 'z':
		keycode = AKEY_z;
		break;
	case ' ':
		keycode = AKEY_SPACE;
		break;
	case '\t':
		keycode = AKEY_TAB;
		break;
	case '!':
		keycode = AKEY_EXCLAMATION;
		break;
	case '"':
		keycode = AKEY_DBLQUOTE;
		break;
	case '#':
		keycode = AKEY_HASH;
		break;
	case '$':
		keycode = AKEY_DOLLAR;
		break;
	case '%':
		keycode = AKEY_PERCENT;
		break;
	case '&':
		keycode = AKEY_AMPERSAND;
		break;
	case '\'':
		keycode = AKEY_QUOTE;
		break;
	case '@':
		keycode = AKEY_AT;
		break;
	case '(':
		keycode = AKEY_PARENLEFT;
		break;
	case ')':
		keycode = AKEY_PARENRIGHT;
		break;
	case '<':
		keycode = AKEY_LESS;
		break;
	case '>':
		keycode = AKEY_GREATER;
		break;
	case '=':
		keycode = AKEY_EQUAL;
		break;
	case '?':
		keycode = AKEY_QUESTION;
		break;
	case '-':
		keycode = AKEY_MINUS;
		break;
	case '+':
		keycode = AKEY_PLUS;
		break;
	case '*':
		keycode = AKEY_ASTERISK;
		break;
	case '/':
		keycode = AKEY_SLASH;
		break;
	case ':':
		keycode = AKEY_COLON;
		break;
	case ';':
		keycode = AKEY_SEMICOLON;
		break;
	case ',':
		keycode = AKEY_COMMA;
		break;
	case '.':
		keycode = AKEY_FULLSTOP;
		break;
	case '_':
		keycode = AKEY_UNDERSCORE;
		break;
	case '[':
		keycode = AKEY_BRACKETLEFT;
		break;
	case ']':
		keycode = AKEY_BRACKETRIGHT;
		break;
	case '^':
		keycode = AKEY_CIRCUMFLEX;
		break;
	case '\\':
		keycode = AKEY_BACKSLASH;
		break;
	case '|':
		keycode = AKEY_BAR;
		break;
	default:
		keycode = AKEY_NONE;
		break;
	}

	printf("keycode of %c %d\n", code, keycode);

	return keycode;
}

input_template_t input;

void mainloop()
{
	static int frame = 0;
	static int idx = 0;
	static unsigned char pixels[CANVAS_WIDTH * CANVAS_HEIGHT * 4];
	static char msg[] = "abcdefghijklmnopqrstuvwxyz 1234567890 -=_+ [] | ,.<> '\" ;: !@#$%^&*()";

	if (frame == 0)
	{
		printf("JJS VALUE IS %d\n", input.keychar);
		printf("JJS VALUE IS %d\n", input.keycode);
	}

	frame++;

	SDL_Event e;
	input.keycode = AKEY_NONE;
	input.keychar = 0;
	input.shift = false;
	input.control = false;

	SDL_StartTextInput();

	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			// quit = true;
		}

		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{

			switch (e.key.keysym.sym)
			{
			case SDLK_RETURN:
				input.keycode = AKEY_RETURN;
				break;
			case SDLK_LEFT:
				input.keycode = AKEY_LEFT;
				break;
			case SDLK_RIGHT:
				input.keycode = AKEY_RIGHT;
				break;
			case SDLK_UP:
				input.keycode = AKEY_UP;
				break;
			case SDLK_DOWN:
				input.keycode = AKEY_DOWN;
				break;
			}
		}

		if (e.type == SDL_TEXTINPUT)
		{
			UBYTE khar = (UBYTE)e.text.text[0];
			input.keycode = ascii2scan(khar);
			input.shift = false;
			input.control = false;
		}
	}

	libatari800_next_frame(&input);

	SDL_Texture *texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		GRAPHICS_WIDTH, GRAPHICS_HEIGHT);

	/* print out portion of screen, assuming graphics 0 display list */
	unsigned char *screen = libatari800_get_screen_ptr();
	int x, y;

	screen += 384 * 24 + 24;
	for (y = 0; y < GRAPHICS_HEIGHT; y++)
	{
		for (x = 0; x < GRAPHICS_WIDTH; x++)
		{
			unsigned char c = screen[x];

			unsigned char r = Colours_GetR(c);
			unsigned char b = Colours_GetB(c);
			unsigned char g = Colours_GetG(c);

			const unsigned int offset = (GRAPHICS_WIDTH * 4 * y) + x * 4;
			pixels[offset + 0] = b;				   // b
			pixels[offset + 1] = g;				   // g
			pixels[offset + 2] = r;				   // r
			pixels[offset + 3] = SDL_ALPHA_OPAQUE; // a
		}
		screen += 384;
	}

	SDL_UpdateTexture(
		texture,
		NULL,
		&pixels[0],
		GRAPHICS_WIDTH * 4);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);
}

int init_system()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("SDL_ttf in SDL2",
							  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  CANVAS_WIDTH, CANVAS_HEIGHT,
							  0);
	renderer = SDL_CreateRenderer(window, -1, 0);

	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	return 0;
}

static void debug_screen()
{
	/* print out portion of screen, assuming graphics 0 display list */
	unsigned char *screen = libatari800_get_screen_ptr();
	int x, y;

	screen += 384 * 24 + 24;
	for (y = 0; y < 32; y++)
	{
		for (x = 8; x < 88; x++)
		{
			unsigned char c = screen[x];
			if (c == 0)
				printf(" ");
			else if (c == 0x94)
				printf(".");
			else if (c == 0x9a)
				printf("X");
			else
				printf("?");
		}
		printf("\n");
		screen += 384;
	}
}

int main(int argc, char **argv)
{
	printf("SYSTEM IS STARTING\n");
	int frame;
	input_template_t input;

	/* force the 400/800 OS to get the Memo Pad */
	char *test_args[] = {
		"atari800",
		"-atari",
	// 	"-basic",
		"-cart",
		"basic.car",
		"web.atr",
	};
	libatari800_init(sizeof(test_args) / sizeof(test_args[0]), test_args);

	libatari800_clear_input_array(&input);

	emulator_state_t state;
	cpu_state_t *cpu;
	unsigned char *pc;

	init_system();
	printf("SYSTEM IS READY TO GO\n");

	emscripten_set_main_loop(mainloop, 60, 1);

	frame = 0;
	while (frame < 200)
	{
		libatari800_get_current_state(&state);
		cpu = (cpu_state_t *)&state.state[state.tags.cpu]; /* order: A,SR,SP,X,Y */
		pc = &state.state[state.tags.pc];
		printf("frame %d: A=%02x X=%02x Y=%02x SP=%02x SR=%02x PC=%04x\n", frame, cpu->A, cpu->X, cpu->Y, cpu->P, cpu->S, pc[0] + 256 * pc[1]);
		libatari800_next_frame(&input);
		if (frame > 100)
		{
			debug_screen();
			input.keychar = 'A';
		}
		frame++;
	}
}