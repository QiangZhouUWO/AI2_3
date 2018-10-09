cmake_minimum_required(VERSION 3.9)
project(Ngrams)

set(CMAKE_CXX_STANDARD 11)

add_executable(Ngrams test.cpp fileRead.cpp fileRead.h vectorHash.h utilsToStudents.h)