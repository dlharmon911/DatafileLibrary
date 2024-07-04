/*
index.ini generates a datafile with a custome object type
*/
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_physfs.h>
#include <physfs.h>
#include "datafile.h"
#include "datafile/d_parser.h"
#include "datafile/d_path.h"
#include "data.h"

const float DEFAULT_BUFFER_WIDTH = 640.0;
const float DEFAULT_BUFFER_HEIGHT = 480.0;
const float DEFAULT_DISPLAY_FULLSCREEN_WIDTH = DEFAULT_BUFFER_WIDTH;
const float DEFAULT_DISPLAY_FULLSCREEN_HEIGHT = DEFAULT_BUFFER_HEIGHT;

ALLEGRO_DISPLAY* display = nullptr;
ALLEGRO_TIMER* timer = nullptr;
ALLEGRO_EVENT_QUEUE* event_queue = nullptr;
std::shared_ptr<dlh::datafile_t> data;
ALLEGRO_BITMAP* buffer = nullptr;
bool kill = false;

typedef struct dim_t
{
	size_t m_width;
	size_t m_height;
} dim_t;

typedef struct tilesheet_t
{
	ALLEGRO_BITMAP* m_bitmap;
	dim_t m_tile;
	dim_t m_grid;
} tilesheet_t;

void destroy_tilesheet(void* tilesheet);

int32_t parse_bitmap(tilesheet_t* t, const dlh::datafile::data_t& data)
{
	if (data.contains("file"))
	{
		const dlh::datafile::data_t node = data.get_child("file");

		if (!node.has_content())
		{
			return -1;
		}

		std::string file = node.get_string();
		std::string filepath = dlh::path::make_canonical(file);
		std::string base;
		std::string ext;
		std::string path;

		dlh::path::split_filepath(file, path, base, ext);

		if (base.size() == 0)
		{
			return -1;
		}

		t->m_bitmap = al_load_bitmap(filepath.c_str());
		if (!t->m_bitmap)
		{
			return -1;
		}

		if (data.contains("masked"))
		{
			const dlh::datafile::data_t node_masked = data.get_child("masked");

			if (!node_masked.has_content())
			{
				return -1;
			}

			bool is_masked = node_masked.get_as<bool>();
			if (is_masked)
			{
				al_convert_mask_to_alpha(t->m_bitmap, al_map_rgb(255, 0, 255));
			}
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

int32_t parse_tile_dimensions(tilesheet_t* t, const dlh::datafile::data_t& data)
{
	if (data.contains("width"))
	{
		const dlh::datafile::data_t node = data.get_child("width");

		if (!node.has_content())
		{
			return -1;
		}

		t->m_tile.m_width = node.get_as<int32_t>();
	}
	else
	{
		return -1;
	}

	if (data.contains("height"))
	{
		const dlh::datafile::data_t node = data.get_child("height");

		if (!node.has_content())
		{
			return -1;
		}

		t->m_tile.m_height = node.get_as<int32_t>();
	}
	else
	{
		return -1;
	}

	return 0;
}

int32_t parse_grid_dimensions(tilesheet_t* t, const dlh::datafile::data_t& data)
{
	if (data.contains("width"))
	{
		const dlh::datafile::data_t node = data.get_child("width");

		if (!node.has_content())
		{
			return -1;
		}

		t->m_grid.m_width = node.get_as<int32_t>();
	}
	else
	{
		return -1;
	}

	if (data.contains("height"))
	{
		const dlh::datafile::data_t node = data.get_child("height");

		if (!node.has_content())
		{
			return -1;
		}

		t->m_grid.m_height = node.get_as<int32_t>();
	}
	else
	{
		return -1;
	}

	return 0;
}


int32_t parse(tilesheet_t* t, const dlh::datafile::data_t& data)
{
	if (data.contains("bitmap"))
	{
		const dlh::datafile::data_t node = data.get_child("bitmap");
		
		if (parse_bitmap(t, node) < 0)
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	if (data.contains("tile"))
	{
		const dlh::datafile::data_t node = data.get_child("tile");

		if (parse_tile_dimensions(t, node) < 0)
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	if (data.contains("grid"))
	{
		const dlh::datafile::data_t node = data.get_child("grid");

		if (parse_grid_dimensions(t, node) < 0)
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

std::shared_ptr<void> parse_tilesheet(const dlh::datafile::data_t& data)
{
	std::shared_ptr<tilesheet_t> rv = std::shared_ptr<tilesheet_t>(new tilesheet_t, destroy_tilesheet);

	if (rv)
	{
		rv.get()->m_bitmap = nullptr;
		rv.get()->m_tile = { 0, 0 };
		rv.get()->m_grid = { 0, 0 };

		if (parse(rv.get(), data) < 0)
		{
			rv.reset();
		}
	}

	return std::static_pointer_cast<void>(rv);
}

void destroy_tilesheet(void* tilesheet)
{
	if (tilesheet)
	{
		tilesheet_t* t = (tilesheet_t*)tilesheet;

		if (t->m_bitmap)
		{
			al_destroy_bitmap(t->m_bitmap);
		}
		delete t;
	}
}

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
	al_set_new_window_title("Datafile Library - Example 03");
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

	dlh::datafile::object::register_type(dlh::datafile::object::type_t::user_defined, "tilesheet", parse_tilesheet);

	if (!al_generate_header_file("data\\index.ini", "include\\data.h"))
	{
		return -1;
	}

	data = dlh::datafile_t::load("data\\index.ini");

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

	data.reset();

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
	tilesheet_t* tilesheet = (data.get()->get<tilesheet_t>(0)).get();

	size_t i = 40;
	float x = (float)((i % (tilesheet->m_grid.m_width) * (tilesheet->m_tile.m_width)));
	float y = (float)((i / (tilesheet->m_grid.m_width) * (tilesheet->m_tile.m_height)));

	al_clear_to_color(al_map_rgb(255, 255, 255));

	ALLEGRO_BITMAP* npc = tilesheet->m_bitmap;

	al_draw_scaled_bitmap(npc,
		x, y,
		(float)(tilesheet->m_tile.m_width),
		(float)(tilesheet->m_tile.m_height),
		100.0f, 100.0f,
		(float)(tilesheet->m_tile.m_width * 2),
		(float)(tilesheet->m_tile.m_height * 2), 0);
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
