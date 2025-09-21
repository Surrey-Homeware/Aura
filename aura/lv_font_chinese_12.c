/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --font NotoSansCJKsc-Regular.otf --size 12 --bpp 1 --format lvgl --symbols  !"#$%&'()+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~°çñü–—▼↓▲↑一七三上下中临二五京亮今位体使例保入六关凭到北午取后吗周器四在城夜天如存定将小市度您感或手报据接搜改效文新无日时暗更有本机来果气没浏消点现用电界的看确笔简索结网置脑行要览言记设访语请调进连配重闭问间需面页预 --output lv_font_chinese_12.c
 ******************************************************************************/

#include "lvgl.h"

#ifndef LV_FONT_CHINESE_12
#define LV_FONT_CHINESE_12 1
#endif

#if LV_FONT_CHINESE_12

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfc, 0x80,

    /* U+0022 "\"" */
    0xb6, 0x80,

    /* U+0023 "#" */
    0x28, 0xa7, 0xca, 0x4b, 0xf4, 0x92, 0x50,

    /* U+0024 "$" */
    0x23, 0xa3, 0x8, 0x30, 0xc1, 0xc, 0x5c, 0x40,

    /* U+0025 "%" */
    0x63, 0x24, 0x89, 0x42, 0x56, 0x6a, 0x42, 0x91,
    0x24, 0x49, 0x21, 0x80,

    /* U+0026 "&" */
    0x38, 0x48, 0x48, 0x50, 0x62, 0x92, 0x9c, 0x8e,
    0x72,

    /* U+0027 "'" */
    0xe0,

    /* U+0028 "(" */
    0x4a, 0x49, 0x24, 0x91, 0x20,

    /* U+0029 ")" */
    0x49, 0x12, 0x49, 0x25, 0x20,

    /* U+002B "+" */
    0x21, 0x3e, 0x42, 0x10,

    /* U+002C "," */
    0xd8,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x10, 0x84, 0x42, 0x10, 0x88, 0x42, 0x21, 0x0,

    /* U+0030 "0" */
    0x76, 0xe3, 0x18, 0xc6, 0x3a, 0x70,

    /* U+0031 "1" */
    0x61, 0x8, 0x42, 0x10, 0x84, 0xf8,

    /* U+0032 "2" */
    0x74, 0x42, 0x11, 0x88, 0x88, 0xf8,

    /* U+0033 "3" */
    0x74, 0x42, 0x33, 0x4, 0x31, 0x70,

    /* U+0034 "4" */
    0x18, 0x62, 0x8a, 0x4b, 0x2f, 0xc2, 0x8,

    /* U+0035 "5" */
    0xfc, 0x21, 0xe1, 0x84, 0x33, 0x70,

    /* U+0036 "6" */
    0x72, 0x21, 0x6c, 0xc6, 0x29, 0x70,

    /* U+0037 "7" */
    0xfc, 0x21, 0x4, 0x20, 0x82, 0x8, 0x20,

    /* U+0038 "8" */
    0x74, 0x63, 0x17, 0x4e, 0x31, 0x70,

    /* U+0039 "9" */
    0x74, 0xe3, 0x17, 0x84, 0x22, 0x70,

    /* U+003A ":" */
    0xc2,

    /* U+003B ";" */
    0xf0, 0xd, 0x80,

    /* U+003C "<" */
    0x1d, 0x8c, 0xc, 0xc,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0xe0, 0x60, 0xcc, 0xc0,

    /* U+003F "?" */
    0xe9, 0x11, 0x24, 0x40, 0x40,

    /* U+0040 "@" */
    0x1f, 0xc, 0x64, 0x9, 0x39, 0x9a, 0x64, 0x99,
    0x26, 0x4a, 0x8f, 0x90, 0x6, 0x0, 0xf8,

    /* U+0041 "A" */
    0x10, 0x50, 0xa1, 0x44, 0x4f, 0x91, 0x21, 0x82,

    /* U+0042 "B" */
    0xfa, 0x18, 0x61, 0xfa, 0x18, 0x63, 0xf8,

    /* U+0043 "C" */
    0x39, 0x18, 0x20, 0x82, 0x8, 0x11, 0x38,

    /* U+0044 "D" */
    0xf2, 0x28, 0x61, 0x86, 0x18, 0x62, 0xf0,

    /* U+0045 "E" */
    0xfc, 0x21, 0xf, 0x42, 0x10, 0xf8,

    /* U+0046 "F" */
    0xfc, 0x21, 0xf, 0x42, 0x10, 0x80,

    /* U+0047 "G" */
    0x3c, 0x86, 0x4, 0x8, 0xf0, 0x60, 0xa1, 0x3c,

    /* U+0048 "H" */
    0x86, 0x18, 0x61, 0xfe, 0x18, 0x61, 0x84,

    /* U+0049 "I" */
    0xff, 0x80,

    /* U+004A "J" */
    0x8, 0x42, 0x10, 0x84, 0x31, 0x70,

    /* U+004B "K" */
    0x8e, 0x69, 0x28, 0xd3, 0x48, 0xa3, 0x84,

    /* U+004C "L" */
    0x84, 0x21, 0x8, 0x42, 0x10, 0xf8,

    /* U+004D "M" */
    0xc3, 0x8f, 0x1d, 0x5a, 0xb5, 0x6c, 0xc9, 0x82,

    /* U+004E "N" */
    0x87, 0x1c, 0x69, 0xb6, 0x58, 0xe3, 0x84,

    /* U+004F "O" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xa2, 0x38,

    /* U+0050 "P" */
    0xfa, 0x18, 0x63, 0xfa, 0x8, 0x20, 0x80,

    /* U+0051 "Q" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xe3, 0x44,
    0x70, 0x60, 0x70,

    /* U+0052 "R" */
    0xfa, 0x18, 0x61, 0xfa, 0x48, 0xa2, 0x84,

    /* U+0053 "S" */
    0x7a, 0x8, 0x30, 0x38, 0x30, 0x61, 0x78,

    /* U+0054 "T" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,

    /* U+0055 "U" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x73, 0x78,

    /* U+0056 "V" */
    0x83, 0x89, 0x12, 0x26, 0xc5, 0xa, 0x14, 0x10,

    /* U+0057 "W" */
    0xc4, 0x53, 0x14, 0xc5, 0x2b, 0x4a, 0x94, 0xa3,
    0x28, 0xc6, 0x31, 0x80,

    /* U+0058 "X" */
    0x44, 0x88, 0xa1, 0x41, 0x5, 0xa, 0x22, 0xc4,

    /* U+0059 "Y" */
    0x44, 0x88, 0xa1, 0x43, 0x2, 0x4, 0x8, 0x10,

    /* U+005A "Z" */
    0xfc, 0x30, 0x84, 0x30, 0x84, 0x30, 0xfc,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x4e,

    /* U+005C "\\" */
    0x84, 0x10, 0x84, 0x10, 0x84, 0x10, 0x84, 0x20,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x9e,

    /* U+005E "^" */
    0x23, 0x14, 0xa9, 0x44,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0xb4,

    /* U+0061 "a" */
    0x70, 0x42, 0xf8, 0xc5, 0xe0,

    /* U+0062 "b" */
    0x82, 0x8, 0x2e, 0xce, 0x18, 0x61, 0x8b, 0xe0,

    /* U+0063 "c" */
    0x7e, 0x21, 0x8, 0x65, 0xe0,

    /* U+0064 "d" */
    0x4, 0x10, 0x5f, 0xc6, 0x18, 0x61, 0xcd, 0xd0,

    /* U+0065 "e" */
    0x79, 0x38, 0x7f, 0x83, 0x3, 0x80,

    /* U+0066 "f" */
    0x74, 0x4e, 0x44, 0x44, 0x44,

    /* U+0067 "g" */
    0x7e, 0x28, 0xa2, 0x72, 0x7, 0xe1, 0x85, 0xe0,

    /* U+0068 "h" */
    0x84, 0x21, 0x6c, 0xc6, 0x31, 0x8c, 0x40,

    /* U+0069 "i" */
    0x9f, 0xc0,

    /* U+006A "j" */
    0x20, 0x12, 0x49, 0x24, 0x9c,

    /* U+006B "k" */
    0x84, 0x21, 0x39, 0x53, 0x9a, 0x94, 0x40,

    /* U+006C "l" */
    0xaa, 0xaa, 0xb0,

    /* U+006D "m" */
    0xb3, 0x66, 0x62, 0x31, 0x18, 0x8c, 0x46, 0x22,

    /* U+006E "n" */
    0xb6, 0x63, 0x18, 0xc6, 0x20,

    /* U+006F "o" */
    0x7b, 0x38, 0x61, 0x87, 0x37, 0x80,

    /* U+0070 "p" */
    0xbb, 0x38, 0x61, 0x86, 0x2f, 0xa0, 0x82, 0x0,

    /* U+0071 "q" */
    0x7f, 0x18, 0x61, 0x87, 0x37, 0x41, 0x4, 0x10,

    /* U+0072 "r" */
    0xba, 0x49, 0x20,

    /* U+0073 "s" */
    0x68, 0x86, 0x11, 0xe0,

    /* U+0074 "t" */
    0x44, 0xf4, 0x44, 0x44, 0x30,

    /* U+0075 "u" */
    0x8c, 0x63, 0x18, 0xcd, 0xa0,

    /* U+0076 "v" */
    0x85, 0x14, 0x92, 0x28, 0xc3, 0x0,

    /* U+0077 "w" */
    0xcc, 0xa6, 0x55, 0x2a, 0xa7, 0x51, 0x98, 0xcc,

    /* U+0078 "x" */
    0x49, 0x23, 0xc, 0x31, 0x2c, 0x80,

    /* U+0079 "y" */
    0x85, 0x14, 0x92, 0x28, 0xc1, 0x4, 0x21, 0x80,

    /* U+007A "z" */
    0xf8, 0x84, 0x44, 0x23, 0xe0,

    /* U+007B "{" */
    0x69, 0x24, 0xa2, 0x49, 0x26,

    /* U+007C "|" */
    0xff, 0xfc,

    /* U+007D "}" */
    0xc9, 0x24, 0x8a, 0x49, 0x2c,

    /* U+007E "~" */
    0x64, 0x70,

    /* U+00B0 "°" */
    0x69, 0x96,

    /* U+00E7 "ç" */
    0x7e, 0x61, 0x8, 0x24, 0xc4, 0x11, 0x80,

    /* U+00F1 "ñ" */
    0x6c, 0xc1, 0x6c, 0xc6, 0x31, 0x8c, 0x40,

    /* U+00FC "ü" */
    0x50, 0x1, 0x18, 0xc6, 0x31, 0x9b, 0x40,

    /* U+2013 "–" */
    0xf8,

    /* U+2014 "—" */
    0xff, 0x80,

    /* U+2191 "↑" */
    0x23, 0xaa, 0x42, 0x10, 0x84, 0x21, 0x8,

    /* U+2193 "↓" */
    0x21, 0x8, 0x42, 0x10, 0x84, 0xab, 0x88,

    /* U+25B2 "▲" */
    0x0, 0x0, 0x60, 0x6, 0x0, 0xf0, 0x1f, 0x81,
    0xf8, 0x3f, 0xc3, 0xfc, 0x7f, 0xe7, 0xfe,

    /* U+25BC "▼" */
    0x7f, 0xe7, 0xfe, 0x3f, 0xc3, 0xfc, 0x1f, 0x81,
    0xf0, 0xf, 0x0, 0x60, 0x6, 0x0, 0x0,

    /* U+4E00 "一" */
    0xff, 0xe0,

    /* U+4E03 "七" */
    0x10, 0x4, 0x1, 0x0, 0x47, 0x7e, 0x24, 0x1,
    0x0, 0x40, 0x10, 0x44, 0x11, 0xf8,

    /* U+4E09 "三" */
    0x7f, 0x80, 0x0, 0x0, 0x0, 0x7f, 0x0, 0x0,
    0x0, 0x0, 0xff, 0xc0,

    /* U+4E0A "上" */
    0x4, 0x0, 0x80, 0x10, 0x2, 0x0, 0x7e, 0x8,
    0x1, 0x0, 0x20, 0x4, 0x1f, 0xfc,

    /* U+4E0B "下" */
    0xff, 0xe0, 0x80, 0x10, 0x3, 0x0, 0x58, 0x8,
    0x81, 0x0, 0x20, 0x4, 0x0, 0x80,

    /* U+4E2D "中" */
    0x8, 0x4, 0x3f, 0xf1, 0x18, 0x8c, 0x47, 0xff,
    0x11, 0x8, 0x4, 0x2, 0x0,

    /* U+4E34 "临" */
    0x24, 0x14, 0xfe, 0xa0, 0x58, 0x8a, 0x1, 0x5f,
    0xaa, 0x55, 0x4a, 0xa9, 0x45, 0xf8, 0xa1, 0x0,

    /* U+4E8C "二" */
    0x7f, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x7, 0xff,

    /* U+4E94 "五" */
    0x7f, 0xc1, 0x0, 0x20, 0x4, 0x7, 0xf8, 0x11,
    0x4, 0x20, 0x84, 0xff, 0xe0,

    /* U+4EAC "京" */
    0x8, 0x3f, 0xf0, 0x1, 0xfe, 0x40, 0x90, 0x27,
    0xf8, 0x20, 0x49, 0xb2, 0x39, 0x80,

    /* U+4EAE "亮" */
    0x4, 0x1f, 0xfc, 0x0, 0x1f, 0xc3, 0xf8, 0x0,
    0x3f, 0xfc, 0x79, 0x11, 0x2, 0x27, 0x87, 0x80,

    /* U+4ECA "今" */
    0x2, 0x0, 0x60, 0x9, 0x3, 0x4c, 0xe2, 0x20,
    0x0, 0x3f, 0xc0, 0x8, 0x1, 0x80, 0x10, 0x2,
    0x0,

    /* U+4F4D "位" */
    0x11, 0x4, 0x20, 0xbf, 0xb0, 0x6, 0x44, 0x48,
    0x89, 0x11, 0x24, 0x22, 0x84, 0x10, 0xbf, 0x80,

    /* U+4F53 "体" */
    0x11, 0x2, 0x10, 0x2f, 0xf6, 0x38, 0x63, 0x82,
    0x54, 0x25, 0x42, 0x92, 0x3f, 0xd2, 0x10, 0x21,
    0x0,

    /* U+4F7F "使" */
    0x11, 0x2, 0xfe, 0x21, 0x6, 0xfe, 0xe9, 0x22,
    0x92, 0x2f, 0xe2, 0x90, 0x26, 0x2, 0x70, 0x38,
    0xe0,

    /* U+4F8B "例" */
    0x3e, 0x29, 0x5, 0x22, 0xe7, 0x5d, 0x2a, 0xa5,
    0x52, 0xaa, 0x25, 0x44, 0x29, 0x5, 0x43, 0x80,

    /* U+4FDD "保" */
    0x0, 0x1, 0xfe, 0x28, 0x22, 0x82, 0x6f, 0xee,
    0x10, 0x21, 0x3, 0xff, 0x23, 0x82, 0x54, 0x29,
    0x22, 0x10,

    /* U+5165 "入" */
    0x8, 0x0, 0xc0, 0x4, 0x0, 0x60, 0x6, 0x0,
    0x50, 0x9, 0x80, 0x88, 0x10, 0x42, 0x6, 0x40,
    0x0,

    /* U+516D "六" */
    0xc, 0x0, 0x80, 0x10, 0x7f, 0xf0, 0x0, 0x2,
    0x4, 0x61, 0x4, 0x60, 0x58, 0xe, 0x0, 0x80,

    /* U+5173 "关" */
    0x0, 0x8, 0x41, 0x21, 0xfe, 0x4, 0x1, 0xf,
    0xfc, 0x30, 0x1a, 0xc, 0xce, 0xe, 0x0,

    /* U+51ED "凭" */
    0x10, 0x3, 0xf8, 0x84, 0x37, 0xf2, 0x10, 0x5f,
    0xc0, 0x0, 0xfc, 0x10, 0x82, 0x10, 0x82, 0xa0,
    0x70,

    /* U+5230 "到" */
    0xfc, 0x48, 0x94, 0xa7, 0xf9, 0x2, 0x48, 0x9f,
    0xe4, 0x89, 0x24, 0x7e, 0x18, 0x1c,

    /* U+5317 "北" */
    0x12, 0x2, 0x40, 0x49, 0x79, 0x61, 0x30, 0x24,
    0x4, 0x80, 0x90, 0xf2, 0x22, 0x44, 0x4f, 0x80,

    /* U+5348 "午" */
    0x20, 0x4, 0x0, 0xff, 0x22, 0x8, 0x40, 0x8,
    0x3f, 0xf8, 0x20, 0x4, 0x0, 0x80, 0x10, 0x2,
    0x0,

    /* U+53D6 "取" */
    0xfc, 0x9, 0x7d, 0x28, 0xbc, 0x94, 0x94, 0xf2,
    0x92, 0x32, 0x64, 0xf9, 0x41, 0x6c, 0x28, 0x80,

    /* U+540E "后" */
    0x1, 0xcf, 0xe1, 0x0, 0x3f, 0xf4, 0x0, 0x80,
    0x17, 0xf2, 0x82, 0xd0, 0x53, 0xfa, 0x41, 0x0,

    /* U+5417 "吗" */
    0xf, 0xb8, 0x2a, 0x8a, 0xa2, 0xa8, 0xab, 0xfa,
    0x7, 0x3d, 0x80, 0x40, 0x10, 0x18,

    /* U+5468 "周" */
    0x7f, 0xd0, 0x15, 0xf5, 0x11, 0x5f, 0x50, 0x15,
    0xf5, 0x45, 0x51, 0x67, 0xd8, 0x1c,

    /* U+5668 "器" */
    0x7b, 0xc9, 0x49, 0x29, 0x3d, 0xe0, 0x99, 0xff,
    0xc8, 0x67, 0xdf, 0x4a, 0x49, 0x49, 0xef, 0x0,

    /* U+56DB "四" */
    0xff, 0xe4, 0x99, 0x26, 0x49, 0x92, 0x68, 0xfc,
    0x6, 0x1, 0xff, 0xe0, 0x10,

    /* U+5728 "在" */
    0x8, 0x1, 0x3, 0xff, 0x88, 0x82, 0x10, 0xdf,
    0xe8, 0x41, 0x8, 0x21, 0x4, 0x20, 0xff, 0x80,

    /* U+57CE "城" */
    0x41, 0x90, 0x55, 0xff, 0xc4, 0x5f, 0x54, 0xd5,
    0x39, 0xca, 0xdf, 0xc4, 0xf2, 0x24,

    /* U+591C "夜" */
    0x4, 0x0, 0x83, 0xff, 0x91, 0x2, 0x7e, 0xc8,
    0xbb, 0x55, 0xa4, 0x22, 0x84, 0x60, 0x9b, 0x1c,
    0x30,

    /* U+5929 "天" */
    0xff, 0xc2, 0x0, 0x83, 0xff, 0xc, 0x3, 0x1,
    0x20, 0x84, 0xc0, 0xc0,

    /* U+5982 "如" */
    0x20, 0x4, 0x7f, 0xe8, 0xa5, 0x14, 0xa2, 0x94,
    0x52, 0x89, 0x91, 0x1a, 0x25, 0x7f, 0x8, 0x0,

    /* U+5B58 "存" */
    0x0, 0x1, 0x3, 0xff, 0x88, 0x3, 0x0, 0x5f,
    0x98, 0x27, 0x8, 0x3f, 0xe4, 0x20, 0x84, 0x13,
    0x80,

    /* U+5B9A "定" */
    0x2, 0x7, 0xfe, 0x40, 0x24, 0x2, 0x1f, 0xc0,
    0x20, 0x13, 0xe3, 0x20, 0x32, 0x6, 0xe0, 0x47,
    0xe0,

    /* U+5C06 "将" */
    0x22, 0x9, 0xfa, 0x85, 0x8e, 0x26, 0xa, 0x26,
    0xfe, 0x82, 0x24, 0x88, 0x22, 0x38,

    /* U+5C0F "小" */
    0x4, 0x0, 0x80, 0x10, 0x12, 0x44, 0x44, 0x88,
    0x91, 0xc, 0x21, 0x4, 0x0, 0x80, 0x70, 0x0,

    /* U+5E02 "市" */
    0xc, 0x0, 0x83, 0xff, 0x82, 0x7, 0xfc, 0x88,
    0x91, 0x12, 0x22, 0x44, 0x48, 0xb8, 0x10, 0x0,

    /* U+5EA6 "度" */
    0x6, 0xf, 0xfd, 0x22, 0x3f, 0xf4, 0x88, 0x9f,
    0x10, 0x3, 0xfe, 0x48, 0x88, 0xe2, 0xf7, 0x80,

    /* U+60A8 "您" */
    0x14, 0x4, 0xfd, 0xa5, 0x72, 0xa2, 0x54, 0x52,
    0x48, 0xc0, 0x22, 0x52, 0x4a, 0x14, 0x7e, 0x0,

    /* U+611F "感" */
    0x2, 0xcf, 0xfd, 0x4, 0x2e, 0xa5, 0xd4, 0xab,
    0x67, 0x6c, 0x13, 0x54, 0x4a, 0x14, 0x7e, 0x0,

    /* U+6216 "或" */
    0x3, 0x80, 0x9f, 0xfc, 0x8, 0x7a, 0xa2, 0xa8,
    0xa9, 0xec, 0x3, 0x7f, 0x50, 0x8c,

    /* U+624B "手" */
    0x1, 0xcf, 0xc0, 0x10, 0x3f, 0xe0, 0x40, 0x8,
    0x3f, 0xf8, 0x20, 0x4, 0x0, 0x80, 0x70, 0x0,

    /* U+62A5 "报" */
    0x2f, 0xc5, 0xb, 0xe1, 0x14, 0xc2, 0xfc, 0x74,
    0xba, 0x91, 0x4c, 0x29, 0x85, 0x3b, 0xa9, 0x80,

    /* U+636E "据" */
    0x2f, 0xe2, 0x82, 0xf8, 0x2f, 0xfe, 0x28, 0x82,
    0xff, 0xf8, 0x82, 0xfe, 0x2c, 0x23, 0x42, 0xd7,
    0xe0,

    /* U+63A5 "接" */
    0x42, 0x13, 0xfe, 0x89, 0x14, 0x5f, 0xd8, 0x8d,
    0xfd, 0x22, 0x4f, 0x10, 0xed, 0xc4,

    /* U+641C "搜" */
    0x21, 0x2, 0xde, 0x29, 0x2f, 0xfe, 0x29, 0x23,
    0x7c, 0xef, 0xe2, 0x44, 0x22, 0x82, 0x38, 0xcc,
    0x60,

    /* U+6539 "改" */
    0x2, 0x3d, 0x1, 0x7c, 0x52, 0xfc, 0xa2, 0xa8,
    0x2a, 0x4, 0xb3, 0x31, 0x20, 0x84,

    /* U+6548 "效" */
    0x22, 0x8, 0x8d, 0xbd, 0x49, 0x8e, 0xb5, 0xa2,
    0x28, 0x84, 0x51, 0x30, 0xa8, 0xc4,

    /* U+6587 "文" */
    0x4, 0x0, 0x83, 0xff, 0x90, 0x41, 0x10, 0x22,
    0x2, 0x80, 0x20, 0xa, 0x6, 0x33, 0x1, 0x80,

    /* U+65B0 "新" */
    0x21, 0xdf, 0xc1, 0x50, 0x2a, 0xf, 0xfe, 0x49,
    0x3f, 0x21, 0x24, 0xac, 0x94, 0x91, 0xa2, 0x0,

    /* U+65E0 "无" */
    0x7f, 0xc0, 0x80, 0x10, 0x2, 0xf, 0xfe, 0x14,
    0x2, 0x80, 0xd0, 0x12, 0x24, 0x47, 0xf, 0x80,

    /* U+65E5 "日" */
    0xff, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81, 0x81,
    0xff, 0x81,

    /* U+65F6 "时" */
    0x0, 0xbc, 0x29, 0xfe, 0x42, 0xf4, 0xa5, 0x29,
    0x2a, 0x4a, 0xf0, 0xa0, 0x20, 0x38,

    /* U+6697 "暗" */
    0xe2, 0x15, 0xfa, 0x92, 0x52, 0x4f, 0xff, 0x40,
    0x2b, 0xf5, 0x42, 0xef, 0xd1, 0x8, 0x3f, 0x0,

    /* U+66F4 "更" */
    0xff, 0xc1, 0x7, 0xf9, 0x12, 0x7f, 0x91, 0x27,
    0xf9, 0x20, 0x28, 0xe, 0xc, 0x7c,

    /* U+6709 "有" */
    0x0, 0x1, 0x3, 0xff, 0x88, 0x3, 0x0, 0x7f,
    0x98, 0x15, 0xfe, 0x20, 0x47, 0xf8, 0x81, 0x10,
    0xc0,

    /* U+672C "本" */
    0x8, 0x2, 0xf, 0xfc, 0x68, 0x1a, 0xa, 0x42,
    0x91, 0x22, 0xbf, 0x42, 0x0, 0x80,

    /* U+673A "机" */
    0x23, 0xc4, 0x4b, 0xe9, 0x11, 0x22, 0x24, 0xe4,
    0xab, 0x15, 0x22, 0x24, 0x64, 0x8c, 0xa1, 0x80,

    /* U+6765 "来" */
    0x4, 0xf, 0xf8, 0x91, 0x12, 0x41, 0x51, 0xff,
    0xc3, 0x80, 0xa8, 0x24, 0x98, 0x8c, 0x10, 0x0,

    /* U+679C "果" */
    0x7f, 0xc8, 0x89, 0xff, 0x22, 0x27, 0xfc, 0x8,
    0x3f, 0xf8, 0x70, 0x35, 0x98, 0x8e, 0x10, 0x0,

    /* U+6C14 "气" */
    0x10, 0x7, 0xfc, 0x80, 0x2f, 0xe4, 0x0, 0x7f,
    0x0, 0x20, 0x4, 0x0, 0x80, 0xc, 0x1, 0x80,

    /* U+6CA1 "没" */
    0x47, 0x84, 0x90, 0x12, 0x44, 0x77, 0x0, 0x1f,
    0x8a, 0x11, 0x24, 0x43, 0x8, 0xf0, 0x61, 0x80,

    /* U+6D4F "浏" */
    0x48, 0x24, 0x14, 0x7a, 0xc2, 0x55, 0x4a, 0x19,
    0x41, 0x2a, 0x65, 0x4a, 0x2a, 0x4, 0x3, 0x80,

    /* U+6D88 "消" */
    0x49, 0x25, 0x24, 0x4, 0x47, 0xf4, 0x82, 0x1f,
    0xca, 0x9, 0x7f, 0x48, 0x29, 0x4, 0x23, 0x80,

    /* U+70B9 "点" */
    0x8, 0x3, 0xf0, 0x81, 0xfe, 0x40, 0x90, 0x27,
    0xf8, 0x0, 0x52, 0xa4, 0x90, 0x0,

    /* U+73B0 "现" */
    0x7, 0xde, 0x89, 0x11, 0x22, 0xaf, 0x54, 0x8a,
    0x91, 0x52, 0x18, 0x73, 0x30, 0xa4, 0x63, 0x80,

    /* U+7528 "用" */
    0x7f, 0xd1, 0x17, 0xfd, 0x11, 0x44, 0x5f, 0xf4,
    0x45, 0x11, 0x84, 0x61, 0x70,

    /* U+7535 "电" */
    0x8, 0x2, 0xf, 0xfa, 0x22, 0xff, 0xa2, 0x28,
    0x8b, 0xfe, 0x88, 0x42, 0x10, 0xfc,

    /* U+754C "界" */
    0x7f, 0x91, 0x27, 0xf9, 0x12, 0x7f, 0x84, 0x82,
    0x1b, 0x4b, 0x12, 0xc, 0x84, 0x20,

    /* U+7684 "的" */
    0x2, 0x11, 0xf, 0x7e, 0x51, 0x90, 0x7c, 0x99,
    0x36, 0x45, 0x90, 0x7c, 0x18, 0x38,

    /* U+770B "看" */
    0x7f, 0xc1, 0x1, 0xff, 0xc, 0xf, 0x7e, 0x7f,
    0x98, 0x15, 0xfe, 0x3f, 0xc4, 0x8, 0xff, 0x0,

    /* U+786E "确" */
    0x2, 0x1e, 0x81, 0x1f, 0x26, 0x44, 0xfe, 0xf2,
    0x75, 0xfe, 0xc9, 0x59, 0x2b, 0xfd, 0xe4, 0xac,
    0xb0,

    /* U+7B14 "笔" */
    0x21, 0xf, 0xfd, 0x5a, 0x0, 0x63, 0xf0, 0x8,
    0x9f, 0xe0, 0x27, 0x7f, 0x0, 0x84, 0x1f, 0x80,

    /* U+7B80 "简" */
    0x21, 0xf, 0xff, 0x29, 0x17, 0xf1, 0x2, 0x80,
    0x53, 0xca, 0x79, 0x49, 0x29, 0xe5, 0x3, 0x80,

    /* U+7D22 "索" */
    0x4, 0x1f, 0xfc, 0x10, 0x7f, 0xf9, 0x82, 0x7c,
    0x6, 0x23, 0xfe, 0x4, 0x2c, 0x9a, 0x30, 0x80,

    /* U+7ED3 "结" */
    0x11, 0x4, 0xfd, 0x24, 0x38, 0x81, 0x7e, 0x40,
    0x1e, 0xfa, 0x21, 0x4, 0x2f, 0x85, 0xf, 0x80,

    /* U+7F51 "网" */
    0xff, 0xe0, 0x18, 0x96, 0xd5, 0xb3, 0x64, 0x9a,
    0xb6, 0xb5, 0xc4, 0x60, 0x18, 0x1c,

    /* U+7F6E "置" */
    0x7f, 0xca, 0x48, 0xb6, 0x7f, 0xf0, 0x80, 0x6f,
    0xf, 0xe1, 0xfc, 0x3f, 0x84, 0x13, 0xff, 0x80,

    /* U+8111 "脑" */
    0x71, 0xa, 0x21, 0x5b, 0xb8, 0x45, 0x6a, 0xab,
    0x5d, 0x6a, 0xbd, 0x56, 0x2a, 0xfe, 0xc0, 0x80,

    /* U+884C "行" */
    0x0, 0x4, 0xfd, 0x0, 0x48, 0x3, 0x0, 0x5f,
    0xd8, 0x25, 0x4, 0x20, 0x84, 0x10, 0x82, 0x11,
    0xc0,

    /* U+8981 "要" */
    0xff, 0xe2, 0x41, 0xff, 0x29, 0x27, 0xfc, 0x10,
    0x3f, 0xf8, 0x88, 0x3b, 0x1, 0xf3, 0x41, 0x0,

    /* U+89C8 "览" */
    0x49, 0x9, 0x7d, 0x3a, 0x4, 0x20, 0x0, 0x7f,
    0x89, 0x91, 0x32, 0x27, 0x43, 0xe1, 0xcf, 0x80,

    /* U+8A00 "言" */
    0x0, 0x0, 0x83, 0xff, 0x80, 0x3, 0xf8, 0x0,
    0xf, 0xe0, 0x0, 0x3f, 0x84, 0x10, 0x82, 0x1f,
    0xc0,

    /* U+8BB0 "记" */
    0x0, 0xd, 0xf8, 0x81, 0x0, 0x2e, 0x4, 0x4f,
    0x89, 0x11, 0x20, 0x24, 0x26, 0x84, 0x1f, 0x0,

    /* U+8BBE "设" */
    0x63, 0xc3, 0x24, 0x2, 0x4e, 0x47, 0x28, 0x2,
    0x7e, 0x24, 0x22, 0x24, 0x39, 0x82, 0x3c, 0xc,
    0x30,

    /* U+8BBF "访" */
    0x41, 0x4, 0x20, 0x3f, 0x81, 0xe, 0x20, 0x47,
    0x89, 0x11, 0x22, 0x34, 0x4d, 0x88, 0x27, 0x0,

    /* U+8BED "语" */
    0x4f, 0xe4, 0x40, 0x3f, 0x71, 0x2e, 0x24, 0x5f,
    0xc8, 0x1, 0x3f, 0x28, 0x27, 0x4, 0x9f, 0x80,

    /* U+8BF7 "请" */
    0x41, 0x4, 0xf8, 0x1f, 0x0, 0x86, 0xee, 0x4f,
    0x89, 0x11, 0x3e, 0x37, 0xc4, 0x88, 0x13, 0x0,

    /* U+8C03 "调" */
    0x4f, 0xe5, 0x4, 0x24, 0x85, 0xde, 0x92, 0x50,
    0x4a, 0xe9, 0x55, 0x3b, 0xa6, 0x44, 0x43, 0x80,

    /* U+8FDB "进" */
    0x42, 0x84, 0x50, 0xbf, 0x81, 0x4e, 0x28, 0x5f,
    0xc9, 0x21, 0x24, 0x28, 0x8a, 0x2, 0x3f, 0x80,

    /* U+8FDE "连" */
    0x42, 0x2, 0xfe, 0x24, 0x0, 0x50, 0xef, 0xe2,
    0x10, 0x21, 0x2, 0xfe, 0x21, 0x5, 0x10, 0x8f,
    0xf0,

    /* U+914D "配" */
    0xfb, 0xd8, 0x1f, 0x87, 0xa1, 0xeb, 0xfb, 0x1d,
    0xc2, 0x30, 0xfc, 0x7f, 0x18, 0xbc,

    /* U+91CD "重" */
    0x0, 0xf, 0xe0, 0x10, 0x7f, 0xf0, 0x40, 0xff,
    0x9f, 0xf2, 0x22, 0x3f, 0x8f, 0xf8, 0x10, 0x7f,
    0xf0,

    /* U+95ED "闭" */
    0x0, 0x17, 0xf4, 0x4, 0x9, 0x82, 0x6f, 0xd8,
    0x66, 0x29, 0x92, 0x68, 0x98, 0xe6, 0x3,

    /* U+95EE "问" */
    0x0, 0x17, 0xf4, 0x4, 0x1, 0x9e, 0x64, 0x99,
    0x26, 0x49, 0x9e, 0x60, 0x18, 0x6, 0x7,

    /* U+95F4 "间" */
    0x0, 0x17, 0xf6, 0x4, 0x1, 0x9e, 0x64, 0x99,
    0x26, 0x79, 0x92, 0x67, 0x98, 0x6, 0x3,

    /* U+9700 "需" */
    0x7f, 0x82, 0xf, 0xfe, 0x21, 0xfb, 0xc0, 0xf,
    0xfd, 0xfe, 0x52, 0x94, 0xa5, 0x28,

    /* U+9762 "面" */
    0xff, 0xe0, 0x80, 0x10, 0x3f, 0xe9, 0x15, 0x3e,
    0xa4, 0x54, 0xfa, 0x91, 0x4f, 0xfa, 0x1, 0x0,

    /* U+9875 "页" */
    0xff, 0xc2, 0x0, 0x81, 0xfe, 0x40, 0x92, 0x24,
    0x89, 0x22, 0x48, 0x84, 0xce, 0xc,

    /* U+9884 "预" */
    0xff, 0xe2, 0x21, 0x9f, 0x9a, 0x1f, 0xd2, 0x5a,
    0x4d, 0x49, 0x29, 0x21, 0x4, 0x4b, 0x90, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 43, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 62, .box_w = 1, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 91, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 5, .adv_w = 107, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 12, .adv_w = 107, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 20, .adv_w = 177, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 32, .adv_w = 131, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 41, .adv_w = 53, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 42, .adv_w = 65, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 47, .adv_w = 65, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 52, .adv_w = 107, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 56, .adv_w = 53, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 57, .adv_w = 67, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 58, .adv_w = 53, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 75, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 67, .adv_w = 107, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 73, .adv_w = 107, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 107, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 85, .adv_w = 107, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 91, .adv_w = 107, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 107, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 107, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 110, .adv_w = 107, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 117, .adv_w = 107, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 123, .adv_w = 107, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 129, .adv_w = 53, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 130, .adv_w = 53, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 133, .adv_w = 107, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 137, .adv_w = 107, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 140, .adv_w = 107, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 144, .adv_w = 91, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 149, .adv_w = 182, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 164, .adv_w = 117, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 126, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 179, .adv_w = 122, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 186, .adv_w = 132, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 193, .adv_w = 113, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 106, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 132, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 140, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 56, .box_w = 1, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 222, .adv_w = 103, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 124, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 235, .adv_w = 104, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 156, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 249, .adv_w = 139, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 256, .adv_w = 142, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 264, .adv_w = 122, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 271, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 282, .adv_w = 122, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 114, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 296, .adv_w = 115, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 304, .adv_w = 138, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 311, .adv_w = 110, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 319, .adv_w = 169, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 331, .adv_w = 110, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 339, .adv_w = 102, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 347, .adv_w = 116, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 354, .adv_w = 65, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 359, .adv_w = 75, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 367, .adv_w = 65, .box_w = 3, .box_h = 13, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 372, .adv_w = 107, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 376, .adv_w = 107, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 377, .adv_w = 116, .box_w = 2, .box_h = 3, .ofs_x = 2, .ofs_y = 8},
    {.bitmap_index = 378, .adv_w = 108, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 119, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 98, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 396, .adv_w = 119, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 404, .adv_w = 106, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 410, .adv_w = 62, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 108, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 423, .adv_w = 117, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 53, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 432, .adv_w = 53, .box_w = 3, .box_h = 13, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 437, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 444, .adv_w = 55, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 447, .adv_w = 178, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 455, .adv_w = 117, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 116, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 466, .adv_w = 119, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 474, .adv_w = 119, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 482, .adv_w = 74, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 485, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 489, .adv_w = 72, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 494, .adv_w = 117, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 499, .adv_w = 100, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 505, .adv_w = 154, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 513, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 519, .adv_w = 100, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 527, .adv_w = 91, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 532, .adv_w = 65, .box_w = 3, .box_h = 13, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 537, .adv_w = 52, .box_w = 1, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 539, .adv_w = 65, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 544, .adv_w = 107, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 546, .adv_w = 71, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 548, .adv_w = 98, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 555, .adv_w = 117, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 562, .adv_w = 117, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 569, .adv_w = 103, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 570, .adv_w = 172, .box_w = 9, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 572, .adv_w = 192, .box_w = 5, .box_h = 11, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 579, .adv_w = 192, .box_w = 5, .box_h = 11, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 586, .adv_w = 192, .box_w = 12, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 601, .adv_w = 192, .box_w = 12, .box_h = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 616, .adv_w = 192, .box_w = 11, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 618, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 632, .adv_w = 192, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 644, .adv_w = 192, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 658, .adv_w = 192, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 672, .adv_w = 192, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 685, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 701, .adv_w = 192, .box_w = 11, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 712, .adv_w = 192, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 725, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 739, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 755, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 772, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 788, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 805, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 822, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 838, .adv_w = 192, .box_w = 12, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 856, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 873, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 889, .adv_w = 192, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 904, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 921, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 935, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 951, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 968, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 984, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1000, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1014, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1028, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1044, .adv_w = 192, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1057, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1073, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1087, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1104, .adv_w = 192, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1116, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1132, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1149, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1166, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1180, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1196, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1212, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1228, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1244, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1260, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1274, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1290, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1306, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1323, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1337, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1354, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1368, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1382, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1398, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1414, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1430, .adv_w = 192, .box_w = 8, .box_h = 10, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 1440, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1454, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1470, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1484, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1501, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1515, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1531, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1547, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1563, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1579, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1595, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1611, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1627, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1641, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1657, .adv_w = 192, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1670, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1684, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1698, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1712, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1728, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1745, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1761, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1777, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1793, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1809, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1823, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1839, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1855, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1872, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1888, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1904, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1921, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1937, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1954, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1970, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1986, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 2002, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 2018, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 2034, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 2051, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 2065, .adv_w = 192, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 2082, .adv_w = 192, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 2097, .adv_w = 192, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 2112, .adv_w = 192, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 2127, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 2141, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 2157, .adv_w = 192, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 2171, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_2[] = {
    0x0, 0x37, 0x41, 0x4c, 0x1f63, 0x1f64, 0x20e1, 0x20e3,
    0x2502, 0x250c, 0x4d50, 0x4d53, 0x4d59, 0x4d5a, 0x4d5b, 0x4d7d,
    0x4d84, 0x4ddc, 0x4de4, 0x4dfc, 0x4dfe, 0x4e1a, 0x4e9d, 0x4ea3,
    0x4ecf, 0x4edb, 0x4f2d, 0x50b5, 0x50bd, 0x50c3, 0x513d, 0x5180,
    0x5267, 0x5298, 0x5326, 0x535e, 0x5367, 0x53b8, 0x55b8, 0x562b,
    0x5678, 0x571e, 0x586c, 0x5879, 0x58d2, 0x5aa8, 0x5aea, 0x5b56,
    0x5b5f, 0x5d52, 0x5df6, 0x5ff8, 0x606f, 0x6166, 0x619b, 0x61f5,
    0x62be, 0x62f5, 0x636c, 0x6489, 0x6498, 0x64d7, 0x6500, 0x6530,
    0x6535, 0x6546, 0x65e7, 0x6644, 0x6659, 0x667c, 0x668a, 0x66b5,
    0x66ec, 0x6b64, 0x6bf1, 0x6c9f, 0x6cd8, 0x7009, 0x7300, 0x7478,
    0x7485, 0x749c, 0x75d4, 0x765b, 0x77be, 0x7a64, 0x7ad0, 0x7c72,
    0x7e23, 0x7ea1, 0x7ebe, 0x8061, 0x879c, 0x88d1, 0x8918, 0x8950,
    0x8b00, 0x8b0e, 0x8b0f, 0x8b3d, 0x8b47, 0x8b53, 0x8f2b, 0x8f2e,
    0x909d, 0x911d, 0x953d, 0x953e, 0x9544, 0x9650, 0x96b2, 0x97c5,
    0x97d4
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 10, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 43, .range_length = 84, .glyph_id_start = 11,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 38869, .glyph_id_start = 95,
        .unicode_list = unicode_list_2, .glyph_id_ofs_list = NULL, .list_length = 113, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 0,
    1, 2, 0, 0, 3, 4, 3, 5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 6, 6, 0, 0, 0, 0,
    0, 7, 8, 9, 10, 11, 12, 13,
    0, 0, 14, 15, 16, 0, 0, 10,
    17, 10, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 2, 27, 0, 0, 0,
    0, 28, 29, 30, 0, 31, 32, 33,
    34, 0, 0, 35, 36, 34, 34, 29,
    29, 37, 38, 39, 40, 37, 41, 42,
    43, 44, 45, 2, 0, 0, 0, 0,
    30, 34, 37, 4, 4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 1, 2, 0, 0, 0, 0,
    2, 0, 3, 0, 4, 5, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 8, 9, 0, 0, 0, 10,
    0, 11, 0, 12, 0, 0, 0, 12,
    0, 0, 13, 0, 0, 0, 0, 12,
    0, 12, 0, 14, 15, 16, 17, 18,
    19, 20, 21, 0, 22, 3, 0, 0,
    0, 23, 0, 24, 24, 24, 25, 26,
    0, 27, 28, 0, 0, 29, 29, 24,
    29, 24, 29, 30, 31, 32, 33, 34,
    35, 36, 37, 0, 0, 3, 0, 0,
    24, 29, 32, 5, 5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, -25, 0, -25, 0, 0,
    0, 0, -12, 0, -20, -2, 0, 0,
    0, 0, -2, 0, 0, 0, 0, -6,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    17, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -21, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -21, -4, -14, -7, 0, -20, 0, 0,
    0, -2, 0, 0, 0, 6, 0, 0,
    -10, 0, -7, -5, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -4, -10, 0, -4,
    -2, -6, -14, -4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -2,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -9, -2,
    -17, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, -7, 0, -2, 6, 6,
    0, 0, 2, -4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -12,
    0, 0, 0, 0, 0, 0, 0, -6,
    -1, -2, 0, 0, -12, -3, -3, 0,
    1, -3, -2, -9, 5, 0, -2, 0,
    0, 0, 0, 5, -3, -1, -2, -1,
    -1, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    -3, -5, 0, -1, -1, -1, -3, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, -3, -2, -2, -3, 0,
    0, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, -6, -2, -5, -4, -3,
    -1, -1, -1, -2, -2, 0, 0, 0,
    0, -4, 0, 0, 0, 0, -6, -2,
    -3, -2, 0, -3, 0, 0, 0, 0,
    0, 0, 0, -4, 0, 0, 0, -2,
    0, -8, 0, -5, 0, -2, -1, -4,
    -4, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    -5, 0, -2, 0, -7, -2, 0, 0,
    0, 0, -16, 0, -16, -15, 0, 0,
    0, -8, -2, -30, -4, 0, 0, 1,
    1, -5, 0, -7, 0, -7, -3, 0,
    -5, 0, 0, -4, -4, -2, -3, -4,
    -3, -6, -3, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, 0, -4, 0, -3, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, -5, 0,
    0, 0, 0, 0, 0, -9, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, -6, 0, 0, 0, 0, -2,
    -2, -4, 0, -2, -4, -3, -3, -2,
    0, -3, 0, 0, 0, -2, 0, 0,
    0, -2, 0, 0, -7, -3, -4, -3,
    -3, -4, -3, 0, -19, 0, 0, -12,
    0, 0, 0, 0, -7, 1, -6, 0,
    -5, -26, -6, -16, -12, 0, -16, 0,
    -17, 0, -2, -3, -1, 0, 0, 0,
    0, -4, -2, -8, -7, 0, -8, 0,
    0, 0, 0, -24, -7, -24, -16, 0,
    0, 0, -11, 0, -31, -2, -5, 0,
    0, 0, -5, -2, -17, 0, -9, -5,
    0, -7, 0, 0, 0, -2, 0, 0,
    0, 0, -3, 0, -4, 0, 0, 0,
    0, -7, 0, 0, 0, 0, 0, -1,
    0, -4, -3, -3, 0, 1, 1, -1,
    0, -2, 0, -1, -2, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, -2, 0, 0, 0, 0, 3, 0,
    0, 0, 0, 0, 0, 0, -3, -3,
    -4, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -23, -16, -23, -19, -4, -4,
    0, -9, -5, -27, -8, 0, 0, 0,
    0, -4, -3, -12, 0, -16, -14, -4,
    -16, 0, 0, -10, -13, -4, -10, -7,
    -7, -8, -7, -16, 0, 0, 0, -3,
    0, -3, -7, 0, 0, 0, -3, 0,
    -10, -2, 0, 0, -1, 0, -2, -3,
    0, 0, -1, 0, 0, -2, 0, 0,
    0, -1, 0, 0, 0, 0, -2, 0,
    0, 0, 0, 0, -14, -4, -14, -10,
    0, 0, 0, -3, -2, -16, -2, 0,
    -2, 2, 0, 0, 0, -4, 0, -5,
    -3, 0, -5, 0, 0, -4, -3, 0,
    -7, -2, -2, -3, -2, -5, 0, 0,
    0, -7, -2, -7, -6, 0, 0, 0,
    0, -1, -14, -1, 0, 0, 0, 0,
    0, 0, -1, 0, -3, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, -2, 0, 0, -6,
    0, 0, 0, 0, 0, 1, -4, 0,
    -3, -4, -2, 0, 0, 0, 0, 0,
    0, -2, -2, -3, 0, 0, 0, 0,
    0, -3, -2, -3, -3, -2, -3, -3,
    0, 0, 0, -19, -14, -19, -14, -5,
    -5, -2, -3, -3, -22, -3, -3, -2,
    0, 0, 0, 0, -6, 0, -14, -9,
    0, -13, 0, 0, -9, -9, -6, -7,
    -3, -5, -7, -3, -10, 0, 0, 0,
    0, -7, 0, 0, 0, 0, 0, -1,
    -4, -7, -7, 0, -2, -1, -1, 0,
    -3, -3, 0, -3, -5, -4, -3, 0,
    0, 0, 0, -3, -5, -3, -3, -5,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -18, -6, -11, -6, 0, -16, 0, 0,
    0, 0, 0, 7, 0, 16, 0, 0,
    0, 0, -4, -2, 0, 3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, -5, 0, -3,
    -1, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -2,
    1, -2, 0, 0, 0, -3, 0, 0,
    0, 0, -12, 0, -4, 0, -1, -10,
    0, -6, -3, 0, 0, 0, 0, 0,
    0, 0, -4, 0, -1, -1, -4, -1,
    -1, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, -3, 0, 0, -5, 0, 0, -2,
    -5, 0, -2, 0, 0, 0, 0, -2,
    0, 1, 1, 2, 1, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0, -2,
    0, 0, -4, -4, -5, 0, -3, -2,
    0, -6, 0, -4, -3, 0, 0, -2,
    0, 0, 0, 0, -2, 0, 1, 1,
    -2, 1, 0, 3, 9, 11, -11, -3,
    -11, -3, 0, 0, 6, 0, 0, 0,
    0, 10, 0, 14, 10, 7, 13, 0,
    13, -4, -2, 0, -3, 0, -2, 0,
    -1, 0, 0, 3, 0, -1, 0, -3,
    0, 0, 3, 0, 0, 0, 10, 0,
    0, -8, 0, 0, 0, 0, -6, 0,
    0, 0, 0, -3, 0, 0, -3, -3,
    0, 0, 0, 8, 0, 0, 0, 0,
    -1, -1, 0, 3, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 0, -5, 0, -2, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, -9, 3,
    0, 3, 3, -3, 0, 0, 0, 0,
    -7, 0, 0, 0, 0, -2, 0, 0,
    -2, -4, 0, -2, 0, -2, 0, 0,
    -4, -3, 0, 0, -2, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, -3, 0, 0, -7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -12, -5, -12, -7,
    6, 6, 0, -3, 0, -12, 0, 0,
    0, 0, 0, 0, 0, -2, 3, -5,
    -2, 0, -2, 0, 0, 0, -1, 0,
    0, 6, 4, 0, 6, -1, 0, 0,
    0, 0, 2, 0, 0, 0, 0, -2,
    0, 0, 0, 0, -5, 0, -2, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, -6,
    1, 2, 3, 3, -6, 0, 0, 0,
    0, -3, 0, 0, 0, 0, -1, 0,
    0, -5, -3, 0, -2, 0, 0, 0,
    -2, -4, 0, 0, 0, -3, 0, 0,
    0, 0, 0, -7, -2, -7, -4, 0,
    0, 0, -2, 0, -9, 0, -4, 0,
    -2, 0, 0, -3, -2, 0, -4, -1,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    -9, 0, -9, -2, 0, 0, 0, -1,
    0, -7, 0, -5, 0, -2, 0, -3,
    -5, 0, 0, -2, -1, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, -4, -3, 0, 0, 2, -3, -2,
    0, 0, 2, 0, 0, -2, 0, -1,
    -7, 0, -3, 0, -2, -7, 0, 0,
    -2, -4, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, -7, -3, 0, 0,
    0, 0, 0, -9, 0, -4, 0, -1,
    0, -1, -2, 0, 0, -4, -1, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, -3, 0, -5, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, -6,
    0, 0, -5, -2, 0, -1, 0, 0,
    0, 0, 0, -2, -1, 0, 0, -1,
    0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 45,
    .right_class_cnt     = 37,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 3,
    .bpp = 1,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t lv_font_chinese_12 = {
#else
lv_font_t lv_font_chinese_12 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 14,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LV_FONT_CHINESE_12*/

