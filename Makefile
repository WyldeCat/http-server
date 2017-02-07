SRC_PATH = ./src
INCLUDE_PATH = ./include

CXX = g++
CPPFLAGS = -g -std=c++11 -I$(INCLUDE_PATH)

SRCS = $(wildcard $(SRC_PATH)/*.cpp)
OBJS = $(SRCS:%.cpp=%.o)

STATIC_LIB = libhttp_server.a

all : $(STATIC_LIB)

$(STATIC_LIB) : $(OBJS)
	ar crv $(STATIC_LIB) $(OBJS)

$(OBJS) : $(SRCS)
	$(CXX) -c -o $@ $(@:%.o=%.cpp) $(CPPFLAGS)


clean :
	rm -f $(OBJS)
	rm -f $(STATIC_LIB)
	rm -f $(SRC_PATH)/.* || true
	rm -f $(INCLUDE_PATH)/.* || true
