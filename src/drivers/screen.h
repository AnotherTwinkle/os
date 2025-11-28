void SCREEN_INIT();

int get_offset(int row, int col);

int get_cursor();
void set_cursor(int offset);

void print_char(char c, int row, int col, char attr);
void print_at(int row, int col, char* msg);
void print(char* msg);
