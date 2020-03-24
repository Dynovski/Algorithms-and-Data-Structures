#include <cstddef>
#include <cstdlib>
#include <string>
#include <chrono>
#include <iostream>

#include "TreeMap.h"
#include "HashMap.h"

namespace
{

template <typename K, typename V>
using TreeMap = aisdi::TreeMap<K, V>;

template <typename K, typename V>
using HashMap = aisdi::HashMap<K, V>;

std::chrono::duration<double> overall_adding_time;
std::chrono::duration<double> overall_finding_time;
std::chrono::duration<double> overall_removing_time;

std::chrono::duration<double> h_overall_adding_time;
std::chrono::duration<double> h_overall_finding_time;
std::chrono::duration<double> h_overall_removing_time;

void perfomTest()
{
  TreeMap<int, int> tree;
  HashMap<int, int> hash;

//adding

  auto start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 16000000; i++)
    tree[i] = i;
  auto stop = std::chrono::high_resolution_clock::now();
  overall_adding_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 16000000; i++)
    hash[i] = i;
  stop = std::chrono::high_resolution_clock::now();
  h_overall_adding_time += stop - start;

//finding

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 16000000; i++)
    tree.find(i);
  stop = std::chrono::high_resolution_clock::now();
  overall_finding_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 16000000; i++)
    hash.find(i);
  stop = std::chrono::high_resolution_clock::now();
  h_overall_finding_time += stop - start;

//removing

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 16000000; i++)
    tree.remove(i);
  stop = std::chrono::high_resolution_clock::now();
  overall_removing_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 16000000; i++)
    hash.remove(i);
  stop = std::chrono::high_resolution_clock::now();
  h_overall_removing_time += stop - start;

}

} // namespace

int main()
{
  perfomTest();

  std::cout << "Adding time:    tree: " << overall_adding_time.count() <<   " hash: " << h_overall_adding_time.count() << std::endl;
  std::cout << "Finding time:   tree: " << overall_finding_time.count() <<  " hash: " << h_overall_finding_time.count() << std::endl;
  std::cout << "Removing time:  tree: " << overall_removing_time.count() << " hash: " << h_overall_removing_time.count() << std::endl;

  return 0;
}
