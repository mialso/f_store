###storage

#####current version: 0.0.1, pre-alpha basic implementation

#####purpose:
   to handle Create Read Update Delete entity operations on memory-mapped
   file

#####implementation goals:
   persistence (file), quick insert-delete(memory mapped) 

###current implementation:
1. data: one structure
2. tests: very simple testing, based on debug implemented

###todo:
* refactor thread-safey and reentrancy
* refactor tests to suites with proper use-case handling
* refactor to support different data structure types
