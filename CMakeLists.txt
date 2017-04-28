cmake_minimum_required(VERSION 3.7)
project(ZTE_toy)

set(CMAKE_CXX_STANDARD 11)

set(SYSTEM "x86-64_linux")
set(LIBFORMAT "static_pic")
set(CPLEXDIR "/opt/ibm/ILOG/CPLEX_Studio1271/cplex")
set(CONCERTDIR "/opt/ibm/ILOG/CPLEX_Studio1271/concert")

# cplex
include_directories(${CPLEXDIR}/include)
link_directories(${CPLEXDIR}/lib/${SYSTEM}/${LIBFORMAT})
# cplex concert
include_directories(${CONCERTDIR}/include)
link_directories(${CONCERTDIR}/lib/${SYSTEM}/${LIBFORMAT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -DIL_STD")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lm -lpthread")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lemon")

find_package(PkgConfig)
pkg_search_module(CAIRO REQUIRED cairo)
include_directories(${CAIRO_INCLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})
link_libraries(${CAIRO_LIBRARIES})
link_libraries(concert)
link_libraries(ilocplex)
link_libraries(cplex)
link_libraries(pthread)
link_libraries(m)


set(SOURCE_FILES toy.cpp instance.h toy.h milp.h draw.h algorithms.h)
add_executable(toy ${SOURCE_FILES})

