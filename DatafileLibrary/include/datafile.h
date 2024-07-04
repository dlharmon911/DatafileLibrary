#ifndef _DLH_DATAFILE_H_
#define _DLH_DATAFILE_H_

#include <allegro5/allegro5.h>
#include <vector>
#include <utility>
#include <string>
#include "datafile/d_memory.h"
#include "datafile/d_base.h"

namespace dlh
{
	class datafile_t
	{
	private:
		using vector_t = std::vector<value_t>;

	public:
		using info_t = std::pair<int32_t, std::string>;

		datafile_t();
		datafile_t(const std::string& filename, const char sListSep = ',');
		datafile_t(const datafile_t& datafile);
		~datafile_t();
		datafile_t& operator = (const datafile_t& datafile);

		size_t size() const;
		bool is_empty() const;
		void clear();

		template <typename T>
		std::shared_ptr<T> get(size_t index)
		{
			ALLEGRO_ASSERT(index < this->m_object_data.size());
			return std::static_pointer_cast<T>(this->m_object_data[index]);
		}

		template <typename T>
		const std::shared_ptr<T> get(size_t index) const
		{
			ALLEGRO_ASSERT(index < this->m_object_data.size());
			return std::static_pointer_cast<T>(this->m_object_data[index]);
		}

		void* operator[](size_t index);
		const void* operator[](size_t index) const;

		info_t& get_info(size_t index);
		const info_t& get_info(size_t index) const;

		template <typename T>
		void push_back(int32_t type, const std::string& name, std::shared_ptr<T>& data)
		{
			this->m_object_data.push_back(std::static_pointer_cast<void>(data));
			this->m_element_data.push_back({ type, name });
		}

		void pop_back();

		class iterator
		{
		private:
			iterator() = default;
		public:
			using pointer_type = value_t*;
			using reference_type = value_t&;
			typedef std::ptrdiff_t difference_type;

			iterator(vector_t::iterator o, std::vector<info_t>::iterator e) : m_o(o), m_e(e) {}
			int32_t type() const { return this->m_e->first; }
			std::string name() const { return this->m_e->second; }
			value_t data() { return *this->m_o; }

			bool operator == (const iterator& it) const { return (this->m_o == it.m_o && this->m_e == it.m_e); }
			bool operator != (const iterator& it) const { return !operator == (it); }
			iterator& operator ++() { ++this->m_o; ++this->m_e; return *this; }
			iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
			iterator& operator --() { --this->m_o; --this->m_e; return *this; }
			iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }
			value_t operator *() { return *this->m_o; }
			pointer_type operator &() { return &(*this->m_o); }

		private:
			vector_t::iterator m_o;
			std::vector<info_t>::iterator m_e;
		};

		class const_iterator
		{
		private:
			const_iterator() = default;
		public:
			using pointer_type = const value_t*;
			using reference_type = const value_t&;
			typedef std::ptrdiff_t difference_type;

			const_iterator(vector_t::const_iterator o, std::vector<info_t>::const_iterator e) : m_o(o), m_e(e) {}
			int32_t type() const { return this->m_e->first; }
			std::string name() const { return this->m_e->second; }
			value_t data() const { return *this->m_o; }

			bool operator == (const const_iterator& it) const { return (this->m_o == it.m_o && this->m_e == it.m_e); }
			bool operator != (const const_iterator& it) const { return !operator == (it); }
			const_iterator& operator ++() { ++this->m_o; ++this->m_e; return *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
			const_iterator& operator --() { --this->m_o; --this->m_e; return *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }
			value_t operator *() const { return *this->m_o; }
			pointer_type operator &() const { return &(*this->m_o); }

		private:
			vector_t::const_iterator m_o;
			std::vector<info_t>::const_iterator m_e;;
		};

		iterator begin();
		iterator end();
		const_iterator cbegin();
		const_iterator cend();

	private:
		vector_t m_object_data;
		std::vector<info_t> m_element_data;
	};

	bool al_generate_header_file(const char* manifest_filename, const char* header_filename, const char sListSep = ',');
}

#endif // !_DLH_DATAFILE_H_
