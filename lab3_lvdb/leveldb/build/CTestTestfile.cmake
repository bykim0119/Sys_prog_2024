# CMake generated Testfile for 
# Source directory: /home/bykim0119/sys_prog2024/lab3_lvdb/leveldb
# Build directory: /home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(leveldb_tests "/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/build/leveldb_tests")
set_tests_properties(leveldb_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/CMakeLists.txt;370;add_test;/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/CMakeLists.txt;0;")
add_test(c_test "/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/build/c_test")
set_tests_properties(c_test PROPERTIES  _BACKTRACE_TRIPLES "/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/CMakeLists.txt;396;add_test;/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/CMakeLists.txt;399;leveldb_test;/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/CMakeLists.txt;0;")
add_test(env_posix_test "/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/build/env_posix_test")
set_tests_properties(env_posix_test PROPERTIES  _BACKTRACE_TRIPLES "/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/CMakeLists.txt;396;add_test;/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/CMakeLists.txt;407;leveldb_test;/home/bykim0119/sys_prog2024/lab3_lvdb/leveldb/CMakeLists.txt;0;")
subdirs("third_party/googletest")
subdirs("third_party/benchmark")
