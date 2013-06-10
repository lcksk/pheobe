C++ wrapper for Mongoose http server.
Version: 1.1
Mongoose baseline version: 3.4

This code repository contains C++ wrapper for mongoose http server library.

C++ wrapper author:
  Piotr Likus

wrapper project home:
  http://code.google.com/r/vpiotr-mongoose-cpp/
    
mongoose project home:
  http://code.google.com/p/mongoose/


Files and directories:
------------------------
\bin         - test executable for wrapper (mongotest.exe)
\build       - project files for library & test program
\lib         - output directory for library (mongoose)
\test        - source code for wrapper test server (mongotest.cpp)

Changes:
------------------------
- no changes required in base files

New code:
------------------------
mongcpp.h, mongcpp.cpp - C++ wrapper for mongoose web server library

Example for C++ web server:
---------------------------
\test\mongotest.cpp

To run it on Windows:
- compile library project "mongoose" - in \build directory
- compile program project "mongotest" - in \build directory
- copy "html" directory from \examples to \bin
- execute "\bin\mongoose.exe" program
- open in your browser address "http://127.0.0.1:8080/"
- or "http://127.0.0.1:8080/info"

Tested compilers:
-----------------
- VS2010 Express SP1
- Code::Blocks 10.05 + MinGW + gcc 4.5

Final notes
---------------------------
Sources of mongoose project updated basing on code
from mongoose website on 2013/01/08

http://code.google.com/p/mongoose/

