cmake_minimum_required(VERSION 3.5.7)
project(Loppi VERSION 0.1.0 LANGUAGES C CXX)

set (CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp classes/Utils/AVLTree.cpp

classes/Analyze/Lexer.cpp

classes/TID/TID.cpp
classes/TID/TIDTree.cpp
classes/TypeOpStack/TypeOpStack.cpp

classes/Utils/Utils.cpp

classes/RPN/RPN.cpp)
