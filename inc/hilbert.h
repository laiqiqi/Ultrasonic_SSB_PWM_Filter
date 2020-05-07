#ifndef HILBERT_H
#define HILBERT_H

#include <stdint.h>

#define HILBERT_BUF_SIZE 41
#define HILBERT_COEFF_CORRECTION 10000
#define HILBERT_DC_OFFSET_CORRECTION (-500)

typedef struct Hilbert_Buf {
    uint32_t* first; // will point to the first value added to the buffer
    uint32_t* phase_0; // will point to the adc value outputted by the phase 0 signal
    uint32_t buf[HILBERT_BUF_SIZE];
} Hilbert_Buf;

void init_Hilbert_Buf(Hilbert_Buf*);
void hilbert_record(Hilbert_Buf*, uint32_t adc_val);
uint32_t hilbert_phase_90_12bit(Hilbert_Buf*);
uint32_t hilbert_phase_0_12bit(Hilbert_Buf*);

#endif
