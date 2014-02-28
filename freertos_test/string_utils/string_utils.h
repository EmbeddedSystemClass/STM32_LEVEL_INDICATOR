#ifndef STRING_UTILS_H
#define STRING_UTILS_H
#include "stm32f10x.h"
uint8_t float_to_string(float val, uint8_t *str,uint8_t decimal_point,uint8_t fill_char);
uint8_t string_to_float(uint8_t *str, float *val);
#endif
