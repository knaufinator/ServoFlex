
#define LGFX_AUTODETECT 
#define LGFX_USE_V1   

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include <LovyanGFX.hpp>
#include "ui.h"
#include <lvgl.h>
#include "lv_conf.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "FastLED.h"

#define DATA_PIN    4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    305
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

static LGFX lcd; 
static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

//max vout signal to AC servo, 
int maxDACOUT = 145;//255 to big for my current calibration on AC servo driver

//LEDs
CRGB leds[NUM_LEDS];



//Parameters 
const int numReadings  = 300;
int readings [numReadings];
int readIndex  = 0;
long total  = 0;
 
//used to keep track for vout smoothing to AC servo
volatile float outputTarget = 0;
volatile long smoothedOutputTarget = 0;

//CPU tasks
TaskHandle_t InterfaceMonitorTask;
TaskHandle_t GPIOLoopTask;
TimerHandle_t wtmr;

FASTLED_USING_NAMESPACE

/*** Function declaration ***/
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void lv_button_demo(void);

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
uint8_t gCurrentPatternNumber = 3; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used 

typedef void (*SimplePatternList[])();


void rainbow() 
{
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 40);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( smoothedOutputTarget, 255, 192);
}

void bpm()
{
  CRGBPalette16 palette = PartyColors_p;
  for( int i = 0; i < NUM_LEDS; i++) { //9948

  leds[i] = ColorFromPalette(palette, smoothedOutputTarget, 255);

  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}


long smoothOutput(int currentValue) { /* function smooth */
  ////Perform average on sensor readings
  long average;
  // subtract the last reading:
  total = total - readings[readIndex];
  // read the sensor:
  readings[readIndex] = currentValue;
  // add value to total:
  total = total + readings[readIndex];
  // handle index
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  // calculate the average:
  average = total / numReadings;

  return average;
}

void ping( TimerHandle_t xTimer )
{ 
    //Resets the watchdog timer in the ESP32/rtos
    //this is needed in order not have the thing reset every few seconds.
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;

    //Restart timer for this method
    xTimerStart(wtmr, 0);
}

float mapfloat(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
 
void SetOutput(int value)//0-100
{
  float scaled = mapfloat(value,0, 100, 0, maxDACOUT);
  outputTarget = scaled;
}

//Thread that handles LED Strip
void InterfaceMonitorCode( void * pvParameters ){
  for(;;){
 // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
 
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically

  } 
}

//Thread/loop that handles UI and servo control
void GPIOLoop( void * pvParameters ){
  for(;;){
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
 
   // stuff target value into moving average
   smoothedOutputTarget = smoothOutput(outputTarget);

   //write smoothed vout to ac servos 
   dacWrite(25, (uint8_t)smoothedOutputTarget);

  }
}

void setup(void)
{
  pinMode(25,OUTPUT);
  dacWrite(25,0);

  Serial.begin(115200); 

  lcd.init(); 
  lv_init();  

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = display_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read;
  lv_indev_drv_register(&indev_drv);
 
  ui_init();

  lv_disp_set_rotation(NULL, LV_DISP_ROT_90);
 
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  xTaskCreatePinnedToCore(
                    InterfaceMonitorCode,   /* Task function. */
                    "InterfaceMonitor",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &InterfaceMonitorTask,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  

 
  xTaskCreatePinnedToCore(
                    GPIOLoop,   /* Task function. */
                  "GPIOLoopTask",     /* name of task. */
                   10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                  &GPIOLoopTask,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
 
  //Timer for watchdog reset
  wtmr = xTimerCreate("wtmr", pdMS_TO_TICKS(1000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(ping));
  xTimerStart(wtmr, 0);

}

//function used from the UI class, on slider move.. this feels hacky..
void ui_event_Audio_Player_Slider1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
           	int16_t value = lv_slider_get_value(target);
		
		SetOutput(value);//0-100	
    }
}

void loop()
{
}

  /*** Display callback to flush the buffer to screen ***/
void display_flush(lv_disp_drv_t * disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    lcd.startWrite();
    lcd.setAddrWindow(area->x1, area->y1, w, h);
    lcd.pushColors((uint16_t *)&color_p->full, w * h, true);
    lcd.endWrite();

    lv_disp_flush_ready(disp);
}

/*** Touchpad callback to read the touchpad ***/
void touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
  uint16_t touchX, touchY;
  bool touched = lcd.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchY;
    data->point.y = touchX;
  }
}
