#ifndef _DLH_PARSER_H_
#define _DLH_PARSER_H_

#include "datafile/d_dson.h"
#include "datafile/d_string.h"

namespace dlh
{
	enum element_type
	{
		datafile = 0,
		bitmap,
		font,
		text,
		user_defined = 0x100
	};

	namespace parser
	{
		class data_t
		{
		public:
			data_t();
			data_t(const dson_t& dson, const dson_t& config);
			data_t(const data_t& data);
			~data_t();
			data_t& operator = (const data_t& data);
			explicit operator bool() const;
			bool has_children() const;
			bool contains(const std::string& name) const;
			const data_t get_child(const std::string& name) const;
			bool has_content() const;

			std::string get_string() const;

			template <typename T>
			T get_as() const = delete;

			template <> int32_t get_as() const;
			template <> double get_as() const;
			template <> bool get_as() const;

			class const_iterator
			{
			public:
				using value_type = data_t;
				typedef std::ptrdiff_t difference_type;

				const_iterator() = default;
				const_iterator(const dson_t::const_iterator& iter, const dson_t& config) : m_iter(iter), m_config(&config) {}

				std::string key() const { return this->m_iter.key(); };
				bool operator == (const const_iterator& it) const { return (this->m_iter == it.m_iter); }
				bool operator != (const const_iterator& it) const { return !operator == (it); }
				const_iterator& operator ++() { ++this->m_iter; return *this; }
				const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
				const_iterator& operator --() { --this->m_iter; return *this; }
				const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }
				value_type operator *() { return data_t((*this->m_iter), *this->m_config); }

			private:
				dson_t::const_iterator m_iter;
				const dson_t* m_config;
			};

			const_iterator cbegin() const;
			const_iterator cend() const;

		private:
			const dson_t* m_dson;
			const dson_t* m_config;
		};

		using deleter_t = void (*)(void*);
		using parser_t = void* (*)(const data_t& data);

		deleter_t get_deleter(int32_t type);
		void register_object_type(int32_t type, const std::string& name, parser_t parser, deleter_t deleter);
	}
}

#endif // !_DLH_PARSER_H_
