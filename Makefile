
MAKE=make
CC=gcc-11.2
CFLAGS=-g 


COMPLILER_FILES=Compiler/Makefile
PARTIALS=$(patsubst %Makefile,%newplugin,$(COMPLILER_FILES))
TEST_FILES=./Test/test.o

# egypt ./gcc_plugin.c.245r.expand | dot -Grankdir=LR -Tsvg -o callgraph.svg
$(TEST_FILES) : $(PARTIALS)  
	$(MAKE) -C  $(dir $@) $(notdir $@) 


$(PARTIALS): 
	$(MAKE) -C $(dir $@) $(notdir $@).so 

clean:
	-rm -rf Compiler/*.o Test/*.o
	-rm -rf Test/test ./test.dSYM *.o
	-rm -rf Test/graph.dot
	-rm -rf Test/graph.log
	-rm -rf Test/graph.svg
	