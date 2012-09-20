#ifndef _PINYIN
#define _PINYIN

#include <stddef.h>

//判断unicode是否为汉字
#define pinyin_ishanzi(uni) ((uni) > 19967 && (uni) < 40870)
//是否为字母
#define pinyin_isabc(uni) (((uni) > 64 && (uni) < 91) || ((uni) > 96 && (uni) < 123))
//转小写
#define pinyin_lowercase(uni) (((uni > 64) && (uni) < 91)? (uni) + 32 : (uni))

int pinyin_get_tones_by_unicode(wchar_t uni, char **tones_out);
int pinyin_get_pinyins_by_unicode(wchar_t uni, const char ***pinyins);

#endif
