/*
There are multiple functions that can be changed depending on the needs of the user needs.
For example, if the user knows the range of the memory block size they will be inserting into this 
memory pool, they can edit the sizeValues vector in the pool class. This will reduce 
internal fragmentation. Additionally, our system can only accept one type of pointers which is char*, 
thus if the blocks of memory all contain a single different type, you can edit the pointers to be of that type.
Lastly, many of our algorithms and data structures are from the std library, which means they are intended for 
more generic purposes and are not optimized for speed and memory efficiency.

For the user to use this memory pool, they will need to provide the size in bytes of the memory to 
instantiate an alloc object. Afterwards, they can call the 'put' function to insert a block of memory. 
The 'get' function to retrieve the allocated block of memory. And 'delete' to erase the allocated block 
of memory.


*/

#ifndef _classes_h_
#define _classes_h_

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <stdio.h>

class size_class {

public:
  /* Member variables */

  std::vector<std::list<char *>>
      page_list; // this will contain 4kb blocks of memory linked together

  size_class(int val, int classMem); // constructor for the specific Memory size
  ~size_class();

  size_class &operator=(const size_class &old);

  std::vector<char *> insert_page(char *value, int memSize);
  void delete_page(std::vector<char *> memoryAddress);

private:
  void createMemBlocks(
  std::vector<std::list<char *> > page_list); // partitions the memory
  std::vector<char *> copyLessThan(char *buffer, int memSize);
  std::vector<char *> copyGreaterThan(char *buffer, int memSize);
  int size;       // this is the size of the pages depends on the index
  int numofPages; // this is how many pages of the specific size we need
};

size_class::size_class(int val, int classMem) {
  /*
      creates an instance of a size_class for the particular val
  */
  size = val;
  // determines the number of pages that a particular size class should have
  numofPages = (classMem / val);

  page_list = std::vector<std::list<char *>>(2);
  createMemBlocks(page_list);
}

size_class::~size_class(){
  auto it = page_list[0].begin();
  while(!page_list[0].empty()){
    free(*it);
    it = page_list[0].erase(it);
  }
  it = page_list[1].begin();
  while(!page_list[1].empty()){
    free(*it);
    it = page_list[1].erase(it);
  }
}

std::vector<char *> size_class::insert_page(char *buffer, int memSize) {

  /* 
     Inserts a copied pointer to memory into our page list
     Returns a vector of page(s) pointer
     buffer: pointer to inserted memory
     memSize: size of the object we want to store
  */

  // transfer the memory to current system
  // stores the location of the memory location in the linkedlist (moves from
  // empty -> full list)

  if (memSize <= size) {
    return copyLessThan(buffer, memSize);    // copy is a placeholder for copying
                                             // the external memory into our system
  } else {
    return copyGreaterThan(buffer, memSize); // copy is a placeholder for copying
                                             // the external memory into our system
  }
}

std::vector<char *> size_class::copyLessThan(char *buffer, int memSize) {
  /*
      transfers value's memory block to the zspool memory
      WE CALL THIS FUNCTION WHEN THE SIZE CLASS IS SMALLER OR EQUAL TO
      THE EXTERNAL MEMORY
      buffer: is the location of key's memory address in the external memory
      returns a vector of page(s)
      pool: is the location that we store the data from the buffer
      memSize: is the size of the memory we are storing
  */
  std::vector<char *> ret;
  
  // copy memory between buffer and pool
  std::copy( (char*) buffer, (char*) buffer + memSize, page_list[0].front());
  
  // rearrange pages
  page_list[1].push_back(page_list[0].front());
  ret.push_back(page_list[0].front());
  page_list[0].pop_front();

  return ret;
}
std::vector<char *> size_class::copyGreaterThan(char *buffer, int memSize) {
  /*
     transfers value's memory block to the zspool memory
     WE CALL THIS FUNCTION WHEN THE SIZE CLASS IS GREATER THAN
     THE EXTERNAL MEMORY
     THEREFORE WE WILL NEED MORE THAN ONE MEMORY BLOCK TO COPY THE DATA
     buffer: is the location of key's memory address in the external memory
     returns an array of void pointers
     pool: is the location that we store the data from the buffer
     memSize: is the size of the memory we are storing
 */
  std::vector<char *> ret;
  int num = ceil(memSize / size);

  int start = 0;
  int end = size;

  for (int i = 0; i < num; i++) {
    std::copy((char*)buffer + start, (char*)buffer + end, page_list[0].front());

    // moves the pages between empty and full page_lists
    page_list[1].push_back(page_list[0].front());
    ret.push_back(page_list[0].front());
    page_list[0].pop_front();

    // increments page size
    start += size;
    end += size;
    end = std::min(end, memSize);
  }
  return ret;
}

void size_class::delete_page(std::vector<char *> memoryAddress) {
  /*
      find the element in list that has the size memory address as memoryAddress
      erases that memory pointer from the full list (page_list[1]) and allocates
      pointer to the empty list (page_list[0])
      memoryAddress: is the location of the key's memory address that we want to
     delete in our system
  */

  auto list_itr = page_list[1].begin();
  int count = memoryAddress.size();
  while (list_itr != page_list[1].end()) {
    if (*list_itr == memoryAddress[0]) {
      for (int i = 0; i < count; i++) {
        list_itr = page_list[1].erase(list_itr);
      }
      break;
    }
    list_itr++;
  }
  for (int i = 0; i < count; i++) {
    page_list[0].push_back(memoryAddress[i]);
  }
}

void size_class::createMemBlocks(std::vector<std::list<char *>> page_list) {
  /*
      we want to partition numOfPages amount of memory block in our system
      and add the pointer to these memory address into our empty page_list[0]
      to indicate that these memory blocks are not in use
      page_list: is an array of linkedlist
  */

  for (int i = 0; i < numofPages; i++) {
    // method that determines the size of the page depending on size_Class index
    char *page = (char*) std::malloc(size); // still an issue with call to calloc
    page_list[0].push_back(page);
  }
}

/* ------------------------ MEMORY POOL ------------------------ */

class pool {
public:
  std::string name;      // pool1, pool2, pool3
  pool(int totalMemory); // constructor
  ~pool();

  // make sure the sizeValues are in sorted order 
  std::vector<int> sizeValues = {32,   48,   64,   128,  256,  512,  1024,
                                 2048, 2336, 2448, 2720, 3072, 3264, 4096};
  size_class **classes = new size_class *[sizeValues.size()];
  int getSize(int index) { return sizeValues[index]; }
  int getSizeIndex(int size);
};

pool::pool(int totalMemory) {
  /*
  totalMemory:
  */
  int classMem = totalMemory / sizeValues.size();
  std::sort(sizeValues.begin(), sizeValues.end());
  for (int i = 1; i <= (int)sizeValues.size(); i++) {
    classes[i - 1] =
        new size_class(sizeValues[i - 1], classMem); // glasses = [->sc1, ->sc2, ->sc3]
  }
}

pool::~pool(){
  for(int i = 0; i < (int)sizeValues.size(); i++){
    delete classes[i];
  }
  delete[] classes;
}

int pool::getSizeIndex(int size) {
  /*
  size:
  */
  
  int low = 0, high = sizeValues.size() - 1;
  int mid;
  while (low <= high) {
    mid = (low + high) / 2;
    if (sizeValues[mid] == size) {
      return mid;
    } else if (sizeValues[mid] > size) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }
  return -1; // does not exist in the array, should not happen though
}

// HASHMAP FOR SIZES THE VALUE IS AN INDEX NOT THE SIZE OF THE CLASS change name
// to be easier
class alloc {
public:
  int getRemainingMem() { return remainingMemory; }

  alloc(int totalMemory); // constructor
  ~alloc();

  // Main Methods
  void put(std::string, int size, char *value);
  void get(std::string key, char *buffer);
  void del(std::string key);
  void transfer(std::vector<char *> &memoryAddress, char *buffer, int size);
  int determineSizeClass(int index, int size, bool iter);

private:
  std::unordered_map<std::string, std::vector<char *>> keys;
  std::unordered_map<std::string, int> index;
  pool * mem_pool;
  int totalMemory;
  int remainingMemory;
};

alloc::alloc(int memory) {
  /* int memory is the total memory in bytes*/
  totalMemory = memory;
  remainingMemory = memory;
  mem_pool = new pool(totalMemory);
}

alloc::~alloc(){
  delete mem_pool;
}
// the put function takes in a block of memory in BYTES
void alloc::put(std::string key, int size, char *value) {
  /*
     Pairs the key and value together inside the memory pool and the map
     Locates the correct size class index within the pool to insert the memory
     into and return an address to that memory which can be used later on to
     access the information again. key: is a string that the user uses to
     reference the block of memory size: is the size of the block of memory the
     user wants us to copy value: is the pointer to the block of memory in the
     user's interface
     key:
     size:
     value:
  */

  std::vector<char *> memoryAddress;
  int sizeC; // this will hold the index of whatever SizeClass it'll belong to

  // determines a prepartition sizeClass that can hold the memory
  int optimalSize = 0;
  for (int i = 0; i < (int)mem_pool->sizeValues.size(); i++) {
    if (mem_pool->sizeValues[i] >= size) {
      optimalSize = mem_pool->sizeValues[i];
      break;
    }
  }
  int i = mem_pool->getSizeIndex(size);
  if (remainingMemory > optimalSize) {
    sizeC = determineSizeClass(i, optimalSize, true);
    // this is the index of the size class it will be place in
  } else {
    sizeC = -1;
  }
  if (sizeC == -1) {
    std::cout << "Memory Pool is full." << std::endl;
    return;
  }
  memoryAddress = this->mem_pool->classes[sizeC]->insert_page(
      value, mem_pool->sizeValues[sizeC - 1]);

  keys.insert(make_pair(key, memoryAddress));
  index.insert(make_pair(key, sizeC));
}

int alloc::determineSizeClass(int index, int size, bool iter) {
  /*
  Finds the index of the size_class corresponding to the given size
  size: is the original inputted memory size that the user wanted to check was
  available index: is the location of the original size
  index:
  iter:
  */

  int i = mem_pool->getSizeIndex(size);
  int originalSize = mem_pool->sizeValues[index];

  // page_list[0] is not empty
  if (index >= i && !this->mem_pool->classes[i]->page_list[0].empty()) {
    return i;
  }
  // if the page is smaller, checks that there is enough small pages to fit the
  // block of memory
  else if (this->mem_pool->classes[i]->page_list[0].size() >
           ceil(originalSize / size)) {
    return i;
  }

  if (i == index && iter == false) {
    return -1;
  }

  if ((int)mem_pool->sizeValues.size() - 1 > index) {
    return this->determineSizeClass(i + 1, size, false);
  } else {
    return this->determineSizeClass(0, size, false);
  }

  return -1; // shouldn't reach this return
}

void alloc::get(std::string key, char *buffer) {
  /*
  key:
  buffer:
  */
  
  transfer(keys[key], buffer, index[key]);
}

void alloc::del(std::string key) {
  /*
  key:
  */
  this->mem_pool->classes[index[key]]->delete_page(keys[key]);
  for(void* ptr: keys[key]){
    std::memset(ptr, 0, mem_pool->getSize(index[key]));
  }
  keys.erase(key);
  index.erase(key);
}

void alloc::transfer(std::vector<char *> &memoryAddress, char *buffer,
                        int index) {
  /*
      Is the reverse of copy
      The purpose of this function is to transfer the objects in
      our memory space into the user's memory space
      memoryAddress: is the location of the memory that the user wants
      copied into their memory space
      buffer: is the memory address of the user
      size: is the index of the size class
  */

  int sizeC = mem_pool->getSize(index);
  int count = 0;
  for (int i = 0; i < (int)memoryAddress.size(); i++) {
    std::copy( (char*) memoryAddress[i], (char*) memoryAddress[i] + sizeC, (char*) buffer + count);
    count += sizeC;
  }
}

#endif
