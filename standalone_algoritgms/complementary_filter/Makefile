## Makefile
##
## Copyright (c) 2020 Navigine. All rights reserved.
##

CXX_SOURCES += src/complementary_filter.cpp
CXX_SOURCES += src/quaternion.cpp
CXX_SOURCES += src/vector3d.cpp

CXX_OBJECTS = $(patsubst %.cpp, %.o, $(CXX_SOURCES))

DIR = "${CURDIR}/logs/"

CFLAGS      = -c -Wall -Wextra -O3 -fPIC -ggdb
CXXFLAGS    = -c -Wall -Wextra -O3 -fPIC -ggdb -std=c++14 -I "${CURDIR}/include" -DTEST_DATA_FOLDER=\"${DIR}\"
LDFLAGS     = -fPIC -ggdb
LIBS        = -lboost_unit_test_framework

complementary_filter_test.exe: tests/complementary_filter_test.o $(CXX_OBJECTS)
	g++ $(LDFLAGS) -o $@ $+ $(LIBS)

cf_example.exe: examples/main.o $(CXX_OBJECTS)
	g++ $(LDFLAGS) -o $@ $+ $(LIBS)

%.o: %.cpp
	g++ $(CXXFLAGS) $(CXXDEFS) -o $@ $<

clean:
	rm -f $(CXX_OBJECTS) src/*.o tests/*.o examples/*.o *.exe
