#ifndef _DLH_DATAFILE_H_
#define _DLH_DATAFILE_H_

#include <allegro5/allegro5.h>
#include <vector>
#include <utility>
#include <string>
#include "al_datafile.h"

namespace dlh
{
	class datafile_t
	{
	public:
		using item_t = std::pair<int32_t, std::string>;

		datafile_t();
		~datafile_t();

		static datafile_t* load(const std::string& filename, const char sListSep = ',');
		static ALLEGRO_DATAFILE* al_convert_to_allegro_datafile(dlh::datafile_t* dv);

		size_t size() const;
		bool is_empty() const;
		void clear();

		template <typename T>
		T get(size_t index)
		{
			ALLEGRO_ASSERT(index < this->m_object_data.size());
			return (T)this->m_object_data[index];
		}

		template <typename T>
		const T get(size_t index) const
		{
			ALLEGRO_ASSERT(index < this->m_object_data.size());
			return (T)this->m_object_data[index];
		}

		void* operator[](size_t index);
		const void* operator[](size_t index) const;
		item_t& get_object_info(size_t index);
		const item_t& get_object_info(size_t index) const;
		void pop_back();

		template <typename T>
		void push_back(int32_t type, const std::string& name, T* data)
		{
			this->m_object_data.push_back(data);
			this->m_element_data.push_back({ type, name });
		}

		class iterator
		{
		private:
			iterator() = default;
		public:
			using value_type = void*;
			using pointer_type = value_type*;
			using reference_type = value_type&;
			typedef std::ptrdiff_t difference_type;

			iterator(std::vector<void*>::iterator o, std::vector<item_t>::iterator e) : m_o(o), m_e(e) {}
			int32_t type() const { return this->m_e->first; }
			std::string name() const { return this->m_e->second; }
			pointer_type data() { return (pointer_type)(*this->m_o); }

			bool operator == (const iterator& it) const { return (this->m_o == it.m_o && this->m_e == it.m_e); }
			bool operator != (const iterator& it) const { return !operator == (it); }
			iterator& operator ++() { ++this->m_o; ++this->m_e; return *this; }
			iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
			iterator& operator --() { --this->m_o; --this->m_e; return *this; }
			iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }
			pointer_type operator *() { return (pointer_type)(*this->m_o); }
			reference_type operator &() { return (reference_type)(this->m_o); }

		private:
			std::vector<void*>::iterator m_o;
			std::vector<item_t>::iterator m_e;
		};

		class const_iterator
		{
		private:
			const_iterator() = default;
		public:
			using value_type = void*;
			using pointer_type = const value_type*;
			using reference_type = const value_type&;
			typedef std::ptrdiff_t difference_type;

			const_iterator(std::vector<void*>::const_iterator o, std::vector<item_t>::const_iterator e) : m_o(o), m_e(e) {}
			int32_t type() const { return this->m_e->first; }
			std::string name() const { return this->m_e->second; }
			pointer_type data() const { return (pointer_type)(*this->m_o); }

			bool operator == (const const_iterator& it) const { return (this->m_o == it.m_o && this->m_e == it.m_e); }
			bool operator != (const const_iterator& it) const { return !operator == (it); }
			const_iterator& operator ++() { ++this->m_o; ++this->m_e; return *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
			const_iterator& operator --() { --this->m_o; --this->m_e; return *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }
			pointer_type operator *() const { return (pointer_type)(*this->m_o); }
			reference_type operator &() const { return (reference_type)(this->m_o); }

		private:
			std::vector<void*>::const_iterator m_o;
			std::vector<item_t>::const_iterator m_e;
		};

		iterator begin();
		iterator end();
		const_iterator cbegin();
		const_iterator cend();

	private:
		std::vector<void*> m_object_data;
		std::vector<item_t> m_element_data;
	};
}

#endif // !_DLH_DATAFILE_H_
