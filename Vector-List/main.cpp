#include <cstddef>
#include <cstdlib>
#include <string>
#include <chrono>

#include "Vector.h"
#include "LinkedList.h"

namespace
{

template <typename T>
using LinearCollection = aisdi::LinkedList<T>;

template <typename T>
using Vector = aisdi::Vector<T>;

std::chrono::duration<double> overall_append_time;
std::chrono::duration<double> overall_prepend_time;
std::chrono::duration<double> overall_insert_time;
std::chrono::duration<double> overall_popfirst_time;
std::chrono::duration<double> overall_poplast_time;
std::chrono::duration<double>overall_erase_time;
std::chrono::duration<double>overall_erase_range_time;

std::chrono::duration<double> v_overall_append_time;
std::chrono::duration<double>v_overall_prepend_time;
std::chrono::duration<double>v_overall_insert_time;
std::chrono::duration<double>v_overall_popfirst_time;
std::chrono::duration<double>v_overall_poplast_time;
std::chrono::duration<double>v_overall_erase_time;
std::chrono::duration<double>v_overall_erase_range_time;

void perfomTest()
{
  LinearCollection<std::string> collection;
  Vector<std::string> collection2;

  //append

  auto start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 10000; i++)
    collection.append("DONE");
  auto stop = std::chrono::high_resolution_clock::now();
  overall_append_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 10000; i++)
    collection2.append("DONE");
  stop = std::chrono::high_resolution_clock::now();
  v_overall_append_time += stop - start;

  for(size_t i = 0; i < 10000; i++)
    collection.append("DONE");

  for(size_t i = 0; i < 10000; i++)
    collection2.append("DONE");

  //prepend

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 10000; i++)
    collection.prepend("DONE");
  stop = std::chrono::high_resolution_clock::now();
  overall_prepend_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 10000; i++)
    collection2.prepend("DONE");
  stop = std::chrono::high_resolution_clock::now();
  v_overall_prepend_time += stop - start;

  //insert

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 1000; i++)
    collection.insert(collection.begin() + 20,"DONE");
  stop = std::chrono::high_resolution_clock::now();
  overall_insert_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 1000; i++)
    collection2.insert(collection2.begin() + 20,"DONE");
  stop = std::chrono::high_resolution_clock::now();
  v_overall_insert_time += stop - start;

  //poplast

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 10000; i++)
    collection.popLast();
  stop = std::chrono::high_resolution_clock::now();
  overall_poplast_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 10000; i++)
    collection2.popLast();
  stop = std::chrono::high_resolution_clock::now();
  v_overall_poplast_time += stop - start;

  //popfirst

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 10000; i++)
    collection.popFirst();
  stop = std::chrono::high_resolution_clock::now();
  overall_popfirst_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 10000; i++)
    collection2.popFirst();
  stop = std::chrono::high_resolution_clock::now();
  v_overall_popfirst_time += stop - start;

  //filling collections

  for(size_t i = 0; i < 100000; i++)
    collection.append("DONE");

  for(size_t i = 0; i < 100000; i++)
    collection2.append("DONE");

  //erase

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 1000; i++)
    collection.erase(collection.begin() + 40);
  stop = std::chrono::high_resolution_clock::now();
  overall_erase_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 1000; i++)
    collection2.erase(collection2.begin() + 40);
  stop = std::chrono::high_resolution_clock::now();
  v_overall_erase_time += stop - start;

  //erase range

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 100; i++)
    collection.erase(collection.begin() + 100, collection.end() - 100);
  stop = std::chrono::high_resolution_clock::now();
  overall_erase_range_time += stop - start;

  start = std::chrono::high_resolution_clock::now();
  for(size_t i = 0; i < 100; i++)
    collection2.erase(collection2.begin() + 100, collection2.end() - 100);
  stop = std::chrono::high_resolution_clock::now();
  v_overall_erase_range_time += stop - start;
}


}// namespace

int main()
{
  perfomTest();

  std::cout << "1 000 000 append time:         vector: " << v_overall_append_time.count() <<      " list: " << overall_append_time.count() << std::endl;
  std::cout << "Prepend time:                  vector: " << v_overall_prepend_time.count() <<     " list: " << overall_prepend_time.count() << std::endl;
  std::cout << "Insert in the middle time:     vector: " << v_overall_insert_time.count() <<      " list: " << overall_insert_time.count() << std::endl;
  std::cout << "1 000 000 popfirst time:       vector: " << v_overall_popfirst_time.count() <<    " list: " << overall_popfirst_time.count() << std::endl;
  std::cout << "Poplast time:                  vector: " << v_overall_poplast_time.count() <<     " list: " << overall_poplast_time.count() << std::endl;
  std::cout << "Erase in the middle time:      vector: " << v_overall_erase_time.count() <<       " list: " << overall_erase_time.count() << std::endl;
  std::cout << "10 000 erase range time:       vector: " << v_overall_erase_range_time.count() << " list: " << overall_erase_range_time.count() << std::endl;
  
  return 0;
}

