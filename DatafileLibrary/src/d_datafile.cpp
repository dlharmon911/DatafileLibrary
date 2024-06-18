#include "datafile.h"

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

	void* datafile_t::operator[](size_t index)
	{
		ALLEGRO_ASSERT(index < this->m_object_data.size());
		return this->m_object_data[index];
	}

	const void* datafile_t::operator[](size_t index) const
	{
		ALLEGRO_ASSERT(index < this->m_object_data.size());
		return this->m_object_data[index];
	}

	datafile_t::item_t& datafile_t::get_object_info(size_t index)
	{
		ALLEGRO_ASSERT(index < this->m_element_data.size());
		return this->m_element_data[index];
	}

	const datafile_t::item_t& datafile_t::get_object_info(size_t index) const
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
