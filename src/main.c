//░░▄▄░░░░▄░░░░▄░░░ 
//░░░░░░█▄░░░█░░█▀░░░░ 
//░░░░░░░▀█▄░▀░░░░░░░░ 
//░░░░▄░░░▄▄███▄░░░▄▄░ 
//░░░▀▀░░▄█░░█░█▄░░░▀█ 
//░░░░░░░█░░░░░░█░░░░░ 
//░░░░░░░████████░░░░░ 
//░░░░░░░█▄▄░░░░█░░░░░ 
//░░░░░░░█░░░░▀▀█░░░░░ 
//░░░░░░░█▀▀▀░▄▄█░░░░░ 
//░░░░░░░█░░░░▀▀█░░░░░ 
//░░░░░░░█▀▀▀░▄▄█░░░░░
//░░░░░░░█░░░░░░█▄░░░░ 
//▄▄▄▄██▀▀░░░░░░░▀██░░ 
//░▄█▀░▀░░░░▄░░░░░░█▄▄ 
//▀▀█▄▄▄░░░▄██░░░░▄█░░ 
//░█▀█▄▄▄▄█▀░██▄▄██▄▄░ 
//░░░░▀░░░▀░░░▀░░░░░░░






#include <pebble.h>
//define base window
static Window *s_main_window;

//Time
static TextLayer *s_time_layer;

//Date
static TextLayer *s_date_layer;

//Version
static TextLayer *s_version_layer;

//PROTOTYPE LOADS

//Load main window
static void main_window_load(Window *window);

//kill main window
static void main_window_unload(Window *window);

//Update time
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

//gesture support
static void tap_handler(AccelAxisType axis, int32_t direction);
static void handle_deinit(void);
static void handle_init(void);

//every second add one second for all seconds this second, per second.
static void update_time();

//start
static void init();

//stop
static void deinit();


int main(void) {
  init();
  app_event_loop();
  deinit();
}

//INITIALIZE
static void init() {
  //register tap handler
  accel_tap_service_subscribe(tap_handler);
  //make the main window
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });
  //show the window with animations = true
  window_stack_push(s_main_window, true);
}


static void deinit() {
  //destroy the main window
  window_destroy(s_main_window);
}


//actually load
static void main_window_load(Window *window) {
  
  //Custom Font Declarations
  GFont time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONTDICKBALLS_64));
  
  
  //get information about the window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(s_main_window, GColorDarkCandyAppleRed);
  
  //Create Time Layer
  s_time_layer = text_layer_create(GRect(0, 30, bounds.size.w, 80));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //add time layer as child
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  
  //Create Date layer
  s_date_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 100), bounds.size.w, 80));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "Date");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_date_layer, GTextOverflowModeWordWrap);
  
  //add date layer as child
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  //VERSION TEXT
  s_version_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 0), bounds.size.w, 35));
  text_layer_set_background_color(s_version_layer, GColorBlack);
  text_layer_set_text_color(s_version_layer, GColorWhite);
  text_layer_set_text(s_version_layer, "Unknown");
  text_layer_set_font(s_version_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_version_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_version_layer, GTextOverflowModeWordWrap);
  //Add version layer as child
  layer_add_child(window_layer, text_layer_get_layer(s_version_layer));
}

static void main_window_unload(Window *window) {
  // Destroy the timelayer
  text_layer_destroy(s_time_layer);
  // Destroy date layer
  text_layer_destroy(s_date_layer);
  // Destroy the Version layer
  text_layer_destroy(s_version_layer);
}

static void update_time() {
  //get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  //write the current hours and minutes to a buffer
  static char s_time_buffer[8];
  
  //24 hour/12 hour selection logic and remove leading 0 for 12 hour
  
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);
    text_layer_set_text(s_time_layer,s_time_buffer); 
  } else {
    // Use 12 hour format
    strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M", tick_time);
    text_layer_set_text(s_time_layer,s_time_buffer+(('0' == s_time_buffer[0])?1:0)); 
  }
  
  static char s_date_buffer[30];
  strftime(s_date_buffer, sizeof(s_date_buffer),
          "%A, %B %e", tick_time);
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  switch (axis) {
    case ACCEL_AXIS_Y:
    if (direction > 0) {
      text_layer_set_text(s_version_layer, "Positive");
    } else {
      text_layer_set_text(s_version_layer, "Negative");
    }
    break;
    case ACCEL_AXIS_X:
    break;
    case ACCEL_AXIS_Z:
    break;
  }
}
static void handle_init(void){
  
}

static void handle_deinit(void){
  
}












