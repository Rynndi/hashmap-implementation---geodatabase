////
////  HashMap.h
////  Di Project 4
////
////  Created by Catherine Di on 3/8/24.
////
//
#ifndef HashMap_h
#define HashMap_h

#include <iostream>
using namespace std;
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <cassert>
template <typename T> class HashMap
{
public:
    HashMap(double max_load = 0.75); // constructor
    ~HashMap();
    int size() const;

    void insert(const std::string& key, const T& value);

    T& operator[](const std::string& key);

    const T* find(const std::string& key) const;

    T* find(const std::string& key) {
    const auto& hm = *this;
    return const_cast<T*>(hm.find(key));

    }
    
    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;

private:
    struct Node{
        string key;
        T value;
    };
    int m_size = 0;
    int max_bucket;
    double m_load;
    
    class Bucket{
    public:
        std::list<Node*> bucketList;
    };
    //make a vector of lists of Node pointers
    vector<Bucket> m_buckets;
};

template<typename T>
HashMap<T>::HashMap(double max_load){
    max_bucket = 10;
    m_load = max_load;
    for(int i=0; i < max_bucket; i++) {
        m_buckets.push_back(Bucket());
    }
}

template<typename T>
HashMap<T>::~HashMap() {
    // delete all the nodes
    for(int i=0; i < m_buckets.size(); i++) {
        while(!m_buckets[i].bucketList.empty()) {
            delete m_buckets[i].bucketList.back();
            //delete all the items on the node
            m_buckets[i].bucketList.pop_back();
        }
    }
}

template<typename T>
int HashMap<T>::size() const {
    return m_size;
}

template<typename T>
void HashMap<T>::insert(const std::string& key, const T& value)
{
    //see if adding one would take over the load factor
    double addOne = m_size + 1.0;
    if ((addOne) / max_bucket > m_load){
        // resize the vector
        max_bucket = max_bucket * 2;
        std::vector<Bucket> new_vector(max_bucket);

        //rehash
        for(int i=0; i < m_buckets.size(); i++) {
            typename std::list<Node*>::iterator it2 = m_buckets[i].bucketList.begin();
            if(!m_buckets[i].bucketList.empty()) {
                while(it2 != m_buckets[i].bucketList.end()){
                    string tKey = (*it2)->key;
                    T tVal = (*it2)->value;
                    unsigned long h = std::hash<std::string>()(tKey); // hashes string
                    int bucketNum = h % max_bucket;
                    //reindex hash function
                    Node* nNode = new Node();
                    nNode->key = tKey;
                    nNode->value = tVal;
                    new_vector[bucketNum].bucketList.push_back(nNode);
                    it2++;
                }
            }
        }
        m_buckets = new_vector;
    }
    
        unsigned long h = std::hash<std::string>()(key); // hashes string
        int bucketNum = h % max_bucket;
        
        // search for the key in the bucket list
        if(m_buckets[bucketNum].bucketList.empty()) {
            Node* newNode = new Node;
            newNode->key = key;
            newNode->value = value;
            m_buckets[bucketNum].bucketList.push_back(newNode);
            m_size++;
            return;
        }
        
        typename std::list<Node*>::iterator it = m_buckets[bucketNum].bucketList.begin();
        
        while(it != m_buckets[bucketNum].bucketList.end()){
            if ((*it)->key == key){
                // key already exists, replace the value
                (*it)->value = value;
            }
            else{
                // key does not exist, add a new node to the list
                Node* newNode = new Node;
                newNode->key = key;
                newNode->value = value;
                m_buckets[bucketNum].bucketList.push_back(newNode);
                m_size++;
            }
            it++;
        }
}

template<typename T>
const T* HashMap<T>::find(const std::string& key) const{
   
    //hash the key,
    unsigned long h = std::hash<std::string>()(key); //hashes string
    int bucketNum = h % max_bucket;
    //search for the bucket
    
    //if that bucket's bucketlist is empty return nullptr
    if(m_buckets[bucketNum].bucketList.empty())
        return nullptr;
    else{
        typename std::list<Node*>::const_iterator it = m_buckets[bucketNum].bucketList.begin();
        while(it != m_buckets[bucketNum].bucketList.end()){
            //iterate through the list until you find the value
            if((*it)->key == key){
                T* val = &((*it)->value);
                return val;
            }
            it++;
        }
    }
    return nullptr;
}

  // Defines the bracket operator for HashMap, so you can use your map like this: // your_map["david"] = 2.99;
  // If the key does not exist in the hashmap, this will create a new entry in the hashmap and map it to the default value of type T. Then it will return a reference to the newly created value in the map.
template <typename T>
T& HashMap<T>::operator[](const std::string& key){
      //key exists in the map
      if (find(key) == nullptr){
          insert(key, T());
      }
      return *find(key);
}

#endif // __STOPS_H__
