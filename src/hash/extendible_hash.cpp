#include <list>

#include "hash/extendible_hash.h"
#include "page/page.h"

#define INITIAL_MAP_SIZE 2

namespace cmudb
{
  template <typename K, typename V>
  Pair<K, V>::Pair(K key, V val)
  {
    m_key = key;
    m_val = val;
  }

  template <typename K, typename V>
  Buffer<K, V>::Buffer(size_t size)
  {
    m_size = size;
    m_data = new Pair<K, V>*[size];
  }

  /*
* constructor
* array_size: fixed array size for each bucket
*/
  template <typename K, typename V>
  ExtendibleHash<K, V>::ExtendibleHash(size_t size)
  {
    m_directory.reserve(INITIAL_MAP_SIZE);
    for(int i=0;i<INITIAL_MAP_SIZE;i++)
    {
      m_directory[i] = new Buffer<K, V>(size);
    }
  }

  /*
* helper function to calculate the hashing address of input key
*/
  template <typename K, typename V>
  size_t ExtendibleHash<K, V>::HashKey(const K &key)
  {
    return (size_t) key;
  }

  /*
* helper function to return global depth of hash table
* NOTE: you must implement this function in order to pass test
*/
  template <typename K, typename V>
  int ExtendibleHash<K, V>::GetGlobalDepth() const
  {
    return 0;
  }

  /*
* helper function to return local depth of one specific bucket
* NOTE: you must implement this function in order to pass test
*/
  template <typename K, typename V>
  int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const
  {
    return 0;
  }

  /*
* helper function to return current number of bucket in hash table
*/
  template <typename K, typename V>
  int ExtendibleHash<K, V>::GetNumBuckets() const
  {
    return 0;
  }

  template <typename K, typename V>
  size_t ExtendibleHash<K, V>::GetBucketId(const K &key)
  {
    size_t h = HashKey(key);
    return h%m_directory.size();
  }

  /*
* lookup function to find value associate with input key
*/
  template <typename K, typename V>
  bool ExtendibleHash<K, V>::Find(const K &key, V &value)
  {
    value = m_directory[0]->m_data[0]->m_val;
    return true;
  }

  /*
* delete <key,value> entry in hash table
* Shrink & Combination is not required for this project
*/
  template <typename K, typename V>
  bool ExtendibleHash<K, V>::Remove(const K &key)
  {
    return false;
  }

  /*
* insert <key,value> entry in hash table
* Split & Redistribute bucket when there is overflow and if necessary increase
* global depth
*/
  template <typename K, typename V>
  void ExtendibleHash<K, V>::Insert(const K &key, const V &value)
  {
    m_directory[0]->m_data[0] = new Pair<K, V>(key, value);
    return;
  }

  template class ExtendibleHash<page_id_t, Page *>;
  template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
  // test purpose
  template class ExtendibleHash<int, std::string>;
  template class ExtendibleHash<int, std::list<int>::iterator>;
  template class ExtendibleHash<int, int>;
} // namespace cmudb
