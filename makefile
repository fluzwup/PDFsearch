
BINARY = IndexPDF

CXXSOURCES = main.cpp

OBJECTS = ${CXXSOURCES:.cpp=.o} 

INCLUDES = -I . -I /usr/include/poppler 

LIBRARIES = -lpoppler-cpp -lpoppler

CXXFLAGS = -ggdb -ansi -Wall -std=c++11
CXX = g++ 

.SUFFIXES:      .cpp .o

.cpp.o:
		@echo
		@echo Building $@		
		${CXX} ${CXXFLAGS} ${INCLUDES} -c -o $@ $<
.c.o:
		@echo
		@echo Building $@		
		${CC} ${CFLAGS} ${INCLUDES} -c -o $@ $<

all:            ${OBJECTS} ${BINARY} 

${BINARY}:      ${OBJECTS}
		@echo
		@echo Building ${BINARY} Executable
		${CXX} -o $@ \
		${OBJECTS}  \
		${LIBRARIES} \
		${LOCATIONS}
                         
clean:
		rm -f ${BINARY} *.o



