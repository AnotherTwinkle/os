typedef unsigned  char  u8;
typedef           char  s8; 
typedef unsigned short u16;
typedef          short s16;
typedef unsigned   int u32;
typedef            int s32;

#define RAND_MAX 32767
#define TRUE 1
#define FALSE 0

void memcpy(char* dest, char* src, int num_bytes);
void memset(char* start, char c, int num_bytes);

int strlen(const char *str);
char *strcpy(char *dest, const char *src);

void reverse(char str[], int length);

char *itoa(int value);
char *itoh(int value);

void sleep(u32 ms);

int rand();
void srand(u32 seed);

