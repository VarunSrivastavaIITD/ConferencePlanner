PROGNAME = main
LIBS = 
INCLUDES = -I/usr/local/include
LDFLAGS = -L./
OBJECTS = main.o Conference.o Session.o SessionOrganizer.o Track.o HillClimb.o

CFLAGS = -Wall -Wextra -O2 -DNDEBUG -march=native -std=c++11 -pedantic

all: $(PROGNAME)

$(PROGNAME): $(OBJECTS)
	@mkdir -p bin
	g++ -O2 -DNDEBUG -march=native -o bin/$(PROGNAME) build/*.o $(LIBS) $(INCLUDES) $(LDFLAGS)

$(OBJECTS): Makefile

%.o: %.cpp
	@mkdir -p build
	g++ -c $(CFLAGS) $(INCLUDES) -o build/$@ $<

clean:
	rm -rf build *.o $(PROGNAME)
