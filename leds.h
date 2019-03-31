#ifndef _LEDS_H_
#define _LEDS_H_

void leds_setup(AdafruitNeoPixelRAAT * pNeoPixels);
bool leds_run(AdafruitNeoPixelRAAT * pNeoPixels, RGBParam * pScanColour, uint8_t nscans, uint32_t scantime);
void leds_test(AdafruitNeoPixelRAAT * pNeoPixels);

void leds_pend_scan_animation();
void leds_pend_no_match_animation();

#endif
