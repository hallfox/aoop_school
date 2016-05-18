#pragma once

#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>
#include <assert.h>

#define return_type(fn, arg_t)                  \
  std::result_of<decltype(fn)&(arg_t)>::type

namespace cs540 {
  template <typename T>
  class InterpStream;

  template <typename ...Ts> struct TypeSet;
  template <typename T, typename U> struct In;

  template <typename T, typename ...Ts>
  struct In<T, TypeSet<T, Ts...> > {
    static constexpr bool value = true;
  };

  template <typename T, typename K, typename ...Ts>
  struct In<T, TypeSet<K, Ts...> > {
    static constexpr bool value = In<T, TypeSet<Ts...>>::value;
  };

  template <typename T>
  struct In<T, TypeSet<> > {
    static constexpr bool value = false;
  };

  template <typename ...Ts>
  InterpStream<std::tuple<const Ts &...> > Interpolate(const std::string &format_string, const Ts &... args) {
    std::tuple<const Ts &...> pack(args...);
    return InterpStream<decltype(pack)>(format_string, pack);
  }

  template <typename T>
  class InterpStream {
  public:
    InterpStream() = delete;
    InterpStream(const std::string &, const T &);

  private:
    template <std::size_t ...ls>
    std::ostream &process(std::ostream &, const std::index_sequence<ls...> &) const;

    template <typename U, typename ...Us>
    std::ostream &actually_process(std::ostream &, const U &, const Us &...) const;
    template <typename ...Us>
    std::ostream &actually_process(std::ostream &os, decltype(std::hex), const Us &...);
    template <typename U, typename ...Us,
              typename std::enable_if<In<U, TypeSet<
                                              return_type(std::resetiosflags, std::ios_base::fmtflags),
                                              return_type(std::setiosflags, std::ios_base::fmtflags),
                                              return_type(std::setbase, int),
                                              return_type(std::setprecision, int),
                                              return_type(std::setw, int)> >::value >::type* = nullptr>
    std::ostream &actually_process(std::ostream &os, const U &op, const Us &...rest) {
      os << op;
      actually_process(rest...);
    }
    // Base case
    std::ostream &actually_process(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const InterpStream &is) {
      return is.process(os, std::make_index_sequence<std::tuple_size<decltype(is.pack_)>::value>());
    }

  private:
    std::string format_;
    T pack_;
    mutable size_t w_head_;
  };

  class WrongNumberOfArgs: public std::runtime_error {
  public:
    WrongNumberOfArgs(const char *what): std::runtime_error(what) {}
    WrongNumberOfArgs(const std::string &what): std::runtime_error(what) {}
  };

  template <typename T>
  InterpStream<T>::InterpStream(const std::string &format_string, const T &pack):
    format_(format_string),
    pack_(pack),
    w_head_() {}

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const InterpStream<T> &is) {
    return is.process(os, std::make_index_sequence<std::tuple_size<decltype(is.pack_)>::value>());
  }

  template <typename T>
  template <std::size_t ...ls>
  std::ostream &InterpStream<T>::process(std::ostream &os, const std::index_sequence<ls...> &) const {
    return actually_process(os, std::get<ls>(pack_)...);
  }

  template <typename T>
  template <typename ...Us>
  std::ostream &InterpStream<T>::actually_process(std::ostream &os, decltype(std::hex) op, const Us &...rest) {
    os << op;
    return actually_process(os, rest...);
  }


  template <typename T>
  template <typename U, typename ...Us>
  std::ostream &InterpStream<T>::actually_process(std::ostream &os, const U &t, const Us &...rest) const {
    auto beg = w_head_;
    for (; w_head_< format_.size(); w_head_++) {
      if (format_[w_head_] == '%') {
        // If % detected
        if (w_head_ == 0 || format_[w_head_-1] != '\\') {
          // If it's not escaped
          os << format_.substr(beg, w_head_-beg) << t;
          w_head_++;
          return actually_process(os, rest...);
        } else {
          // Escaped percent
          os << format_.substr(beg, w_head_-beg-1) << '%';
          beg = w_head_ + 1;
        }
      }
    }
    throw WrongNumberOfArgs("Too many args");
  }

  template <typename T>
  std::ostream &InterpStream<T>::actually_process(std::ostream &os) const {
    auto beg = w_head_;
    for (; w_head_< format_.size(); w_head_++) {
      if (format_[w_head_] == '%') {
        if (w_head_ > 0 && format_[w_head_-1] == '\\') {
          // If it's escaped
          os << format_.substr(beg, w_head_-beg-1) << '%';
          beg = w_head_ + 1;
        } else {
          throw WrongNumberOfArgs("Too few args");
        }
      }
    }
    return os << format_.substr(beg, w_head_-beg);
  }


}
