typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

static void ReadSram_Core(const u8 *src, u8 *dest, u32 size)
{
    while (size--)
        *dest++ = *src++;
}
