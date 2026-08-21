typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

static void ReadSram_Core(const u8 *src, u8 *dest, u32 size)
{
    const u8 *s;
    u8 *d;

    s = src;
    d = dest;
    while (size--)
        *d++ = *s++;
}
