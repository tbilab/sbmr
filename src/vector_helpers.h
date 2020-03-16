#ifndef __VECTOR_HELPERS_INCLUDED__
#define __VECTOR_HELPERS_INCLUDED__

#include <utility>
#include <vector>
#include <random>
#include "error_and_message_macros.h"


template <typename T>
using Vec_of_Vecs = std::vector<std::vector<T>>;


template <typename T>
using U_Ptr = std::unique_ptr<T>;

template <typename T>
using U_Ptr_Vec = std::vector<U_Ptr<T>>;

template <typename T>
bool delete_from_vector(std::vector<T>& vec, const T to_remove) {
  // Get iterator to the element we're deleting
  auto it = std::find(vec.begin(), vec.end(), to_remove);

  // If requested element couldn't be found, return false
  if (it == vec.end())
    return false;

  // Swap element to delete and last element in vector
  std::swap(*it, vec.back());

  // Remove/delete last element of vector
  vec.pop_back();

  return true;
}

template <typename T>
bool delete_from_vector(U_Ptr_Vec<T>& vec, const T* el) {
  // Lambda function to compare smart pointers and normal pointer
  auto find_el = [&el](const U_Ptr<T>& ptr) { return ptr.get() == el; };

  // Get iterator to the element we're deleting
  auto it = std::find_if(vec.begin(), vec.end(), find_el);

  // If requested element couldn't be found, return false
  if (it == vec.end())
    return false;

  // Swap element to delete and last element in vector
  std::swap(*it, vec.back());

  // Remove/delete last element of vector
  vec.pop_back();

  return true;
}

// Total number of elements in a vector of vectors
template <typename T>
int total_num_elements(const Vec_of_Vecs<T>& vec_of_vecs) {
  int total = 0;
  for (const auto& sub_vec : vec_of_vecs) {
    total += sub_vec.size();
  }
  return total;
}

// Total number of elements in a vector of vectors of vectors... all the way down
template <typename T>
int total_num_elements(const std::vector<Vec_of_Vecs<T>>& vec_of_vec_of_vecs) {
  int total = 0;
  for (const auto& vec_of_vecs : vec_of_vec_of_vecs) {
    total += total_num_elements(vec_of_vecs);
  }
  return total;
}


// Total number of elements in a vector of vectors
template <typename T>
T& get_random_element(Vec_of_Vecs<T>& vec_of_vecs, std::mt19937& random_generator) {
  // Make a random uniform to index into vectors
  const int n = total_num_elements(vec_of_vecs);
  if (n == 0) RANGE_ERROR("Can't take a random sample of empty vectors");

  std::uniform_int_distribution<> runif {0, n - 1};

  int random_index = runif(random_generator);

  // Loop through subvectors and see if we can index into sub vector with random index
  // If we can't then subtract the current subvector size from random index and keep going
  for (auto& sub_vec : vec_of_vecs) {
    const int current_size = sub_vec.size();
    if(current_size <= random_index){
      random_index -= current_size;
    } else {
      return sub_vec[random_index];
    }
  }
  LOGIC_ERROR("Random element could not be selected. Check formation of vectors");
  // Default return is just the first element... potentially dangerous
  return vec_of_vecs.at(0).at(0);
}

template <typename T>
T& get_random_element(std::vector<T>& vec, std::mt19937& random_generator) {

  // Make a random uniform to index into vectors
  std::uniform_int_distribution<> runif {0, int(vec.size() - 1)};

  return vec[runif(random_generator)];
}

#endif
