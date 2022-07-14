
CC=gcc-11.2
TEST=linuxcnc
BINUTILSL=./benchmarks/binutils/leak_
BINUTILSD=./benchmarks/binutils/double_free_
BINUTILSU=./benchmarks/binutils/use_after_free_

GITL=./benchmarks/git/leak_
GITD=./benchmarks/git/double_free_
GITU=./benchmarks/git/use_after_free_


OPSSHL=./benchmarks/openssh-portable/leak_
OPSSHD=./benchmarks/openssh-portable/double_free_
OPSSHU=./benchmarks/openssh-portable/use_after_free_

OPSSLL=./benchmarks/openssl/leak_
OPSSLD=./benchmarks/openssl/double_free_
OPSSLU=./benchmarks/openssl/use_after_free_


TMUXL=./benchmarks/tmux/leak_
TMUXD=./benchmarks/tmux/double_free_
TMUXU=./benchmarks/tmux/use_after_free_

FILE_NAME=$(wildcard *.c) 

#
BFILE_NAME1=$(wildcard ./benchmarks/binutils/leak_1/*.c)
BFILE_NAME2=$(wildcard ./benchmarks/binutils/leak_2/*.c)
BFILE_NAME3=$(wildcard ./benchmarks/binutils/leak_3/*.c)
BFILE_NAME4=$(wildcard ./benchmarks/binutils/leak_4/*.c)
BFILE_NAME5=$(wildcard ./benchmarks/binutils/leak_5/*.c)
BFILE_NAME6=$(wildcard ./benchmarks/binutils/leak_6/*.c)
BFILE_NAME7=$(wildcard ./benchmarks/binutils/leak_7/*.c)
BFILE_NAME8=$(wildcard ./benchmarks/binutils/leak_8/*.c)
BFILE_NAME9=$(wildcard ./benchmarks/binutils/leak_9/*.c)
BFILE_NAME10=$(wildcard ./benchmarks/binutils/leak_10/*.c)

BFILE_NAMEU1=$(wildcard ./benchmarks/binutils/use_after_free_1/*.c)
BFILE_NAMEU2=$(wildcard ./benchmarks/binutils/use_after_free_2/*.c)
BFILE_NAMEU3=$(wildcard ./benchmarks/binutils/use_after_free_3/*.c)
BFILE_NAMEU4=$(wildcard ./benchmarks/binutils/use_after_free_4/*.c)
BFILE_NAMEU5=$(wildcard ./benchmarks/binutils/use_after_free_5/*.c)

BFILE_NAMED1=$(wildcard ./benchmarks/binutils/double_free_1/*.c)
BFILE_NAMED2=$(wildcard ./benchmarks/binutils/double_free_2/*.c)
BFILE_NAMED3=$(wildcard ./benchmarks/binutils/double_free_3/*.c)
BFILE_NAMED4=$(wildcard ./benchmarks/binutils/double_free_4/*.c)
BFILE_NAMED5=$(wildcard ./benchmarks/binutils/double_free_5/*.c)
# git

GITFILE_NAME1=$(wildcard ./benchmarks/git/leak_1/*.c)
GITFILE_NAME2=$(wildcard ./benchmarks/git/leak_2/*.c)
GITFILE_NAME3=$(wildcard ./benchmarks/git/leak_3/*.c)
GITFILE_NAME4=$(wildcard ./benchmarks/git/leak_4/*.c)
GITFILE_NAME5=$(wildcard ./benchmarks/git/leak_5/*.c)
GITFILE_NAME6=$(wildcard ./benchmarks/git/leak_6/*.c)
GITFILE_NAME7=$(wildcard ./benchmarks/git/leak_7/*.c)
GITFILE_NAME8=$(wildcard ./benchmarks/git/leak_8/*.c)
GITFILE_NAME9=$(wildcard ./benchmarks/git/leak_9/*.c)
GITFILE_NAME10=$(wildcard ./benchmarks/git/leak_10/*.c)

GITFILE_NAMEU1=$(wildcard ./benchmarks/git/use_after_free_1/*.c)
GITFILE_NAMEU2=$(wildcard ./benchmarks/git/use_after_free_2/*.c)
GITFILE_NAMEU3=$(wildcard ./benchmarks/git/use_after_free_3/*.c)
GITFILE_NAMEU4=$(wildcard ./benchmarks/git/use_after_free_4/*.c)
GITFILE_NAMEU5=$(wildcard ./benchmarks/git/use_after_free_5/*.c)
GITFILE_NAMEU6=$(wildcard ./benchmarks/git/use_after_free_6/*.c)

GITFILE_NAMED1=$(wildcard ./benchmarks/git/double_free_1/*.c)
GITFILE_NAMED2=$(wildcard ./benchmarks/git/double_free_2/*.c)
GITFILE_NAMED3=$(wildcard ./benchmarks/git/double_free_3/*.c)
GITFILE_NAMED4=$(wildcard ./benchmarks/git/double_free_4/*.c)

#openssh

OPSSHFILE_NAME1=$(wildcard ./benchmarks/openssh-portable/leak_1/*.c)
OPSSHFILE_NAME2=$(wildcard ./benchmarks/openssh-portable/leak_2/*.c)
OPSSHFILE_NAME3=$(wildcard ./benchmarks/openssh-portable/leak_3/*.c)
OPSSHFILE_NAME4=$(wildcard ./benchmarks/openssh-portable/leak_4/*.c)
OPSSHFILE_NAME5=$(wildcard ./benchmarks/openssh-portable/leak_5/*.c)
OPSSHFILE_NAME6=$(wildcard ./benchmarks/openssh-portable/leak_6/*.c)
OPSSHFILE_NAME7=$(wildcard ./benchmarks/openssh-portable/leak_7/*.c)
OPSSHFILE_NAME8=$(wildcard ./benchmarks/openssh-portable/leak_8/*.c)
OPSSHFILE_NAME9=$(wildcard ./benchmarks/openssh-portable/leak_9/*.c)
OPSSHFILE_NAME10=$(wildcard ./benchmarks/openssh-portable/leak_10/*.c)

OPSSHFILE_NAMEU1=$(wildcard ./benchmarks/openssh-portable/use_after_free_1/*.c)
OPSSHFILE_NAMEU2=$(wildcard ./benchmarks/openssh-portable/use_after_free_2/*.c)
OPSSHFILE_NAMEU3=$(wildcard ./benchmarks/openssh-portable/use_after_free_3/*.c)


OPSSHFILE_NAMED1=$(wildcard ./benchmarks/openssh-portable/double_free_1/*.c)
OPSSHFILE_NAMED2=$(wildcard ./benchmarks/openssh-portable/double_free_2/*.c)
OPSSHFILE_NAMED3=$(wildcard ./benchmarks/openssh-portable/double_free_3/*.c)
OPSSHFILE_NAMED4=$(wildcard ./benchmarks/openssh-portable/double_free_4/*.c)
OPSSHFILE_NAMED5=$(wildcard ./benchmarks/openssh-portable/double_free_5/*.c)
OPSSHFILE_NAMED6=$(wildcard ./benchmarks/openssh-portable/double_free_6/*.c)
OPSSHFILE_NAMED7=$(wildcard ./benchmarks/openssh-portable/double_free_7/*.c)

#openssl

OPSSLFILE_NAME1=$(wildcard ./benchmarks/openssl/leak_1/*.c)
OPSSLFILE_NAME2=$(wildcard ./benchmarks/openssl/leak_2/*.c)
OPSSLFILE_NAME22=$(wildcard ./benchmarks/openssl/leak_2/*.c)
OPSSLFILE_NAME3=$(wildcard ./benchmarks/openssl/leak_3/*.c)
OPSSLFILE_NAME4=$(wildcard ./benchmarks/openssl/leak_4/*.c)
OPSSLFILE_NAME5=$(wildcard ./benchmarks/openssl/leak_5/*.c)
OPSSLFILE_NAME6=$(wildcard ./benchmarks/openssl/leak_6/*.c)
OPSSLFILE_NAME7=$(wildcard ./benchmarks/openssl/leak_7/*.c)
OPSSLFILE_NAME8=$(wildcard ./benchmarks/openssl/leak_8/*.c)
OPSSLFILE_NAME9=$(wildcard ./benchmarks/openssl/leak_9/*.c)
OPSSLFILE_NAME10=$(wildcard ./benchmarks/openssl/leak_10/*.c)

OPSSLFILE_NAMEU1=$(wildcard ./benchmarks/openssl/use_after_free_1/*.c)
OPSSLFILE_NAMEU2=$(wildcard ./benchmarks/openssl/use_after_free_2/*.c)
OPSSLFILE_NAMEU3=$(wildcard ./benchmarks/openssl/use_after_free_3/*.c)
OPSSLFILE_NAMEU4=$(wildcard ./benchmarks/openssl/use_after_free_4/*.c)
OPSSLFILE_NAMEU5=$(wildcard ./benchmarks/openssl/use_after_free_5/*.c)

OPSSLFILE_NAMED1=$(wildcard ./benchmarks/openssl/double_free_1/*.c)
OPSSLFILE_NAMED2=$(wildcard ./benchmarks/openssl/double_free_2/*.c)
OPSSLFILE_NAMED3=$(wildcard ./benchmarks/openssl/double_free_3/*.c)
OPSSLFILE_NAMED4=$(wildcard ./benchmarks/openssl/double_free_4/*.c)
OPSSLFILE_NAMED5=$(wildcard ./benchmarks/openssl/double_free_5/*.c)

#tmux
TMUXLFILE_NAME1=$(wildcard ./benchmarks/tmux/leak_1/*.c)
TMUXLFILE_NAME2=$(wildcard ./benchmarks/tmux/leak_2/*.c)
TMUXLFILE_NAME3=$(wildcard ./benchmarks/tmux/leak_3/*.c)
TMUXLFILE_NAME4=$(wildcard ./benchmarks/tmux/leak_4/*.c)
TMUXLFILE_NAME5=$(wildcard ./benchmarks/tmux/leak_5/*.c)
TMUXLFILE_NAME6=$(wildcard ./benchmarks/tmux/leak_6/*.c)
TMUXLFILE_NAME7=$(wildcard ./benchmarks/tmux/leak_7/*.c)
TMUXLFILE_NAME8=$(wildcard ./benchmarks/tmux/leak_8/*.c)
TMUXLFILE_NAME9=$(wildcard ./benchmarks/tmux/leak_9/*.c)
TMUXLFILE_NAME10=$(wildcard ./benchmarks/tmux/leak_10/*.c)

TMUXFILE_NAMEU1=$(wildcard ./benchmarks/tmux/use_after_free_1/*.c)
TMUXFILE_NAMEU2=$(wildcard ./benchmarks/tmux/use_after_free_2/*.c)
TMUXFILE_NAMEU3=$(wildcard ./benchmarks/tmux/use_after_free_3/*.c)
TMUXFILE_NAMEU4=$(wildcard ./benchmarks/tmux/use_after_free_4/*.c)
TMUXFILE_NAMEU5=$(wildcard ./benchmarks/tmux/use_after_free_5/*.c)
TMUXFILE_NAMEU6=$(wildcard ./benchmarks/tmux/use_after_free_6/*.c)
TMUXFILE_NAMEU7=$(wildcard ./benchmarks/tmux/use_after_free_7/*.c)

TMUXFILE_NAMED1=$(wildcard ./benchmarks/tmux/double_free_1/*.c)
TMUXFILE_NAMED2=$(wildcard ./benchmarks/tmux/double_free_2/*.c)
TMUXFILE_NAMED3=$(wildcard ./benchmarks/tmux/double_free_3/*.c)





NO_OF_FILES := $(words $(FILE_NAME)) 

LTO=-flto
WHOLE=-fwhole-program 
# https://gcc.gnu.org/onlinedocs/gcc/Developer-Options.html
#-fno-inline-functions-called-once
# FLAGS=-O1 -fno-tree-dse  -fno-tree-fre -fipa-pta -fno-inline  -pthread   -ftime-report
# -pthread  --param max-ssa-name-query-depth=10   -fno-inline-small-functions   -fno-strict-aliasing -fno-inline-functions
FLAGS=-O1 -fno-tree-dse -fno-tree-dce  -fno-tree-fre -fipa-pta -fno-inline  -pthread   -ftime-report 
# FLAGS=-O1 -fno-tree-dse  -fno-tree-fre -fipa-pta -fdump-tree-cfg-graph -pthread  --param max-ssa-name-query-depth=10  -ftime-report
# FLAGS=-O1 -fno-tree-dse  -fno-tree-fre -fipa-pta   --param max-ssa-name-query-depth=10 -ftime-report
# -fno-inline 
# -flto-report
# -ftime-report
#FLAGS=-O1   -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta  -pthread   
#  -I/root/x21321219/mysql8.0/mysql-8.0.23/extra/icu/source/i18n/../common -I/root/x21321219/mysql8.0/mysql-8.0.23/extra/icu/source/i18n 
#FLAGS=-O1    -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta  -fdump-tree-all -pthread -fno-diagnostics-color 
#FLAGS=-O1    -fno-tree-dse  -fno-tree-fre -fno-tree-dce   -nostartfiles -g  -fdump-tree-all -fno-dce -fno-dse -fno-tree-dce    -fno-tree-builtin-call-dce -fipa-pta -pthread -fno-strict-aliasing
# PLUGIN=
# PLUGIN=-fplugin=../Compiler/newplugin.so  -fplugin-arg-newplugin-debugmod=1 -fplugin-arg-newplugin-vscode_extensionmod=1
PLUGIN :=-fplugin=../Compiler/newplugin.so 
DEBUGMOD= -fplugin=../Compiler/newplugin.so -fplugin-arg-newplugin-debugmod=1 -fplugin-arg-newplugin-vscode_extensionmod=1
debug=0
# $(eval PLUGIN=-fanalyzer -Wanalyzer-malloc-leak -Wanalyzer-double-free -Wanalyzer-use-after-free)

all: test.o


test.o:  
ifeq ($(debug),0)
	$(eval PLUGIN=-fplugin=../Compiler/newplugin.so -fplugin-arg-newplugin-contextsentive=1 -fplugin-arg-newplugin-debugoutput=1 -fplugin-arg-newplugin-debugmod=0)
else ifeq ($(debug),1)
	$(eval PLUGIN=-fplugin=../Compiler/newplugin.so -fplugin-arg-newplugin-debugmod=0 -fplugin-arg-newplugin-vscode_extensionmod=1 -fplugin-arg-newplugin-debugoutput=1)
else ifeq ($(debug),2)
	$(eval PLUGIN=-fplugin=../Compiler/newplugin.so -fplugin-arg-newplugin-debugmod=0 -fplugin-arg-newplugin-vscode_extensionmod=1 -fplugin-arg-newplugin-debugoutput=1 -fplugin-arg-newplugin-contextsentive=1)
else ifeq ($(debug),3)
	$(eval PLUGIN=-fplugin=../Compiler/newplugin.so -fplugin-arg-newplugin-debugmod=1 -fplugin-arg-newplugin-vscode_extensionmod=0  -fplugin-arg-newplugin-contextsentive=0)
else ifeq ($(debug),4)
	$(eval PLUGIN=-fplugin=../Compiler/newplugin.so -fplugin-arg-newplugin-debugmod=0 -fplugin-arg-newplugin-vscode_extensionmod=0 )
endif
	# @echo $(debug)
	# mod 1
	# # $(CC) $(PLUGIN) $(FLAGS) $(LTO)  $(FILE_NAME) -o $@ 
	$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(FILE_NAME) -o $@ 2> check.log
	# @sh ./test.sh $(FILE_NAME)
	# # #mod 2
	# #binutils
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME1)  -o $@ 2> $(BINUTILSL)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME2)  -o $@ 2> $(BINUTILSL)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME3)  -o $@ 2> $(BINUTILSL)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME4)  -o $@ 2> $(BINUTILSL)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME5)  -o $@ 2> $(BINUTILSL)5/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME6)  -o $@ 2> $(BINUTILSL)6/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME7)  -o $@ 2> $(BINUTILSL)7/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME8)  -o $@ 2> $(BINUTILSL)8/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME9)  -o $@ 2> $(BINUTILSL)9/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAME10)  -o $@ 2> $(BINUTILSL)10/check.log

	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMEU1)  -o $@ 2> $(BINUTILSU)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMEU2)  -o $@ 2> $(BINUTILSU)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMEU3)  -o $@ 2> $(BINUTILSU)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMEU4)  -o $@ 2> $(BINUTILSU)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMEU5)  -o $@ 2> $(BINUTILSU)5/check.log

	
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMED1)  -o $@ 2> $(BINUTILSD)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMED2)  -o $@ 2> $(BINUTILSD)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMED3)  -o $@ 2> $(BINUTILSD)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMED4)  -o $@ 2> $(BINUTILSD)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(BFILE_NAMED5)  -o $@ 2> $(BINUTILSD)5/check.log
	
	# # git
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME1)  -o $@ 2> $(GITL)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME2)  -o $@ 2> $(GITL)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME3)  -o $@ 2> $(GITL)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME4)  -o $@ 2> $(GITL)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME5)  -o $@ 2> $(GITL)5/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME6)  -o $@ 2> $(GITL)6/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME7)  -o $@ 2> $(GITL)7/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME8)  -o $@ 2> $(GITL)8/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME9)  -o $@ 2> $(GITL)9/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAME10)  -o $@ 2> $(GITL)10/check.log

	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMEU1)  -o $@ 2> $(GITU)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMEU2)  -o $@ 2> $(GITU)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMEU3)  -o $@ 2> $(GITU)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMEU4)  -o $@ 2> $(GITU)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMEU5)  -o $@ 2> $(GITU)5/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMEU6)  -o $@ 2> $(GITU)6/check.log

	
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMED1)  -o $@ 2> $(GITD)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMED2)  -o $@ 2> $(GITD)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMED3)  -o $@ 2> $(GITD)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(GITFILE_NAMED4)  -o $@ 2> $(GITD)4/check.log

	# #openssh

	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME1)  -o $@ 2> $(OPSSHL)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME2)  -o $@ 2> $(OPSSHL)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME3)  -o $@ 2> $(OPSSHL)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME4)  -o $@ 2> $(OPSSHL)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME5)  -o $@ 2> $(OPSSHL)5/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME6)  -o $@ 2> $(OPSSHL)6/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME7)  -o $@ 2> $(OPSSHL)7/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME8)  -o $@ 2> $(OPSSHL)8/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME9)  -o $@ 2> $(OPSSHL)9/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAME10)  -o $@ 2> $(OPSSHL)10/check.log

	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMEU1)  -o $@ 2> $(OPSSHU)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMEU2)  -o $@ 2> $(OPSSHU)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMEU3)  -o $@ 2> $(OPSSHU)3/check.log

	
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMED1)  -o $@ 2> $(OPSSHD)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMED2)  -o $@ 2> $(OPSSHD)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMED3)  -o $@ 2> $(OPSSHD)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMED4)  -o $@ 2> $(OPSSHD)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMED5)  -o $@ 2> $(OPSSHD)5/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMED6)  -o $@ 2> $(OPSSHD)6/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSHFILE_NAMED7)  -o $@ 2> $(OPSSHD)7/check.log


	# #openssl

	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME1)  -o $@ 2> $(OPSSLL)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME2)  -o $@ 2> $(OPSSLL)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME3)  -o $@ 2> $(OPSSLL)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME4)  -o $@ 2> $(OPSSLL)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME5)  -o $@ 2> $(OPSSLL)5/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME6)  -o $@ 2> $(OPSSLL)6/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME7)  -o $@ 2> $(OPSSLL)7/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME8)  -o $@ 2> $(OPSSLL)8/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME9)  -o $@ 2> $(OPSSLL)9/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAME10)  -o $@ 2> $(OPSSLL)10/check.log

	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMED1)  -o $@ 2> $(OPSSLD)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMED2)  -o $@ 2> $(OPSSLD)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMED3)  -o $@ 2> $(OPSSLD)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMED4)  -o $@ 2> $(OPSSLD)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMED5)  -o $@ 2> $(OPSSLD)5/check.log

	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMEU1)  -o $@ 2> $(OPSSLU)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMEU2)  -o $@ 2> $(OPSSLU)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMEU3)  -o $@ 2> $(OPSSLU)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMEU4)  -o $@ 2> $(OPSSLU)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(OPSSLFILE_NAMEU5)  -o $@ 2> $(OPSSLU)5/check.log

	# #tmux
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME1)  -o $@ 2> $(TMUXL)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME2)  -o $@ 2> $(TMUXL)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME3)  -o $@ 2> $(TMUXL)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME4)  -o $@ 2> $(TMUXL)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME5)  -o $@ 2> $(TMUXL)5/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME6)  -o $@ 2> $(TMUXL)6/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME7)  -o $@ 2> $(TMUXL)7/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME8)  -o $@ 2> $(TMUXL)8/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME9)  -o $@ 2> $(TMUXL)9/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXLFILE_NAME10)  -o $@ 2> $(TMUXL)10/check.log

	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMEU1)  -o $@ 2> $(TMUXU)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMEU2)  -o $@ 2> $(TMUXU)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMEU3)  -o $@ 2> $(TMUXU)3/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMEU4)  -o $@ 2> $(TMUXU)4/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMEU5)  -o $@ 2> $(TMUXU)5/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMEU6)  -o $@ 2> $(TMUXU)6/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMEU7)  -o $@ 2> $(TMUXU)7/check.log
	
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMED1)  -o $@ 2> $(TMUXD)1/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMED2)  -o $@ 2> $(TMUXD)2/check.log
	# -$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(TMUXFILE_NAMED3)  -o $@ 2> $(TMUXD)3/check.log

unittestml:
	# binutils
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_1/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_2/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_3/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_4/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_5/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_6/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_7/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_8/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_9/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/binutils/leak_10/check.log | uniq -c    | wc -l

	# git
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_1/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_2/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_3/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_4/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_5/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_6/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_7/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_8/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_9/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/git/leak_10/check.log | uniq -c    | wc -l

	#openssh
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_1/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_2/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_3/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_4/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_5/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_6/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_7/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_8/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_9/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssh-portable/leak_10/check.log | uniq -c    | wc -l

	#openssl
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_1/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_2/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_3/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_4/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_5/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_6/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_7/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_8/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_9/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/openssl/leak_10/check.log | uniq -c    | wc -l
	
	#tmux
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_1/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_2/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_3/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_4/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_5/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_6/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_7/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_8/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_9/check.log | uniq -c    | wc -l
	grep "no free stmt possible memory leak"   ./benchmarks/tmux/leak_10/check.log | uniq -c    | wc -l
unittestml2:
	# binutils
	grep "CWE-401"   ./benchmarks/binutils/leak_1/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_2/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_3/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_4/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_5/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_6/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_7/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_8/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_9/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/binutils/leak_10/check.log | uniq -c    | wc -l

	# git
	grep "CWE-401"   ./benchmarks/git/leak_1/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_2/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_3/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_4/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_5/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_6/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_7/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_8/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_9/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/git/leak_10/check.log | uniq -c    | wc -l

	#openssh
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_1/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_2/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_3/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_4/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_5/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_6/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_7/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_8/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_9/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssh-portable/leak_10/check.log | uniq -c    | wc -l

	#openssl
	grep "CWE-401"   ./benchmarks/openssl/leak_1/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_2/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_3/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_4/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_5/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_6/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_7/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_8/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_9/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/openssl/leak_10/check.log | uniq -c    | wc -l
	
	#tmux
	grep "CWE-401"   ./benchmarks/tmux/leak_1/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_2/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_3/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_4/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_5/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_6/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_7/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_8/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_9/check.log | uniq -c    | wc -l
	grep "CWE-401"   ./benchmarks/tmux/leak_10/check.log | uniq -c    | wc -l
unittestdf:
	# binutils
	grep "possible double free"   ./benchmarks/binutils/double_free_1/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/binutils/double_free_2/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/binutils/double_free_3/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/binutils/double_free_4/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/binutils/double_free_5/check.log | uniq -c    | wc -l
	# git
	grep "possible double free"   ./benchmarks/git/double_free_1/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/git/double_free_2/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/git/double_free_3/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/git/double_free_4/check.log | uniq -c    | wc -l
	#openssh
	grep "possible double free"   ./benchmarks/openssh-portable/double_free_1/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssh-portable/double_free_2/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssh-portable/double_free_3/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssh-portable/double_free_4/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssh-portable/double_free_5/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssh-portable/double_free_6/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssh-portable/double_free_7/check.log | uniq -c    | wc -l
	#openssl
	grep "possible double free"   ./benchmarks/openssl/double_free_1/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssl/double_free_2/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssl/double_free_3/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssl/double_free_4/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/openssl/double_free_5/check.log | uniq -c    | wc -l

	#tmux

	grep "possible double free"   ./benchmarks/tmux/double_free_1/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/tmux/double_free_2/check.log | uniq -c    | wc -l
	grep "possible double free"   ./benchmarks/tmux/double_free_3/check.log | uniq -c    | wc -l
unittestdf2:
	# binutils
	grep "CWE-415"   ./benchmarks/binutils/double_free_1/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/binutils/double_free_2/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/binutils/double_free_3/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/binutils/double_free_4/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/binutils/double_free_5/check.log | uniq -c    | wc -l
	# git
	grep "CWE-415"   ./benchmarks/git/double_free_1/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/git/double_free_2/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/git/double_free_3/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/git/double_free_4/check.log | uniq -c    | wc -l
	#openssh
	grep "CWE-415"   ./benchmarks/openssh-portable/double_free_1/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssh-portable/double_free_2/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssh-portable/double_free_3/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssh-portable/double_free_4/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssh-portable/double_free_5/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssh-portable/double_free_6/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssh-portable/double_free_7/check.log | uniq -c    | wc -l
	#openssl
	grep "CWE-415"   ./benchmarks/openssl/double_free_1/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssl/double_free_2/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssl/double_free_3/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssl/double_free_4/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/openssl/double_free_5/check.log | uniq -c    | wc -l

	#tmux

	grep "CWE-415"   ./benchmarks/tmux/double_free_1/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/tmux/double_free_2/check.log | uniq -c    | wc -l
	grep "CWE-415"   ./benchmarks/tmux/double_free_3/check.log | uniq -c    | wc -l


unittestuaf:
	# binutils

	grep "===== possiable asias ====="   ./benchmarks/binutils/use_after_free_1/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/binutils/use_after_free_2/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/binutils/use_after_free_3/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/binutils/use_after_free_4/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/binutils/use_after_free_5/check.log | uniq -c    | wc -l

	# git

	grep "===== possiable asias ====="   ./benchmarks/git/use_after_free_1/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/git/use_after_free_2/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/git/use_after_free_3/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/git/use_after_free_4/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/git/use_after_free_5/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/git/use_after_free_6/check.log | uniq -c    | wc -l

	#openssh

	grep "===== possiable asias ====="   ./benchmarks/openssh-portable/use_after_free_1/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/openssh-portable/use_after_free_2/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/openssh-portable/use_after_free_3/check.log | uniq -c    | wc -l

	#openssl
	grep "===== possiable asias ====="   ./benchmarks/openssl/use_after_free_1/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/openssl/use_after_free_2/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/openssl/use_after_free_3/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/openssl/use_after_free_4/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/openssl/use_after_free_5/check.log | uniq -c    | wc -l
	#tmux

	grep "===== possiable asias ====="   ./benchmarks/tmux/use_after_free_1/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/tmux/use_after_free_2/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/tmux/use_after_free_3/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/tmux/use_after_free_4/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/tmux/use_after_free_5/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/tmux/use_after_free_6/check.log | uniq -c    | wc -l
	grep "===== possiable asias ====="   ./benchmarks/tmux/use_after_free_7/check.log | uniq -c    | wc -l

unittestuaf2:
	# binutils

	grep "CWE-416"   ./benchmarks/binutils/use_after_free_1/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/binutils/use_after_free_2/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/binutils/use_after_free_3/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/binutils/use_after_free_4/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/binutils/use_after_free_5/check.log | uniq -c    | wc -l

	# git

	grep "CWE-416"   ./benchmarks/git/use_after_free_1/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/git/use_after_free_2/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/git/use_after_free_3/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/git/use_after_free_4/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/git/use_after_free_5/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/git/use_after_free_6/check.log | uniq -c    | wc -l

	#openssh

	grep "CWE-416"   ./benchmarks/openssh-portable/use_after_free_1/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/openssh-portable/use_after_free_2/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/openssh-portable/use_after_free_3/check.log | uniq -c    | wc -l

	#openssl
	grep "CWE-416"   ./benchmarks/openssl/use_after_free_1/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/openssl/use_after_free_2/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/openssl/use_after_free_3/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/openssl/use_after_free_4/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/openssl/use_after_free_5/check.log | uniq -c    | wc -l
	#tmux

	grep "CWE-416"   ./benchmarks/tmux/use_after_free_1/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/tmux/use_after_free_2/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/tmux/use_after_free_3/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/tmux/use_after_free_4/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/tmux/use_after_free_5/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/tmux/use_after_free_6/check.log | uniq -c    | wc -l
	grep "CWE-416"   ./benchmarks/tmux/use_after_free_7/check.log | uniq -c    | wc -l

unittest:
	#unittet
	# binutils
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_6/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_7/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_8/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_9/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/leak_10/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/use_after_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/use_after_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/use_after_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/use_after_free_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/use_after_free_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/double_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/double_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/double_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/double_free_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/binutils/double_free_5/check.log | uniq -c    | wc -l
	# git
	grep "Please submit a full bug report"   ./benchmarks/git/leak_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_6/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_7/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_8/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_9/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/leak_10/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/use_after_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/use_after_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/use_after_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/use_after_free_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/use_after_free_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/use_after_free_6/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/double_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/double_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/double_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/git/double_free_4/check.log | uniq -c    | wc -l
	#openssh
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_6/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_7/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_8/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_9/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/leak_10/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/use_after_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/use_after_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/use_after_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/double_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/double_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/double_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/double_free_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/double_free_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/double_free_6/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssh-portable/double_free_7/check.log | uniq -c    | wc -l
	#openssl
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_6/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_7/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_8/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_9/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/leak_10/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/double_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/double_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/double_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/double_free_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/double_free_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/use_after_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/use_after_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/use_after_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/use_after_free_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/openssl/use_after_free_5/check.log | uniq -c    | wc -l
	#tmux
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_6/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_7/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_8/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_9/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/leak_10/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/use_after_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/use_after_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/use_after_free_3/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/use_after_free_4/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/use_after_free_5/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/use_after_free_6/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/use_after_free_7/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/double_free_1/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/double_free_2/check.log | uniq -c    | wc -l
	grep "Please submit a full bug report"   ./benchmarks/tmux/double_free_3/check.log | uniq -c    | wc -l

undeffun:
	# binutils				
	grep "undefined reference to"   ./benchmarks/binutils/leak_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_6/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_7/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_8/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_9/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/leak_10/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/use_after_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/use_after_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/use_after_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/use_after_free_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/use_after_free_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/double_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/double_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/double_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/double_free_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/binutils/double_free_5/check.log | uniq -c    | wc -l				
	# git				
	grep "undefined reference to"   ./benchmarks/git/leak_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_6/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_7/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_8/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_9/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/leak_10/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/use_after_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/use_after_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/use_after_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/use_after_free_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/use_after_free_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/use_after_free_6/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/double_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/double_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/double_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/git/double_free_4/check.log | uniq -c    | wc -l				
	#openssh				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_6/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_7/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_8/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_9/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/leak_10/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/use_after_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/use_after_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/use_after_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/double_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/double_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/double_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/double_free_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/double_free_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/double_free_6/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssh-portable/double_free_7/check.log | uniq -c    | wc -l				
	#openssl				
	grep "undefined reference to"   ./benchmarks/openssl/leak_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_6/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_7/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_8/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_9/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/leak_10/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/double_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/double_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/double_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/double_free_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/double_free_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/use_after_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/use_after_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/use_after_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/use_after_free_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/openssl/use_after_free_5/check.log | uniq -c    | wc -l				
	#tmux				
	grep "undefined reference to"   ./benchmarks/tmux/leak_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_6/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_7/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_8/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_9/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/leak_10/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/use_after_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/use_after_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/use_after_free_3/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/use_after_free_4/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/use_after_free_5/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/use_after_free_6/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/use_after_free_7/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/double_free_1/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/double_free_2/check.log | uniq -c    | wc -l				
	grep "undefined reference to"   ./benchmarks/tmux/double_free_3/check.log | uniq -c    | wc -l	

# graphviz:  			
gpz:  
	@sh ./test.sh $(FILE_NAME)
clean:
	-rm -rf ./test ./test.dSYM *.o
	-rm -rf ./check.log
	-rm -rf $(BINUTILSL)1/check.log
	-rm -rf $(BINUTILSL)2/check.log
	-rm -rf $(BINUTILSL)3/check.log
	-rm -rf $(BINUTILSL)4/check.log
	-rm -rf $(BINUTILSL)5/check.log
	-rm -rf $(BINUTILSL)6/check.log
	-rm -rf $(BINUTILSL)7/check.log
	-rm -rf $(BINUTILSL)8/check.log
	-rm -rf $(BINUTILSL)9/check.log
	-rm -rf $(BINUTILSL)10/check.log

	-rm -rf $(BINUTILSU)1/check.log
	-rm -rf $(BINUTILSU)2/check.log
	-rm -rf $(BINUTILSU)3/check.log
	-rm -rf $(BINUTILSU)4/check.log
	-rm -rf $(BINUTILSU)5/check.log

	-rm -rf $(BINUTILSD)1/check.log
	-rm -rf $(BINUTILSD)2/check.log
	-rm -rf $(BINUTILSD)3/check.log
	-rm -rf $(BINUTILSD)4/check.log
	-rm -rf $(BINUTILSD)5/check.log
	

	-rm -rf $(GITL)1/check.log
	-rm -rf $(GITL)2/check.log
	-rm -rf $(GITL)3/check.log
	-rm -rf $(GITL)4/check.log
	-rm -rf $(GITL)5/check.log
	-rm -rf $(GITL)6/check.log
	-rm -rf $(GITL)7/check.log
	-rm -rf $(GITL)8/check.log
	-rm -rf $(GITL)9/check.log
	-rm -rf $(GITL)10/check.log

	-rm -rf $(GITU)1/check.log
	-rm -rf $(GITU)2/check.log
	-rm -rf $(GITU)3/check.log
	-rm -rf $(GITU)4/check.log
	-rm -rf $(GITU)5/check.log
	-rm -rf $(GITU)6/check.log

	-rm -rf  $(GITD)1/check.log
	-rm -rf  $(GITD)2/check.log
	-rm -rf  $(GITD)3/check.log

	-rm -rf  $(OPSSHL)1/check.log
	-rm -rf  $(OPSSHL)2/check.log
	-rm -rf  $(OPSSHL)3/check.log
	-rm -rf  $(OPSSHL)4/check.log
	-rm -rf  $(OPSSHL)5/check.log
	-rm -rf  $(OPSSHL)6/check.log
	-rm -rf  $(OPSSHL)7/check.log
	-rm -rf  $(OPSSHL)8/check.log
	-rm -rf  $(OPSSHL)9/check.log
	-rm -rf  $(OPSSHL)10/check.log

	-rm -rf $(OPSSHU)1/check.log
	-rm -rf $(OPSSHU)2/check.log
	-rm -rf $(OPSSHU)3/check.log

	-rm -rf $(OPSSHD)1/check.log
	-rm -rf $(OPSSHD)2/check.log
	-rm -rf $(OPSSHD)3/check.log
	-rm -rf $(OPSSHD)4/check.log
	-rm -rf $(OPSSHD)5/check.log
	-rm -rf $(OPSSHD)6/check.log
	-rm -rf $(OPSSHD)7/check.log


	-rm -rf $(OPSSLL)1/check.log
	-rm -rf $(OPSSLL)2/check.log
	-rm -rf $(OPSSLL)3/check.log
	-rm -rf $(OPSSLL)4/check.log
	-rm -rf $(OPSSLL)5/check.log
	-rm -rf $(OPSSLL)6/check.log
	-rm -rf $(OPSSLL)7/check.log
	-rm -rf $(OPSSLL)8/check.log
	-rm -rf $(OPSSLL)9/check.log
	-rm -rf $(OPSSLL)10/check.log

	-rm -rf  $(OPSSLU)1/check.log
	-rm -rf  $(OPSSLU)2/check.log
	-rm -rf  $(OPSSLU)3/check.log
	-rm -rf  $(OPSSLU)4/check.log
	-rm -rf  $(OPSSLU)5/check.log

	-rm -rf  $(OPSSLD)1/check.log
	-rm -rf  $(OPSSLD)2/check.log
	-rm -rf  $(OPSSLD)3/check.log
	-rm -rf  $(OPSSLD)4/check.log
	-rm -rf  $(OPSSLD)5/check.log


	-rm -rf $(TMUXL)1/check.log
	-rm -rf $(TMUXL)2/check.log
	-rm -rf $(TMUXL)3/check.log
	-rm -rf $(TMUXL)4/check.log
	-rm -rf $(TMUXL)5/check.log
	-rm -rf $(TMUXL)6/check.log
	-rm -rf $(TMUXL)7/check.log
	-rm -rf $(TMUXL)8/check.log
	-rm -rf $(TMUXL)9/check.log
	-rm -rf $(TMUXL)10/check.log

	-rm -rf $(TMUXU)1/check.log
	-rm -rf $(TMUXU)2/check.log
	-rm -rf $(TMUXU)3/check.log
	-rm -rf $(TMUXU)4/check.log
	-rm -rf $(TMUXU)5/check.log
	-rm -rf $(TMUXU)6/check.log
	-rm -rf $(TMUXU)7/check.log

	-rm -rf $(TMUXD)1/check.log
	-rm -rf $(TMUXD)2/check.log
	-rm -rf $(TMUXD)3/check.log

	
# debug:
# 	$(eval PLUGIN=-fplugin-arg-newplugin-debugmod=1 -fplugin-arg-newplugin-vscode_extensionmod=1)
# 	@$(MAKE) test.o