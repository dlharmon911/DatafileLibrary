#include <allegro5/allegro5.h>
#include <allegro5\allegro_physfs.h>
#include <physfs.h>
#include <map>
#include "datafile/d_path.h"
#include "datafile/d_string.h"
#include "datafile/d_object.h"
#include "datafile/d_parser.h"
#include "datafile.h"

void al_destroy_datafile(ALLEGRO_DATAFILE* datafile);

namespace dlh
{
	datafile_t::datafile_t() : m_object_data(), m_element_data() {}

	datafile_t::~datafile_t()
	{
		this->clear();
	}

	void datafile_t::clear()
	{
		while (this->m_object_data.size() > 0)
		{
			this->pop_back();
		}
		this->m_element_data.clear();
	}

	size_t datafile_t::size() const
	{
		return this->m_object_data.size();
	}

	bool datafile_t::is_empty() const
	{
		return this->m_object_data.empty();
	}

	void datafile_t::pop_back()
	{
		ALLEGRO_ASSERT(this->m_object_data.size() > 0);
		this->m_object_data.pop_back();
		this->m_element_data.pop_back();
	}

	datafile_t::value_t& datafile_t::operator[](size_t index)
	{
		ALLEGRO_ASSERT(index < this->m_object_data.size());
		return this->m_object_data[index];
	}

	const datafile_t::value_t& datafile_t::operator[](size_t index) const
	{
		ALLEGRO_ASSERT(index < this->m_object_data.size());
		return this->m_object_data[index];
	}

	datafile_t::info_t& datafile_t::get_object_info(size_t index)
	{
		ALLEGRO_ASSERT(index < this->m_element_data.size());
		return this->m_element_data[index];
	}

	const datafile_t::info_t& datafile_t::get_object_info(size_t index) const
	{
		ALLEGRO_ASSERT(index < this->m_element_data.size());
		return this->m_element_data[index];
	}

	datafile_t::iterator datafile_t::begin()
	{
		return datafile_t::iterator(this->m_object_data.begin(), this->m_element_data.begin());
	}

	datafile_t::iterator datafile_t::end()
	{
		return datafile_t::iterator(this->m_object_data.end(), this->m_element_data.end());
	}

	datafile_t::const_iterator datafile_t::cbegin()
	{
		return datafile_t::const_iterator(this->m_object_data.cbegin(), this->m_element_data.cbegin());
	}

	datafile_t::const_iterator datafile_t::cend()
	{
		return datafile_t::const_iterator(this->m_object_data.cend(), this->m_element_data.cend());
	}

	std::shared_ptr<datafile_t> datafile_t::load(const std::string& filename, const char sListSep)
	{
		std::shared_ptr<datafile_t> dfile;
		std::string filepath = path::make_canonical(filename);
		std::string base;
		std::string ext;
		std::string path;

		filepath = dlh::path::make_canonical(filename);
		dlh::path::split_filepath(filepath, path, base, ext);

		bool archive = false;

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
				dfile = datafile::parser::parse("index.ini", sListSep);
				PHYSFS_unmount(filename.c_str());
			}

			al_set_new_file_interface(file_interface);
		}
		else
		{
			std::string dir = al_get_current_directory();

			al_change_directory((dir + ALLEGRO_NATIVE_PATH_SEP + path).c_str());
			dfile = datafile::parser::parse(base + "." + ext, sListSep);
			al_change_directory(dir.c_str());
		}

		return dfile;
	}

	ALLEGRO_DATAFILE* datafile_t::al_convert_to_allegro_datafile(std::shared_ptr<dlh::datafile_t>& datafile)
	{
		ALLEGRO_DATAFILE* rv = nullptr;

		if (datafile)
		{
			size_t size = datafile->size();
			size_t index = 0;
			bool error = false;

			rv = new ALLEGRO_DATAFILE[size + 1];

			if (rv)
			{
				for (auto o = datafile->begin(); o != datafile->end(); ++o)
				{
					rv[index].type = o.type();
					rv[index].data = o.data();

					if (o.type() == dlh::datafile::object::type_t::datafile)
					{
						std::shared_ptr<datafile_t> d = std::static_pointer_cast<datafile_t>(o.data());

						rv[index].data = std::static_pointer_cast<void>(std::shared_ptr<ALLEGRO_DATAFILE>(al_convert_to_allegro_datafile(d), al_destroy_datafile));

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
}
