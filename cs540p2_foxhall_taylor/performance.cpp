#include "Map.hpp"
#include <chrono>
#include <random>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>
#include <assert.h>
#include <map>
#include <initializer_list>
#include <set>

//Enables iteration test on a map larger than the memory available to the remote cluster
//WARNING: This will be VERY slow.
#define DO_BIG_ITERATION_TEST 0

namespace cs540 {
  template <typename K, typename V>
  class StdMapWrapper {
  private:
    using base_map = std::map<K, V>;
    
  public:
    typedef typename base_map::iterator Iterator;
    typedef typename base_map::const_iterator ConstIterator;
    typedef typename base_map::reverse_iterator ReverseIterator;
    typedef typename base_map::const_reverse_iterator ConstReverseIterator;
    typedef typename base_map::value_type value_type;
    typedef typename base_map::mapped_type mapped_type;
    typedef typename base_map::key_type key_type;
    
    StdMapWrapper() {}
    StdMapWrapper(std::initializer_list<std::pair<K,V>> il) {
      for(auto x : il) {
        m_map.insert(x);
      }
    }
    
    StdMapWrapper(StdMapWrapper &&other)
      : m_map(std::move(other.m_map))
    {}
    
    StdMapWrapper(const StdMapWrapper &other)
      : m_map(other.m_map)
    {}
    
    StdMapWrapper &operator=(const StdMapWrapper &other) {
      if(this != &other) {
        StdMapWrapper tmp(other);
        std::swap(m_map, tmp.m_map);
      }
      return *this;
    }
    
    StdMapWrapper &operator=(const StdMapWrapper &&other) {
      StdMapWrapper tmp(other);
      std::swap(tmp.m_map, m_map);
      return *this;
    }
    
    ///////// Iterators
    Iterator begin() {
      return m_map.begin();
    }
    
    ConstIterator begin() const {
      return m_map.begin();
    }
    
    ConstIterator cbegin() const {
      return m_map.begin();
    }
    
    ReverseIterator rbegin() {
      return m_map.rbegin();
    }
    
    /*
      ConstReverseIterator rbegin() const {
      return m_map.rbegin();
      }
      
      ConstReverseIterator crbegin() const {
      return m_map.crbegin();
      }
    */
    
    Iterator end() {
      return m_map.end();
    }
    
    ConstIterator end() const {
      return m_map.end();
    }
    
    ConstIterator cend() const {
      return m_map.cend();
    }
    
    ReverseIterator rend() {
      return m_map.rend();
    }
    
    /*
      ConstReverseIterator rend() const {
      return m_map.rend();
      }
    
      ConstReverseIterator crend() const {
      return m_map.crend();
      }
    */
    
    ///////// Capacity
    size_t size() const {
      return m_map.size();
    }
    
    size_t max_size() const {
      return m_map.max_size();
    }
    
    bool empty() const {
      return m_map.empty();
    }
    
    
    ///////// Modifiers
    Iterator insert(const value_type &value) {
      return m_map.insert(value).first;
    }
    
    Iterator insert(value_type &&value) {
      return m_map.insert(std::move(value)).first;
    }
    
    void erase(const K &k) {
      m_map.erase(k);
    }
    
    
    void erase(Iterator it) {
      m_map.erase(it);
    }
    
    ///////// Lookup
    V &at(const K &k) {
      return m_map.at(k);
    }
    
    const V &at(const K &k) const {
      return m_map.at(k);
    }
    
    Iterator find(const K &k) {
      return m_map.find(k);
    }
    
    ConstIterator find(const K &k) const {
      return m_map.find(k);
    }
    
    V &operator[](const K &k) {
      return m_map[k];
    }
    
    
  private:
    base_map m_map;
    
    template<typename A, typename B>
    friend
    bool operator==(const StdMapWrapper<A,B>&, const StdMapWrapper<A,B>&);
    
    template<typename A, typename B>
    friend bool operator!=(const StdMapWrapper<A,B>&, const StdMapWrapper<A,B>&);
    template<typename A, typename B>
    friend bool operator<=(const StdMapWrapper<A,B>&, const StdMapWrapper<A,B>&);
    template<typename A, typename B>
    friend bool operator<(const StdMapWrapper<A,B>&, const StdMapWrapper<A,B>&);
    template<typename A, typename B>
    friend bool operator>=(const StdMapWrapper<A,B>&, const StdMapWrapper<A,B>&);
    template<typename A, typename B>
    friend bool operator>(const StdMapWrapper<A,B>&, const StdMapWrapper<A,B>&);
  };
  
  template<typename K, typename T>
  bool operator==(const StdMapWrapper<K,T> &a, const StdMapWrapper<K,T> &b) {
    return a.m_map == b.m_map;
  }
  
  template<typename K, typename T>
  bool operator!=(const StdMapWrapper<K,T> &a, const StdMapWrapper<K,T> &b) {
    return a.m_map != b.m_map;
  }
  
  template<typename K, typename T>
  bool operator<=(const StdMapWrapper<K,T> &a, const StdMapWrapper<K,T> &b) {
    return a.m_map <= b.m_map;
  }
  
  template<typename K, typename T>
  bool operator<(const StdMapWrapper<K,T> &a, const StdMapWrapper<K,T> &b) {
    return a.m_map < b.m_map;
  }
  
  template<typename K, typename T>
  bool operator>=(const StdMapWrapper<K,T> &a, const StdMapWrapper<K,T> &b) {
    return a.m_map >= b.m_map;
  }
  
  template<typename K, typename T>
  bool operator>(const StdMapWrapper<K,T> &a, const StdMapWrapper<K,T> &b) {
    return a.m_map > b.m_map;
  }
  
}

using Milli = std::chrono::duration<double, std::ratio<1,1000>>;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

void dispTestName(const char *testName, const char *typeName) {
  std::cout << std::endl << std::endl << "************************************" << std::endl;
  std::cout << "\t" << testName << " for " << typeName << "\t" << std::endl;
  std::cout << "************************************" << std::endl << std::endl;
}

template <typename T>
T ascendingInsert(int count, bool print = true) {
  using namespace std::chrono;
  TimePoint start, end;
  start = system_clock::now();
  T map; 
  for(int i = 0; i < count; i++) {
    map.insert(std::pair<int, int>(i,i));
  }
  end = system_clock::now();
  
  Milli elapsed = end - start;
  
  if(print)
    std::cout << "Inserting " << count << " elements in aescending order took " << elapsed.count() << " milliseconds" << std::endl;
  
  return map;
}

template <typename T>
T descendingInsert(int count, bool print = true) {
  using namespace std::chrono;
  TimePoint start, end;
  start = system_clock::now();
  T map; 
  for(int i = count; i > 0; i--) {
    map.insert(std::pair<int, int>(i,i));
  }
  end = system_clock::now();
  
  Milli elapsed = end - start;
  
  if(print)
    std::cout << "Inserting " << count << " elements in descending order took " << elapsed.count() << " milliseconds" << std::endl;
  return map;
}

template <typename T>
void deleteTest() {
  using namespace std::chrono;
  TimePoint start, end;
  T m1 = ascendingInsert<T>(10000, false);
  T m2 = ascendingInsert<T>(100000, false);
  T m3 = ascendingInsert<T>(1000000, false);
  T m4 = ascendingInsert<T>(10000000, false);
  
  std::set<int> toDelete;
  for(int i = 0; i < 10000; i++) {
    toDelete.insert(i);
  }
  
  start = system_clock::now();
  for(const int e : toDelete)
    m1.erase(e);
  end = system_clock::now();
  
  Milli elapsed1 = end - start;
  
  std::cout << "deleting 10000 elements from a map of size 10000 took " << elapsed1.count() << " milliseconds" << std::endl;
  
  {
    toDelete.clear();
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,99999);
    while(toDelete.size() < 10000) {
      toDelete.insert(distribution(generator));
    }
  }
  
  start = system_clock::now();
  for(const int e : toDelete)
    m2.erase(e);
  end = system_clock::now();
  
  Milli elapsed2 = end - start;
  
  std::cout << "deleting 10000 elements from a map of size 100000 took " << elapsed2.count() << " milliseconds" << std::endl;
  
  {
    toDelete.clear();
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,999999);
    while(toDelete.size() < 10000) {
      toDelete.insert(distribution(generator));
    }
  }
  
  start = system_clock::now();
  for(const int e : toDelete)
    m3.erase(e);
  end = system_clock::now();
  
  Milli elapsed3 = end - start;
  
  std::cout << "deleting 10000 elements from a map of size 1000000 took " << elapsed3.count() << " milliseconds" << std::endl;
  
  {
    toDelete.clear();
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,9999999);
    while(toDelete.size() < 10000) {
      toDelete.insert(distribution(generator));
    }
  }
  
  start = system_clock::now();
  for(const int e : toDelete)
    m4.erase(e);
  end = system_clock::now();
  
  Milli elapsed4 = end - start;
  
  std::cout << "deleting 10000 elements from a map of size 10000000 took " << elapsed4.count() << " milliseconds" << std::endl;
}

template <typename T>
void findTest() {
  using namespace std::chrono;
  TimePoint start, end;
  T m1 = ascendingInsert<T>(10000, false);
  T m2 = ascendingInsert<T>(100000, false);
  T m3 = ascendingInsert<T>(1000000, false);
  T m4 = ascendingInsert<T>(10000000, false);
  T m11;
  T m22;
  T m33;
  T m44;
  
  std::vector<int> toFind;
  for(int i = 0; i < 10000; i++) {
    toFind.push_back(i);
  }
  
  start = system_clock::now();
  for(const int e : toFind) {
    auto it = m1.find(e);
    m11.insert(*it);
  }
  end = system_clock::now();
  
  Milli elapsed1 = end - start;
  
  std::cout << "Finding 10000 elements from a map of size " << m1.size() << " took " << elapsed1.count() << " milliseconds" << std::endl;
  
  {
    toFind.clear();
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,99999);
    while(toFind.size() < 10000) {
      toFind.push_back(distribution(generator));
    }
  }
  
  start = system_clock::now();
  for(const int e : toFind) {
    auto it = m2.find(e);
    m22.insert(*it);
  }
  end = system_clock::now();
  
  Milli elapsed2 = end - start;
  
  std::cout << "Finding 10000 elements from a map of size " << m2.size() << " took " << elapsed2.count() << " milliseconds" << std::endl;
  
  {
    toFind.clear();
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,999999);
    while(toFind.size() < 10000) {
      toFind.push_back(distribution(generator));
    }
  }
  
  start = system_clock::now();
  for(const int e : toFind) {
    auto it = m3.find(e);
    m33.insert(*it);
  }
  end = system_clock::now();
  
  Milli elapsed3 = end - start;
  
  std::cout << "Finding 10000 elements from a map of size " << m3.size() << " took " << elapsed3.count() << " milliseconds" << std::endl;
  
  {
    toFind.clear();
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,9999999);
    while(toFind.size() < 10000) {
      toFind.push_back(distribution(generator));
    }
  }
  
  start = system_clock::now();
  for(const int e : toFind) {
    auto it = m4.find(e);
    m44.insert(*it);
  }
  end = system_clock::now();
  
  Milli elapsed4 = end - start;
  
  std::cout << "Finding 10000 elements from a map of size " << m4.size() << " took " << elapsed4.count() << " milliseconds" << std::endl;
  
}

template <typename T>
void iterationTest(int count) {
  using namespace std::chrono;
  T m = ascendingInsert<T>(count,false);
  
  TimePoint start, end;
  
  for(int j = 0; j < 3; j++) {
    start = system_clock::now();
    for(auto it = m.begin(); it != m.end(); it++) {
      if(j==2)
        (*it).second += j;
    }
    end = system_clock::now();
  }
  
  Milli elapsed = end - start;
  
  std::cout << "Iterating across " << count << " elements in a map of size " << count << " took " << elapsed.count() << " milliseconds time per iteration was " << elapsed.count()/double(count)*1e6 << " nanoseconds" << std::endl;
}

template <typename T>
void copyTest(int count) {
  using namespace std::chrono;
  T m = ascendingInsert<T>(count,false);
  
  TimePoint start, end;
  
  start = system_clock::now();
  T m2(m);
  end = system_clock::now();

  Milli elapsed = end - start;
  
  std::cout << "Copy construction of a map of size " << m2.size() << " took " << elapsed.count() << " milliseconds" << std::endl;
}


/*
  #include <assert.h>

  using namespace std;

  ostream &
  operator<<(ostream &os, const type_info &ti) {
  int ec;
  const char *demangled_name = abi::__cxa_demangle(ti.name(), 0, 0, &ec);
  assert(ec == 0);
  os << demangled_name;
  free((void *) demangled_name);
  return os;
  }

  template <typename T>
  void foo(T &&o) {
  //o = 2;
  cout << typeid(const int &) << endl;
  }

  int main() {
  const int i = 1;
  foo(i);
  }
*/

class comma_numpunct : public std::numpunct<char> {
protected:
  virtual char do_thousands_sep() const { return ','; }
  virtual std::string do_grouping() const { return "\03"; }
};


int main() {
  //separate all printed numbers with commas
  std::locale comma_locale(std::locale(), new comma_numpunct());
  std::cout.imbue(comma_locale);
  
  auto demangle = [](const std::type_info &ti) {
    int ec;
    return abi::__cxa_demangle(ti.name(), 0, 0, &ec);
    assert(ec == 0);
  };
  
  const char *w = demangle(typeid(cs540::StdMapWrapper<int,int>));
  const char *m = demangle(typeid(cs540::Map<int,int>));
  
  {
    dispTestName("Ascending insert", m);
    ascendingInsert<cs540::Map<int,int>>(1000);
    ascendingInsert<cs540::Map<int,int>>(10000);
    ascendingInsert<cs540::Map<int,int>>(100000);
    ascendingInsert<cs540::Map<int,int>>(1000000);
    ascendingInsert<cs540::Map<int,int>>(10000000);
    dispTestName("Ascending insert", w);
    ascendingInsert<cs540::StdMapWrapper<int,int>>(1000);
    ascendingInsert<cs540::StdMapWrapper<int,int>>(10000);
    ascendingInsert<cs540::StdMapWrapper<int,int>>(100000);
    ascendingInsert<cs540::StdMapWrapper<int,int>>(1000000);
    ascendingInsert<cs540::StdMapWrapper<int,int>>(10000000);
  }
  
  {
    dispTestName("Descending insert", m);
    descendingInsert<cs540::Map<int,int>>(1000);
    descendingInsert<cs540::Map<int,int>>(10000);
    descendingInsert<cs540::Map<int,int>>(100000);
    descendingInsert<cs540::Map<int,int>>(1000000);
    descendingInsert<cs540::Map<int,int>>(10000000);
    dispTestName("Descending insert", w);
    descendingInsert<cs540::StdMapWrapper<int,int>>(1000);
    descendingInsert<cs540::StdMapWrapper<int,int>>(10000);
    descendingInsert<cs540::StdMapWrapper<int,int>>(100000);
    descendingInsert<cs540::StdMapWrapper<int,int>>(1000000);
    descendingInsert<cs540::StdMapWrapper<int,int>>(10000000);
  }
  
  {
    dispTestName("Delete test", m);
    deleteTest<cs540::Map<int,int>>();
    dispTestName("Delete test", w);
    deleteTest<cs540::StdMapWrapper<int,int>>();
  }
  
  {
    dispTestName("Find test", m);
    findTest<cs540::Map<int,int>>();
    dispTestName("Find test", w);
    findTest<cs540::StdMapWrapper<int,int>>();
  }
  
  /*
    Remember that some of these maps get quite large - iteration times may be affected by things other than the scaling of your algorithm.
    How do the many levels of the memory heirarchy in a computer relate?
    How do they perform relative to one another?
    How might this have affected other performance tests?
  */
  {
    dispTestName("Iteration test", m);
    iterationTest<cs540::Map<int,int>>(10000);
    iterationTest<cs540::Map<int,int>>(20000);
    iterationTest<cs540::Map<int,int>>(40000);
    iterationTest<cs540::Map<int,int>>(80000);
    iterationTest<cs540::Map<int,int>>(160000);
    iterationTest<cs540::Map<int,int>>(320000);
    iterationTest<cs540::Map<int,int>>(640000);
    iterationTest<cs540::Map<int,int>>(1280000);
    iterationTest<cs540::Map<int,int>>(2560000);
    iterationTest<cs540::Map<int,int>>(5120000);
#if DO_BIG_ITERATION_TEST
    //Optional test. This is more ram than the remote machines have and will likely take a long time to run.
    iterationTest<cs540::Map<int,int>>(600000000);
#endif
    dispTestName("Iteration test", w);
    iterationTest<cs540::StdMapWrapper<int,int>>(10000);
    iterationTest<cs540::StdMapWrapper<int,int>>(20000);
    iterationTest<cs540::StdMapWrapper<int,int>>(40000);
    iterationTest<cs540::StdMapWrapper<int,int>>(80000);
    iterationTest<cs540::StdMapWrapper<int,int>>(160000);
    iterationTest<cs540::StdMapWrapper<int,int>>(320000);
    iterationTest<cs540::StdMapWrapper<int,int>>(640000);
    iterationTest<cs540::StdMapWrapper<int,int>>(1280000);
    iterationTest<cs540::StdMapWrapper<int,int>>(5120000);
#if DO_BIG_ITERATION_TEST
  //Optional test. This is more ram than the remote machines have and will likely take a long time to run.
  iterationTest<cs540::Map<int,int>>(600000000);
#endif
  }
  
  {
    //Test copy constructor scaling
    dispTestName("Copy test", m);
    copyTest<cs540::Map<int,int>>(10000);
    copyTest<cs540::Map<int,int>>(100000);
    copyTest<cs540::Map<int,int>>(1000000);
    copyTest<cs540::Map<int,int>>(10000000);
    dispTestName("Copy test", w);
    copyTest<cs540::StdMapWrapper<int,int>>(10000);
    copyTest<cs540::StdMapWrapper<int,int>>(100000);
    copyTest<cs540::StdMapWrapper<int,int>>(1000000);
    copyTest<cs540::StdMapWrapper<int,int>>(10000000);
  }
  
  //Add your own indexibility scaling test here
}
