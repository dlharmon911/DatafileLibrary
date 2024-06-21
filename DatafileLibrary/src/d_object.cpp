#include <allegro5/allegro5.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <map>
#include "datafile/d_path.h"
#include "datafile/d_parser.h"
#include "datafile/d_object.h"
#include "datafile.h"

namespace dlh
{
	namespace datafile
	{
		namespace object
		{
			map_info_t m_info;

			deleter_t get_deleter(int32_t type)
			{
				auto d = m_info.find(type);
				if (d != m_info.end())
				{
					return d->second.deleter;
				}

				return nullptr;
			}

			int32_t get_type(const std::string& name)
			{
				for (auto t : m_info)
				{
					if (name == t.second.name) return t.first;
				}

				return -1;
			}

			void register_type(int32_t type, const std::string& name, parser_t parser, deleter_t deleter)
			{
				m_info[type] = { name, parser, deleter };
			}

			void* datafile_parser(const data_t& data)
			{
				bool error = false;
				datafile_t* datafile = new datafile_t();

				if (datafile)
				{
					int32_t nested = 0;

					for (auto i = data.cbegin(); i != data.cend(); ++i)
					{
						std::string key = i.key();
						if (key[0] == '$')
						{
							continue;
						}

						int type = get_type(key);
						if (type < 0)
						{
							error = true;
							break;
						}

						for (auto o = (*i).cbegin(); o != (*i).cend(); ++o)
						{
							std::string oname = o.key();

							if (oname[0] != '$')
							{
								data_t op = data.get_child(key);
								if (!op)
								{
									error = true;
									break;
								}

								data_t on = op.get_child(oname);
								if (!on)
								{
									error = true;
									break;
								}

								void* result = m_info[type].parser(on);
								if (!result)
								{
									error = true;
									break;
								}

								datafile->push_back(type, string::to_upper(oname), result);
							}
						}
					}
				}

				if (datafile)
				{
					if (error)
					{
						delete datafile;
						datafile = nullptr;
					}
				}

				return datafile;
			}

			void* bitmap_parser(const data_t& data)
			{
				ALLEGRO_BITMAP* bitmap = nullptr;

				if (data.contains("file"))
				{
					const data_t node_file = data.get_child("file");

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
						const data_t node_masked = data.get_child("masked");

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

			void* font_parser(const data_t& data)
			{
				ALLEGRO_FONT* font = nullptr;
				if (data.contains("file"))
				{
					const data_t node_file = data.get_child("file");

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
						data_t node_size = data.get_child("size");

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
						data_t node_truetype = data.get_child("truetype");

						if (!node_truetype.has_content())
						{
							return nullptr;
						}

						truetype = node_truetype.get_as<bool>();
					}

					if (data.contains("flags"))
					{
						data_t node_flags = data.get_child("flags");

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

			void* text_parser(const data_t& data)
			{
				if (data.has_content())
				{
					return new std::string(data.get_string());
				}

				return nullptr;
			}

			void destroy_datafile(void* object)
			{
				if (object)
				{
					delete (datafile_t*)object;
				}
			}

			void destroy_text(void* object)
			{
				if (object)
				{
					delete (std::string*)object;
				}
			}
		}
	}
}
