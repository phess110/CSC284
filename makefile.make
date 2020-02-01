CC=gcc
CXX=clang++
RM=rm -f
CPPFLAGS=-Wall -Wextra -Werror -O3
LDFLAGS=

SRCS=Graph.cpp Vertex.cpp maxflow.cpp
OBJS=$(subst .cc,.o,$(SRCS))

maxflow: $(OBJS)
    $(CXX) $(LDFLAGS) -o a $(OBJS)

depend: .depend

.depend: $(SRCS)
    $(RM) ./.depend
    $(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
    $(RM) $(OBJS)

distclean: clean
    $(RM) *~ .depend

include .depend