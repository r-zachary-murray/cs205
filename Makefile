# Include the configuration file
include ./config.mk

# Lists of files to be built
objs=dop853.o common.o SAP.o 
src=$(patsubst %.o,%.cc,$(objs) $(mg_objs))
execs=SAP_tester

all: $(execs)

# Include the file dependencies
-include Makefile.dep

# A Makefile target to refresh the dependency file
depend:
	$(cxx) -MM $(src) >Makefile.dep

%.o: %.cc
	$(cxx) $(cflags) -c $<

SAP_tester: SAP_tester.cpp $(objs)
	$(cxx) $(cflags) -o $@ $^ $(lflags)

# A Makefile target to remove all the built files
clean:
	rm -f $(objs) $(execs)

.PHONY: clean depend
