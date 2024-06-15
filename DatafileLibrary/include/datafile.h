#ifndef _ALLEGRO_DATAFILE_H_
#define _ALLEGRO_DATAFILE_H_

#include <vector>
#include <string>

namespace datafile
{
	class datafile_t
	{
	public:
		typedef struct item_t
		{
			int32_t type;
			std::string name;
		} item_t;

		datafile_t();
		~datafile_t();
		size_t size() const;
		bool is_empty() const;
		void clear();

		template <typename T>
		T get(size_t index)
		{
			assert(index < this->m_object_data.size());
			return (T)this->m_object_data[index];
		}

		template <typename T>
		const T get(size_t index) const
		{
			assert(index < this->m_object_data.size());
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
			int32_t type() const { return this->m_e->type; }
			std::string name() const { return this->m_e->name; }
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
			int32_t type() const { return this->m_e->type; }
			std::string name() const { return this->m_e->name; }
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

	datafile_t* load_datafile(const std::string& filename, const char sListSep = ',');
}

typedef struct ALLEGRO_DATAFILE
{
	void* data;
	int32_t type;
} ALLEGRO_DATAFILE;

bool write_header_file(const char* manifest_filename, const char* header_filename, const char sListSep = ',');
ALLEGRO_DATAFILE* al_load_datafile(const char* filename, const char sListSep = ',');
void al_destroy_datafile(ALLEGRO_DATAFILE* datafile);

#endif // !_ALLEGRO_DATAFILE_H_
