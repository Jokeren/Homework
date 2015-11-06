#ifndef SMM_GRAPH_READER_H
#define SMM_GRAPH_READER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "reader.h"
#include "graph.h"

namespace smm {
	/*
	 * unwighted graph reader for both directed and undirected graphs
	 * using a partial specialization
	 */
	template <typename T>
		class GraphReader : private Reader<T>
	{
		public:
			GraphReader<T>() : _m_graph(false, false) {};

			GraphReader<T>(const std::string& file_name, 
					char delim, bool is_weighted = false): 
				Reader<T>(file_name, delim, false), _m_graph(false, is_weighted) {};

			virtual const Graph<T>& get_instance();

			virtual void open(const std::string& file_name);

			virtual void open(const char* file_name);

		protected:
			virtual bool read_file();

		protected:
			Graph<T> _m_graph;
	};

	template <typename T>
		const Graph<T>& GraphReader<T>::get_instance()
		{
			if (this->_m_file_read)
				return _m_graph;
			else {
				if (read_file())
					this->_m_file_read = true;
				return _m_graph;
			}
		}

	template <typename T>
		void GraphReader<T>::open(const std::string& file_name) 
		{
			this->_m_file_read = false;
			_m_graph.reallocate();

			this->_m_file_name = file_name;
		}

	template <typename T>
		void GraphReader<T>::open(const char* file_name) 
		{
			this->_m_file_read = false;
			_m_graph.reallocate();

			this->_m_file_name = std::string(file_name);
		}

	template <typename T>
		bool GraphReader<T>::read_file()
		{
			std::ifstream file;
			file.open(this->_m_file_name.c_str());

			if (file.is_open()) {
				std::string buffer;
				T from, to;

				while (std::getline(file, buffer, this->_m_delim)) {
					if (buffer.length() <= 0)
						continue;

					std::stringstream ss(buffer);

					ss >> from >> to;
					_m_graph.add_edge(from, to);
				}
			} else {
				std::cerr << "open file " << this->_m_file_name 
					<< " error !" << std::endl;
				return false;
			}

			file.close();

			return true;
		}


	/*
	 * wighted graph reader for both directed and undirected graphs
	 */
	template <typename T>
		class WeightedReader : public GraphReader<T>
	{
		public:
			WeightedReader<T>() {};

			WeightedReader<T>(const std::string& file_name, char delim): 
				Reader<T>(file_name, delim) {};

		protected:
			virtual void read_file();
	};

	//under implementing
	template <typename T>
		void WeightedReader<T>::read_file()
		{
			std::ifstream file;
			file.open(this->_m_file_name);

			if (file.is_open()) {

			} else {
				std::cerr << "open file " << this->_m_file_name 
					<< " error !" << std::endl;
				exit(1);
			}

			file.close();
		}
}//msd

#endif
