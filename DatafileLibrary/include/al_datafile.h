#ifndef _ALLEGRO_DATAFILE_H_
#define _ALLEGRO_DATAFILE_H_

#include <allegro5/allegro5.h>
#include <memory>

typedef struct ALLEGRO_DATAFILE
{
	std::shared_ptr<void> data;
	int32_t type;
} ALLEGRO_DATAFILE;

bool al_generate_header_file(const char* manifest_filename, const char* header_filename, const char sListSep = ',');
std::shared_ptr<ALLEGRO_DATAFILE> al_load_datafile(const char* filename, const char sListSep = ',');

#endif  // !_ALLEGRO_DATAFILE_H_