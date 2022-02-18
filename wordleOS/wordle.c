#include "words.h"
#include <support/hardware/scancodes.h>
#include <support/include/stdkern.h>
#include <support/include/vfs.h>

struct state {
	char word[6];
	char tries[6][6];
	char last;
	char letter;
	char outcome;
};

static vfs_node_t *s_screen;
static vfs_node_t *s_keyboard;
struct state s;

// This function will print the letter in the position X, Y, with the given
// color. The color should be green, yellow or gray, although you can
// technically use any color you want.
void
print_letter(char letter, int color, int x, int y)
{
	// Compute the real offset.
	int offset = (80 * y + x) * 2;

	short value = (color << 8) | letter;
	fs_write(s_screen, offset, &value, 2);
}

void
print_word(char *word, int color, int x, int y)
{
	while (word && *word) {
		print_letter(*word, color, x, y);
		x = x + 2;
		word++;
	}
}

void
render_options()
{
	for (int o = 0; o < s.last; o++) {
		for (int i = 0; i < 5; i++) {
			print_letter(s.tries[o][i], 15, 35 + 2 * i, 2 + 2 * o);
		}
	}
	if (s.last < 6) {
		for (int i = 0; i < s.letter; i++) {
			print_letter(s.tries[s.last][i],
			             15,
			             35 + 2 * i,
			             2 + 2 * s.last);
		}
	}
}

void
render_keyboard()
{
	char *letters1 = "QWERTYUIOP";
	char *letters2 = "ASDFGHJKL~";
	char *letters3 = "ZXCVBNM";

	for (int i = 0; i < strlen(letters1); i++) {
		print_letter(letters1[i], 15, 35 + 2 * i, 18);
	}
	for (int i = 0; i < strlen(letters2); i++) {
		print_letter(letters2[i], 15, 35 + 2 * i, 20);
	}
	for (int i = 0; i < strlen(letters3); i++) {
		print_letter(letters3[i], 15, 37 + 2 * i, 22);
	}
}

void
paint_at(int x, int y, int color)
{
	int offx = x;
	int offy = y;
	int off = 80 * offy + offx;
	char col = color;
	fs_write(s_screen, 2 * off + 1, &col, 1);
}

void
paint_try(int x, int y, int color)
{
	paint_at(35 + 2 * x, 2 + 2 * y, color);
}

void
paint_keyboard(char letter, int color)
{
	char *letters1 = "QWERTYUIOP";
	char *letters2 = "ASDFGHJKL~";
	char *letters3 = "ZXCVBNM";

	for (int i = 0; i < strlen(letters1); i++) {
		if (letter == letters1[i]) {
			paint_at(35 + 2 * i, 18, color);
			return;
		}
	}
	for (int i = 0; i < strlen(letters2); i++) {
		if (letter == letters2[i]) {
			paint_at(35 + 2 * i, 20, color);
			return;
		}
	}
	for (int i = 0; i < strlen(letters3); i++) {
		if (letter == letters3[i]) {
			paint_at(37 + 2 * i, 22, color);
			return;
		}
	}
}

int
in_word(char letter)
{
	for (int i = 0; i < 6; i++) {
		if (s.word[i] == letter) {
			return 1;
		}
	}
	return 0;
}

void
paint()
{
	for (int y = 0; y < s.last; y++) {
		for (int x = 0; x < 6; x++) {
			char ref = s.word[x];
			char this = s.tries[y][x];
			if (ref == this) {
				/* Paint it green. */
				paint_try(x, y, 10);
				paint_keyboard(this, 10);
			} else if (in_word(this)) {
				/* Paint it yellow. */
				paint_try(x, y, 14);
				paint_keyboard(this, 14);
			} else {
				/* Paint it gray. */
				paint_try(x, y, 8);
				paint_keyboard(this, 8);
			}
		}
	}
}

void
clear()
{
	for (int y = 0; y < 25; y++) {
		for (int x = 0; x < 80; x++) {
			int offt = 80 * y + x;
			short letter = ' ' | 0x0F << 8;
			fs_write(s_screen, offt * 2, &letter, 2);
		}
	}
}

void
render_screen()
{
	clear();
	render_options();
	render_keyboard();
	paint();
}

void
wait_input()
{
	char letter;
	int read;

	for (;;) {
		read = fs_read(s_keyboard, 0, &letter, 1);
		if (read > 0 && (letter & 0x80) == 0) {
			letter = us_scancodes_1[letter].shift;
			// Sorry, no time for EÑE
			if (letter == ':') {
				letter = '~';
			}
			/* It is a letter */
			if ((letter >= 'A' && letter <= 'Z' || letter == '~')
			    && s.letter < 5) {
				s.tries[s.last][s.letter] = letter;
				s.letter++;
				return;
			}

			/* It is a line break */
			if ((letter == VK_LF) && s.letter == 5) {
				s.letter = 0;
				if (!strcmp(s.word, s.tries[s.last])) {
					s.outcome = 1;
				}
				s.last++;
				if (s.last == 6) {
					s.outcome = -1;
				}
				return;
			}

			/* It is a backspace. */
			if ((letter == VK_BS) && s.letter > 0) {
				s.letter--;
				s.tries[s.last][s.letter] = 0;
				return;
			}
		}
	}
}

int
decode_date(vfs_node_t *clock)
{
	char date[16];
	fs_read(clock, 0, date, 16);

	// YYYYMMDD
	int number = 0;
	number = number * 10 + (date[4] - 0x30);
	number = number * 10 + (date[5] - 0x30);
	number = number * 10 + (date[6] - 0x30);
	number = number * 10 + (date[7] - 0x30);

	return number;
}

void
wordle(vfs_node_t *screen, vfs_node_t *keyboard, vfs_node_t *clock)
{
	int date = decode_date(clock);
	s_screen = screen;
	s_keyboard = keyboard;
	strcpy(&s.word, words_list[date]);
	for (int i = 0; i < 6; i++) {
		s.word[i] = s.word[i] & ~(1 << 5);
	}
	s.last = 0;
	s.letter = 0;

	for (;;) {
		render_screen();
		if (s.outcome == -1) {
			/* You lose */
			print_word("LOSER", 9, 34, 14);
			return;
		} else if (s.outcome == 1) {
			/* You win */
			print_word("WINNER", 10, 32, 14);
			return;
		}
		wait_input();
	}
}
