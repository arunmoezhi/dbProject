#include <list>

#include "hash/extendible_hash.h"
#include "page/page.h"
#include <assert.h>

#define INITIAL_GLOBAL_DEPTH 2

namespace cmudb
{
  template <typename K, typename V>
  Pair<K, V>::Pair(K key, V val)
  {
    m_key = key;
    m_val = val;
  }

  template <typename K, typename V>
  Bucket<K, V>::Bucket(size_t size, size_t localDepth)
  {
    m_size = size;
    m_localDepth = localDepth;
    m_data = new Pair<K, V>*[size];
    for(size_t i=0;i<size;i++)
    {
      m_data[i] = NULL;
    }
  }

  /*
* constructor
* array_size: fixed array size for each bucket
*/
  template <typename K, typename V>
  ExtendibleHash<K, V>::ExtendibleHash(size_t size)
  {
    m_globalDepth = INITIAL_GLOBAL_DEPTH;
    m_numOfEntriesInABucket = size;
    size_t numOfBuckets = 1<<m_globalDepth;
    m_directory.reserve(numOfBuckets);
    for(size_t i=0;i<numOfBuckets;i++)
    {
      m_directory.push_back(new Bucket<K, V>(m_numOfEntriesInABucket, m_globalDepth));
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
    return m_globalDepth;
  }

  /*
* helper function to return local depth of one specific bucket
* NOTE: you must implement this function in order to pass test
*/
  template <typename K, typename V>
  int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const
  {
    return m_directory[bucket_id]->m_localDepth;
  }

  /*
* helper function to return current number of bucket in hash table
*/
  template <typename K, typename V>
  int ExtendibleHash<K, V>::GetNumBuckets() const
  {
    return (1<<m_globalDepth);
  }

  template <typename K, typename V>
  size_t ExtendibleHash<K, V>::GetBucketId(const K &key)
  {
    size_t h = HashKey(key);
    return h & ((1<<m_globalDepth)-1);
  }

  /*
* lookup function to find value associate with input key
*/
  template <typename K, typename V>
  bool ExtendibleHash<K, V>::Find(const K &key, V &value)
  {
    const auto bucketId = GetBucketId(key);
    const auto& bucket = m_directory[bucketId];
    for(size_t i=0;i<m_numOfEntriesInABucket;i++)
    {
      if(bucket->m_data[i] && key == bucket->m_data[i]->m_key)
      {
        value = bucket->m_data[i]->m_val;
        return true;
      }
    }
    return false;
  }

  /*
* delete <key,value> entry in hash table
* Shrink & Combination is not required for this project
*/
  template <typename K, typename V>
  bool ExtendibleHash<K, V>::Remove(const K &key)
  {
    const auto bucketId = GetBucketId(key);
    const auto& bucket = m_directory[bucketId];
    for(size_t i=0;i<m_numOfEntriesInABucket;i++)
    {
      if(bucket->m_data[i] && key == bucket->m_data[i]->m_key)
      {
        bucket->m_data[i] = NULL;
        return true;
      }
    }
    return false;
  }
  
  template <typename K, typename V>
  void ExtendibleHash<K, V>::doubleDirectorySize()
  {
    size_t l = 1<<m_globalDepth;
    m_directory.reserve(2*l);
    for(size_t i=0;i<l;i++)
    {
      // copy (mirror image) the bucket pointers
      //m_directory[l+i] = m_directory[i];
      m_directory.push_back(m_directory[i]);
    }
    m_globalDepth++;
    return;
  }
  
  /*
* insert <key,value> entry in hash table
* Split & Redistribute bucket when there is overflow and if necessary increase
* global depth
*/
  template <typename K, typename V>
  void ExtendibleHash<K, V>::Insert(const K &key, const V &value)
  {
    const auto bucketId = GetBucketId(key);
    Bucket<K, V>* bucket = m_directory[bucketId];
    assert(bucket != NULL);
    int firstEmptySpot=-1;
    for(int i=0;i<(int)m_numOfEntriesInABucket;i++)
    {
      if(bucket->m_data[i] == NULL)
      {
        if(firstEmptySpot == -1)
        {
          firstEmptySpot = i;
        }
      }
      else if (key == bucket->m_data[i]->m_key)
      {
        // found a duplicate key. Overwrite the value and return
        bucket->m_data[i]->m_val = value;
        return;
      }
    }
    if(firstEmptySpot == -1)
    {
      // bucket is full
      size_t localDepth = bucket->m_localDepth;
      if(m_globalDepth == localDepth)
      {
        doubleDirectorySize(); // Note: This would increase the global depth by 1
      }
      
      // At this point the local depth is less than the global depth.
      // Rehash the keys so that some keys might go to the mirror bucket.
      
      
      /*
      size_t oldBucketId;
      size_t newBucketId;
      if(bucketId == (bucketId & ((1<<localDepth)-1))) // eg 0xx & 011 => 0xx for localDepth=2
      {
        oldBucketId = bucketId;
        newBucketId = bucketId | (1<<localDepth); // eg 0xx | 100 => 1xx for localDepth=2
      }
      else
      {
        newBucketId = bucketId;
        oldBucketId = bucketId & ((1<<localDepth)-1); // eg 1xx & 011 => 0xx for localDepth=2
      }
      */
      
      size_t oldBucketId = bucketId  & ((1<<localDepth)-1); // eg aaa & 011 => 0aa
      size_t newBucketId = bucketId | (1<<localDepth); // eg aaa | 100 => 1aa
      
      // Create a new bucket with the updated localDepth
      Bucket<K, V>* newBucket = new Bucket<K, V>(m_numOfEntriesInABucket, (localDepth+1));
      
      // update the relevant bucket pointers in the directory to point to the new bucket
      for(int i=0;i<(1<<m_globalDepth);i++)
      {
        if(m_directory[i] == bucket && (i != (i & ((1<<localDepth)-1))))
        {
          m_directory[i] = newBucket;
        }
      }
      
      // increment the localDepth of the old bucket
      bucket = m_directory[oldBucketId];
      bucket->m_localDepth = bucket->m_localDepth + 1;
      
      
      // Rehash all the keys in the current bucket so it might go into the new bucket
      size_t index=0;
      for(size_t i=0;i<m_numOfEntriesInABucket;i++)
      {
        const auto bId = GetBucketId(bucket->m_data[i]->m_key);
        if(bId == newBucketId)
        {
          newBucket->m_data[index++] = bucket->m_data[i];
          bucket->m_data[i] = NULL;
        }
      }
      Insert(key, value);
      return;
    }
    // At this point we know that the key does not exist in the map and a free slot is available
    bucket->m_data[firstEmptySpot] = new Pair<K, V>(key, value);
    return;
  }

  template class ExtendibleHash<page_id_t, Page *>;
  template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
  // test purpose
  template class ExtendibleHash<int, std::string>;
  template class ExtendibleHash<int, std::list<int>::iterator>;
  template class ExtendibleHash<int, int>;
} // namespace cmudb
