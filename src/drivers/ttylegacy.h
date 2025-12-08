#define TTY_VIDEO_ADDRESS 0xb8000
#define TTY_MAXR 25
#define TTY_MAXC 80

#define TTY_WHITE_ON_BLACK 0x0f

#define REG_TTY_SCREEN_CTRL 0x3D4
#define REG_TTY_SCREEN_DATA 0x3D5

void TTY_SCREEN_INIT();
void tty_clear_screen();

int get_tty_offset(int row, int col);

int get_tty_cursor();
void set_tty_cursor(int offset);

void tty_print_char(char c, int row, int col, char attr);
void tty_print_at(int row, int col, char* msg);
void tty_print(char* msg);
void tty_println(char* msg);
void tty_flush(char *buf);


void tty_scroll_up();
int  tty_handle_scroll(int offset);
