# new_gccplugin

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
## run!
```bash
cd ..
make
cat /Test/check.log
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
## memory error detect run!
```bash
cd ..
make
cat /Test/check.log
```

## Detail detection 
```bash
cd Test
//memory leak
make unittestml
//double free 
make unittestdf
//use-after-free
make unittestuaf
```
### unittest check expand gimple ir no error 
```bash
cd Test
//memory leak
make unittest
```
### unittest check gimple ir incomplete
```bash
cd Test
//memory leak
make unittest
```

# Juliet_Test_Suite 
## Juliet_Test_Suite please disable this code
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
rebuild the gcc plugin 
```
make
```
## memory leak
```bash
cp ./Compiler/newplugin.so ./Test/benchmarks/Juliet_Test_Suite_v1.3_for_C_Cpp/C/testcases/CWE401_Memory_Leak/s01/
cd ./Test/benchmarks/Juliet_Test_Suite_v1.3_for_C_Cpp/C/testcases/CWE401_Memory_Leak/s01/
make
```
### error detect 
```bash
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__char_calloc_*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__char_malloc*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__char_realloc*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__int_calloc*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__int_malloc*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__int_realloc*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__int64_t_calloc*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__int64_t_malloc*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__int64_t_realloc*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__malloc_realloc_char*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__malloc_realloc_int_*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__malloc_realloc_int64_t*.o.log | uniq -c    | wc -l			
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__malloc_realloc_struct_twoIntsStruct*.o.log | uniq -c    | wc -l	
grep  "no free stmt possible memory leak" ./CWE401_Memory_Leak__malloc_realloc_twoIntsStruct*.o.log | uniq -c    | wc -l			
```
## double free 
```bash
cp ./Compiler/newplugin.so ./Test/benchmarks/Juliet_Test_Suite_v1.3_for_C_Cpp/C/testcases/CWE415_Double_Free/s01/
cd ./Test/benchmarks/Juliet_Test_Suite_v1.3_for_C_Cpp/C/testcases/CWE415_Double_Free/s01/
make
```
### error detect 
```bahs
grep  "possible double free" ./CWE415_Double_Free__malloc_free_char_*.o.log | uniq -c    | wc -l		
grep  "possible double free" ./CWE415_Double_Free__malloc_free_int*.o.log | uniq -c    | wc -l		
grep  "possible double free" ./CWE415_Double_Free__malloc_free_int64_t_*.o.log | uniq -c    | wc -l		
grep  "possible double free" ./CWE415_Double_Free__malloc_free_long*.o.log | uniq -c    | wc -l		
grep  "possible double free" ./CWE415_Double_Free__malloc_free_struct*.o.log | uniq -c    | wc -l		
grep  "possible double free" ./CWE415_Double_Free__malloc_free_wchar_t*.o.log | uniq -c    | wc -l	

```
## use-after-free
```bash
cp ./Compiler/newplugin.so ./Test/benchmarks/Juliet_Test_Suite_v1.3_for_C_Cpp/C/testcases/CWE416_Use_After_Free/
cd ./Test/benchmarks/Juliet_Test_Suite_v1.3_for_C_Cpp/C/testcases/CWE416_Use_After_Free/
make
```
### error detect 
```bash
grep  "===== possiable asias =====" ./CWE416_Use_After_Free__malloc_free_char_*.o.log | uniq -c    | wc -l				
grep  "===== possiable asias =====" ./CWE416_Use_After_Free__malloc_free_int_*.o.log | uniq -c    | wc -l				
grep  "===== possiable asias =====" ./CWE416_Use_After_Free__malloc_free_int64_t_*.o.log | uniq -c | wc -l				
grep  "===== possiable asias =====" ./CWE416_Use_After_Free__malloc_free_long_*.o.log | uniq -c    | wc -l				
grep  "===== possiable asias =====" ./CWE416_Use_After_Free__malloc_free_struct_*.o.log | uniq -c    | wc -l				
grep  "===== possiable asias =====" ./CWE416_Use_After_Free__return_freed_ptr_*.o.log | uniq -c    | wc -l		
```


# new_gccplugin fucntion 
```c
/*interprocedural analysis*/
bool ipa = true;
bool freeanysis = true; // no free no anysis
bool vscode_extensionmod = false;     
bool vscode_extensionIgonefreemod = true;
bool memoryleakcheck = true;
// open_source testcase need turn on
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
![](https://i.imgur.com/jSMgN6P.png)
```bahs
cd Test
make graphiz

```
Use  browser open ./graph.svg

# vscode extension support
Use filters to detect partial code fragments
![](https://i.imgur.com/xTv3L2o.png)
press f5 start debug extension
![](https://i.imgur.com/WE5qvIT.png)

```add new fucntion
	"contributes": {
		"commands": [
			{
				"command": "extension.helloWorld",
				"title": "GCC Analyzer"
			},
			{
				"command": "extension.removebreakp",
				"title": "GCC Analyzer remove breakpoint"
			}
		]
```


# rolloback gcc 7.5
/root/x21321219/new_gccplugin/Compiler/Algorithm.h
```c
cgraph_node *caller = e->caller->inlined_to ? e->caller->inlined_to : e->caller;
to 
cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;

```
welcome to gcc 7.5!