#ifndef _MAP_H_
#define _MAP_H_

#include <utility>
#include <memory>
#include <vector>
#include <stdexcept>

namespace cs540 {
  static const unsigned int MAX_SKIP_LIST_HEIGHT = 20;

  template <typename Key_T, typename Mapped_T>
  class Map {
  public:
    using ValueType = std::pair<const Key_T, Mapped_T>;

    class Iterator {
    public:
      Iterator(const Iterator&);
      virtual ~Iterator();
      Iterator& operator=(const Iterator&);
      virtual Iterator& operator++();
      virtual Iterator operator++(int);
      virtual Iterator& operator--();
      virtual Iterator operator--(int);
      virtual ValueType& operator*() const;
      virtual ValueType *operator->() const;
      friend bool operator==(const Iterator&, const Iterator&);
      friend bool operator!=(const Iterator&, const Iterator&);
    private:
      SkipNode *_it;
    };
    class ConstIterator: public Iterator {
    public:
      ConstIterator& operator=(const ConstIterator&);
      const ValueType& operator*() const;
      const ValueType *operator->() const;
    };
    class ReverseIterator: public Iterator {
    public:
      Iterator& operator++();
      Iterator operator++(int);
      Iterator& operator--();
      Iterator operator--(int);
    };


    // Constructors and assignment ops
    Map();
    Map(const Map&);
    Map& operator=(const Map&);
    ~Map();

    // Size
    size_t size() const;
    bool empty() const;

    // Iterators
    Iterator begin();
    Iterator end();
    ConstIterator begin() const;
    ConstIterator end() const;
    ReverseIterator rbegin();
    ReverseIterator rend();

    // Element access
    Iterator find(const Key_T&);
    ConstIterator find(const Key_T&) const;
    Mapped_T& at(const Key_T&);
    const Mapped_T& at(const Key_T&) const;
    Mapped_T& operator[](const Key_T&);

    // Modifiers
    std::pair<Iterator, bool> insert(const ValueType&);
    template <typename IT_T>
    void insert(IT_T range_beg, IT_T range_end);
    void erase(Iterator pos);
    void erase(const Key_T&);
    void clear();

    // Comparison
    friend bool operator==(const Map&, const Map&);
    friend bool operator!=(const Map&, const Map&);
    friend bool operator<(const Map&, const Map&);
  private:
    // Skip list internals here
    struct SkipNode {
      SkipNode() = default;
      SkipNode(const ValueType& d): data(d) {}
      std::vector<SkipNode *> next;
      ValueType data;

      static int rand_height();
    };

    int _height;
    SkipNode *_head;
    SkipNode *_sentinel;

    size_t _size;
  };

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>::Map():
    _height(1),
    _head(new SkipNode),
    _sentinel(new SkipNode),
    _size(0) {
    // This guy will change depending on the height and whatnot
    _head->next = std::vector<SkipNode *> {_sentinel};
    // This will never change
    _sentinel->next = std::vector<SkipNode *> {_sentinel};
  }

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>::Map(const Map<Key_T, Mapped_T>& map):
    _height(map._height),
    _head(new SkipNode),
    _sentinel(new SkipNode),
    _size(map._size) {
    // Copy skip list
  }

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>& Map<Key_T, Mapped_T>::operator=(const Map& map) {
    if (&map == this) {
      return *this;
    }

    delete _head;
    delete _sentinel;

    _height = map._height;
    _head = new SkipNode;
    _sentinel = new SkipNode;

    // Copy skip list
  }

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>::~Map() {
    delete _head;
    delete _sentinel;
  }

  template <typename Key_T, typename Mapped_T>
  size_t Map<Key_T, Mapped_T>::size() const {
    return _size;
  }

  template <typename Key_T, typename Mapped_T>
  bool Map<Key_T, Mapped_T>::empty() const {
    return _size == 0;
  }

  template <typename Key_T, typename Mapped_T>
  typename Map<Key_T, Mapped_T>::Iterator Map<Key_T, Mapped_T>::find(const Key_T& key) {
    if (empty()) {
      return end();
    }

    int level;
    SkipNode *it = _head;
    for (level = _height - 1; level >= 0; level--) {
      while (it->next != _sentinel && it->next[level].data.first < key) {
        it = it->next;
      }
    }

    if (it->next[level].data.first == key) {
      return Iterator(it->next);
    }
  }

  template <typename Key_T, typename Mapped_T>
  Mapped_T& Map<Key_T, Mapped_T>::at(const Key_T& key) {
    auto it = find(key);
    if (it == end()) {
      throw std::out_of_range("Key not found");
    } else {
      return it->second;
    }
  }

  template <typename Key_T, typename Mapped_T>
  Mapped_T& Map<Key_T, Mapped_T>::operator[](const Key_T& key) {
    auto res = insert(key, Mapped_T());
    return res.first->second;
  }

  template <typename Key_T, typename Mapped_T>
  std::pair<typename Map<Key_T, Mapped_T>::Iterator, bool> Map<Key_T, Mapped_T>::insert(const ValueType& val) {
    if (empty()) {
      return end();
    }

    int level;
    SkipNode *it = _head;
    for (level = _height - 1; level >= 0; level--) {
      while (it->next != _sentinel && it->next[level].data.first < val.first) {
        it = it->next;
      }
    }

    if (it->next[level].data.first == val.first) {
      return std::make_pair(Iterator(it->next), false);
    } else {
      // Get a height for the new entry
      // Make a new skip node and set the appropriate nexts
      // May need to remember where dropoff points were to set next
      // values appropriately
      // Set lane
      SkipNode *sn = new SkipNode(val);
      int sn_height = SkipNode::rand_height();
      sn->next = std::vector<SkipNode *>(sn_height, _sentinel);
      // Do more shit
      return std::make_pair(Iterator(it->next), true);
    }
  }
}


#endif /* _MAP_H_ */
