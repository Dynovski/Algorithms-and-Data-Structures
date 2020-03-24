#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;
private:

  class Node
  {
  public:
    Node* parent;
    Node* leftChild;
    Node* rightChild;
    value_type data;
    size_type height;

    Node(key_type key, mapped_type value, Node* prnt = nullptr) : parent(prnt), data(key, value)
    {
      leftChild = rightChild = nullptr;
      height = 1;
    }

    void assignNewHeight()
    {
      size_type leftHeight = (leftChild != nullptr) ? leftChild->height : 0;
      size_type rightHeight = (rightChild != nullptr) ? rightChild->height : 0;
      if(leftHeight > rightHeight)
        height = 1 + leftHeight;
      else
        height = 1 + rightHeight;
    }

  };

  Node* root;
  size_type size;

  void deleteNodesFrom(Node* startFrom)
  {
    if(startFrom == nullptr)
      return;
    if(startFrom != root)
    {
      if(startFrom->parent->leftChild == startFrom)
        startFrom->parent->leftChild = nullptr;
      else  
        startFrom->parent->rightChild = nullptr;
    }
    deleteNodesFrom(startFrom->rightChild);
    deleteNodesFrom(startFrom->leftChild);
    delete startFrom;
  }

  void clearTree()
  {
    deleteNodesFrom(root);
    size = 0;
    root = nullptr;
  }

  void rightRotate(Node* grandparent)
  {
    if(grandparent == nullptr)
      throw std::logic_error("in function: rightRotate(), cannot rotate end()");
    auto parentNode = grandparent->leftChild;
    grandparent->leftChild = parentNode->rightChild;
    if(parentNode->rightChild != nullptr)
      parentNode->rightChild->parent = grandparent;
    if(grandparent == root)
      root = parentNode;
    else if(grandparent == grandparent->parent->leftChild)
      grandparent->parent->leftChild = parentNode;
    else
      grandparent->parent->rightChild = parentNode;
    parentNode->rightChild = grandparent;
    parentNode->parent = grandparent->parent;
    grandparent->parent = parentNode;

    grandparent->assignNewHeight();
    parentNode->assignNewHeight();
  }

  void leftRotate(Node* grandparent)
  {
    if(grandparent == nullptr)
      throw std::logic_error("in function: leftRotate(), cannot rotate nullptr");
    auto parentNode = grandparent->rightChild;
    grandparent->rightChild = parentNode->leftChild;
    if(parentNode->leftChild != nullptr)
      parentNode->leftChild->parent = grandparent;
    if(grandparent == root)
      root = parentNode;
    else if(grandparent == grandparent->parent->leftChild)
      grandparent->parent->leftChild = parentNode;
    else
      grandparent->parent->rightChild = parentNode;
    parentNode->leftChild = grandparent;
    parentNode->parent = grandparent->parent;
    grandparent->parent = parentNode;

    grandparent->assignNewHeight();
    parentNode->assignNewHeight();
  }

  void rightLeftRotate(Node* grandparent)
  {
    rightRotate(grandparent->rightChild);
    leftRotate(grandparent);
  }

  void leftRightRotate(Node* grandparent)
  {
    leftRotate(grandparent->leftChild);
    rightRotate(grandparent);
  }

  void rebalanceTree(Node* node)
  {
    while(node != nullptr)
    {
      node->assignNewHeight();
      size_type rightChildHeight = node->rightChild ? node->rightChild->height : 0;
      size_type leftChildHeight = node->leftChild ? node->leftChild->height : 0;
      if(rightChildHeight > leftChildHeight + 1)
      {
        size_type rightGrandchildHeight = node->rightChild->rightChild ? node->rightChild->rightChild->height : 0;
        size_type leftGrandchildHeight = node->rightChild->leftChild ? node->rightChild->leftChild->height : 0;
        if(rightGrandchildHeight > leftGrandchildHeight)
        {
          leftRotate(node);
          return;
        }
        else
        {
          rightLeftRotate(node);
          return;
        }
      }
      else if(leftChildHeight > rightChildHeight + 1)
      {
        size_type rightGrandchildHeight = node->leftChild->rightChild ? node->leftChild->rightChild->height : 0;
        size_type leftGrandchildHeight = node->leftChild->leftChild ? node->leftChild->leftChild->height : 0;
        if(leftGrandchildHeight > rightGrandchildHeight)
        {
          rightRotate(node);
          return;
        }
        else
        {
          leftRightRotate(node);
          return;
        }
      }
      node = node->parent;
    }
  }

public:
  TreeMap()
  {
    root = nullptr;
    size = 0;
  }

  TreeMap(std::initializer_list<value_type> list)
  {
    root = nullptr;
    size = 0;
    for(auto &item : list)
      this->operator[](item.first) = item.second;
  }

  TreeMap(const TreeMap& other)
  {
    root = nullptr;
    size = 0;
    for(auto &item : other)
      this->operator[](item.first) = item.second;
  }

  TreeMap(TreeMap&& other)
  {
    root = nullptr;
    size = 0;
    std::swap(root, other.root);
    std::swap(size, other.size);
  }

  ~TreeMap()
  {
    clearTree();
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(this == &other)
      return *this;
    clearTree();
    for(auto &item : other)
      this->operator[](item.first) = item.second;
    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    if(this == &other)
      return *this;
    clearTree();
    std::swap(root, other.root);
    std::swap(size, other.size);
    return *this;
  }

  bool isEmpty() const
  {
    return size == 0;
  }

  mapped_type& operator[](const key_type& key)
  {
    Node* current = root;
    Node* parent = nullptr;

    while(current != nullptr)
    {
      parent = current;
      if(key == current->data.first)
        return current->data.second;
      else if(key > current->data.first)
        current = current->rightChild;
      else
        current = current->leftChild;
    }

    Node* newElement = new Node(key, mapped_type{}, parent);
    if(parent == nullptr)
      root = newElement;
    else if(key > parent->data.first)
      parent->rightChild = newElement;
    else
      parent->leftChild = newElement;
    ++size;
    rebalanceTree(parent);

    return newElement->data.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    auto iteratorToElement = find(key);
    if(iteratorToElement == cend())
      throw std::out_of_range("in function valueOf(const key_type&), invalid key");
    return (*iteratorToElement).second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    auto iteratorToElement = find(key);
    if(iteratorToElement == end())
      throw std::out_of_range("in function valueOf(const key_type&), invalid key");
    return (*iteratorToElement).second;
  }

  const_iterator find(const key_type& key) const
  {
    if(isEmpty())
      return cend();
    Node* temp = root;
    while(temp != nullptr && temp->data.first != key)
    {
      if(key > temp->data.first)
        temp = temp->rightChild;
      else if(key < temp->data.first)
        temp = temp->leftChild;
    }
    return ConstIterator(this, temp);
  }

  iterator find(const key_type& key)
  {
    if(isEmpty())
      return end();
    Node* temp = root;
    while(temp != nullptr && temp->data.first != key)
    {
      if(key > temp->data.first)
        temp = temp->rightChild;
      else if(key < temp->data.first)
        temp = temp->leftChild;
    }
    return Iterator(this, temp);
  }

  void remove(const key_type& key)
  {
    remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(it == end())
      throw std::out_of_range("in function: remove(const const_iterator&), cannot remove end() or if empty or non-existing element");
    auto nodeToDelete = it.current;
    auto parentOfDeleted = nodeToDelete->parent;
    if(nodeToDelete->leftChild == nullptr && nodeToDelete->rightChild == nullptr)
    {
      if(parentOfDeleted == nullptr)
        root = nullptr;
      else if(nodeToDelete == parentOfDeleted->leftChild)
        parentOfDeleted->leftChild = nullptr;
      else 
        parentOfDeleted->rightChild = nullptr;
      delete nodeToDelete;
    }
    else if(nodeToDelete->leftChild != nullptr && nodeToDelete->rightChild == nullptr)
    {
      if(parentOfDeleted == nullptr)
      {  
        root = nodeToDelete->leftChild;
        root->parent = nullptr;
      }
      else if(nodeToDelete == parentOfDeleted->leftChild)
      {
        parentOfDeleted->leftChild = nodeToDelete->leftChild;
        nodeToDelete->leftChild->parent = parentOfDeleted;
      }
      else
      {
        parentOfDeleted->rightChild = nodeToDelete->leftChild;
        nodeToDelete->leftChild->parent = parentOfDeleted;
      }
      delete nodeToDelete;
    }
    else if(nodeToDelete->leftChild == nullptr && nodeToDelete->rightChild != nullptr)
    {
      if(parentOfDeleted == nullptr)
      {  
        root = nodeToDelete->rightChild;
        root->parent = nullptr;
      }
      else if(nodeToDelete == parentOfDeleted->leftChild)
      {
        parentOfDeleted->leftChild = nodeToDelete->rightChild;
        nodeToDelete->rightChild->parent = parentOfDeleted;
      }
      else
      {
        parentOfDeleted->rightChild = nodeToDelete->rightChild;
        nodeToDelete->rightChild->parent = parentOfDeleted;
      }
      delete nodeToDelete;
    }
    else
    {
      auto successor = nodeToDelete->rightChild;
      if(successor->leftChild != nullptr)
      {
        while(successor->leftChild != nullptr)
          successor = successor->leftChild;
        successor->parent->leftChild = successor->rightChild;
        if(successor->rightChild != nullptr)
        successor->rightChild->parent = successor->parent;
      }
      else
      {
        successor->parent->rightChild = successor->rightChild;
        if(successor->rightChild != nullptr)
          successor->rightChild->parent = successor->parent;
        Node* temp = successor->parent;
        while(temp != nullptr)
        {
          temp->assignNewHeight();
          temp = temp->parent;
        }
      }
      if(parentOfDeleted == nullptr)
      {  
        root = successor;
        successor->leftChild = nodeToDelete->leftChild;
        successor->rightChild = nodeToDelete->rightChild;
        nodeToDelete->leftChild->parent = successor;
        nodeToDelete->rightChild->parent = successor;
        root->parent = nullptr;
      }
      else if(nodeToDelete == parentOfDeleted->leftChild)
      {
        parentOfDeleted->leftChild = successor;
        successor->leftChild = nodeToDelete->leftChild;
        successor->rightChild = nodeToDelete->rightChild;
        nodeToDelete->leftChild->parent = successor;
        nodeToDelete->rightChild->parent = successor;
        successor->parent = nodeToDelete->parent;
      }
      else 
      {
        parentOfDeleted->rightChild = successor;
        successor->leftChild = nodeToDelete->leftChild;
        successor->rightChild = nodeToDelete->rightChild;
        nodeToDelete->leftChild->parent = successor;
        nodeToDelete->rightChild->parent = successor;
        successor->parent = nodeToDelete->parent;
      }
      delete nodeToDelete;
      successor->assignNewHeight();
    }
    --size;
    rebalanceTree(parentOfDeleted);
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const TreeMap& other) const
  {
    if(size != other.size)
      return false;
    auto thisTree = cbegin();
    auto otherTree = other.cbegin();
    while(thisTree != cend() && otherTree != other.cend())
    {
      if((*thisTree).first != (*otherTree).first || (*thisTree).second != (*otherTree).second)
        return false;
      ++thisTree;
      ++otherTree;
    }
    return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    if(isEmpty())
      return end();
    auto temp = root;
    while(temp->leftChild != nullptr)
      temp = temp->leftChild;
    return Iterator(this, temp);
  }

  iterator end()
  {
    return Iterator(this, nullptr);
  }

  const_iterator cbegin() const
  {
    if(isEmpty())
      return cend();
    auto temp = root;
    while(temp->leftChild != nullptr)
      temp = temp->leftChild;
    return ConstIterator(this, temp);
  }

  const_iterator cend() const
  {
    return ConstIterator(this, nullptr);
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
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;
private:
  const TreeMap* tree_ptr;
  TreeMap<KeyType, ValueType>::Node* current;
  friend void TreeMap<KeyType, ValueType>::remove(const const_iterator&);

public:
  explicit ConstIterator(const TreeMap<KeyType, ValueType>* tree = nullptr, TreeMap<KeyType, ValueType>::Node* curr = nullptr) : tree_ptr(tree), current(curr) {}

  ConstIterator(const ConstIterator& other)
  {
    tree_ptr = other.tree_ptr;
    current = other.current;
  }

  ConstIterator& operator++()
  {
    if(current == nullptr) 
      throw std::out_of_range("in function: operator++(), cannot increment end()");
    if(current->rightChild != nullptr)
    {
      current = current->rightChild;
      while(current->leftChild != nullptr)
        current = current->leftChild;
    }
    else
    {
      while(current->parent != nullptr && current->parent->rightChild == current)
        current = current->parent;
      current = current->parent;
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    if(current == nullptr) 
      throw std::out_of_range("in function: operator++(int), cannot increment end()");
    auto temp(*this);
    if(current->rightChild != nullptr)
    {
      current = current->rightChild;
      while(current->leftChild != nullptr)
        current = current->leftChild;
    }
    else
    {
      while(current->parent != nullptr && current->parent->rightChild == current)
        current = current->parent;
      current = current->parent; 
    }
    return temp;
  }

  ConstIterator& operator--()
  {
    if (*this == tree_ptr->cbegin() || tree_ptr->isEmpty()) 
      throw std::out_of_range("in function: operator--(), cannot decrement begin() or empty tree");
    if(current == nullptr)
    {
      current = tree_ptr->root;
      while(current->rightChild != nullptr)
        current = current->rightChild;
    }
    else
    {
      if(current->leftChild != nullptr)
      {
        current = current->leftChild;
        while(current->rightChild != nullptr)
          current = current->rightChild;
      }
      else
      {  
        while(current->parent != nullptr && current->parent->leftChild == current)
          current = current->parent;
        current = current->parent;
      }
    }
    return *this;
  }

  ConstIterator operator--(int)
  {
    if (*this == tree_ptr->cbegin() || tree_ptr->isEmpty()) 
      throw std::out_of_range("in function: operator--(int), cannot decrement begin() or empty tree");
    auto temp(*this);
    if(current == nullptr)
    {
      current = tree_ptr->root;
      while(current->rightChild != nullptr)
        current = current->rightChild;
    }
    else
    {
      if(current->leftChild != nullptr)
      {
        current = current->leftChild;
        while(current->rightChild != nullptr)
          current = current->rightChild;
      }
      else
      {  
        while(current->parent != nullptr && current->parent->leftChild == current)
          current = current->parent;
        current = current->parent; 
      }
    }
    return temp;
  }

  reference operator*() const
  {
    if (current == nullptr)
      throw std::out_of_range("in function: operator*(), dereferencing end()");
    return current->data;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return current == other.current;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator(const TreeMap<KeyType, ValueType>* tree = nullptr, TreeMap<KeyType, ValueType>::Node* curr = nullptr) : ConstIterator(tree, curr) 
  {}

  Iterator(const ConstIterator& other) : ConstIterator(other)
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

#endif /* AISDI_MAPS_MAP_H */
