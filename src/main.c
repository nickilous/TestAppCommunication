#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MESSAGE_SIZE 32
#define MY_UUID { 0xB3, 0xBE, 0x75, 0x48, 0x31, 0x7D, 0x4D, 0xEE, 0xAE, 0x1D, 0x22, 0x3C, 0x0D, 0x5C, 0x5A, 0xE7 }
PBL_APP_INFO(MY_UUID,
             "Template App", "Your Company",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

static struct MessageData{
	Window window;
	TextLayer message_layer;
	
	//taking received_message and storing it in current_message for use by app
	char received_message[MESSAGE_SIZE];
	char current_message[MESSAGE_SIZE];
	
	AppSync sync;
	uint8_t sync_buffer[MESSAGE_SIZE];
} m_data;

enum {
  MESSAGE_KEY = 0x0,         // TUPLE_CSTRING
  //WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING
};

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

  switch (key) {
  case MESSAGE_KEY:
    text_layer_set_text(&m_data.message_layer, new_tuple->value->cstring);
	layer_mark_dirty(&m_data.message_layer.layer);
    break;
  default:
    return;
  }
}

// TODO: Error handling
static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
}


void handle_init(AppContextRef ctx) {
  (void)ctx;
	strncat(m_data.current_message, "none", MESSAGE_SIZE - 1);
	
	Window* window = &m_data.window;
  	window_init(window, "AppComTest");
	window_set_background_color(window, GColorBlack);
	window_set_fullscreen(window, true);
	
	//Setup Text Layer
	text_layer_init(&m_data.message_layer, GRect(0,100,144, 68));
	text_layer_set_text_color(&m_data.message_layer, GColorWhite);
	text_layer_set_background_color(&m_data.message_layer, GColorClear);
	text_layer_set_font(&m_data.message_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(&m_data.message_layer, GTextAlignmentCenter);
	text_layer_set_text(&m_data.message_layer, m_data.current_message);
	layer_add_child(&window->layer, &m_data.message_layer.layer);
	
	//initial values monitored for change.  Setup when app first runs.
	Tuplet initial_values[] = {
    	TupletCString(MESSAGE_KEY, m_data.current_message),
    };
	
	//initiation of app syncing
	app_sync_init(&m_data.sync, m_data.sync_buffer, sizeof(m_data.sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);
	
  	window_stack_push(window, true /* Animated */);
}




static void handle_deinit(AppContextRef ctx) {
  app_sync_deinit(&m_data.sync);
  
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 32,
				.outbound = 16,
			}
		}
  };
  app_event_loop(params, &handlers);
}
