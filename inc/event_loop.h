#ifndef EVENT_LOOP_H_
#define EVENT_LOOP_H_

enum main_event {
    EVENTS_INIT = 0,
    EVENT_ADC_AVG_HALF = 1 << 0,
    EVENT_ADC_AVG_FULL = 1 << 1,
    EVENT_HILBERT_COMPUTE = 1 << 2,
    EVENT_SET_PWM = 1 << 3
};

#define SET_EVENT(E,X) (*(E) |= (X))
#define RESET_EVENT(E,X) (*(E) &= ~(X))

#define EVENT_IS_SET(E,X) (*(E) & (X))

#endif
