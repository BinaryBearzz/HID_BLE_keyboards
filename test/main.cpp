#include <Arduino.h>
#include <BleKeyboard.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#define enble_debug 0
#define debug if (enble_debug) Serial

std::string nameDevice = "noomKeyboard";
// BleKeyboard bleKeyboard(nameDevice, "Espressif", 100);
BleKeyboard bleKeyboard;

static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
void InitLVGL();
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
/* Display flushing */

void setup() {
  Serial.begin(115200);
  InitLVGL();
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()) {
    Serial.println("Sending 'Hello world'...");
    bleKeyboard.print("Hello world");

    delay(1000);

    Serial.println("Sending Enter key...");
    bleKeyboard.write(KEY_NUM_0);
    delay(1000);

    // Serial.println("Sending Play/Pause media key...");
    // bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    // delay(1000);
   //
   // Below is an example of pressing multiple keyboard modifiers 
   // which by default is commented out.
    /*
    Serial.println("Sending Ctrl+Alt+Delete...");
    bleKeyboard.press(KEY_LEFT_CTRL);
    bleKeyboard.press(KEY_LEFT_ALT);
    bleKeyboard.press(KEY_DELETE);
    delay(100);
    bleKeyboard.releaseAll();
    */
  }

  Serial.println("Waiting 5 seconds...");
  delay(5000);
}


void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;

  bool touched = tft.getTouch(&touchX, &touchY, 600);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    debug.print("Data x ");
    debug.println(touchX);

    debug.print("Data y ");
    debug.println(touchY);
  }
}

void InitLVGL(){
  Serial.println("BF->S_RAM: " + String(ESP.getFreeHeap()));
  Serial.println("BF->PS_RAM: " + String(ESP.getFreePsram()));

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");
  // ledcSetup(10, 5000 /*freq*/, 10 /*resolution*/);
  // ledcAttachPin(32, 10);
  // analogReadResolution(10);
  // ledcWrite(10, 768);
  lv_init();
  // lv_font_load(th_psk);
  // #if LV_USE_LOG != 0
  //     lv_log_register_print_cb( my_print ); /* register print function for debugging */
  // #endif

  tft.begin();        /* TFT init */
  tft.setRotation(3); /* Landscape orientation, flipped */

  /*Set the touchscreen calibration data,
   the actual data for your display can be acquired using
   the Generic -> Touch_calibrate example from the TFT_eSPI library*/
  uint16_t calData[5] = {275, 3620, 264, 3532, 1};
  tft.setTouch(calData);

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
}