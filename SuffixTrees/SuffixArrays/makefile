CC=gcc
CXX=clang++
RM=rm -f
CPPFLAGS=-Wall -Wextra -Werror -O3
LDFLAGS=

SRCS=SuffixArray.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

a: $(OBJS)
	$(CXX) $(LDFLAGS) -o a.out $(OBJS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
