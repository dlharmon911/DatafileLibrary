#ifndef _DLH_DATAFILE_OBJECT_H_
#define _DLH_DATAFILE_OBJECT_H_

#include <map>
#include "datafile/d_data.h"
#include "datafile/d_memory.h"
#include "datafile/d_base.h"

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

			using parser_func_t = value_t(*)(const data_t& data);

			int32_t get_type(const std::string& name);
			void register_type(int32_t type, const std::string& name, parser_func_t parser);
			
			value_t datafile_parser(const data_t& data);
			value_t bitmap_parser(const data_t& data);
			value_t font_parser(const data_t& data);
			value_t text_parser(const data_t& data);
		}
	}
}

#endif // !_DLH_DATAFILE_DATA_H_
