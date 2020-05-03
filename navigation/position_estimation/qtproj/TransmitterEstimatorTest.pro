TARGET = nearest_beacon_estimator_test.exe

HEADERS += ../include/measurement_preprocessor.h
HEADERS += ../include/nearest_transmitter_estimator.h
HEADERS += ../include/navigation_structures.h
HEADERS += ../include/position_smoother.h
HEADERS += ../include/transmitter.h

SOURCES += ../src/measurement_preprocessor.cpp
SOURCES += ../src/nearest_transmitter_estimator.cpp
SOURCES += ../src/position_smoother.cpp
SOURCES += ../examples/main.cpp

QMAKE_CXXFLAGS += -std=c++14
INCLUDEPATH += $$PWD/../include

DEFINES += TEST_DATA_FOLDER=\\\"$$PWD/../logs/\\\"
