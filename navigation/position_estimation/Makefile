## Makefile
##
## Copyright (c) 2020 Navigine. All rights reserved.
##

CXX_SOURCES += ./src/measurement_preprocessor.cpp
CXX_SOURCES += ./src/nearest_transmitter_estimator.cpp
CXX_SOURCES += ./src/position_smoother.cpp

CXX_OBJECTS = $(patsubst %.cpp, %.o, $(CXX_SOURCES))

DIR = "${CURDIR}/logs/"

CFLAGS      = -c -Wall -Wextra -O3 -fPIC -ggdb
CXXFLAGS    = -c -Wall -Wextra -O3 -fPIC -ggdb -std=c++14 -I "${CURDIR}/include" -DTEST_DATA_FOLDER=\"${DIR}\"
LDFLAGS     = -fPIC -ggdb

nearest_transmitter_estimator_test.exe: examples/main.o $(CXX_OBJECTS)
	g++ $(LDFLAGS) -o $@ $+ $(LIBS)

%.o: %.cpp
	g++ $(CXXFLAGS) $(CXXDEFS) -o $@ $<

clean:
	rm -f $(CXX_OBJECTS) src/*.o examples/*.o *.exe
