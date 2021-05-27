## Makefile
##
## Copyright (c) 2020 Navigine. All rights reserved.
##

CXX_SOURCES += src/pedometer.cpp
CXX_SOURCES += src/vector3d.cpp

CXX_OBJECTS = $(patsubst %.cpp, %.o, $(CXX_SOURCES))

CFLAGS      = -c -Wall -Wextra -O3 -fPIC -ggdb
CXXFLAGS    = -c -Wall -Wextra -O3 -fPIC -ggdb -std=c++14 -I "${CURDIR}/include" -DTEST_DATA_FOLDER=\"${CURDIR}/logs\" 
LDFLAGS     = -fPIC -ggdb
LIBS        = -lboost_unit_test_framework

pedometer_test.exe: tests/pedometer_test.o $(CXX_OBJECTS)
	g++ $(LDFLAGS) -o $@ $+ $(LIBS)

example.exe: examples/main.o $(CXX_OBJECTS)
	g++ $(LDFLAGS) -o $@ $+ $(LIBS)

%.o: %.cpp
	g++ $(CXXFLAGS) $(CXXDEFS) -o $@ $<

clean:
	rm -f $(CXX_OBJECTS) src/*.o tests/*.o examples/*.o *.exe

