#pragma once

// Helper functions that get used throughout code 
#include <map>

inline double ent(const double e_rs, const double e_r, const double e_s)
{
  // OUT_MSG << "e_rs: " << e_rs
  //         << ", e_r: " << e_r
  //         << ", e_s: " << e_s
  //         << " -> " << e_rs * std::log(e_rs / (e_r * e_s))
  //         << std::endl;
  return e_rs * std::log(e_rs / (e_r * e_s));
}

template <typename T>
using Const_Ptr = const T*;

template <typename T> 
using Count_Map = std::map<T, int>;

template <typename T>
inline void reduce_edge_count(Count_Map<Const_Ptr<T>>& count_map, Const_Ptr<T> block, const int dec_amt)
{
  const int new_value = count_map[block] - dec_amt;
  // If we've reduced the value to zero, then remove from map
  if (new_value == 0) {
    count_map.erase(block);
  } else {
    count_map[block] = new_value;
  }
}

template <typename T>
inline void increase_edge_count(Count_Map<Const_Ptr<T>>& count_map, Const_Ptr<T> block, const int inc_amt)
{
  count_map[block] += inc_amt;
}