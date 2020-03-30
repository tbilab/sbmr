#pragma once

#include <unordered_set>

template<typename T>
T * ptr(T & obj) { return &obj; } //turn reference into pointer!

template<typename T>
T * ptr(T * obj) { return obj; } //obj is already pointer, return it!

template<typename T>
inline bool equal(const T& a, const T& b) { return *ptr(a) == *ptr(b);}

template<typename T>
inline bool less_than(const T& a, const T& b) { return *ptr(a) < *ptr(b); }

template <typename T>
class Ordered_Pair {
 private:
  T val_1;
  T val_2;

 public:
  Ordered_Pair(const T a, const T b)
      : val_1(less_than(a, b) ? a : b), val_2(less_than(a, b) ? b : a) {}

  std::pair<T, T> get() const { return std::make_pair(val_1, val_2); }

  T first() const { return val_1; }
  T second() const { return val_2; }

  bool is_matching() const { return equal(val_1, val_2); }
};

template <typename T>
bool operator==(const Ordered_Pair<T>& a, const Ordered_Pair<T>& b) {
  return equal(a.first(), b.first()) & equal(a.second(), b.second());
}

template <typename T>
bool operator<(const Ordered_Pair<T>& a, const Ordered_Pair<T>& b) {
  // If the first value is same as second, then check the second
  if (equal(a.first(), b.first())) {
    return less_than(a.second(), b.second());
  } else {
    return less_than(a.first(), b.first());
  }
}

// Hash function for ordered pairs so they can be used in hashed containers like
// unordered_map and unordered_set
template <typename T>
struct Ordered_Pair_Hash {
  size_t operator()(const Ordered_Pair<T> p) const {
    return std::hash<T>()(p.first()) ^ std::hash<T>()(p.second());
  }
};

template <typename T>
using Ordered_Pair_Set =
    std::unordered_set<Ordered_Pair<T>, Ordered_Pair_Hash<T>>;
