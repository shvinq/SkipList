#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cmath>
#include <mutex>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>

#define STORE_FILE "store/dumpFile"

std::mutex mtx;
std::string delimiter = ":";


template< typename K, typename V >
class Node
{
    public:

        Node() {};
        Node(K k, V v, int);
        ~Node();
        K get_key() const;
        V get_value() const;
        void set_value(V);

        Node< K, V > **forward;
        int node_level;
    
    private:
        K key;
        V value;
};


template < typename K, typename V > 
class SkipList
{
    public:
        SkipList(int);
        ~SkipList();
        int get_random_level();
        Node< K, V >* create_node(K, V, int);
        int insert_element(K, V);
        void display();
        bool search_element(K);
        void delete_element(K);
        void dump_file();
        void load_file();
        int size();
    
    private:
        void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
        bool is_valid(const std::string& str);

};


/******************************Node类实现****************************/

template< typename K, typename V >
Node< K, V>::Node(const K k, const V v, int level) : key(k), value(v), node_level(level)
{
    this->forward = new Node< K, V >*[level + 1];
    memset(this->forward, 0, sizeof(Node< K, V >*) * (level + 1));
}

template< typename K, typename V >
Node< K, V >::~Node()
{
    delete [] forward;
}

template< typename K, typename V >
inline K Node< K, V >::get_key() const
{
    return key;
}

template< typename K, typename V >
inline V Node< K, V >::get_value() const
{
    return value;
}

template< typename K, typename V >
void Node< K, V >::set_value(V value)
{
    this->value = value;
}

/*********************************SkipList类实现************************************/
template< typename K, typename V >



#endif