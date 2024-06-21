#include "datafile/d_data.h"
#include "datafile/d_config.h"

namespace dlh
{
	namespace datafile
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
}