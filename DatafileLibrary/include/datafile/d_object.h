#ifndef _DLH_DATAFILE_OBJECT_H_
#define _DLH_DATAFILE_OBJECT_H_

#include <map>
#include "datafile/d_data.h"

namespace dlh
{
	namespace datafile
	{
		namespace object
		{
			namespace type_t
			{
				enum
				{
					datafile = 0,
					bitmap,
					font,
					text,
					user_defined = 0x100
				};
			}

			using deleter_t = void (*)(void*);
			using parser_t = void* (*)(const data_t& data);

			typedef struct type_info_t
			{
				std::string name;
				parser_t parser = nullptr;
				deleter_t deleter = nullptr;
			} type_info_t;

			using map_info_t = std::map<int32_t, type_info_t>;

			extern map_info_t m_info;

			deleter_t get_deleter(int32_t type);
			int32_t get_type(const std::string& name);
			void register_type(int32_t type, const std::string& name, parser_t parser, deleter_t deleter);
			
			void* datafile_parser(const data_t& data);
			void* bitmap_parser(const data_t& data);
			void* font_parser(const data_t& data);
			void* text_parser(const data_t& data);

			void destroy_datafile(void* object);
			void destroy_text(void* object);
		}
	}
}

#endif // !_DLH_DATAFILE_DATA_H_
