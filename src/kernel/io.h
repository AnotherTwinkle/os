// Low level io implementation

#define byte unsigned char
#define word unsigned short

byte inportb(word port);
void outportb(word port, byte data);

word inportw(word port);
void outportw(word port, word data);