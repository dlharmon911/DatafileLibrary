#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_physfs.h>
#include <physfs.h>
#include <string>
#include <vector>
#include "datafile/d_dson.h"
#include "datafile/d_path.h"
#include "datafile.h"

const std::string title_text = "Datafile Header Generator";
const std::string version_text = "0.0.1";

typedef struct data_t
{
	std::string input;
	std::string output;
} data_t;

int32_t process_args(const std::vector<std::string>& args, data_t& data);
void version();
void usage();

int32_t init(const std::vector<std::string>& args, data_t& data)
{
	if (!al_init())
	{
		return -1;
	}

	if (!PHYSFS_init(args[0].c_str()))
	{
		return -1;
	}

	if (process_args(args, data) < 0)
	{
		return -1;
	}

	return 0;
}

void kill()
{
	PHYSFS_deinit();
}

int32_t main(int32_t argc, char** argv)
{
	int32_t rv = 0;

	if (argc == 1)
	{
		usage();
		rv = -1;
	}
	else
	{
		data_t data;

		std::vector<std::string> args;
		for (int32_t i = 1; i < argc; ++i)
		{
			args.push_back(argv[i]);
		}

		if (init(args, data) < 0)
		{
			rv = -1;
		}
		else
		{
			if (data.output.empty())
			{
				data.output = "output.h";
			}

			std::cout << "Generating header file:\n";
			std::cout << "\tInput: " << data.input << "\n";
			std::cout << "\tOutput: " << data.output << std::endl;

			if (!al_generate_header_file(data.input.c_str(), data.output.c_str()))
			{
				std::cout << "Error" << std::endl;
				rv = -1;
			}

			kill();
		}
	}

	return 0;
}

void usage()
{
	std::cout << title_text << std::endl;
	std::cout << "Usage <input_file> <options>: " << std::endl;
	std::cout << "\t-version " << std::endl;
	std::cout << "\t-o %output_file " << std::endl << std::endl;
}

void version()
{
	std::cout << title_text << std::endl;
	std::cout << "\tversion = " << version_text << std::endl << std::endl;
}

int32_t process_args(const std::vector<std::string>& args, data_t& data)
{
	for (auto a = args.cbegin(); a != args.cend(); ++a)
	{
		if ((*a)[0] == '-')
		{
			if (a->length() == 1)
			{
				return -1;
			}
			else
			{
				std::string command = a->substr(1, a->size() - 1);

				if (command == "version")
				{
					version();
					continue;
				}

				++a;
				if (a == args.cend())
				{
					return -1;
				}
				else
				{
					std::string argument = *a;

					if (command == "o")
					{
						data.output = argument;
					}
				}
			}
		}
		else
		{
			if (data.input.empty())
			{
				data.input = *a;
			}
			else
			{
				return -1;
			}
		}
	}

	return 0;
}