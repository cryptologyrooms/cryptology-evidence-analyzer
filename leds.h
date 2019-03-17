#ifndef _LEDS_H_
#define _LEDS_H_

void leds_setup(AdafruitNeoPixelRAAT * pNeoPixels);
void leds_run(AdafruitNeoPixelRAAT * pNeoPixels);
void leds_test(AdafruitNeoPixelRAAT * pNeoPixels);

void leds_pend_scan_animation();
void leds_pend_no_match_animation();

#endif
