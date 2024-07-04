#ifndef _DLH_PARSER_H_
#define _DLH_PARSER_H_

#include "datafile/d_dson.h"
#include "datafile/d_string.h"
#include "datafile/d_data.h"
#include "datafile/d_object.h"
#include "datafile/d_memory.h"
#include "datafile.h"

namespace dlh
{
	namespace datafile
	{
		namespace parser
		{
			std::shared_ptr<datafile_t> parse(const std::string& filename, const char sListSep);
		}
	}
}

#endif // !_DLH_PARSER_H_
