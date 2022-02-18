# new_gccplugin
# define.h
```c
/*interprocedural analysis*/
bool ipa = true;
bool freeanysis = true; // no free no anysis
bool vscode_extensionmod = false;     
bool vscode_extensionIgonefreemod = true;
bool memoryleakcheck = true;
// kroren testcase need turn on
bool Looserulesfree = true;
bool useafterfree = true;
bool debugmod = true; //draw graphiz
bool threadmod = false;
bool relatemod = true;
bool freemod = true;
bool retmod = true;
bool pthread_detachedmod = true;
bool pthread_exitmod = true;
bool tracerelatestmt = true;
bool prebranchexit = false;
bool stmtloopcheck = true;
bool samefucntioncheck = true;
bool maxbasicblockcalc = true;
bool pathconstraint = false;
```
# unittest check draw graphiz
```bahs
cd Test
make graphiz
```
# unittest check single file test
```bash
cd Test
# mod 1
# # $(CC) $(PLUGIN) $(FLAGS) $(LTO)  $(FILE_NAME) -o $@ 
$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(FILE_NAME) -o $@ 2> check.log
@sh ./test.sh $(FILE_NAME)
```
## test.c 
```c
int main(void)
{
    int *a;
    int test;
    a = foo();
    if (test)
        a[2] = 10;
    else
    {
        a = (int *)malloc(40);
        free(a);
    }
    free(a);
}
```

# unittest check memory error detect
```bash
cd Test
# mod 2
# binutils
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME1)  -o $@ 2> $(BINUTILSL)1/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME2)  -o $@ 2> $(BINUTILSL)2/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME3)  -o $@ 2> $(BINUTILSL)3/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME4)  -o $@ 2> $(BINUTILSL)4/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME5)  -o $@ 2> $(BINUTILSL)5/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME6)  -o $@ 2> $(BINUTILSL)6/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME7)  -o $@ 2> $(BINUTILSL)7/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME8)  -o $@ 2> $(BINUTILSL)8/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME9)  -o $@ 2> $(BINUTILSL)9/check.log
-$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME10)  -o $@ 2> $(BINUTILSL)10/check.log
# git
~
#tmux
``` 
```bash
cd Test
//memory leak
make unittestml
//double free 
make unittestdf
//use-after-free
make unittestuaf
```
## unittest check expand gimple ir no error 
```bash
cd Test
//memory leak
make unittest
```
## unittest check gimple ir incomplete
```bash
cd Test
//memory leak
make unittest
```
# Juliet_Test_Suite please disable this code
## collect.h
```c
	if ((gimple_call_num_args(gc) != 0))
	{
	    for (int i = 0; i < gimple_call_num_args(gc); i++)
		{
			tree second = NULL_TREE;
			if (TREE_CODE(gimple_call_arg(gc, i)) == ADDR_EXPR)
				second = TREE_OPERAND(gimple_call_arg(gc, i), 0);
			else if (TREE_CODE(gimple_call_arg(gc, i)) == SSA_NAME){
				second = gimple_call_arg(gc, i);
			}
			if (second){
				// debug_tree(second);
				set_ptb(bb, ptable, second, loc, 0, gc, node);
			}
		}
	}
```