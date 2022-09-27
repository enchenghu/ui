cmake_minimum_required(VERSION 2.8)

#souce
aux_source_directory(. DIR_SRCS)

#create static libraries
add_library(os ${DIR_SRCS})


#libraries
target_link_libraries(os pthread)


