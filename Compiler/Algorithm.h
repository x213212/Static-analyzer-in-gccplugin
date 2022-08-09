void detect(struct plugin_argument *argv, int argc)
{
	struct rusage ru;
	struct timeval utime;
	struct timeval stime;
	struct cgraph_node *node;
	struct var_points_to vpt;
	const int N = 1e2;
	unsigned i;
	breakpoint getbp;
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
	
	fprintf(stderr, "--------plugin argument-----------\n");
	for (int i = 0; i < argc; i++)
	{

		// relatemod
		if (!strcmp(argv[i].key, "relatemod"))
			if (strtol(argv[i].value, NULL, 10)) // char* to int
				relatemod = true;
			else
				relatemod = false;
		if (!strcmp(argv[i].key, "retnotheapobjskipcheck"))
			if (strtol(argv[i].value, NULL, 10)) // char* to int
				retnotheapobjskipcheck = true;
			else
				retnotheapobjskipcheck = false;
		if (!strcmp(argv[i].key, "traceallfucntion"))
			if (strtol(argv[i].value, NULL, 10)) // char* to int
				traceallfucntion = true;
			else
				traceallfucntion = false;
		if (!strcmp(argv[i].key, "prebranchexit"))
			if (strtol(argv[i].value, NULL, 10)) // char* to int
				prebranchexit = true;
			else
				prebranchexit = false;
		if (!strcmp(argv[i].key, "debugoutput"))
			if (strtol(argv[i].value, NULL, 10)) // char* to int
				debugoutput = true;
			else
				debugoutput = false;
		else if (!strcmp(argv[i].key, "debugmod"))
			if (strtol(argv[i].value, NULL, 10)) // char* to int
				debugmod = true;
			else
				debugmod = false;
		else if (!strcmp(argv[i].key, "vscode_extensionmod"))
			if (strtol(argv[i].value, NULL, 10)) // char* to int
				vscode_extensionmod = true;
			else
				vscode_extensionmod = false;
		else if (!strcmp(argv[i].key, "contextsentive"))
			if (strtol(argv[i].value, NULL, 10)) // char* to int
				contextsentive = true;
			else
				contextsentive = false;
		fprintf(stderr, "%s %s\n", argv[i].key, argv[i].value);
	}

	if (vscode_extensionmod)
	{
		std::ifstream ifs("/root/.vscode-server/data/User/globalStorage/x213212.helloworld-sample/breakpoint.txt", std::ios::in);
		fprintf(stderr, "==============breakpoint=========\n");
		if (!ifs.is_open())
		{
			fprintf(stderr, "vscode_extensionmod is open ,no set breakpoint.\n");
			// cout << "vscode_extensionmod is open ,no set breakpoint.\n";
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
	clock_gettime(CLOCK_MONOTONIC, &Globaltime);

	fprintf(stderr, "=============== The first stage : Point of interest stmt collect =================\n");
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		// debug_tree(DECL_ATTRIBUTES(node->decl));

		if (!gimple_has_body_p(node->decl))
			continue;
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(node->decl));
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
				// fprintf(stderr, "node:= %d \n ", bb->index);
				// fprintf(stderr,"node:= %d \n succs:=",bb->index);
				FOR_EACH_EDGE(e, ei, bb->succs)
				{
					DFS.addEdge(bb->index, e->dest->index);
					// fprintf(stderr, "	from_branch :=%d\n", e->src->index);
					// fprintf(stderr, "	next_branch :=%d\n", e->dest->index);
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
				else if (is_gimple_assign(gc))
				{
					collect_function_call(gc, node, bb);
				}
				else if (gimple_code(gc) == GIMPLE_RETURN)
				{

					collect_function_return(gc, node, bb);
				}
				else if (gimple_code(gc) == GIMPLE_PREDICT)
				{
					collect_function_continue(gc, node, bb);
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
	double time_used3;
	time_t rawtime;
	struct tm *timeinfo;
	// count_now collect breakpoint
	int Entrypoint = 0;
	clock_gettime(CLOCK_MONOTONIC, &aend);
	temp = diff(Globaltime, aend);
	time_used3 = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;
	fprintf(stderr, "start PointerConstraint\n");

	// fprintf(stderr, "pointer ftable is %d \n", ftable->size);
	fprintf(stderr, "ptable point %d \n", pointtablecount);
	fprintf(stderr, "===============The second stage : Mapping stmt=================\n");
	FunctionStmtMappingAssign(ptable, used_stmt);

	ptb *processtable = ptable;
	fprintf(stderr, "===============The second stage : Program slicing=================\n");
	int allcolectCount = 0;
	if (GIMPLE_FREE_COUNT || freeanysis)
	{

		if (processtable->size > 0)
		{

			int count = 0;

			while (processtable != NULL)
			{

				// if (TREE_CODE(TREE_TYPE(processtable->target)) == METHOD_TYPE  || TREE_CODE(TREE_TYPE(processtable->target)) == RECORD_TYPE || !(TREE_CODE(processtable->target) == SSA_NAME))
				// tree treetype = TREE_TYPE(processtable->target);
				// if (treetype)
				// {
				// 	debug(treetype);
				// 	if (TREE_CODE(treetype) == METHOD_TYPE || TREE_CODE(treetype) == RECORD_TYPE)
				// 	{
				// 		processtable = processtable->next;
				// 		continue;
				// 	}
				// }

				// debug_tree(processtable->target);
				if (processtable->target == NULL)
				{
					processtable = processtable->next;
					continue;
				}
				if (TREE_CODE(processtable->target) == INTEGER_CST)
				{
					processtable = processtable->next;
					continue;
				}
				// debug_tree(processtable->target);
				// fprintf(stderr, "===============pt_solution=================\n");
				// struct ptr_info_def *pi1;
				// pi1 = SSA_NAME_PTR_INFO(processtable->target);
				// struct pt_solution *pt1 = &pi1->pt;
				// dump_points_to_solution(stderr, pt1);
				// fprintf(stderr, "===============pt_solution=================\n");
				int colectCount = 0;
				used_stmt = NULL;
				gimple_array start;
				start.stmt = NULL;
				used_stmt = &start;
				const char *name;
				if ((TREE_CODE(processtable->target) == SSA_NAME))
				{
					// a=(_1,_b,&a,b)
					// debug_tree(processtable->target);
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
				}
				else
				{
					Varnew_search_imm_use(used_stmt, processtable->last_stmt, processtable->target, processtable->target);
				}

				now_tree = processtable->target;
				// debug_tree( processtable->target);
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
					// debug_gimple_stmt(new_gimple_array[new_gimple_array.size()]);
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
				fprintf2(stderr, "=============== *this point analyzable =================\n");
				debug2(processtable->last_stmt);
				fprintf2(stderr, "=============== program slcing stmt count:%d=================\n", colectCount);
				allcolectCount += colectCount;
				warning_at2(gimple_location_safe(processtable->last_stmt), 0, "use location");
				fprintf2(stderr, "========================================================\n");
				// debug(processtable->swap_stmt);

				if (processtable->next == NULL)
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
		// sleep(5); //5s check time
		clock_gettime(CLOCK_MONOTONIC, &aend);
		temp = diff(Globaltime, aend);
		time_used2 = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;

		printfBasicblock();
	}
	else
		fprintf(stderr, "\033[40;41m GIMPLE STMT NO FREE STMT\033[0m\n");

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
	fprintf(stderr, "\033[40;34m    analyzable ptable point (function return heap-object): %d \033[0m\n", FUCNTION_RETURN_HEAP_OBJECT + GIMPLE_MALLOC_COUNT);
	fprintf(stderr, "\033[40;34m    relate stmt of analyzable ptable : %d \033[0m\n", allcolectCount);
	fprintf(stderr, "\033[40;34m    used_stmt array stack totalsize of : %f mb\033[0m\n", (totalsize * 0.000001));
	fprintf(stderr, "\033[40;34m    collect time: : %f s \033[0m\n", time_used3);
	fprintf(stderr, "\033[40;34m    mapping time: : %f s \033[0m\n", time_used);
	fprintf(stderr, "\033[40;34m    algorithm time: %f s \033[0m\n", time_used2);
	fprintf(stderr, "\033[40;34m    gimple stmt count : : %d \033[0m\n", gimplestmt_count);
	fprintf(stderr, "\033[40;34m    this report analysis in %s \033[0m\n", asctime(timeinfo));
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
}