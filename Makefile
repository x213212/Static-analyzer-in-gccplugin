
MAKE=make
CC=gcc
CFLAGS=-g




COMPLILER_FILES=Compiler/Makefile

PARTIALS=$(patsubst %Makefile,%misra,$(COMPLILER_FILES))

TEST_FILES=./Test/test.o

$(TEST_FILES) : $(PARTIALS)  
	$(MAKE) -C $(dir $@) $(notdir $@) 

$(PARTIALS): 
	$(MAKE) -C $(dir $@) $(notdir $@).so 

clean:
	rm -rf Compiler/*.o Test/*.o
