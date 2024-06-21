#ifndef _ALLEGRO_DATAFILE_H_
#define _ALLEGRO_DATAFILE_H_

#include <allegro5/allegro5.h>

typedef struct ALLEGRO_DATAFILE
{
	void* data;
	int32_t type;
} ALLEGRO_DATAFILE;

bool al_generate_header_file(const char* manifest_filename, const char* header_filename, const char sListSep = ',');
ALLEGRO_DATAFILE* al_load_datafile(const char* filename, const char sListSep = ',');
void al_destroy_datafile(ALLEGRO_DATAFILE* datafile);

#endif  // !_ALLEGRO_DATAFILE_H_