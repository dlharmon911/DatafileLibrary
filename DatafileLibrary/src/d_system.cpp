#include <ctime>
#include "datafile/d_system.h"

namespace dlh
{
	namespace system
	{
		std::string timestamp()
		{
			time_t t;
			struct tm local;
			char buffer[1024];

			time(&t);

#ifdef _MSC_VER
			localtime_s(&local, &t);

			sprintf_s<1024>(buffer,

#else
			local = localtime(&t);

			sprintf(buffer,
#endif

				"%2d:%02d:%02d %s %2d/%02d/%4d",
				(local.tm_hour % 12 == 0 ? 12 : local.tm_hour % 12),
				local.tm_min,
				local.tm_sec,
				(local.tm_hour > 11 ? "p.m." : "a.m."),
				local.tm_mon + 1,
				local.tm_mday,
				1900 + local.tm_year);

			return buffer;
		}
	}
}

