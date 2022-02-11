#include <iostream>
#include <list>
using namespace std;
#include "system.h"
#include "config.h"
#include "cfgloop.h"
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>
#include <stack>
#include <string.h>
#include <math.h>
#include "dfs.h"
#include "define.h"
#include "unit.h"
#include "debug.h"
#include "gimplearray.h"
#include "getinfo.h"
#include "trace.h"
#include "collect.h"
#include "mapping.h"
#include "performance.h"
#include "tree-cfg.h"
#include "stmt.h"
#include "pretty-print.h"
#include "dumpfile.h"
#include "new_imm_use.h"
#include "check.h"
#include "analyze.h"
#include <unistd.h>
#include <string>
#include <cstdio>
#include <cstring>

void record_fucntion(cgraph_node *node)
{
	cgraph_edge *e;
	basic_block bb;
	FOR_EACH_DEFINED_FUNCTION(node)
	{

		if (!gimple_has_body_p(node->decl))
			continue;
		push_cfun(node->get_fun());

		if (cfun == NULL)
		{
			pop_cfun();
			continue;
		}
		enum availability avail;

		function_path_array fun_array;

		vector<function_path> function_path_array;
		fun_array.function_path_array = function_path_array;

		for (e = node->callees; e; e = e->next_callee)
		{
			cgraph_node *caller = e->caller->inlined_to ? e->caller->inlined_to : e->caller;
			cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);

			if (callee != NULL)
			{
				int find = 0;
				vector<function_path>::iterator it_i;
				for (it_i = fun_array.function_path_array.begin(); it_i != fun_array.function_path_array.end(); ++it_i)
				{
					if (it_i->next == callee->decl)
						find = 1;
				}

				if (find == 0)
				{

					struct function_path path_type;
					path_type.cgnext = callee;
					path_type.next = callee->decl;
					fun_array.function_path_array.push_back(path_type);
				}
			}
		}

		FOR_EACH_BB_FN(bb, cfun)
		{
			for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *gc = gsi_stmt(gsi);
				if (gimple_cond_code(gc))
				{
					enum tree_code code = gimple_cond_code(gc);
					// if (code == LT_EXPR || code == GT_EXPR || code == LE_EXPR || code == GE_EXPR || code == EQ_EXPR || code == NE_EXPR)

					// 	fprintf(stderr, "--------GIMPLE ok -------\n");
					// if (dom_info_state(CDI_DOMINATORS) == DOM_OK)

					if (!is_gimple_assign(gc))
					{
						symbolicExecution.push_back(bb);
						symbolicinfo symbolicinfo;
						symbolicinfo.cond_stmt = gc;
						symbolicinfo.cond_lhs = gimple_cond_lhs(gc);
						symbolicinfo.cond_rhs = gimple_cond_rhs(gc);
						symbolicinfo.node = node;

						if (bb != cfun->cfg->x_exit_block_ptr->prev_bb)
						{
							edge e;
							edge_iterator ei;

							int init = 0;
							FOR_EACH_EDGE(e, ei, bb->succs)
							{
								if (init == 0)
								{
									symbolicinfo.cond_truebranch = e->dest;
									// fprintf(stderr, " true succs:= %d\n", e->dest->index);
								}

								else
								{
									symbolicinfo.cond_falsebranch = e->dest;
									// fprintf(stderr, " false succs:= %d\n", e->dest->index);
								}
								init++;
							}
						}
						syminfo->put(bb, symbolicinfo);
					}
				}
			}
		}
		function_path_collect->put(node->get_fun()->decl, fun_array);
		pop_cfun();
	}
}

void PointerConstraint(ptb *ptable, ptb *ftable)
{
	struct timespec astart, aend;
	gimple_array *used_stmt = NULL;
	gimple_array start;
	start.stmt = NULL;
	used_stmt = &start;
	clock_gettime(CLOCK_MONOTONIC, &astart);
	struct cgraph_node *node;
	struct timespec temp;
	double time_used;
	double time_used2;
	// count_now collect breakpoint
	int Entrypoint = 0;

	fprintf(stderr, "start PointerConstraint\n");

	// fprintf(stderr, "pointer ftable is %d \n", ftable->size);
	fprintf(stderr, "ptable point %d \n", pointtablecount);
	fprintf(stderr, "===============The second stage : Mapping stmt=================\n");
	FunctionStmtMappingAssign(ptable, used_stmt);

	ptb *processtable = ptable;
	fprintf(stderr, "===============The second stage : Program slicing=================\n");

	if (GIMPLE_FREE_COUNT)
	{

		if (processtable->size > 0)
		{

			int count = 0;

			while (processtable->next != NULL)
			{

				if (TREE_CODE(TREE_TYPE(processtable->target)) == METHOD_TYPE || TREE_CODE(TREE_TYPE(processtable->target)) == FUNCTION_TYPE || TREE_CODE(TREE_TYPE(processtable->target)) == RECORD_TYPE || !(TREE_CODE(processtable->target) == SSA_NAME))
				{
					processtable = processtable->next;
					continue;
				}

				if (TREE_CODE(processtable->target) == INTEGER_CST)
				{
					processtable = processtable->next;
					continue;
				}

				int colectCount = 0;
				used_stmt = NULL;
				gimple_array start;
				start.stmt = NULL;
				used_stmt = &start;
				const char *name;

				gimple *def_stmt = SSA_NAME_DEF_STMT(processtable->target);
				levelsize = 0;

				if (gimple_code(def_stmt) != GIMPLE_NOP || gimple_code(def_stmt) != ADDR_EXPR)
				{
					int pass = 0;
					if (gimple_code(def_stmt) == GIMPLE_CALL)
						if (TREE_CODE(gimple_call_fn(def_stmt)) != MEM_REF)
							pass = 1;

					if (gimple_code(def_stmt) == GIMPLE_ASSIGN)
						if (TREE_CODE(gimple_assign_rhs1(def_stmt)) != MEM_REF && TREE_CODE(gimple_assign_rhs1(def_stmt)) != VAR_DECL)
							pass = 1;

					if (TREE_CODE(processtable->target) != ADDR_EXPR && pass == 1)
						if (def_stmt)
							if (gimple_code(def_stmt) == GIMPLE_CALL)
								if (gimple_call_fn(def_stmt) && gimple_call_fndecl(def_stmt))
								{

									name = get_name(gimple_call_fn(def_stmt));
									if (name != NULL)
										if (strcmp(name, "malloc"))
											Prenew_search_imm_use(used_stmt, processtable->target, processtable->target);
								}
				}

				now_tree = processtable->target;

				new_search_imm_use(used_stmt, processtable->target, processtable->target);
				set_gimple_array(used_stmt, processtable->last_stmt, processtable->target, processtable->target, NULL);

				if (!strcmp(get_tree_code_name(TREE_CODE(used_stmt->target)), "<invalid tree code>"))
				{
					processtable = processtable->next;
					continue;
				}

				while (new_gimple_array.size())
				{
					colectCount++;
					// debug_gimple_stmt(new_gimple_array.back());
					new_gimple_array.pop_back();
				}
				while (new_gimpletree_array.size())
				{
					new_gimpletree_array.pop_back();
				}

				if (treeGimpleArray->get(processtable->target) != NULL)
				{
					processtable = processtable->next;
					continue;
				}

				treeGimpleArray->put(processtable->target, *used_stmt);
				Entrypoint++;
				fprintf(stderr, "=============== *this point analyzable =================\n");
				debug(processtable->last_stmt);
				// fprintf(stderr, "=============== *this point analyzable =================\n");
				// debug(processtable->swap_stmt);

				if (processtable->next->target == NULL)
					break;
				else
				{
					processtable = processtable->next;
				}
			}
		}

		FunctionStmtMappingRet(ptable, ftable, used_stmt);
		fprintf(stderr, "===============The second stage : build fucntion type=================\n");
		fprintf(stderr, "\033[40;41mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
		fprintf(stderr, "\033[40;41mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
		fprintf(stderr, "    =()=\n");
		fprintf(stderr, " ,/'\_||_\n");
		fprintf(stderr, "  (___  `.\n");
		fprintf(stderr, " ./  `=='\n");
		fprintf(stderr, "      |||\n");
		fprintf(stderr, "      |||\n");
		fprintf(stderr, "      |||\n");
		fprintf(stderr, "      |||\n");
		fprintf(stderr, "\033[40;41mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
		fprintf(stderr, "\033[40;41mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");

		clock_gettime(CLOCK_MONOTONIC, &aend);
		temp = diff(astart, aend);
		time_used = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;
		fprintf(stderr, "\n=============== The third stage : Start detection  =================\n");
		dump_fucntion(node, ptable, used_stmt);

		// fprintf(stderr, "physicalMem%d\n", GetProcessMemory().physicalMem);
		// fprintf(stderr, "virtualMem%d\n", GetProcessMemory().virtualMem);

		fprintf(stderr, "\n=============== The third stage : detection  End=================\n");
		clock_gettime(CLOCK_MONOTONIC, &aend);
		temp = diff(astart, aend);

		time_used2 = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;

		printfBasicblock();
	}
	else
		fprintf(stderr, "\033[40;41m GIMPLE STMT NO FREE STMT\033[0m\n");
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "    =()=\n");
	fprintf(stderr, " ,/'\_||_\n");
	fprintf(stderr, "  (___  `.\n");
	fprintf(stderr, " ./  `=='\n");
	fprintf(stderr, "                   \n");
	fprintf(stderr, "    ~~~~~~~~~~~  \n");
	fprintf(stderr, " ~~~~~~\n");
	fprintf(stderr, "           ~~~~~~~\n");
	fprintf(stderr, "\033[40;34m    gimple malloc count : %d \033[0m\n", GIMPLE_MALLOC_COUNT);
	fprintf(stderr, "\033[40;34m    gimple free   count : %d \033[0m\n", GIMPLE_FREE_COUNT);
	fprintf(stderr, "\033[40;34m    all ptable point : %d \033[0m\n", pointtablecount);
	fprintf(stderr, "\033[40;34m    analyzable ptable point : %d \033[0m\n", Entrypoint);
	fprintf(stderr, "\033[40;34m    used_stmt array stack totalsize of : %f mb\033[0m\n", (totalsize * 0.000001));
	fprintf(stderr, "\033[40;34m    collect time: : %f s \033[0m\n", time_used);
	fprintf(stderr, "\033[40;34m    algorithm time: %f s \033[0m\n", time_used2);
	fprintf(stderr, "\033[40;34m    gimple stmt count : : %d \033[0m\n", gimplestmt_count);
	fprintf(stderr, "\033[40;34m    this report analysis in %s \033[0m\n", asctime(timeinfo));
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
}
void detect()
{
	struct rusage ru;
	struct timeval utime;
	struct timeval stime;
	struct cgraph_node *node;
	struct var_points_to vpt;
	const int N = 1e2;
	unsigned i;
	breakpoint getbp;
	tree ptr;
	tree attr;
	function *ofun;
	function *fn;
	basic_block bb;
	ipa = true;
	cgraph_edge *e;
	tvpt = new hash_map<tree, var_points_to>;
	treeGimpleArray = new hash_map<tree, gimple_array>;
	syminfo = new hash_map<basic_block, symbolicinfo>;
	fDFS = new hash_map<cgraph_node *, Graph>;
	function_return_collect = new hash_map<tree, function_return_array>;
	function_assign_collect = new hash_map<tree, function_assign_array>;
	pthread_attr_setdetachstates = new hash_map<tree, pthread_attr_array>;
	function_pthread_detched_collect = new hash_map<tree, function_pthread_detched_array>;
	function_pthread_exit_collect = new hash_map<tree, function_pthread_exit_array>;
	function_path_collect = new hash_map<tree, function_path_array>;
	function_free_collect = new hash_map<tree, function_free_array>;
	function_graph_collect = new hash_map<tree, function_graph_array>;
	function_relate_collect = new hash_map<tree, function_relate_array>;

	if (vscode_extensionmod)
	{
		std::ifstream ifs("/root/.vscode-server/data/User/globalStorage/vscode-samples.helloworld-sample/breakpoint.txt", std::ios::in);
		fprintf(stderr, "==============breakpoint=========\n");
		if (!ifs.is_open())
		{
			cout << "Failed to open file.\n";
			return;
		}
		else
		{
			std::string name;
			int score;
			while (ifs >> name >> score)
			{
				getbp.name = name;
				getbp.line = score;
				fprintf(stderr, "%s %d\n", getbp.name.c_str(), getbp.line);

				vbreakpoint.push_back(getbp);
			}
			ifs.close();
		}
		ifs.close();
	}

	srand((unsigned)time(NULL) + getpid());
	fprintf(stderr, "=======ipa_pta=========\n");
	init_table();

	double s = 0.0;
	struct timespec start, end;

	clock_gettime(CLOCK_MONOTONIC, &start);
	getrusage(RUSAGE_SELF, &ru);
	utime = ru.ru_utime;
	stime = ru.ru_stime;

	fprintf(stderr, "=============== The first stage : Point of interest stmt collect =================\n");
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(node->decl));
		// debug_tree(DECL_ATTRIBUTES(node->decl));

		if (!gimple_has_body_p(node->decl))
			continue;
		push_cfun(node->get_fun());

		if (cfun == NULL)
		{
			pop_cfun();
			free_dominance_info(CDI_DOMINATORS);
			// calculate_dominance_info(CDI_DOMINATORS);
			continue;
		}
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(node->decl));
		// debug_tree(DECL_ATTRIBUTES(node->decl));
		/*calculate dominator tree*/
		calculate_dominance_info(CDI_DOMINATORS);

		/*create DFS graph, Algorithm 1 and 2*/
		Graph DFS;
		DFS.init_Graph(cfun->cfg->x_last_basic_block);
		FOR_EACH_BB_FN(bb, cfun)
		{
			if (bb != cfun->cfg->x_exit_block_ptr->prev_bb)
			{
				edge e;
				edge_iterator ei;
				fprintf(stderr, "node:= %d \n ", bb->index);
				// fprintf(stderr,"node:= %d \n succs:=",bb->index);
				FOR_EACH_EDGE(e, ei, bb->succs)
				{
					DFS.addEdge(bb->index, e->dest->index);
					// fprintf(stderr, "	from_branch :=%d\n", e->src->index);
					fprintf(stderr, "	next_branch :=%d\n", e->dest->index);
					// fprintf(stderr,"%d",e->dest->index);
				}
			}
			fDFS->put(node, DFS);
			// debug_bb(bb);

			for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *gc = gsi_stmt(gsi);

				if (is_gimple_call(gc))
				{

					collect_function_call(gc, node, bb);
				}
				if (is_gimple_assign(gc))
				{

					collect_function_call(gc, node, bb);
				}
				if (gimple_code(gc) == GIMPLE_RETURN)
				{

					collect_function_return(gc, node, bb);
				}
			}
		}

		pop_cfun();
	}

	fprintf(stderr, "=============== The first stage :  record fucntion =================\n");
	record_fucntion(node);
	// Start analysis
	if (ipa)
		PointerConstraint(ptable, ftable);

	// fprintf(stderr, "physicalMem%f\n", GetProcessMemory().physicalMem);
	// fprintf(stderr, "virtualMem%f\n", GetProcessMemory().virtualMem);

	delete tvpt;
	delete treeGimpleArray;
	delete syminfo;
	delete fDFS;
	delete function_return_collect;
	delete function_assign_collect;
	delete pthread_attr_setdetachstates;
	delete function_pthread_detched_collect;
	delete function_pthread_exit_collect;
	delete function_path_collect;
	delete function_free_collect;
	delete function_graph_collect;
	delete function_relate_collect;
	clock_gettime(CLOCK_MONOTONIC, &end);
	struct timespec temp = diff(start, end);
	double time_used;
	time_used = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;
	struct timevar_time_def now;

	ofstream myfile("time.txt");
	if (myfile.is_open())
	{
		myfile << "utime_used: " << time_used << " s;\n";
		myfile << "stime_used: " << time_used << " s;\n";

		myfile.close();
	}
	else
		cout << "Unable to open file";
}

void insert_always_inline()
{
	cgraph_node *node;
	const char *name;
	bool always_inline;
	// fprintf(stderr,"=======inline=========\n");

	FOR_EACH_DEFINED_FUNCTION(node)
	{
		basic_block bb;
		cgraph_edge *e;

		if (!gimple_has_body_p(node->decl))
			continue;
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

		tree attr;
		enum availability avail;
		for (e = node->callees; e; e = e->next_callee)
		{

			cgraph_node *caller = e->caller->inlined_to ? e->caller->inlined_to : e->caller;
			cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);
			tree callee_tree = callee ? DECL_FUNCTION_SPECIFIC_OPTIMIZATION(callee->decl) : NULL;
			// DECL_DISREGARD_INLINE_LIMITS (callee->decl)=1;
			if (DECL_ATTRIBUTES(callee->decl) != NULL)
			{
				attr = get_attribute_name(DECL_ATTRIBUTES(callee->decl));
				// debug_tree(attr);
			}
			else
			{
				// if(callee->decl)
				// DECL_ATTRIBUTES (callee->decl) = tree_cons (get_identifier ("always_inline"),
				// NULL, DECL_ATTRIBUTES (callee->decl));
			}

			if (lookup_attribute("noinline", DECL_ATTRIBUTES(callee->decl)) == NULL)
			{
				fprintf(stderr, "=======node_fun:%s=========\n", get_name(callee->decl));
				DECL_ATTRIBUTES(callee->decl) = tree_cons(get_identifier("noinline"), NULL, DECL_ATTRIBUTES(callee->decl));
				DECL_DISREGARD_INLINE_LIMITS(callee->decl) = 1;
				debug_tree(DECL_ATTRIBUTES(callee->decl));
			}
			else
			{
				fprintf(stderr, "=======node_fun:%s=========\n", get_name(callee->decl));

				debug_tree(DECL_ATTRIBUTES(callee->decl));
			}
			// if (!lookup_attribute("noinline", DECL_ATTRIBUTES(callee->decl)))
			// {
			// 	fprintf(stderr, "=======node_fun:%s=========\n", get_name(callee->decl));
			// 	DECL_ATTRIBUTES(callee->decl) = tree_cons(get_identifier("noinline"), NULL, DECL_ATTRIBUTES(callee->decl));
			// 	DECL_DISREGARD_INLINE_LIMITS(callee->decl) = 1;
			// 	debug_tree(DECL_ATTRIBUTES(callee->decl));
			// }
			// 	always_inline = (DECL_DISREGARD_INLINE_LIMITS(callee->decl) && lookup_attribute("noinline", DECL_ATTRIBUTES(callee->decl)));
			// debug_tree(DECL_ATTRIBUTES(callee->decl));
		}
		push_cfun(DECL_STRUCT_FUNCTION(node->decl));
		// if (cfun == NULL)
		// {
		// }

		// if (!MAIN_NAME_P(DECL_NAME(node->decl)))
		// {
		// debug_tree(DECL_ATTRIBUTES(cfun->decl));
		// if( !lookup_attribute("noinline", DECL_ATTRIBUTES(node->decl))){
		// DECL_ATTRIBUTES(cfun->decl) = tree_cons(get_identifier("noinline"), NULL, DECL_ATTRIBUTES(cfun->decl));
		// DECL_DISREGARD_INLINE_LIMITS(cfun->decl) = 1;}

		// debug_tree(DECL_ATTRIBUTES(cfun->decl));
		// }
		// FOR_EACH_BB_FN(bb, cfun)
		// {

		// 	for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
		// 	{
		// 		gimple *gc = gsi_stmt(gsi);

		// 		if (is_gimple_call(gc))
		// 		{

		// 			if (gimple_call_fn(gc) == NULL)
		// 				continue;
		// 			name = get_name(gimple_call_fn(gc));
		// 			if (name == NULL)
		// 				continue;

		// 			if (!strcmp(name, "free") ||
		// 				!strcmp(name, "xfree") ||
		// 				!strcmp(name, "malloc") ||
		// 				!strcmp(name, "realloc") ||
		// 				!strcmp(name, "xmalloc") ||
		// 				!strcmp(name, "calloc") ||
		// 				!strcmp(name, "xcalloc") ||
		// 				!strcmp(name, "strdup"))
		// 			{

		// 				//always_inline = (DECL_DISREGARD_INLINE_LIMITS(node->decl) && lookup_attribute("no_inline", DECL_ATTRIBUTES(node->decl)));
		// 				if ( !MAIN_NAME_P(DECL_NAME(node->decl)))
		// 				{
		// 					DECL_ATTRIBUTES(node->decl) = tree_cons(get_identifier("no_inline"), NULL, DECL_ATTRIBUTES(node->decl));
		// 					DECL_DISREGARD_INLINE_LIMITS(node->decl) = 1;
		// 				}
		// 			}
		// 		}
		// 	}
		// }
		pop_cfun();
	}
};