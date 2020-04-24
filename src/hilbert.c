#include "hilbert.h"

#define PHASE_0_OFFSET ((HILBERT_BUF_SIZE - 1)/2)
#define NEXT_ENTRY(HBUF, PTR) ((PTR == (HBUF->buf + HILBERT_BUF_SIZE - 1)) ? HBUF->buf : (PTR + 1))

const int32_t hilbert_coeff[HILBERT_BUF_SIZE] = {0, -335, 0, -374, 0, -424, 0, -490, 0, -579, 0, -707, 0, -909, 0, -1273, 0, -2122, 0, -6366, 0, 6366, 0, 2122, 0, 1273, 0, 909, 0, 707, 0, 579, 0, 490, 0, 424, 0, 374, 0, 335, 0};

void init_Hilbert_Buf(Hilbert_Buf* hbuf) {
    int i;

    for (i = 0; i < HILBERT_BUF_SIZE; i++) {
        hbuf->buf[i] = 0;
    }

    hbuf->first = hbuf->buf;
    hbuf->phase_0 = hbuf->buf + PHASE_0_OFFSET;
}

void hilbert_record(Hilbert_Buf* hbuf, uint32_t adc_val) {
    *hbuf->first = adc_val;
    
    hbuf->first = NEXT_ENTRY(hbuf, hbuf->first);
    hbuf->phase_0 = NEXT_ENTRY(hbuf, hbuf->phase_0);
}

uint32_t hilbert_phase_90_12bit(Hilbert_Buf* hbuf) {
    int i;
    uint32_t* hptr = hbuf->first;
    int32_t hval = 0;
    
    for (i = 0; i < HILBERT_BUF_SIZE; i++) {
        hval += (hilbert_coeff[i] * ((*hptr) - 2047));
        hptr = NEXT_ENTRY(hbuf, hptr);
    }
    
    hval = (hval/HILBERT_CORRECTION) + 2047;

    if (hval < 0)
        return 0;
    if (hval > 4095)
        return 4095;

    return (uint32_t)hval;
}

uint32_t hilbert_phase_0_12bit(Hilbert_Buf* hbuf) {
    return *hbuf->phase_0;
}

