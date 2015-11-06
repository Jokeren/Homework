#ifndef SMM_GRAPH_H
#define SMM_GRAPH_H
#include <vector>
#include <map>
#include <queue>
#include <deque>
#include "abstract.h"

namespace smm {
	struct edge_base_t {
	};

	struct vertex_base_t {
	};

	template <typename T> 
		class Graph : public Abstract {
			public:
				typedef std::size_t size_type;


			private:
				struct edge_t : public edge_base_t {
					T to_label;	
					size_type edge_id;
					size_type from_id;
					size_type to_id;
					double weight;

					edge_t (T _to_label, size_type _edge_id, size_type _from_id, size_type _to_id) : 
						to_label(_to_label), edge_id(_edge_id), from_id(_from_id), to_id(_to_id) {}

					edge_t () {};
				};

				struct vertex_t : public vertex_base_t {
					T label;
					size_type vertex_id;
					std::vector<struct edge_t*> edges;

					vertex_t (size_type _vertex_id, T _label) : 
						vertex_id(_vertex_id), label(_label) {}

					vertex_t () {};
				};

			public:
				typedef typename std::vector<typename Graph<T>::vertex_t>::iterator vertex_iterator;
				typedef typename std::deque<typename Graph<T>::edge_t>::iterator edge_iterator;
				typedef typename std::vector<typename Graph<T>::edge_t*>::iterator vertex_edge_iterator;

			private:
				void transfer(const Graph<T>& other);

			private:
				//the map functions can be implemented by hash_map in c++11. 
				//id start from 0
				bool _m_is_directed;
				bool _m_is_weighted;
				//TODO: hashmap implement
				//for weighted graph, not implemented yet
				std::map<T, size_type> _m_edge_to_id;
				std::map<size_type, T> _m_id_to_edge;
				std::map<T, size_type> _m_vertex_to_id;
				std::map<size_type, T> _m_id_to_vertex;
				//deque: not move memory
				std::deque<struct edge_t> _m_edges;
				std::vector<struct vertex_t> _m_vertice;

			public:
				Graph<T>() : _m_is_directed(false), _m_is_weighted(false) {}

				Graph<T>(bool is_directed, bool is_weighted) : 
					_m_is_directed(is_directed), _m_is_weighted(is_weighted) {}

				Graph<T>(size_type size, bool is_directed, bool is_weighted) : 
					_m_vertice(size), _m_is_directed(is_directed), _m_is_weighted(is_weighted) {}

				//not explicit !!!
				Graph<T>(const Graph<T>& other): _m_vertice(other._m_vertice), 
				_m_is_directed(other._m_is_directed),
				_m_is_weighted(other._m_is_weighted),
				_m_edge_to_id(other._m_edge_to_id),
				_m_id_to_edge(other._m_id_to_edge),
				_m_vertex_to_id(other._m_vertex_to_id),
				_m_id_to_vertex(other._m_id_to_vertex) {}

				Graph<T>& operator = (const Graph<T>& other);

				~Graph<T>()
				{
				}

				//clear values, not for memory
				void clear();

				//reallocating memory
				void reallocate();

				//utilities
				//TODO: const_iterator
				vertex_iterator vertex_begin()
				{
					return _m_vertice.begin();
				}

				vertex_iterator vertex_end()
				{
					return _m_vertice.end();
				}

				edge_iterator edge_begin()
				{
					return _m_edges.begin();
				}

				edge_iterator edge_end()
				{
					return _m_edges.end();
				}

				vertex_edge_iterator vertex_edge_begin(size_type id)
				{
					return _m_vertice[id].edges.begin();
				}

				vertex_edge_iterator vertex_edge_end(size_type id)
				{
					return _m_vertice[id].edges.end();
				}

				size_type size() const
				{
					return _m_vertice.size();
				}

				size_type get_vertex_id(const T& vertex)
				{
					return _m_vertex_to_id[vertex];
				}

				const T& get_vertex_name(size_type id)
				{
					return _m_id_to_vertex[id];
				}

				bool is_weighted()
				{
					return _m_is_weighted;
				}

				inline void add_edge(const T& from, const T& to);

		};

	template <class T>
		void Graph<T>::transfer(const Graph<T>& other)
		{
			_m_is_directed = other._m_is_directed;
			_m_is_weighted = other._m_is_weighted;
			_m_edge_to_id = other._m_edge_to_id;
			_m_id_to_edge = other._m_id_to_edge;
			_m_vertex_to_id = other._m_vertex_to_id;
			_m_id_to_vertex = other._m_id_to_vertex;
			_m_edges = other._m_edges;
			_m_vertice = other._m_vertice;
		}

	template <class T>
		Graph<T>& Graph<T>::operator = (const Graph<T>& other)
		{
			transfer(other);
			return *this;
		}

	template <class T>
		void Graph<T>::clear()
		{
			_m_is_weighted = false;
			_m_is_directed = false;	
			_m_edge_to_id.clear();
			_m_id_to_edge.clear();
			_m_vertex_to_id.clear();
			_m_id_to_vertex.clear();
			_m_edges.clear();
			_m_vertice.clear();
		}

	template <class T>
		void Graph<T>::reallocate()
		{
			_m_is_weighted = false;
			_m_is_directed = false;	
			std::map<T, size_type>().swap(_m_vertex_to_id);
			std::map<T, size_type>().swap(_m_edge_to_id);
			std::map<size_type, T>().swap(_m_id_to_vertex);
			std::map<size_type, T>().swap(_m_id_to_edge);
			std::deque<struct edge_t>().swap(_m_edges);
			std::vector<struct vertex_t>().swap(_m_vertice);
		}

	template <class T>
		inline void Graph<T>::add_edge(const T& from, const T& to)
		{
			size_type from_id, to_id;

			if (_m_vertex_to_id.find(from) == _m_vertex_to_id.end()) {
				_m_vertice.push_back(vertex_t(_m_vertice.size(), from));
				_m_vertex_to_id[from] = _m_vertice.size() - 1;
				_m_id_to_vertex[_m_vertice.size() - 1] = from;
			}

			from_id = _m_vertex_to_id[from];

			if (_m_vertex_to_id.find(to) == _m_vertex_to_id.end()) {
				_m_vertice.push_back(vertex_t(_m_vertice.size(), to));
				_m_vertex_to_id[to] = _m_vertice.size() - 1;
				_m_id_to_vertex[_m_vertice.size() - 1] = to;
			}

			to_id = _m_vertex_to_id[to];

			_m_edges.push_back(edge_t(to, _m_edges.size(), from_id, to_id));
			_m_vertice[from_id].edges.push_back(&(_m_edges[_m_edges.size() - 1]));

			if (!_m_is_directed) {
				_m_edges.push_back(edge_t(from, _m_edges.size(), to_id, from_id));
				_m_vertice[to_id].edges.push_back(&(_m_edges[_m_edges.size() - 1]));
			}
		}
}//namespace msd

#endif
