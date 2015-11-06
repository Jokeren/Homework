#ifndef SMM_READER_H
#define SMM_READER_H

#include <string>
#include "abstract.h"

namespace smm {
	template <typename T>
		class Reader
		{
			public:
				Reader<T>() : _m_delim('\n'), _m_file_read(false) {};

				Reader<T>(const std::string& file_name, char delim, bool file_read) : 
					_m_file_name(file_name), _m_delim(delim), _m_file_read(false) {};

				virtual const Abstract& get_instance() = 0;

				virtual void open(const std::string& file_name) = 0;

				virtual void open(const char* file_name) = 0;

			protected:
				virtual bool read_file() = 0;

			protected:
				bool _m_file_read;
				std::string _m_file_name;
				char _m_delim;
		};
}//msd

#endif 
