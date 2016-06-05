CC= g++

EXE=bin/adunaEngine

FILES= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

SRCDIR= src
SRCEXT= cpp

OBJECTS= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(FILES:.$(SRCEXT)=.o))
OBJDIR= build

LIBS= -lSDL2 -lGLEW -lGL -lGLU -std=c++11
DISABLE_WARNING=-w

all: run

run: $(OBJECTS)
	@echo "Linking..."
	mkdir -p bin
	$(CC) $^ -o $(EXE) $(LIBS)
	./$(EXE)

$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/util
	mkdir -p $(OBJDIR)/ui
	$(CC) $(DISABLE_WARNING) -c -o $@ $< $(LIBS)

clean:
	@echo "\nCleaning executables"
	rm -rf bin
	@echo "\nCleaning object files"
	rm -rf $(OBJDIR)
	@echo "\nCleaning backup files"
	rm -rf *.h~ *.cpp~ *Makefile~
