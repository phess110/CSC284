CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags) -O3
LDFLAGS=-g $(shell root-config --ldflags)

SRCS=Graph.cpp Vertex.cpp maxflow.cpp
OBJS=$(subst .cc,.o,$(SRCS))

all: maxflow
tool: $(OBJS)
    $(CXX) $(LDFLAGS) -o maxflow $(OBJS)

depend: .depend

.depend: $(SRCS)
    $(RM) ./.depend
    $(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
    $(RM) $(OBJS)

distclean: clean
    $(RM) *~ .depend

include .depend