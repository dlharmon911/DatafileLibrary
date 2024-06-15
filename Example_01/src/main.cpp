/*
index.ini generates a datafile with one bitmap
*/
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_physfs.h>
#include <physfs.h>
#include "datafile.h"
#include "data.h"

const float DEFAULT_BUFFER_WIDTH = 640.0;
const float DEFAULT_BUFFER_HEIGHT = 480.0;
const float DEFAULT_DISPLAY_FULLSCREEN_WIDTH = DEFAULT_BUFFER_WIDTH;
const float DEFAULT_DISPLAY_FULLSCREEN_HEIGHT = DEFAULT_BUFFER_HEIGHT;

ALLEGRO_DISPLAY* display = nullptr;
ALLEGRO_TIMER* timer = nullptr;
ALLEGRO_EVENT_QUEUE* event_queue = nullptr;
ALLEGRO_DATAFILE* data = nullptr;
ALLEGRO_BITMAP* buffer = nullptr;
bool kill = false;

int init(int argc, char** argv)
{
	if (!al_init())
	{
		return -1;
	}

	if (!al_install_mouse())
	{
		return -1;
	}

	if (!al_install_keyboard())
	{
		return -1;
	}

	if (!al_init_image_addon())
	{
		return -1;
	}

	if (!al_init_font_addon())
	{
		return -1;
	}

	if (!al_init_ttf_addon())
	{
		return -1;
	}

	if (!PHYSFS_init(argv[0]))
	{
		return -1;
	}

	timer = al_create_timer(1.0f / 30.0f);
	if (!timer)
	{
		return -1;
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
	al_set_new_window_title("Datafile Library - Example 01");
	display = al_create_display(DEFAULT_DISPLAY_FULLSCREEN_WIDTH, DEFAULT_DISPLAY_FULLSCREEN_HEIGHT);
	if (!display)
	{
		return -1;
	}

	buffer = al_create_bitmap(DEFAULT_BUFFER_WIDTH, DEFAULT_BUFFER_HEIGHT);
	if (!buffer)
	{
		return -1;
	}

	event_queue = al_create_event_queue();
	if (!event_queue)
	{
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

	if (!al_generate_header_file("data\\index.ini", "include\\data.h"))
	{
		return -1;
	}

	data = al_load_datafile("data\\index.ini");

	if (!data)
	{
		return -1;
	}

	al_start_timer(timer);

	return 0;
}

void shutdown()
{
	if (timer)
	{
		al_stop_timer(timer);
	}

	if (data)
	{
		al_destroy_datafile(data);
		data = nullptr;
	}

	if (event_queue)
	{
		al_unregister_event_source(event_queue, al_get_display_event_source(display));
		al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
		al_unregister_event_source(event_queue, al_get_keyboard_event_source());
		al_unregister_event_source(event_queue, al_get_mouse_event_source());
		al_destroy_event_queue(event_queue);
		event_queue = nullptr;
	}

	if (buffer)
	{
		al_destroy_bitmap(buffer);
		buffer = nullptr;
	}

	if (display)
	{
		al_destroy_display(display);
		display = nullptr;
	}

	if (timer)
	{
		al_destroy_timer(timer);
		timer = nullptr;
	}

	PHYSFS_deinit();
}

void draw()
{
	ALLEGRO_BITMAP* logo = (ALLEGRO_BITMAP*)data[0].data;

	al_draw_scaled_bitmap(logo,
		0.0f, 0.0f,
		(float)al_get_bitmap_width(logo),
		(float)al_get_bitmap_height(logo),
		0.0f, 0.0f,
		al_get_bitmap_width(buffer),
		al_get_bitmap_height(buffer), 0);
}

void input()
{
	static ALLEGRO_EVENT event;

	while (!al_event_queue_is_empty(event_queue))
	{
		al_get_next_event(event_queue, &event);

		switch (event.type)
		{
		case ALLEGRO_EVENT_TIMER:
		{
		} break;

		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			al_acknowledge_resize(display);
		} break;

		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			kill = true;
		} break;
		}
	}
}

void loop()
{
	static ALLEGRO_BITMAP* target = nullptr;

	while (!kill)
	{
		target = al_get_target_bitmap();
		al_set_target_bitmap(buffer);
		draw();
		al_set_target_bitmap(target);
		al_draw_scaled_bitmap(buffer,
			0.0f, 0.0f,
			al_get_bitmap_width(buffer),
			al_get_bitmap_height(buffer),
			0.0f, 0.0f,
			al_get_display_width(display),
			al_get_display_height(display), 0);
		al_flip_display();

		input();
		al_rest(0.0);
	}
}

int main(int argc, char** argv)
{
	if (init(argc, argv) == 0)
	{
		loop();
	}

	shutdown();

	return 0;
}
