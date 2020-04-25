cmake_minimum_required(VERSION 3.3)
project(gh)

LINK_DIRECTORIES(/usr/lib64/)
set(SOURCE_FILES geohash.c geohash.h)

add_library(geohash SHARED geohash.c)

enable_testing()

add_executable(gh_test geohash_test.c)
target_link_libraries(gh_test geohash )

