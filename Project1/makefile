CXX=g++ 
CXXFLAGS=
#add C++ source files here
SRCS=main.cc
TARGET=mips_sim
OBJS := $(patsubst %.cc,%.o,$(SRCS)) 
all: $(TARGET)
%.o:%.cc
	$(CXX) $(CXXFLAGS) $< -c -o $@ 
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ 
	
.PHONY=clean

clean:
	rm -f $(OBJS) $(TARGET)
