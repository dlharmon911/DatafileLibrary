/*
	OneLoneCoder - DataFile v1.00
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	An "easy to use" serialisation/deserialisation class that yields
	human readable hierachical files.

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2022 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, OneLoneCoder 2019, 2020, 2021, 2022

*/

#ifndef _DATAFILE_DSON_H_
#define _DATAFILE_DSON_H_

#include <vector>
#include <unordered_map>
#include <string>

namespace dh
{
	class dson_t
	{
	public:
		dson_t();

		const std::string get_string(const size_t nItem = 0) const;
		void set_string(const std::string& sString, const size_t nItem = 0);

		template <typename T>
		T get_as(const size_t nItem = 0) const = delete;

		template <> int32_t get_as(const size_t nItem) const;
		template <> double get_as(const size_t nItem) const;
		template <> bool get_as(const size_t nItem) const;

		template <typename T>
		void set_as(const T value, const size_t nItem = 0) = delete;

		template <> void set_as(const int32_t value, const size_t nItem);
		template <>	void set_as(const double value, const size_t nItem);
		template <>	void set_as(const bool value, const size_t nItem);

		size_t get_content_count() const;
		size_t get_children_count() const;
		bool contains(const std::string& sName) const;
		dson_t& get_property(const std::string& name);
		const dson_t& get_property(const std::string& name) const;
		dson_t& get_indexed_property(const std::string& name, const size_t nIndex);
		static bool write(const dson_t& n, const std::string& sFileName, const std::string& sIndent = "\t", const char sListSep = ',');
		static bool read(dson_t& n, const std::string& sFileName, const char sListSep = ',');
		dson_t& operator[](const std::string& name);
		const dson_t& operator[](const std::string& name) const;
		bool has_content() const;
		bool has_children() const;
		void clear();
		bool empty();

	private:
		std::vector<std::string> m_vContent;
		std::vector<std::pair<std::string, dson_t>> m_vecObjects;
		std::unordered_map<std::string, size_t>       m_mapObjects;

	public:
		class iterator
		{
		public:
			using value_type = dson_t;
			using pointer_type = value_type*;
			using reference_type = value_type&;
			typedef std::ptrdiff_t difference_type;

			iterator() = default;
			iterator(dson_t& dfile, std::unordered_map<std::string, size_t>::iterator iter) : m_data(&dfile), m_iter(iter) {}

			std::string key() const { return this->m_data->m_vecObjects[this->m_iter->second].first; };
			bool operator == (const iterator& it) const { return (this->m_iter == it.m_iter); }
			bool operator != (const iterator& it) const { return !operator == (it); }
			iterator& operator ++() { ++this->m_iter; return *this; }
			iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
			iterator& operator --() { --this->m_iter; return *this; }
			iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }
			pointer_type operator->() { return &this->m_data->m_vecObjects[this->m_iter->second].second; }
			reference_type operator *() { return this->m_data->m_vecObjects[this->m_iter->second].second; }

		private:
			dson_t* m_data;
			std::unordered_map<std::string, size_t>::iterator m_iter;
		};

		class const_iterator
		{
		public:
			using value_type = dson_t;
			using pointer_type = const value_type*;
			using reference_type = const value_type&;
			typedef std::ptrdiff_t difference_type;

			const_iterator() = default;
			const_iterator(const dson_t& dfile, std::unordered_map<std::string, size_t>::const_iterator iter) : m_data(&dfile), m_iter(iter) {}

			std::string key() const { return this->m_data->m_vecObjects[this->m_iter->second].first; };
			bool operator == (const const_iterator& it) const { return (this->m_iter == it.m_iter); }
			bool operator != (const const_iterator& it) const { return !operator == (it); }
			const_iterator& operator ++() { ++this->m_iter; return *this; }
			const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
			const_iterator& operator --() { --this->m_iter; return *this; }
			const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }
			pointer_type operator->() { return &this->m_data->m_vecObjects[this->m_iter->second].second; }
			reference_type operator *() { return this->m_data->m_vecObjects[this->m_iter->second].second; }

		private:
			const dson_t* m_data;
			std::unordered_map<std::string, size_t>::const_iterator m_iter;
		};

		iterator begin();
		iterator end();
		const_iterator cbegin() const;
		const_iterator cend() const;

	protected:
		bool m_bIsComment = false;
	};
}

#endif // !_DATAFILE_DSON_H_