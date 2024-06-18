#include <allegro5/allegro5.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_physfs.h>
#include <physfs.h>
#include <map>
#include "datafile/d_parser.h"
#include "datafile/d_path.h"
#include "datafile/d_string.h"
#include "datafile/d_config.h"
#include "datafile/d_system.h"
#include "datafile.h"

namespace dlh
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

	void* object_parser_datafile(const parser::data_t& data);
	void* object_parser_bitmap(const parser::data_t& data);
	void* object_parser_font(const parser::data_t& data);
	void* object_parser_text(const parser::data_t& data);

	datafile_t* read(const std::string& filename, const char sListSep = ',');
	datafile_t* read(const dson_t& dson, config_t& config);

	typedef struct object_type_info_t
	{
		std::string name;
		parser::parser_t parser = nullptr;
		parser::deleter_t deleter = nullptr;
	} object_type_info_t;

	void destroy_datafile(datafile_t* dlh)
	{
		if (dlh) delete dlh;
	}

	void destroy_cstring(std::string* str)
	{
		if (str)
			delete str;
	}

	using map_object_info_t = std::map<int32_t, object_type_info_t>;

	map_object_info_t m_object_info;
	bool m_initialized = false;

	namespace parser
	{
		parser::deleter_t get_deleter(int32_t type)
		{
			auto d = m_object_info.find(type);
			if (d != m_object_info.end())
			{
				return d->second.deleter;
			}

			return nullptr;
		}

		void register_object_type(int32_t type, const std::string& name, parser::parser_t parser, parser::deleter_t deleter)
		{
			m_object_info[type] = { name, parser, deleter };
		}
	}

	int32_t get_object_type(const std::string& name)
	{
		for (auto t : m_object_info)
		{
			if (name == t.second.name) return t.first;
		}

		return -1;
	}

	void register_default_object_types()
	{
		if (!m_initialized)
		{
			parser::register_object_type(element_type::bitmap, "bitmap", (parser::parser_t)object_parser_bitmap, (parser::deleter_t)al_destroy_bitmap);
			parser::register_object_type(element_type::font, "font", (parser::parser_t)object_parser_font, (parser::deleter_t)al_destroy_font);
			parser::register_object_type(element_type::text, "text", (parser::parser_t)object_parser_text, (parser::deleter_t)destroy_cstring);
			parser::register_object_type(element_type::datafile, "datafile", (parser::parser_t)object_parser_datafile, (parser::deleter_t)destroy_datafile);
		}
		m_initialized = true;
	}

	std::vector<std::string> m_path;

	int32_t process_shortcuts(const dson_t& dson, config_t& config, const std::string& prefix = "")
	{
		for (auto i = dson.cbegin(); i != dson.cend(); ++i)
		{
			std::string key = i.key();
			if (key[0] == '$')
			{
				std::string name = key.substr(1, key.length() - 1);
				std::string value = i->get_string();

				if (value.size() == 0)
				{
					return -1;
				}

				if (prefix.size() == 0)
				{
					config[name].set_string(value);
				}
				else
				{

					config.get_property(prefix)[name].set_string(value);
				}
			}
			else
			{
				std::string _prefix = prefix;
				if (_prefix.size() > 0)
				{
					_prefix.push_back('.');
				}

				if (process_shortcuts(dson[key], config, _prefix + key) < 0)
				{
					return -1;
				}
			}
		}

		return 0;
	}

	datafile_t* read(const dson_t& dson, config_t& config)
	{
		datafile_t* dlh = nullptr;

		auto i = dson.cbegin();
		dson_t output;

		if (process_shortcuts(*i, config) == 0)
		{
			parser::data_t data((*i), config);

			dlh = (datafile_t*)object_parser_datafile(data);
		}

		return dlh;
	}

	datafile_t* read(const std::string& filename, const char sListSep)
	{
		dson_t dson;
		std::string working_path;
		std::string index_path;
		std::string file_name;
		config_t config;

		register_default_object_types();

		if (al_filename_exists(filename.c_str()))
		{
			if (dson_t::read(dson, filename))
			{
				if (dson.get_children_count() == 1)
				{
					return read(dson, config);
				}
			}
		}

		return nullptr;
	}

	void* object_parser_datafile(const parser::data_t& data)
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

				int type = get_object_type(key);
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
						parser::data_t op = data.get_child(key);
						if (!op)
						{
							error = true;
							break;
						}

						parser::data_t on = op.get_child(oname);
						if (!on)
						{
							error = true;
							break;
						}

						void* result = m_object_info[type].parser(on);
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

	bool write_header_object(ALLEGRO_FILE* pfile, const dson_t& dson, std::vector<std::string>& header, const std::string& name, int32_t& n)
	{
		for (auto i = dson.cbegin(); i != dson.cend(); ++i)
		{
			std::string key = i.key();

			if (key[0] == '$') continue;

			header.push_back(key);
			std::string out = "constexpr auto " + string::fuse(header, '_') + " = " + std::to_string(n) + ';';
			while (out.size() < 51) out.push_back(' ');
			out.append("/* ");
			out.append(string::to_upper(name));
			out.append(" */\n");
			al_fputs(pfile, out.c_str());
			header.pop_back();
			++n;
		}

		return true;
	}

	bool write_header_datafile(ALLEGRO_FILE* pfile, const dson_t& dson, std::vector<std::string>& header, int32_t& n)
	{
		for (auto i = dson.cbegin(); i != dson.cend(); ++i)
		{
			std::string key = i.key();

			if (key[0] == '$') continue;

			header.push_back(key);
			write_header_object(pfile, dson[key], header, key, n);
			header.pop_back();

			if (key == "dlh")
			{
				int32_t n = 0;
				const dson_t& dlh = dson[key];

				for (auto j = dlh.cbegin(); j != dlh.cend(); ++j)
				{
					header.push_back(j.key());
					write_header_datafile(pfile, dlh, header, n);
					header.pop_back();
				}
			}
		}

		return true;
	}

	bool write_header(const dson_t& dson, const std::string& input_text_filename, const std::string& output_header_filename)
	{
		bool rv = false;
		std::vector<std::string> header;
		ALLEGRO_FILE* pfile = nullptr;

		if (dson.get_children_count() == 1)
		{
			std::string output = path::make_canonical(output_header_filename);

			pfile = al_fopen(output.c_str(), "wb");

			if (pfile)
			{
				int32_t n = 0;

				al_fputs(pfile, "/* Datafile object indexes */\n");
				al_fputs(pfile, "/* Index File: ");
				al_fputs(pfile, input_text_filename.c_str());
				al_fputs(pfile, " */\n");
				al_fputs(pfile, "/* Date: ");
				al_fputs(pfile, system::timestamp().c_str());
				al_fputs(pfile, " */ \n");
				al_fputs(pfile, "/* Do not hand edit! */\n\n");

				auto it = dson.cbegin();
				std::string key = it.key();

				header.push_back(key);

				write_header_datafile(pfile, dson[key], header, n);

				header.pop_back();

				al_fputs(pfile, "\n\n");

				al_fclose(pfile);

				rv = true;
			}
		}

		return rv;
	}

	datafile_t* datafile_t::load(const std::string& filename, const char sListSep)
	{
		datafile_t* dv = nullptr;
		std::string filepath = path::make_canonical(filename);
		std::string base;
		std::string ext;
		std::string path;

		filepath = dlh::path::make_canonical(filename);
		dlh::path::split_filepath(filepath, path, base, ext);

		bool archive = false;


		//std::string work_path;

		//ALLEGRO_PATH* base_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
		//if (base_path)
		//{
		//	al_make_path_canonical(base_path);
		//	work_path = al_path_cstr(base_path, ALLEGRO_NATIVE_PATH_SEP);
		//	al_destroy_path(base_path);
		//}

		const PHYSFS_ArchiveInfo** i = nullptr;
		for (i = PHYSFS_supportedArchiveTypes(); *i != NULL; i++)
		{
			if (string::to_upper(ext) == (*i)->extension)
			{
				archive = true;
				break;
			}
		}

		if (archive)
		{
			const ALLEGRO_FILE_INTERFACE* file_interface = al_get_new_file_interface();

			if (PHYSFS_mount(filename.c_str(), NULL, 1))
			{
				al_set_physfs_file_interface();
				dv = read("index.ini", sListSep);
				PHYSFS_unmount(filename.c_str());
			}

			al_set_new_file_interface(file_interface);
		}
		else
		{
			std::string dir = al_get_current_directory();

			al_change_directory((dir + ALLEGRO_NATIVE_PATH_SEP + path).c_str());
			dv = read(base + "." + ext, sListSep);
			al_change_directory(dir.c_str());
		}

		return dv;
	}
}


ALLEGRO_DATAFILE* al_convert_to_allegro_datafile(dlh::datafile_t* dv)
{
	ALLEGRO_DATAFILE* rv = nullptr;

	if (dv)
	{
		size_t size = dv->size();
		size_t index = 0;
		bool error = false;

		rv = new ALLEGRO_DATAFILE[size + 1];

		if (rv)
		{
			for (auto o = dv->begin(); o != dv->end(); ++o)
			{
				rv[index].type = o.type();
				rv[index].data = o.data();

				if (o.type() == dlh::element_type::datafile)
				{
					rv[index].data = (void*)al_convert_to_allegro_datafile((dlh::datafile_t*)(o.data()));
					if (!rv[index].data)
					{
						al_destroy_datafile(rv);
						error = true;
						size = index;
						break;
					}
				}
				++index;
			}

			rv[size].data = nullptr;
			rv[size].type = 0;

			if (error)
			{
				al_destroy_datafile(rv);
				rv = nullptr;
			}
		}
	}

	return rv;
}

bool al_generate_header_file(const char* manifest_filename, const char* header_filename, const char sListSep)
{
	bool rv = false;
	dlh::dson_t dson;
	std::string filepath;
	std::string base;
	std::string ext;
	std::string path;
	bool archive = false;

	filepath = dlh::path::make_canonical(manifest_filename);
	dlh::path::split_filepath(filepath, path, base, ext);

	const PHYSFS_ArchiveInfo** i = nullptr;
	for (i = PHYSFS_supportedArchiveTypes(); *i != NULL; i++)
	{
		if (dlh::string::to_upper(ext) == (*i)->extension)
		{
			archive = true;
			break;
		}
	}

	if (archive)
	{
		const ALLEGRO_FILE_INTERFACE* file_interface = al_get_new_file_interface();

		if (PHYSFS_mount(manifest_filename, NULL, 1))
		{
			al_set_physfs_file_interface();

			rv = dlh::dson_t::read(dson, "index.ini", sListSep);

			PHYSFS_unmount(manifest_filename);
		}

		al_set_new_file_interface(file_interface);
	}
	else
	{

		rv = dlh::dson_t::read(dson, manifest_filename, sListSep);

		//std::string work_path;
		//ALLEGRO_PATH* base_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
		//if (base_path)
		//{
		//	al_make_path_canonical(base_path);
		//	work_path = al_path_cstr(base_path, ALLEGRO_NATIVE_PATH_SEP);
		//	al_destroy_path(base_path);
		//}

		//std::string dir = al_get_current_directory();

		//al_change_directory((work_path + ALLEGRO_NATIVE_PATH_SEP + path).c_str());


		//al_change_directory(dir.c_str());
	}

	if (rv)
	{
		rv = write_header(dson, manifest_filename, header_filename);
	}

	return rv;
}

ALLEGRO_DATAFILE* al_load_datafile(const char* filename, const char sListSep)
{
	dlh::datafile_t* dv = dlh::datafile_t::load(filename, sListSep);

	if (dv)
	{
		return al_convert_to_allegro_datafile(dv);
		delete dv;
	}

	return nullptr;
}

void al_destroy_datafile(ALLEGRO_DATAFILE* dlh)
{
	if (dlh)
	{
		ALLEGRO_DATAFILE* object = dlh;
		while (object->data)
		{
			dlh::parser::deleter_t deleter = dlh::parser::get_deleter(object->type);

			if (object->type == dlh::element_type::datafile)
			{
				deleter = (dlh::parser::deleter_t)al_destroy_datafile;
			}

			deleter(object->data);
			object->data = nullptr;
			++object;
		}

		delete[] dlh;
	}
}
