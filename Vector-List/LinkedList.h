#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class LinkedList
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
  class Node;
  Node* head;
  Node* tail;
  Node* guardian;
  size_type list_length;
  void clear_all_data()
  {
    while(head != nullptr && head != guardian)
    {
      Node* to_delete = head;
      head = head->next_Node;
      delete to_delete;
    }
    list_length = 0;
    head = tail = nullptr;
  }
public:
  LinkedList(Node* head_initializer = nullptr, Node* tail_initializer = nullptr, size_type size = 0) : head(head_initializer), tail(tail_initializer), list_length(size)
  {
    guardian = new Node();
  }

  LinkedList(std::initializer_list<Type> l)
  {
      list_length = 0;
      head = tail = nullptr;
      guardian = new Node();

      if(l.size() != 0)
      {
        for(auto &item: l)
          append(item);
      }
  }

  LinkedList(const LinkedList& other)
  {
    list_length = 0;
    head = tail = nullptr;
    guardian = new Node();

    if(other.list_length != 0)
    {
      for(auto &item: other)
        append(item);
    }
  }

  LinkedList(LinkedList&& other)
  {
    head = other.head;
    tail = other.tail;
    guardian = other.guardian;
    list_length = other.list_length;
    other.list_length = 0;
    other.head = other.tail = other.guardian = nullptr;
  }

  ~LinkedList()
  {
    while(head != nullptr && head != guardian)
    {
      Node* to_delete = head;
      head = head->next_Node;
      delete to_delete;
    }
      delete guardian;
  }

  LinkedList& operator=(const LinkedList& other)
  {
    if(this == &other)
      return *this;
    clear_all_data();
    if(other.list_length != 0)
    {
      for(auto &item: other)
        append(item);
    }
    return *this;
  }

  LinkedList& operator=(LinkedList&& other)
  {
    if(this == &other)
      return *this;
    clear_all_data();
    delete guardian;
    head = other.head;
    tail = other.tail;
    guardian = other.guardian;
    list_length = other.list_length;
    other.list_length = 0;
    other.head = other.tail = other.guardian = nullptr;
    return *this;
  }

  bool isEmpty() const
  {
    return list_length == 0;
  }

  size_type getSize() const
  {
    return list_length;
  }

  void append(const Type& item)
  {
    if(head == nullptr)
    {
      head = tail = new Node(item, guardian);
      guardian->previous_Node = tail;
      ++list_length;
      return;
    }
    else
    {
      tail->next_Node = new Node(item, guardian, tail);
      tail = tail->next_Node;
      guardian->previous_Node = tail;
      ++list_length;
    }
  }

  void prepend(const Type& item)
  {
    if(head == nullptr)
    {
      head = tail = new Node(item, guardian);
      guardian->previous_Node = tail;
      ++list_length;
      return;
    }
    else
    {
      head = new Node(item, head);
      head->next_Node->previous_Node = head;
      ++list_length;
    }
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    if(insertPosition == begin())
    {
      prepend(item);
      return;
    }
    if(insertPosition == end())
    {
      append(item);
      return;
    }
    for(iterator p = begin(); p != end(); p++)
    {
      if(p == insertPosition)
      {
        Node* next_one = insertPosition.return_Node_pointer();
        Node* previous_one = (insertPosition - 1).return_Node_pointer();
        Node* inserted_Node = new Node(item, next_one, previous_one);
        previous_one->next_Node = inserted_Node;
        next_one->previous_Node = inserted_Node;
        ++list_length;
        return;
      }
    }
    throw std::out_of_range("in funtion: void insert(const_iterator&, const Type&)");
  }

  Type popFirst()
  {
    if(head == nullptr)
      throw std::out_of_range("in function: popFirst()");
    Node* temp_node = head;
    Type storage = temp_node->data;
    if(head == tail)
    {
      head = tail = guardian->previous_Node = nullptr;
      delete temp_node;
      --list_length;
    }
    else
    {
      head = head->next_Node;
      head->previous_Node = nullptr;
      delete temp_node;
      --list_length;
    }
    return storage;
  }

  Type popLast()
  {
    if(tail == nullptr)
      throw std::out_of_range("in function: popLast()");
    Node* temp_node = tail;
    Type storage = temp_node->data;
    if(head == tail)
    {
      head = tail = guardian->previous_Node = nullptr;
      delete temp_node;
      --list_length;
    }
    else
    {
      tail = tail->previous_Node;
      tail->next_Node = guardian;
      guardian->previous_Node = tail;
      delete temp_node;
      --list_length;
    }
    return storage;
  }

  void erase(const const_iterator& possition)
  {
    if(possition == begin())
    {
      popFirst();
      return;
    }
    else
    {
      for(iterator p = begin() + 1; p != end(); p++)
      {
        if(p == possition)
        {
          Node* to_delete_node = possition.return_Node_pointer();
          to_delete_node->next_Node->previous_Node = to_delete_node->previous_Node;
          to_delete_node->previous_Node->next_Node = to_delete_node->next_Node;
          delete to_delete_node;
          --list_length;
          return;
        }
      }
    }
    throw std::out_of_range("in funtion: void insert(const_iterator&, const Type&)");
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    if(firstIncluded == lastExcluded)
      return;
    if(firstIncluded == begin() && lastExcluded == end())
    {
      clear_all_data();
      return;
    }

    bool iterator_1_in_range = false;
    bool iterator_2_in_range = false;
    for(auto p = begin(); p != end(); p++)
    {
      if(p == firstIncluded)
        iterator_1_in_range = true;
      else if(p == lastExcluded)
        iterator_2_in_range = true;
      if(iterator_1_in_range && iterator_2_in_range)
        break;
    }
    if(lastExcluded == end())
      iterator_2_in_range = true;
    if(!(iterator_1_in_range && iterator_2_in_range))
      throw std::out_of_range("in function: void erase(const_iterator&, const_iterator&)");
    else
    {
      Node* previous = firstIncluded.return_Node_pointer()->previous_Node;
      Node* to_delete = firstIncluded.return_Node_pointer();
      Node* next;
      while(to_delete != lastExcluded.return_Node_pointer())
      {
        next = to_delete->next_Node;
        delete to_delete;
        to_delete = next;
        --list_length;
      }
      if(previous == nullptr)
      {
        lastExcluded.return_Node_pointer()->previous_Node = nullptr;
        head = lastExcluded.return_Node_pointer();
        return;
      }
      else if(lastExcluded.return_Node_pointer() == guardian)
      {
        tail = previous;
        previous->next_Node = lastExcluded.return_Node_pointer();
        lastExcluded.return_Node_pointer()->previous_Node = previous;
      }
      else
      {
        previous->next_Node = lastExcluded.return_Node_pointer();
        lastExcluded.return_Node_pointer()->previous_Node = previous;
      }
      
    }
  }

  iterator begin()
  {
    return Iterator(head);
  }

  iterator end()
  {
    if(isEmpty())
      return Iterator(tail);
    return Iterator(guardian);
  }

  const_iterator cbegin() const
  {
    return ConstIterator(head);
  }

  const_iterator cend() const
  {
    if(isEmpty())
      return ConstIterator(tail);
    return ConstIterator(guardian);
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
class LinkedList<Type>::Node
{
public:
  Type data;
  Node* next_Node;
  Node* previous_Node;

  Node(Node* next = nullptr, Node* previous = nullptr) : next_Node(next), previous_Node(previous)
  {}
  
  Node(const Type& item, Node* next = nullptr, Node* previous = nullptr) : data(item), next_Node(next), previous_Node(previous)
  {}
};

template <typename Type>
class LinkedList<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename LinkedList::value_type;
  using difference_type = typename LinkedList::difference_type;
  using pointer = typename LinkedList::const_pointer;
  using reference = typename LinkedList::const_reference;
private:
  Node* node_pointer;
public:
  explicit ConstIterator(Node* pointer = nullptr) : node_pointer(pointer)
  {}

  Node* return_Node_pointer() const
  {
    return this->node_pointer;
  }

  reference operator*() const
  {
    if(node_pointer == nullptr || node_pointer->next_Node == nullptr)
      throw std::out_of_range("in function: operator++()");
    return node_pointer->data;
  }

  ConstIterator& operator++()
  {
    if(node_pointer == nullptr || node_pointer->next_Node == nullptr)
      throw std::out_of_range("in function: operator++()");
    node_pointer = node_pointer->next_Node;
    return *this;
  }

  ConstIterator operator++(int)
  {
    if(node_pointer == nullptr || node_pointer->next_Node == nullptr)
      throw std::out_of_range("in function: operator++(int)");
    auto temp(*this);
    node_pointer = node_pointer->next_Node;
    return temp;
  }

  ConstIterator& operator--()
  {
    if(node_pointer == nullptr || node_pointer->previous_Node == nullptr)
      throw std::out_of_range("in function: operator--()");
    node_pointer = node_pointer->previous_Node;
    return *this;
  }

  ConstIterator operator--(int)
  {
    if(node_pointer == nullptr || node_pointer->previous_Node == nullptr)
      throw std::out_of_range("in function: operator--(int)");
    auto temp(*this);
    node_pointer = node_pointer->previous_Node;
    return temp;
  }

  ConstIterator operator+(difference_type d) const
  {
    if(node_pointer == nullptr)
        throw std::out_of_range("in function: operator+(difference type)");
    Node* pointed_position = node_pointer;
    while(d > 0)
    {
      pointed_position = pointed_position->next_Node;
      d--;
      if(pointed_position == nullptr)
        throw std::out_of_range("in function: operator+(difference type)");
    }
    return  ConstIterator(pointed_position);
  }

  ConstIterator operator-(difference_type d) const
  {
    if(node_pointer == nullptr)
        throw std::out_of_range("in function: operator+(difference type)");
    Node* pointed_position = node_pointer;
    while(d > 0)
    {
      pointed_position = pointed_position->previous_Node;
      d--;
      if(pointed_position == nullptr)
        throw std::out_of_range("in function: operator+(difference type)");
    }
    return  ConstIterator(pointed_position);
  }

  bool operator==(const ConstIterator& other) const
  {
    return node_pointer == other.node_pointer;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return node_pointer != other.node_pointer;
  }
};

template <typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
{
public:
  using pointer = typename LinkedList::pointer;
  using reference = typename LinkedList::reference;

  explicit Iterator(Node* pointer = nullptr) : ConstIterator(pointer)
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

}//namespace

#endif // AISDI_LINEAR_LINKEDLIST_H
