#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <functional>
#include <vector>
#include <list>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using bucket_list = std::list<value_type>;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  std::vector<bucket_list> tableOfBuckets;
  size_type numberOfBuckets;
  size_type elementsInMap;
  std::hash<key_type> GetNumberFromKey;
  const double MAX_LOAD_FACTOR = 1.25; 
  const size_type INITIAL_NUM_OF_BUCKETS = 32;

  size_type getIndexFromKey(const key_type& key) const
  {
    return GetNumberFromKey(key) % numberOfBuckets;
  }

  void rehash()
  {
    size_type oldNumberOfBuckets = numberOfBuckets;
    numberOfBuckets *= 2;
    std::vector<bucket_list> newTableOfBuckets(numberOfBuckets);
    for(size_type i = 0; i < oldNumberOfBuckets; i++)
    {
      if(tableOfBuckets[i].size() != 0)
      {
        for(auto& item : tableOfBuckets[i])
          newTableOfBuckets[getIndexFromKey(item.first)].push_back(item);
      } 
    }
    tableOfBuckets = std::move(newTableOfBuckets);
  }

public:
  HashMap()
  {
    numberOfBuckets = INITIAL_NUM_OF_BUCKETS;
    elementsInMap = 0;
    tableOfBuckets.resize(numberOfBuckets);
  }

  HashMap(std::initializer_list<value_type> list)
  {
    elementsInMap = 0;
    numberOfBuckets = INITIAL_NUM_OF_BUCKETS;
    while(list.size() > numberOfBuckets)
      numberOfBuckets = 2 * numberOfBuckets;
    tableOfBuckets.resize(numberOfBuckets);
    for(auto& item : list)
    {
      tableOfBuckets[getIndexFromKey(item.first)].push_back(item);
      ++elementsInMap;
    }
  }

  HashMap(const HashMap& other)
  {
    numberOfBuckets = INITIAL_NUM_OF_BUCKETS;
    elementsInMap = 0;
    tableOfBuckets.resize(numberOfBuckets);
    for(auto iter = other.begin(); iter != other.end(); ++iter)
      this->operator[](iter->first) = iter->second;
  }

  HashMap(HashMap&& other)
  {
    numberOfBuckets = INITIAL_NUM_OF_BUCKETS;
    elementsInMap = 0;
    tableOfBuckets.resize(numberOfBuckets);
    std::swap(numberOfBuckets, other.numberOfBuckets);
    std::swap(elementsInMap, other.elementsInMap);
    std::swap(tableOfBuckets, other.tableOfBuckets);
  }

  HashMap& operator=(const HashMap& other)
  {
    if(this == &other)
      return *this;
    tableOfBuckets.clear();
    elementsInMap = 0;
    numberOfBuckets = other.numberOfBuckets;
    tableOfBuckets.resize(numberOfBuckets);
    for(auto iter = other.begin(); iter != other.end(); ++iter)
      this->operator[](iter->first) = iter->second;
    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if(this == &other)
      return *this;
    tableOfBuckets.clear();
    elementsInMap = 0;
    numberOfBuckets = INITIAL_NUM_OF_BUCKETS;
    tableOfBuckets.resize(numberOfBuckets);
    std::swap(numberOfBuckets, other.numberOfBuckets);
    std::swap(elementsInMap, other.elementsInMap);
    std::swap(tableOfBuckets, other.tableOfBuckets);
    return *this;
  }

  bool isEmpty() const
  {
    return elementsInMap == 0;
  }

  mapped_type& operator[](const key_type& key)
  {
    size_type index = getIndexFromKey(key);
    if(!tableOfBuckets[index].empty())
    {
      for(auto iter = tableOfBuckets[index].begin(); iter != tableOfBuckets[index].end(); ++iter)
      {
        if((*iter).first == key)
          return (*iter).second;
      }
      tableOfBuckets[index].emplace_back(value_type(key, mapped_type{}));
      ++elementsInMap;
      if(elementsInMap / static_cast<double>(numberOfBuckets > MAX_LOAD_FACTOR))
        rehash();
      return tableOfBuckets[getIndexFromKey(key)].back().second;
    }
    else
    {
      tableOfBuckets[index].emplace_front(value_type(key, mapped_type{}));
        ++elementsInMap;
        if((elementsInMap / static_cast<double>(numberOfBuckets)) > MAX_LOAD_FACTOR)
          rehash();
        return tableOfBuckets[getIndexFromKey(key)].begin()->second;
    }
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    auto iteratorToElement = find(key);
    if(iteratorToElement == cend())
      throw std::out_of_range("in function valueOf(const key_type&), cannot find value of invalid key");
    return (*iteratorToElement).second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    auto iteratorToElement = find(key);
    if(iteratorToElement == end())
      throw std::out_of_range("in function valueOf(const key_type&), cannot find value of invalid key");
    return (*iteratorToElement).second;
  }

  const_iterator find(const key_type& key) const
  {
    size_type index = getIndexFromKey(key);
    if(tableOfBuckets[index].empty())
      return cend();
    else
    {
      for(auto iter = tableOfBuckets[index].begin(); iter != tableOfBuckets[index].end(); ++iter)
      {
        if((*iter).first == key)
          return ConstIterator(this, index, iter);
      }
      return cend();
    }
  }

  iterator find(const key_type& key)
  {
    size_type index = getIndexFromKey(key);
    if(tableOfBuckets[index].empty())
      return end();
    else
    {
      for(auto iter = tableOfBuckets[index].begin(); iter != tableOfBuckets[index].end(); ++iter)
      {
        if((*iter).first == key)
          return Iterator(this, index, iter);
      }
      return end();
    }
  }

  void remove(const key_type& key)
  {
    auto toRemove = find(key);
    if(toRemove == end())
      throw std::out_of_range("in function: remove(const const_iterator&), cannot remove non-existing element");
    tableOfBuckets[toRemove.bucketIndex].erase(toRemove.bucketListConstIterator);
    --elementsInMap;
  }

  void remove(const const_iterator& it)
  {
    if(it == end())
      throw std::out_of_range("in function: remove(const const_iterator&), cannot remove end()");
    tableOfBuckets[it.bucketIndex].erase(it.bucketListConstIterator);
    --elementsInMap;
  }

  size_type getSize() const
  {
    return elementsInMap;
  }

  bool operator==(const HashMap& other) const
  {
    if(elementsInMap != other.elementsInMap)
      return false;
    auto thisMap = cbegin();
    auto otherMap = other.cbegin();
    while(thisMap != cend() && otherMap != other.cend())
    {
      if((*thisMap).first != (*otherMap).first || (*thisMap).second != (*otherMap).second)
        return false;
      ++thisMap;
      ++otherMap;
    }
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    if(isEmpty())
      return end();
    size_type index = 0;
    while(index < numberOfBuckets)
    {
      if(!tableOfBuckets[index].empty())
        break;
      ++index;
    }
    return Iterator(this, index, tableOfBuckets[index].begin());
  }

  iterator end()
  {
    return Iterator(this, numberOfBuckets - 1, tableOfBuckets[numberOfBuckets - 1].end());
  }

  const_iterator cbegin() const
  {
    if(isEmpty())
      return cend();
    size_type index = 0;
    while(index < numberOfBuckets)
    {
      if(!tableOfBuckets[index].empty())
        break;
      ++index;
    }
    return ConstIterator(this, index, tableOfBuckets[index].begin());
  }

  const_iterator cend() const
  {
    return ConstIterator(this, numberOfBuckets - 1, tableOfBuckets[numberOfBuckets - 1].end());
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;
  using list_iterator = typename HashMap::bucket_list::const_iterator;

private:
  const HashMap* map_ptr;
  HashMap::size_type bucketIndex;
  list_iterator bucketListConstIterator;
  friend class HashMap<KeyType, ValueType>;

  bool pointsAtBeginning() const
  {
    return (bucketIndex == 0 &&  bucketListConstIterator == map_ptr->tableOfBuckets[bucketIndex].begin());
  }

  bool pointsAtEnd() const
  {
    return (map_ptr->numberOfBuckets == bucketIndex + 1 &&  bucketListConstIterator == map_ptr->tableOfBuckets[bucketIndex].end());
  }

public:
  explicit ConstIterator(const HashMap<KeyType, ValueType>* map, HashMap::size_type bucketIdx, list_iterator listConstIterator) : map_ptr(map), 
                                                                                    bucketIndex(bucketIdx), bucketListConstIterator(listConstIterator)
  {}

  ConstIterator(const ConstIterator& other)
  {
    map_ptr = other.map_ptr;
    bucketIndex = other.bucketIndex;
    bucketListConstIterator = other.bucketListConstIterator;
  }

  ConstIterator& operator++()
  {
    if(map_ptr->isEmpty() || pointsAtEnd())
      throw std::out_of_range("in function: operator++(), cannot increment end() or empty map");
    auto lastOfList = --map_ptr->tableOfBuckets[bucketIndex].end();
    if(bucketListConstIterator == lastOfList)
    {
      do
      {
        ++bucketIndex;
        if(!map_ptr->tableOfBuckets[bucketIndex].empty())
        {
          bucketListConstIterator = map_ptr->tableOfBuckets[bucketIndex].begin();
          return *this;
        }
        if(map_ptr->numberOfBuckets == bucketIndex + 1)
        {
          bucketListConstIterator = map_ptr->tableOfBuckets[bucketIndex].end();
          return *this;
        }
      } while(true);
    }
    else
    {
      ++bucketListConstIterator;
      return *this;
    }
  }

  ConstIterator operator++(int)
  {
    if(map_ptr->isEmpty() || pointsAtEnd())
      throw std::out_of_range("in function: operator++(int), cannot increment end() or empty map");
    auto lastOfList = --map_ptr->tableOfBuckets[bucketIndex].end();
    auto temp(*this);
    if(bucketListConstIterator == lastOfList)
    {
      do
      {
        ++bucketIndex;
        if(!map_ptr->tableOfBuckets[bucketIndex].empty())
        {
          bucketListConstIterator = map_ptr->tableOfBuckets[bucketIndex].begin();
          return temp;
        }
        if(map_ptr->numberOfBuckets == bucketIndex + 1)
        {
          bucketListConstIterator = map_ptr->tableOfBuckets[bucketIndex].end();
          return temp;
        }
      } while(true);
    }
    else
    {
      ++bucketListConstIterator;
      return temp;
    }
  }

  ConstIterator& operator--()
  {
    if(map_ptr->isEmpty() || pointsAtBeginning())
      throw std::out_of_range("in function: operator--(), cannot decrement begin() or empty map");
    if(bucketListConstIterator == map_ptr->tableOfBuckets[bucketIndex].begin())
    {
      do
      {
        --bucketIndex;
        if(!map_ptr->tableOfBuckets[bucketIndex].empty())
        {
          bucketListConstIterator = --(map_ptr->tableOfBuckets[bucketIndex].end());
          return *this;
        }
        if(bucketIndex == 0)
        {
          bucketListConstIterator = map_ptr->tableOfBuckets[bucketIndex].begin();
          return *this;
        }
      } while(true);
    }
    else
    {
      --bucketListConstIterator;
      return *this;
    }
  }

  ConstIterator operator--(int)
  {
    if(map_ptr->isEmpty() || pointsAtBeginning())
      throw std::out_of_range("in function: operator--(int), cannot decrement begin() or empty map");
    auto temp(*this);
    if(bucketListConstIterator == map_ptr->tableOfBuckets[bucketIndex].begin())
    {
      do
      {
        --bucketIndex;
        if(!map_ptr->tableOfBuckets[bucketIndex].empty())
        {
          bucketListConstIterator = --(map_ptr->tableOfBuckets[bucketIndex].end());
          return *this;
        }
        if(bucketIndex == 0)
        {
          bucketListConstIterator = map_ptr->tableOfBuckets[bucketIndex].begin();
          return *this;
        }
      } while(true);
    }
    else
    {
      --bucketListConstIterator;
      return *this;
    }
  }

  reference operator*() const
  {
    if(pointsAtEnd())
      throw std::out_of_range("in function: operator*(), cannot dereference end()");
    return *bucketListConstIterator;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return (bucketIndex == other.bucketIndex && bucketListConstIterator == other.bucketListConstIterator);
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;
  using list_iterator = typename HashMap::bucket_list::iterator;


  explicit Iterator(const HashMap<KeyType, ValueType>* map, size_t bucketIdx, list_iterator listConstIterator) : ConstIterator(map, bucketIdx, listConstIterator)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
