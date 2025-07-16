cmake_minimum_required(VERSION 3.10)

project(Bassil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Bassil 
    src/main.cpp 
    src/errorhandler.cpp
    src/file.cpp 
    src/levenshtein.cpp 
    src/lexer.cpp 
    src/parser.cpp 
    src/runargs.cpp

    # for better IDE support
    src/errorhandler.hpp
    src/file.hpp
    src/levenshtein.hpp
    src/lexer.hpp
    src/parser.hpp
    src/runargs.hpp
    src/tokens.hpp
    src/argparse.hpp
    src/indicators.hpp
)

target_include_directories(Bassil PRIVATE src)

# target_include_directories(Bassil PRIVATE include)