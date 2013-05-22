#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xE1, 0x9C, 0x17, 0xA3, 0xCE, 0x09, 0x4A, 0xF1, 0xB2, 0x90, 0xC0, 0x91, 0x8E, 0xF7, 0xDE, 0xE5 }
PBL_APP_INFO(MY_UUID,
             "Adv Time: Jake", "ThomW",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

BmpContainer background_image_container;

RotBmpPairContainer hour_hand_image_container;
RotBmpPairContainer minute_hand_image_container;


void update_display(PblTm* t) {

  // SUPER huge thanks to dansl for sharing the code for his silly-walk watch that made this possible
  // https://github.com/dansl/pebble-silly-walk/tree/master/silly_walk

  hour_hand_image_container.layer.black_layer.rotation = hour_hand_image_container.layer.white_layer.rotation = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 30) + (t->tm_min / 2)) / 360;
  hour_hand_image_container.layer.layer.frame.origin.x = (144 - hour_hand_image_container.layer.layer.frame.size.w) * 0.5;
  hour_hand_image_container.layer.layer.frame.origin.y = (168 - hour_hand_image_container.layer.layer.frame.size.h) * 0.5;
  layer_mark_dirty(&hour_hand_image_container.layer.layer);

  minute_hand_image_container.layer.black_layer.rotation = minute_hand_image_container.layer.white_layer.rotation = TRIG_MAX_ANGLE * (t->tm_min  * 6) / 360;
  minute_hand_image_container.layer.layer.frame.origin.x = (144 - minute_hand_image_container.layer.layer.frame.size.w) * 0.5;
  minute_hand_image_container.layer.layer.frame.origin.y = (168 - minute_hand_image_container.layer.layer.frame.size.h) * 0.5;
  layer_mark_dirty(&minute_hand_image_container.layer.layer);
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;

  update_display(t->tick_time);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "AT Jake");
  window_stack_push(&window, true);

  window_set_background_color(&window, GColorBlack);  

  resource_init_current_app(&APP_RESOURCES);

  // Set up a layer for the static watch face background
  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image_container);
  background_image_container.layer.layer.frame.origin.y = 0;
  background_image_container.layer.layer.frame.origin.y = 0;
  layer_add_child(&window.layer, &background_image_container.layer.layer);

  // Set up a layer for the hour hand
  rotbmp_pair_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_WHITE, RESOURCE_ID_IMAGE_HOUR_HAND_BLACK, &hour_hand_image_container);
  rotbmp_pair_layer_set_src_ic(&hour_hand_image_container.layer, GPoint(10, 45));
  layer_add_child(&window.layer, &hour_hand_image_container.layer.layer);

  // Set up a layer for the minute hand
  rotbmp_pair_init_container(RESOURCE_ID_IMAGE_MINUTE_HAND_WHITE, RESOURCE_ID_IMAGE_MINUTE_HAND_BLACK, &minute_hand_image_container);
  rotbmp_pair_layer_set_src_ic(&minute_hand_image_container.layer, GPoint(10, 62));
  layer_add_child(&window.layer, &minute_hand_image_container.layer.layer);

  // Avoid a blank screen on watch start
  PblTm tick_time;
  get_time(&tick_time);
  update_display(&tick_time);

}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&background_image_container);
  rotbmp_pair_deinit_container(&hour_hand_image_container);
  rotbmp_pair_deinit_container(&minute_hand_image_container);
}


void pbl_main(void *params) {
  
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };

  app_event_loop(params, &handlers);
}
