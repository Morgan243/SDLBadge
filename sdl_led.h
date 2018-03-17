
// Stub out Hardware LED calls for SDL - these are normally in timer_int1.c

void led_brightness(unsigned char bright);

void red(unsigned char onPWM);

void green(unsigned char onPWM);

void blue(unsigned char onPWM);

void led(unsigned char r, unsigned char g, unsigned char b);
