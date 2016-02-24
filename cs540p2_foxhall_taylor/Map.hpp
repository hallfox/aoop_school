#ifndef _MAP_H_
#define _MAP_H_

#include <utility>

namespace cs540 {
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
      int _p;
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
    Mapped_T& operator[](const Key_T&) const;

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
  };

  template <typename Key_T, typename Mapped_T>
  Map<Key_T, Mapped_T>::Map() {
  }
}


#endif /* _MAP_H_ */
