#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "datafile\d_parser.h"
#include "datafile\d_path.h"
#include "datafile\d_string.h"
#include "datafile\d_config.h"

namespace datafile
{
	namespace parser
	{
		data_t::data_t() : m_dson(nullptr), m_config(nullptr)
		{
		}

		data_t::data_t(const dson_t& dson, const dson_t& config) : m_dson(&dson), m_config(&config)
		{
		}

		data_t::data_t(const data_t& data) : m_dson(data.m_dson), m_config(data.m_config)
		{
		}

		data_t::~data_t()
		{
		}

		data_t& data_t::operator = (const data_t& data)
		{
			this->m_dson = data.m_dson;
			this->m_config = data.m_config;
			return *this;
		}

		data_t::operator bool() const
		{
			return (this->m_dson != nullptr);
		}

		bool data_t::has_children() const
		{
			return this->m_dson->has_children();
		}

		bool data_t::contains(const std::string& name) const
		{
			return this->m_dson->contains(name);
		}

		const data_t data_t::get_child(const std::string& name) const
		{
			return data_t((*this->m_dson)[name], *this->m_config);
		}

		bool data_t::has_content() const
		{
			return this->m_dson->has_content();
		}

		std::string data_t::get_string() const
		{
			std::string output;

			if (this->m_dson->has_content())
			{
				std::string temp = this->m_dson->get_string();
				int rv = config::expand_string(*this->m_config, temp, output);

				if (rv < 0)
				{
					output.clear();
				}
			}

			return output;
		}

		template <> int32_t data_t::get_as() const
		{
			return std::atoi(this->get_string().c_str());
		}

		template <> double data_t::get_as() const
		{
			return std::atof(this->get_string().c_str());
		}

		template <> bool data_t::get_as() const
		{
			return string::to_lower(this->get_string()) == "true";
		}

		data_t::const_iterator data_t::cbegin() const
		{
			return data_t::const_iterator(this->m_dson->cbegin(), *this->m_config);
		}

		data_t::const_iterator data_t::cend() const
		{
			return data_t::const_iterator(this->m_dson->cend(), *this->m_config);
		}
	}

	void* object_parser_bitmap(const parser::data_t& data)
	{
		ALLEGRO_BITMAP* bitmap = nullptr;

		if (data.contains("file"))
		{
			const parser::data_t node_file = data.get_child("file");

			if (!node_file.has_content())
			{
				return nullptr;
			}

			std::string file = node_file.get_string();
			std::string filepath = path::make_canonical(file);
			std::string base;
			std::string ext;
			std::string path;

			path::split_filepath(file, path, base, ext);

			if (base.size() == 0)
			{
				return nullptr;
			}

			bitmap = al_load_bitmap(filepath.c_str());
			if (!bitmap)
			{
				return nullptr;
			}

			if (data.contains("masked"))
			{
				const parser::data_t node_masked = data.get_child("masked");

				if (!node_masked.has_content())
				{
					al_destroy_bitmap(bitmap);
					return nullptr;
				}

				bool is_masked = node_masked.get_as<bool>();
				if (is_masked)
				{
					al_convert_mask_to_alpha(bitmap, al_map_rgb(255, 0, 255));
				}
			}
		}

		return (void*)bitmap;
	}

	void* object_parser_font(const parser::data_t& data)
	{
		ALLEGRO_FONT* font = nullptr;
		if (data.contains("file"))
		{
			const parser::data_t node_file = data.get_child("file");

			if (!node_file.has_content())
			{
				return nullptr;
			}

			std::string file = node_file.get_string();
			std::string filepath = path::make_canonical(file);
			std::string base;
			std::string ext;
			std::string path;
			int32_t size = 0;
			bool truetype = false;
			int32_t flags = 0;

			path::split_filepath(file, path, base, ext);

			if (base.size() == 0) return nullptr;

			if (data.contains("size"))
			{
				parser::data_t node_size = data.get_child("size");

				if (!node_size.has_content())
				{
					return nullptr;
				}

				size = node_size.get_as<int32_t>();
			}
			else
			{
				return nullptr;
			}

			if (data.contains("truetype"))
			{
				parser::data_t node_truetype = data.get_child("truetype");

				if (!node_truetype.has_content())
				{
					return nullptr;
				}

				truetype = node_truetype.get_as<bool>();
			}

			if (data.contains("flags"))
			{
				parser::data_t node_flags = data.get_child("flags");

				if (!node_flags.has_content())
				{
					return nullptr;
				}

				std::string str = node_flags.get_string();
				std::vector<std::string> f;
				size_t sz = string::separate(str, f, '|');

				for (auto a : f)
				{
					if (truetype)
					{
						if (a == "no_kerning") flags |= ALLEGRO_TTF_NO_KERNING;
						if (a == "monochrome") flags |= ALLEGRO_TTF_MONOCHROME;
					}
					else
					{
						if (a == "no_premultified_alpha") flags |= ALLEGRO_NO_PREMULTIPLIED_ALPHA;
					}
				}
			}

			if (truetype)
			{
				font = al_load_ttf_font(filepath.c_str(), size, flags);
			}
			else
			{
				font = al_load_font(filepath.c_str(), size, flags);
			}
			if (!font)
			{
				return nullptr;
			}
		}

		return (void*)font;
	}

	void* object_parser_text(const parser::data_t& data)
	{
		if (data.has_content())
		{
			return new std::string(data.get_string());
		}

		return nullptr;
	}
}
