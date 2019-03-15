#ifndef _LEDS_H_
#define _LEDS_H_

void leds_setup(AdafruitNeoPixelRAAT * pNeoPixels);
void leds_run(AdafruitNeoPixelRAAT * pNeoPixels);

void leds_start_match_animation();
void leds_start_no_match_animation();

#endif
