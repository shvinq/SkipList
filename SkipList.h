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


/******************************Node类定义及其实现****************************/

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

        Node< K, V >* *forward;         //forward数组存放着每一层的下一个节点指针。比如节点1有两层索引，就有forward[0],forward[1],forward[2],forward[1]后存放这该层下一个节点的指针比如是节点3的指针
        int node_level;
    
    private:
        K key;
        V value;
};



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



/*********************************SkipList类定义及其实现************************************/

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
        bool is_valid_string(const std::string& str);

    private:
        int m_max_level;
        int m_skip_list_level;
        Node< K, V > *m_header;

        std::ofstream m_file_writer;
        std::ifstream m_file_reader;

        int m_element_count;

};


template< typename K, typename V >
SkipList< K, V >::SkipList(int max_level)
{
    this->m_max_level = max_level;
    this->m_skip_list_level = 0;
    this->m_element_count = 0;

    K k;
    V v;
    this->m_header = new Node< K, V >(k, v, m_max_level);
}

template< typename K, typename V >
SkipList< K, V >::~SkipList()
{
    if(m_file_writer.is_open()) m_file_writer.close();
    if(m_file_reader.is_open()) m_file_reader.close();
    delete m_header;
}

template< typename K, typename V >
int SkipList< K, V >::get_random_level()
{
    int k = 1;
    while(rand() % 2) k++;
    k = (k < m_max_level) ? k : m_max_level;
    return k;
}

template< typename K, typename V >
Node< K, V >* SkipList< K, V >::create_node(const K k, const V v, int level)
{
    Node< K, V > *n = new Node< K, V >(k, v, level);
    return v;
}

template< typename K, typename V >
int SkipList< K, V >::insert_element(const K key, const V value)
{
    mtx.lock();
    Node< K, V > *current = this->m_header;

    Node< K , V > *update[m_max_level + 1];
    memset(update, 0, sizeof(Node< K, V >*) * (m_max_level + 1));

    for(int i = m_skip_list_level; i >= 0; i--)
    {
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key)
            current = current->forward[i];
        update[i] = current;
    }

    current = current->forward[0];
    if(current != NULL && current->get_key() == key)
    {
        std::cout << "key: " << key << ", exits" << std::endl;
        mtx.unlock();
        return 1;
    }

    if(current == NULL || current->get_key() != key)
    {
        int random_level = get_random_level();
        if(random_level > m_skip_list_level)
        {
            for(int i = m_skip_list_level + 1; i < random_level; i++) update[i] = m_header;
            m_skip_list_level = random_level;
        }

        Node< K, V >* inserted_node = create_node(key, value, random_level);
        for(int i = 0; i <= random_level; i++)
        {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key: " << key << ", value: " << value << std::endl;
        m_element_count++;
    }
    mtx.unlock();
    return 0;
}

template< typename K, typename V >
void SkipList< K, V >::display()
{
    std::cout << "\n******Skip List*****" << "\n";
    for(int i = 0; i <= m_skip_list_level; i++)
    {
        Node< K, V > *node = this->m_header->forward[i];
        std::cout << "Level: " << i << ":";
        while(node)
        {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template< typename K, typename V >
bool SkipList< K, V >::search_element(K key)
{
    std::cout << "search_element-----------------" << std::endl;
    Node< K, V >* current = this->m_header;
    for(int i = m_skip_list_level; i >= 0; i--)
    {
        while(current != NULL && current->forward[i]->get_key() < key) current = current->forward[i];
    }

    current = current->forward[0];
    if(current != NULL && current->get_key() == key)
    {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }
    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

template< typename K, typename V >
void  SkipList< K, V >::delete_element(K key)
{
    mtx.lock();
    Node< K, V >* current = this->m_header;
    Node< K, V >* update[m_max_level + 1];
    memset(update, 0, sizeof(Node< K, V >*) * (m_max_level + 1));

    for(int i = m_skip_list_level; i >= 0; i--)
    {
        while(current != NULL && current->forward[i]->get_key() < key) current = current->forward[i];
        update[i] = current;
    }

    current = current->forward[0];
    if(current != NULL && current->get_key() == key)
    {
        for(int i = 0; i <= m_skip_list_level; i++)
        {
            if(update[i]->forward[i] != current) break;
            update[i]->forward[i] = current->forward[i];
        }
        while(m_skip_list_level > 0 && m_header[m_skip_list_level] == 0) m_skip_list_level--;
        std::cout << "Successfully deleted key " << key << std::endl;
        m_element_count--;
    }
    mtx.unlock();
    return;
}

template< typename K, typename V >
void SkipList< K, V >::dump_file()
{
    std::cout << "dump_file-----------------" << std::endl;
    m_file_writer.open(STORE_FILE);
    Node< K, V > *node = this->m_header->forward[0];

    while(node)
    {
        m_file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }
    m_file_writer.flush();
    m_file_writer.close();
}

template< typename K, typename V >
void SkipList< K, V >::load_file()
{
    m_file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(*key, *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    m_file_reader.close();
}

template< typename K, typename V >
int SkipList< K, V >::size()
{
    return m_element_count;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value)
{
    if(!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str)
{

    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}
#endif