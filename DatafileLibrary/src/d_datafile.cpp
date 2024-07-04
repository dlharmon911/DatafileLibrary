#include <allegro5/allegro5.h>
#include <allegro5\allegro_physfs.h>
#include <physfs.h>
#include <map>
#include "datafile/d_path.h"
#include "datafile/d_string.h"
#include "datafile/d_object.h"
#include "datafile/d_parser.h"
#include "datafile.h"

namespace dlh
{
	datafile_t::datafile_t() : m_object_data(), m_element_data() {}

	datafile_t::datafile_t(const std::string& filename, const char sListSep) : m_object_data(), m_element_data()
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

		*this = *(dfile.get());
	}

	datafile_t::datafile_t(const datafile_t& datafile) : m_object_data(datafile.m_object_data), m_element_data(datafile.m_element_data) {}

	datafile_t::~datafile_t()
	{
		this->clear();
	}

	datafile_t& datafile_t::operator = (const datafile_t& datafile)
	{
		this->m_object_data = datafile.m_object_data;
		this->m_element_data = datafile.m_element_data;

		return *this;
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

	void* datafile_t::operator[](size_t index)
	{
		ALLEGRO_ASSERT(index < this->m_object_data.size());
		return this->m_object_data[index].get();
	}

	const void* datafile_t::operator[](size_t index) const
	{
		ALLEGRO_ASSERT(index < this->m_object_data.size());
		return this->m_object_data[index].get();
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
}
