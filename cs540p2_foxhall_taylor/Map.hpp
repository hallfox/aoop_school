#ifndef _MAP_H_
#define _MAP_H_

#include <utility>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <random>

namespace cs540 {
  static const int MAX_SKIP_LIST_HEIGHT = 20; // Beware magic number

  template <typename Key_T, typename Mapped_T>
  class Map {
  public:
    using ValueType = std::pair<const Key_T, Mapped_T>;

    // Constructors and assignment ops
    Map();
    Map(const Map&);
    Map& operator=(const Map&);
    Map(std::initializer_list<ValueType>);
    ~Map();

    // Size
    size_t size() const;
    bool empty() const;

    // Iterators
    class Iterator;
    class ConstIterator;
    class ReverseIterator;

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
    friend bool operator==(const Map& map1, const Map& map2) {
      if (map1.size() != map2.size()) return false;

      auto it1 = map1.begin();
      auto it2 = map2.begin();

      // Good thing Skip Lists are ordered otherwise this wouldn't work
      while (it1 != map1.end() && it2 != map2.end() && *it1 == *it2) {
        it1++; it2++;
      }

      return true;
    }

    friend bool operator!=(const Map& map1, const Map& map2) {
      return !(map1 == map2);
    }

    friend bool operator<(const Map& map1, const Map& map2){
      auto it1 = map1.begin();
      auto it2 = map2.begin();

      // Good thing Skip Lists are ordered otherwise this wouldn't work as fast
      // Although I guess in BSTs one could just do an inorder traversal
      // Yeah that'd be cool
      // I think if I used a RB tree this would be the exact same code
      while (it1 != map1.end() && it2 != map2.end()) {
        if (*it1 < *it2) {
          return true;
        }
        it1++; it2++;
      }

      return map1.size() < map2.size();
    }

  private:
    // Skip list internals here
    struct SkipNode {
      // Can't assume we have these since we can't assume Mapped and Key have them
      SkipNode& operator=(const SkipNode&) = delete;

      SkipNode(): back(this), data(nullptr) {}
      // I don't want to be able to copy construct a node
      SkipNode(const SkipNode& other) = delete;
      ~SkipNode() {
        next = std::vector<SkipNode *>{};
        back = nullptr;
        delete data;
      }

      SkipNode(const ValueType& d): data(new ValueType(d)) {}

      std::vector<SkipNode *> next;
      SkipNode *back;
      ValueType *data;

      static int rand_height() {
        int seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine gen(seed);
        std::geometric_distribution<int> dist(0.5);

        return std::min(dist(gen)+1, MAX_SKIP_LIST_HEIGHT);
      }
    };

    // Some helper functions because I don't have a real SkipList class
    void _init_skip_list();
    void _delete_skip_list();
    void _copy_skip_list(const SkipNode *);

    int _height;
    SkipNode *_head;
    SkipNode *_sentinel;

    size_t _size;

  private:
    // Iterators
    class BaseIterator {
    public:
      BaseIterator() = delete;
      BaseIterator(SkipNode *it): _it(it) {}
      BaseIterator(const BaseIterator&) = default;
      BaseIterator& operator=(const BaseIterator&) = default;
      virtual ~BaseIterator() = default;

      virtual ValueType& operator*() const {
        return *(_it->data);
      }
      virtual ValueType *operator->() const {
        return _it->data;
      }
      friend bool operator==(const BaseIterator& i1, const BaseIterator& i2) {
        return i1._it == i2._it;
      }
      friend bool operator!=(const BaseIterator& i1, const BaseIterator& i2) {
        return !(i1 == i2);
      }
    protected:
      SkipNode *_it;
    };
  public:
    class Iterator: public BaseIterator {
    public:
      Iterator() = delete;
      Iterator(SkipNode *it): BaseIterator(it) {}
      Iterator(const Iterator&) = default;
      Iterator& operator=(const Iterator&) = default;
      ~Iterator() = default;

      Iterator& operator++() {
        BaseIterator::_it = BaseIterator::_it->next[0];
        return *this;
      }
      Iterator operator++(int) {
        Iterator t(*this);
        ++(*this);
        return t;
      }
      Iterator& operator--() {
        BaseIterator::_it = BaseIterator::_it->back;
        return *this;
      }
      Iterator operator--(int) {
        Iterator t(*this);
        --(*this);
        return t;
      }
    };
    class ReverseIterator: public BaseIterator {
    public:
      ReverseIterator() = delete;
      ReverseIterator(SkipNode *it): BaseIterator(it) {}
      ReverseIterator(const ReverseIterator&) = default;
      ReverseIterator& operator=(const ReverseIterator&) = default;
      ~ReverseIterator() = default;

      ReverseIterator& operator++() {
        BaseIterator::_it = BaseIterator::_it->back;
        return *this;
      }
      ReverseIterator operator++(int) {
        ReverseIterator t(*this);
        ++(*this);
        return t;
      }
      ReverseIterator& operator--() {
        BaseIterator::_it = BaseIterator::_it->next[0];
        return *this;
      }
      ReverseIterator operator--(int) {
        ReverseIterator t(*this);
        --(*this);
        return t;
      }
    };
    class ConstIterator {
    public:
      ConstIterator() = delete;
      ConstIterator(const SkipNode *it): _it(it) {}
      ConstIterator(const ConstIterator&) = default;
      ConstIterator& operator=(const ConstIterator&) = default;
      ~ConstIterator() = default;

      ConstIterator& operator++() {
        _it = _it->next[0];
        return *this;
      }
      ConstIterator operator++(int) {
        ConstIterator t(*this);
        ++(*this);
        return t;
      }
      ConstIterator& operator--() {
        _it = _it->back;
        return *this;
      }
      ConstIterator operator--(int) {
        ConstIterator t(*this);
        --(*this);
        return t;
      }
      const ValueType& operator*() const {
        return *(_it->data);
      }
      const ValueType *operator->() const {
        return &(*(_it->data));
      }
      friend bool operator==(const ConstIterator& i1, const ConstIterator& i2) {
        return i1._it == i2._it;
      }
      friend bool operator!=(const ConstIterator& i1, const ConstIterator& i2) {
        return !(i1 == i2);
      }
      friend bool operator==(const BaseIterator& i1, const ConstIterator& i2) {
        return i1._it == i2._it;
      }
      friend bool operator!=(const BaseIterator& i1, const ConstIterator& i2) {
        return !(i1 == i2);
      }
      friend bool operator==(const ConstIterator& i1, const BaseIterator& i2)  {
        return i2 == i1;
      }
      friend bool operator!=(const ConstIterator& i1, const BaseIterator& i2) {
        return !(i1 == i2);
      }
    private:
      const SkipNode *_it;
    };
  };

  template <typename Key_T, typename Mapped_T>
  void Map<Key_T, Mapped_T>::_init_skip_list() {
    _head->next = std::vector<SkipNode *>(MAX_SKIP_LIST_HEIGHT, _sentinel);
    _head->back = _head;
    _sentinel->next = std::vector<SkipNode *>(MAX_SKIP_LIST_HEIGHT, _sentinel);
    _sentinel->back = _head;
  }

  template <typename Key_T, typename Mapped_T>
  void Map<Key_T, Mapped_T>::_delete_skip_list() {
    SkipNode *it = _head;
    while (it != _sentinel) {
      SkipNode *t = it->next[0];
      delete it;
      it = t;
    }
    delete _sentinel;
  }

  template <typename Key_T, typename Mapped_T>
  void Map<Key_T, Mapped_T>::_copy_skip_list(const SkipNode *other) {
    SkipNode *it = _head;
    const SkipNode *other_it = other;
    std::vector<SkipNode *> lasts(_height, _head);
    for (size_t i = 0; i < _size; i++) {
      // Create a new skip node based off of the next guy
      SkipNode *mimic_sn = other_it->next[0];
      SkipNode *sn = new SkipNode(*(mimic_sn->data));
      sn->next = std::vector<SkipNode *>(mimic_sn->next.size(), _sentinel);
      sn->back = it;
      // Insert him into the skip list, after it but also setting last->next appropriately
      for (size_t j = 0; j < sn->next.size(); j++) {
        lasts[j]->next[j] = sn;
        lasts[j] = sn;
      }

      it = sn;
      other_it = other_it->next[0];
    }

    _sentinel->back = lasts[0];
  }

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>::Map():
    _height(1),
    _head(new SkipNode),
    _sentinel(new SkipNode),
    _size(0) {
    // Set head and sent to default values
    _init_skip_list();
  }

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>::Map(const Map<Key_T, Mapped_T>& map):
    _height(map._height),
    _head(new SkipNode),
    _sentinel(new SkipNode),
    _size(map._size) {
    // Set head and sent to default values
    _init_skip_list();
    // Copy skip list
    _copy_skip_list(map._head);
  }

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>& Map<Key_T, Mapped_T>::operator=(const Map& map) {
    if (&map == this) {
      return *this;
    }

    // Get rid of what's already here
    _delete_skip_list();

    // Reinitialize
    _height = map._height;
    _head = new SkipNode;
    _sentinel = new SkipNode;
    _size = map._size;

    // Basically do what copy constructor do
    _init_skip_list();
    _copy_skip_list(map._head);

    return *this;
  }

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>::Map(std::initializer_list<ValueType> init_list):
    _height(1),
    _head(new SkipNode),
    _sentinel(new SkipNode),
    _size(0) {
    // Set head and sent to default values
    _init_skip_list();
    // Copy skip list
    for (auto& v: init_list) {
      insert(v);
    }
  }

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>::~Map() {
    _delete_skip_list();
    _head = nullptr;
    _sentinel = nullptr;
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
  typename Map<Key_T, Mapped_T>::Iterator Map<Key_T, Mapped_T>::begin() {
    return Iterator(_head->next[0]);
  }

  template <typename Key_T, typename Mapped_T>
  typename Map<Key_T, Mapped_T>::Iterator Map<Key_T, Mapped_T>::end() {
    return Iterator(_sentinel);
  }

  template <typename Key_T, typename Mapped_T>
  typename Map<Key_T, Mapped_T>::ConstIterator Map<Key_T, Mapped_T>::begin() const {
    return ConstIterator(_head->next[0]);
  }

  template <typename Key_T, typename Mapped_T>
  typename Map<Key_T, Mapped_T>::ConstIterator Map<Key_T, Mapped_T>::end() const {
    return ConstIterator(_sentinel);
  }

  template <typename Key_T, typename Mapped_T>
  typename Map<Key_T, Mapped_T>::ReverseIterator Map<Key_T, Mapped_T>::rbegin() {
    return ReverseIterator(_sentinel->back);
  }

  template <typename Key_T, typename Mapped_T>
  typename Map<Key_T, Mapped_T>::ReverseIterator Map<Key_T, Mapped_T>::rend() {
    return ReverseIterator(_head);
  }

  template <typename Key_T, typename Mapped_T>
  typename Map<Key_T, Mapped_T>::Iterator Map<Key_T, Mapped_T>::find(const Key_T& key) {
    if (empty()) {
      return end();
    }

    int level;
    SkipNode *it = _head;
    for (level = _height - 1; level >= 0; level--) {
      while (it->next[level] != _sentinel && it->next[level]->data->first < key) {
        it = it->next[level];
      }
    }

    if (it->next[0] != _sentinel && it->next[0]->data->first == key) {
      return Iterator(it->next[0]);
    }

    return end();
  }

  template <typename Key_T, typename Mapped_T>
  typename Map<Key_T, Mapped_T>::ConstIterator Map<Key_T, Mapped_T>::find(const Key_T& key) const {
    // TODO Literally an exact copy of above, should find some way to make this better
    if (empty()) {
      return end();
    }

    int level;
    SkipNode *it = _head;
    for (level = _height - 1; level >= 0; level--) {
      while (it->next[level] != _sentinel && it->next[level]->data->first < key) {
        it = it->next[level];
      }
    }

    if (it->next[0]->data->first == key) {
      return ConstIterator(it->next[0]);
    }

    return end();
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
  const Mapped_T& Map<Key_T, Mapped_T>::at(const Key_T& key) const {
    auto it = find(key);
    if (it == end()) {
      throw std::out_of_range("Key not found");
    } else {
      return it->second;
    }
  }

  template <typename Key_T, typename Mapped_T>
  Mapped_T& Map<Key_T, Mapped_T>::operator[](const Key_T& key) {
    auto res = insert(std::make_pair(key, Mapped_T()));
    return res.first->second;
  }

  template <typename Key_T, typename Mapped_T>
  std::pair<typename Map<Key_T, Mapped_T>::Iterator, bool> Map<Key_T, Mapped_T>::insert(const ValueType& val) {
    if (empty()) {
      // Handle empty case
      SkipNode *sn = new SkipNode(val);
      int sn_height = SkipNode::rand_height();

      if (sn_height > _height) {
        _height = sn_height;
      }

      sn->next = std::vector<SkipNode *>(sn_height, _sentinel);

      sn->back = _head;
      _sentinel->back = sn;
      for (int i = 0; i < sn_height; i++) {
        _head->next[i] = sn;
      }

      _size++;
      return std::make_pair(Iterator(sn), true);
    }

    int level;
    SkipNode *it = _head;
    std::vector<SkipNode *> path(_height, _sentinel);
    for (level = _height - 1; level >= 0; level--) {
      while (it->next[level] != _sentinel && it->next[level]->data->first < val.first) {
        it = it->next[level];
      }
      path[level] = it;
    }

    if (it->next[0] != _sentinel && it->next[0]->data->first == val.first) {
      return std::make_pair(Iterator(it->next[0]), false);
    } else {
      // Get a height for the new entry
      // Make a new skip node and set the appropriate nexts
      // May need to remember where dropoff points were to set next
      // values appropriately
      SkipNode *sn = new SkipNode(val);
      int sn_height = SkipNode::rand_height();
      sn->next = std::vector<SkipNode *>(sn_height, _sentinel);

      int copy_height = sn_height;
      if (sn_height > _height) {
        // Adjust height
        copy_height = _height;
        _height = sn_height;

        // Make head point to the newest tallest lane
        for (int i = copy_height; i < sn_height; i++) {
          _head->next[i] = sn;
        }
      }

      // Set back pointers
      path[0]->next[0]->back = sn;
      sn->back = path[0];

      // Set next pointers
      for (int i = 0; i < copy_height; i++) {
        sn->next[i] = path[i]->next[i];
        path[i]->next[i] = sn;
      }

      _size++;
      return std::make_pair(Iterator(sn), true);
    }
  }

  template <typename Key_T, typename Mapped_T>
  template <typename IT_T>
  void Map<Key_T, Mapped_T>::insert(IT_T range_beg, IT_T range_end) {
    for (auto it = range_beg; it != range_end; it++) {
      insert(*it);
    }
  }

  template <typename Key_T, typename Mapped_T>
  void Map<Key_T, Mapped_T>::erase(Iterator pos) {
    // What if if pos == end?
    if (pos == end()) {
      // Eh, stop it I guess?
      return;
    }

    // lazy af
    // this could probably be an O(1) op if only every Node had every back pointer
    erase(pos->first);
  }

  template <typename Key_T, typename Mapped_T>
  void Map<Key_T, Mapped_T>::erase(const Key_T& key) {
    // This operation could use find() if SkipNodes had matching back pointers
    // But we really need the path vector
    // Also pray that find actually works or I'm going to have to fix every instance
    // of it that I copied into insert and erase
    int level;
    SkipNode *it = _head;
    std::vector<SkipNode *> path(_height, _sentinel);
    for (level = _height - 1; level >= 0; level--) {
      while (it->next[level] != _sentinel && it->next[level]->data->first < key) {
        it = it->next[level];
      }
      path[level] = it;
    }

    SkipNode *target = it->next[0];
    if (target == _sentinel || !(target->data->first == key)) {
      throw std::out_of_range("Key not found");
    }

    // Adjust target's next back pointer
    target->next[0]->back = it;

    // Forward the prev's nexts to target's nexts
    int target_height = target->next.size();
    for (int i = 0; i < target_height; i++) {
      path[i]->next[i] = target->next[i];
    }

    // Kill target
    delete target;
    _size--;

    // TODO Maybe readjust _height if we just deleted the last of the tallest height
  }

  template <typename Key_T, typename Mapped_T>
  void Map<Key_T, Mapped_T>::clear() {
    // Assuming operator= works this should be fine
    *this = Map<Key_T, Mapped_T>();
  }


}

template class cs540::Map<std::string, std::string>;

#endif /* _MAP_H_ */
