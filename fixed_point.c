#include "fixed_point.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void print_fixed(int16_t raw, int16_t q) {
    if (raw < 0) {
        printf("-");
        raw = (int16_t)(-raw);
    }

    int16_t integer_part = raw >> q;
    printf("%d.", integer_part);

    uint32_t fractional_bits = raw & (((uint32_t)1 << q) - 1);

    for (int i = 0; i < 6; i++) {
        fractional_bits *= 10;
        printf("%u", (unsigned int)(fractional_bits >> q));
        fractional_bits &= (((uint32_t)1 << q) - 1);
    }
}


int16_t add_fixed(int16_t a, int16_t b) {
    return (int16_t)(a + b);
}

int16_t subtract_fixed(int16_t a, int16_t b) {
    return (int16_t)(a - b);
}

int16_t multiply_fixed(int16_t a, int16_t b, int16_t q) {
    int64_t res = (int64_t)a * b;
    return (int16_t)(res >> q);
}

void eval_poly_ax2_minus_bx_plus_c_fixed(int16_t x, int16_t a, int16_t b, int16_t c, int16_t q) {
    int16_t x_squared = multiply_fixed(x, x, q);
    int16_t ax2 = multiply_fixed(a, x_squared, q);
    int16_t bx = multiply_fixed(b, x, q);
    int16_t ax2_minus_bx = subtract_fixed(ax2, bx);
    int16_t y = add_fixed(ax2_minus_bx, c);

    printf("the polynomial output for a=");
    print_fixed(a, q);
    printf(", b=");
    print_fixed(b, q);
    printf(", c=");
    print_fixed(c, q);
    printf(" is ");
    print_fixed(y, q);
    printf("\n");
}

