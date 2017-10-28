# Change this to wherever you install OMPL
# /usr/local is the default for the virtual machine and installation script
# /usr       is if you installed OMPL through the debian packages
# /opt/local is the default for OS X through MacPorts
OMPL_DIR      = /usr/local

# Change -O2 to -g when you are debugging your code
CXXFLAGS      = -std=c++11 -O2

# Include directories
INCLUDE_FLAGS = -I${OMPL_DIR}/include

# Linker options and libraries
LD_FLAGS      = -L${OMPL_DIR}/lib -lompl -lompl_app_base -lompl_app -lboost_program_options

# The C++ compiler to invoke (must have support for C++11)
CXX           = c++

# Change PROGRAM to the name of your program
PROGRAM       = project4
SRC_DIR       = src/RG-RRT
BIN_DIR       = src
UTIL_DIR      = utils

# The Makefile will compile all *.cpp files in SRC_DIR and UTIL_DIR
SOURCES       = $(wildcard $(UTIL_DIR)/*.cpp $(SRC_DIR)/*.cpp $(BIN_DIR)/*.cpp)
OBJECTS       = $(patsubst %.cpp, %.o, $(SOURCES))
PROGRAM_OBJECTS   = $(filter-out src/Project4benchmark.o, $(OBJECTS))

.PHONY : all
all : $(PROGRAM)

# Remove everything (object files in SRC_DIR, the compiled PROGRAM)
clean : 
	$(RM) $(UTIL_DIR)/*.o
	$(RM) $(SRC_DIR)/*.o
	$(RM) $(BIN_DIR)/*.o
	$(RM) $(PROGRAM)

# Compile all *.cpp files to object files
%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Compile the object files into the main program
$(PROGRAM) : $(PROGRAM_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_FLAGS)
