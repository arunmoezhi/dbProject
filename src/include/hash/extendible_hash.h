/*
* extendible_hash.h : implementation of in-memory hash table using extendible
* hashing
*
* Functionality: The Bucket pool manager must maintain a page table to be able
* to quickly map a PageId to its corresponding memory location; or alternately
* report that the PageId does not match any currently-Bucketed page.
*/

#pragma once

#include <cstdlib>
#include <vector>
#include <string>

#include "hash/hash_table.h"

namespace cmudb
{
  template <typename K, typename V> class ExtendibleHash;

  template <typename K, typename V>
  class Pair
  {
    friend class ExtendibleHash<K, V>;
    public:
      // constructor
      Pair(K key, V val);

    private:
    K m_key;
    V m_val;
  };

  template <typename K, typename V>
  class Bucket
  {
    friend class ExtendibleHash<K, V>;
    public:
      // constructor
      Bucket(size_t size, size_t localDepth);

    private:
    size_t m_size;
    Pair<K, V>** m_data;
    size_t m_localDepth;
  };

  template <typename K, typename V>
  class ExtendibleHash : public HashTable<K, V>
  {
    public:
      // constructor
      ExtendibleHash(size_t size);
      // helper function to generate hash addressing
      size_t HashKey(const K &key);
      size_t GetBucketId(const K& key);
      // helper function to get global & local depth
      int GetGlobalDepth() const;
      int GetLocalDepth(int bucket_id) const;
      int GetNumBuckets() const;
      // lookup and modifier
      bool Find(const K &key, V &value) override;
      bool Remove(const K &key) override;
      void Insert(const K &key, const V &value) override;

    private:
      void doubleDirectorySize();
      std::vector<Bucket<K, V>*> m_directory;
      size_t m_globalDepth;
      size_t m_numOfEntriesInABucket;
  };
} // namespace cmudb
