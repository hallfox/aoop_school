#pragma once

#include <iomanip>
#include <iostream>

namespace cs540 {
  template <typename T>
  class InterpStream;

  template <typename ...Ts>
  InterpStream<const Ts &...> Interpolate(const std::string &format_string, const Ts &... args) {
    std::tuple<const Ts &...> pack(args...);
    return InterpStream<const Ts &...>(format_string, pack);
  }

  template <typename T>
  class InterpStream {
  public:
    InterpStream() = delete;
    InterpStream(const std::string &format_string, const T &pack):
      format_(format_string),
      pack_(pack) {}

  private:
    template <std::size_t ...ls>
    std::ostream &process(std::ostream &, std::index_sequence<ls...> &);

    template <typename ...Ts>
    std::ostream &actually_process(std::ostream &, const T &, const Ts &...);
    // Base case
    std::string actually_process(std::ostream &os);

    friend std::ostream &operator<<(std::ostream &, const InterpStream &);

  private:
    std::string format_;
    T pack_;
  };

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const InterpStream<T> &is) {
    return is.process(os, std::make_index_sequence<std::tuple_size<decltype(is.pack_)>::value>());
  }

  template <typename T>
  template <std::size_t ...ls>
  std::ostream &InterpStream<T>::process(std::ostream &os, std::index_sequence<ls...> &) {
    return acutally_process(os, std::get<ls>(pack_)...);
  }

  template <typename T>
  template <typename ...Ts>
  std::ostream &InterpStream<T>::actually_process(std::ostream &os, const T &t, const Ts &...rest) {
    for (int i = 0; i < format_.size(); i++) {
      if (format_[i] == '%') {
        // If % detected
        if (i > 0 && format_[i-1] == '\\') {
          // If it's preceeded by a backslash
          if (i > 1 && format_[i-2] == '\\') {
            // If the backslash is escaped
            auto first = format_.substr(0, i);
            format_ = format_.substr(i);
            return os << first << t << actually_process(os, rest...);
          }
          continue;
        }
        auto first = format_.substr(0, i);
        format_ = format_.substr(i);
        return os << first << t << actually_process(os, rest...);
      }
    }
  }

  template <typename T>
  std::string InterpStream<T>::actually_process(std::ostream &os) {
    return "";
  }
}
