#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <iostream>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class Vector
{
public:
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using const_pointer = const Type*;
  using const_reference = const Type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  pointer container_data;
  size_type container_size;
  size_type container_capacity;
  enum {DEFAULT_CAPACITY = 10};
  void increase_capacity()
  {
    if(container_capacity == 0)
      container_capacity = DEFAULT_CAPACITY;
    else
      container_capacity *= 2;
    pointer temp = new value_type[container_capacity];
    for(size_type i = 0; i < container_size; i++)
      temp[i] = container_data[i];
    delete [] container_data;
    container_data = temp;
  }
  void move_right(size_type start_from)
  {
    for(size_type i = container_size; i > start_from; i--)
      container_data[i] = container_data[i - 1];
  }

  void move_left(size_type fill_first)
  {
    for(size_type i = fill_first; i < container_size - 1; i++)
      container_data[i] = container_data[i + 1];
  }

public:
  Vector() : container_data(nullptr), container_size(0), container_capacity(0)
  {}

  Vector(std::initializer_list<Type> l)
  {
    container_capacity =  l.size();
    container_size = 0;
    container_data = new value_type[container_capacity];
    for(auto &item : l)
      append(item);
  }

  Vector(const Vector& other)
  {
    container_capacity = other.container_capacity;
    container_size = 0;
    container_data = new value_type[container_capacity];
    for(auto &item : other)
      append(item);
  }

  Vector(Vector&& other)
  {
    container_capacity = other.container_capacity;
    container_size = other.container_size;
    container_data = other.container_data;
    other.container_data = nullptr;
    other.container_capacity = other.container_size = 0;
  }

  ~Vector()
  {
    delete [] container_data;
  }

  Vector& operator=(const Vector& other)
  {
    if(this == &other)
      return *this;
    delete [] container_data;
    container_capacity = other.container_capacity;
    container_size = 0;
    container_data = new value_type[container_capacity];
    for(auto &item : other)
      append(item);
    return *this;
  }

  Vector& operator=(Vector&& other)
  {
    if(this == &other)
      return *this;
    delete [] container_data;
    container_capacity = other.container_capacity;
    container_size = other.container_size;
    container_data = other.container_data;
    other.container_data = nullptr;
    other.container_capacity = other.container_size = 0;
    return *this;
  }

  bool isEmpty() const
  {
    return container_size == 0;
  }

  size_type getSize() const
  {
    return container_size;
  }

  void append(const Type& item)
  {
    if(container_size == container_capacity)
      increase_capacity();
    container_data[container_size] = item;
    ++container_size;
  }

  void prepend(const Type& item)
  {
    if(container_size == container_capacity)
      increase_capacity();
    move_right(0);
    container_data[0] = item;
    ++container_size;
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    size_type position;
    if(insertPosition == end())
      position = container_size;
    else
      position = &(*insertPosition) - &(*begin());
    if(position > container_size)
      throw std::out_of_range("in function: void erase(const_iterator&)");

    if(insertPosition == begin())
    {
      prepend(item);
    }
    else if(insertPosition == end())
    {
      append(item);
    }
    else
    {
      if(container_size == container_capacity)
        increase_capacity();
      move_right(position);
      container_data[position] = item;
      ++container_size;
    }
  }

  Type popFirst()
  {
    if(isEmpty())
      throw std::out_of_range("in function: Type popFirst()");

    value_type temp = container_data[0];
    move_left(0);
    --container_size;
    return temp;
  }

  Type popLast()
  {
    if(isEmpty())
      throw std::out_of_range("in function: Type popLast()");

    value_type temp = container_data[container_size - 1];
    --container_size;
    return temp;
  }

  void erase(const const_iterator& possition)
  {
    size_type position;
    if(possition == end())
      position = container_size;
    else
      position = &(*possition) - &(*begin());
    if(position >= container_size)
      throw std::out_of_range("in function: void erase(const_iterator&)");

    move_left(position);
    --container_size;
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    if(firstIncluded == lastExcluded)
      return;
    if(firstIncluded == begin() && lastExcluded == end())
    {
      container_size = 0;
      return;
    }

    size_type firstposition = &(*firstIncluded) - &(*begin());
    size_type lastposition;
    if(lastExcluded == end())
      lastposition = container_size;
    else
      lastposition = &(*lastExcluded) - &(*begin());
    if(firstposition > container_size || lastposition > container_size)
      throw std::out_of_range("in function: void erase(const_iterator&, const_iterator&)");

    auto deleted_elements = lastposition - firstposition;
    while(lastposition < container_size)
    {
      container_data[firstposition] = container_data[lastposition];
      ++firstposition;
      ++lastposition;
    }
    container_size -= deleted_elements;
  }

  iterator begin()
  {
    return Iterator(this);
  }

  iterator end()
  {
    return Iterator(this, container_size);
  }

  const_iterator cbegin() const
  {
    return ConstIterator(this);
  }

  const_iterator cend() const
  {
    return ConstIterator(this, container_size);
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

template <typename Type>
class Vector<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::const_pointer;
  using reference = typename Vector::const_reference;
private:
  const Vector* container_pointer;
  Vector::size_type data_index;
public:
  explicit ConstIterator(const Vector<value_type>* pointer_init = nullptr, Vector::size_type idx_init = 0) : container_pointer(pointer_init), data_index(idx_init)
  {}

  reference operator*() const
  {
    if(container_pointer == nullptr || data_index >= container_pointer->container_size)
      throw std::out_of_range("in function: operator*()");
    return container_pointer->container_data[data_index];
  }

  ConstIterator& operator++()
  {
    if(data_index + 1 > container_pointer->container_size)
      throw std::out_of_range("in function: operator++()");
    data_index++;
    return *this;
  }

  ConstIterator operator++(int)
  {
    if(data_index + 1 > container_pointer->container_size)
      throw std::out_of_range("in function: operator++(int)");
    auto temp_iterator(*this);
    data_index++;
    return temp_iterator;
  }

  ConstIterator& operator--()
  {
    int temp = data_index - 1;
    if(temp < 0)
      throw std::out_of_range("in function: operator--()");
    data_index--;
    return *this;
  }

  ConstIterator operator--(int)
  {
    int temp = data_index - 1;
    if(temp < 0)
      throw std::out_of_range("in function: operator--(int)");
    auto temp_iterator(*this);
    data_index--;
    return temp_iterator;
  }

  ConstIterator operator+(difference_type d) const
  {
    if(data_index + d > container_pointer->container_size)
      throw std::out_of_range("in function: operator+(difference_type)");
    return ConstIterator(container_pointer, data_index + d);
  }

  ConstIterator operator-(difference_type d) const
  {
    int temp = data_index - d;
    if(temp < 0)
      throw std::out_of_range("in function: operator-(difference_type)");
    return ConstIterator(container_pointer, data_index - d);
  }

  bool operator==(const ConstIterator& other) const
  {
    return (container_pointer == other.container_pointer && data_index == other.data_index);
  }

  bool operator!=(const ConstIterator& other) const
  {
    return (container_pointer != other.container_pointer || data_index != other.data_index);
  }
};

template <typename Type>
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
public:
  using pointer = typename Vector::pointer;
  using reference = typename Vector::reference;

  explicit Iterator(const Vector<value_type>* pointer_init = nullptr, Vector::size_type idx_init = 0) : ConstIterator(pointer_init, idx_init)
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

  Iterator operator+(difference_type d) const
  {
    return ConstIterator::operator+(d);
  }

  Iterator operator-(difference_type d) const
  {
    return ConstIterator::operator-(d);
  }

  reference operator*() const
  {
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif // AISDI_LINEAR_VECTOR_H