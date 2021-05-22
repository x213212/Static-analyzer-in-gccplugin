
void dump_fucntion2(cgraph_node *node, tree function_tree, tree mallocStmt_tree)
{

	cgraph_edge *e;
	basic_block bb;
	tree t;
	if (node == NULL)

		fprintf(stderr, "=======node_fun: =========\n");

	FOR_EACH_DEFINED_FUNCTION(node)
	{
		//   if (!gimple_has_body_p (node->decl))
		//       continue;
		int fucntion_level = 0;

		push_cfun(node->get_fun());

		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

		if (cfun == NULL)
			continue;
		//mutlple entry point
		if (cfun->decl == function_tree)
		{
			/*calculate dominator tree*/
			calculate_dominance_info(CDI_DOMINATORS);

			/*create DFS graph, Algorithm 1 and 2*/
			FOR_EACH_BB_FN(bb, cfun)
			{
				// debug_bb(bb);
				// if (bb != cfun->cfg->x_exit_block_ptr->prev_bb)
				// {
				// 	edge e;
				// 	edge_iterator ei;
				// 	//fprintf(stderr,"node:= %d \n succs:=",bb->index);
				// 	FOR_EACH_EDGE(e, ei, bb->succs)
				// 	{
				// 		DFS.addEdge(bb->index, e->dest->index);
				// 		//fprintf(stderr,"%d",e->dest->index);
				// 	}
				// }
				for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
				{
					gimple *gc = gsi_stmt(gsi);

					if (is_gimple_assign(gc))
					{
						// debug_gimple_stmt(gc);
						// if (gimple_assign_lhs(gc) != NULL)
						// 	debug_tree(gimple_assign_lhs(gc));
						tree parms, p;
						// debug_tree(DECL_ARGUMENTS(node->get_fun()->decl));
						// static_chain_decl
						// debug_tree(DECL_ARGUMENTS(node->get_fun()->decl));
						// debug_tree(DECL_CHAIN(DECL_ARGUMENTS(node->get_fun()->decl)));
						p = DECL_ARGUMENTS(node->get_fun()->decl);
						/* Fix up the types of parms passed by invisible reference.  */
						fprintf(stderr, "\033[40;36m  find relate stmt with malloc pointer \033[0m\n");
						fprintf(stderr, "\033[40;36m =======node_fun:%s========= \033[0m\n", get_name(function_tree));

						fprintf(stderr, "\033[40;36m =======relate malloc argument:%s========= \033[0m\n", get_name(mallocStmt_tree));
						// debug_tree(mallocStmt_tree);
						for (t = DECL_ARGUMENTS(node->get_fun()->decl); t; t = DECL_CHAIN(t))
						{
							// debug_tree(t);
							if (gimple_assign_lhs(gc) != NULL)
								if (ptr_derefs_may_alias_p(t, mallocStmt_tree) && ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
								{
									fprintf(stderr, "\033[40;36m =======relate node_fun argument:%s========= \033[0m\n", get_name(t));
									// debug_tree(t);

									fprintf(stderr, "\033[40;36m =======relate gimple_assign_lhs:%s========= \033[0m\n", gimple_assign_lhs(gc));
									debug_gimple_stmt(gc);
									warning_at(gimple_location(gc), 0, "use location");

									fprintf(stderr, "\n ================== warring ================== \n");
									// sfprintf(stderr, "function return value related stmt \n");
									fprintf(stderr, "\033[40;35m    this pointer possible  reference other address \033[0m\n");
									fprintf(stderr, "\033[40;35m    or assign other \033[0m\n");
									// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
									// fprintf(stderr, "this stmt possible is heap-object 。\n");
									fprintf(stderr, "\n ================== warring ================== \n");
								}
						}
					
					}
				}
			}
		}
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}

int trace_function_path(tree function_tree, int fucntion_level, tree mallocStmt_tree, int *freecount)
{
	if (function_path_collect->get(function_tree) == NULL)
		return 0;
	function_path_array fun_array = *(function_path_collect->get(function_tree));

	vector<function_path> function_path_array = fun_array.function_path_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;

	// function_return_array callerFunArray = *(function_free_collect->get(function_tree));
	// vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
	// fprintf(stderr, "=======trace_function_path %s  function_call count: %d level :%d========\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	fprintf(stderr, "\033[40;44m =======trace_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	if (mallocStmt_tree != NULL_TREE)
		if (function_free_collect->get(function_tree) != NULL)
		{
			function_free_array callerFunArray = *(function_free_collect->get(function_tree));
			vector<free_type> callerRetTypearray = callerFunArray.free_type_array;
			for (int k = 0; k < callerRetTypearray.size(); k++)
			{
				if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
				{
					fprintf(stderr, "\033[40;31m  find free stmt free same pointer \033[0m\n");
					// fprintf(stderr, "find free stmt free same pointer\n");
					debug_gimple_stmt((callerRetTypearray)[k].stmt);
					warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
					++(*freecount);

					// debug_tree((callerRetTypearray)[k].free_tree);
				}
				// debug_tree((callerRetTypearray)[k].free_tree);
				// debug_tree (mallocStmt_tree);
			}
		}

	if (function_pthread_detched_collect->get(function_tree) != NULL)
	{
		function_pthread_detched_array callerFunArray = *(function_pthread_detched_collect->get(function_tree));
		vector<pthread_detched_type> callerRetTypearray = callerFunArray.pthread_detched_array;
		for (int k = 0; k < callerRetTypearray.size(); k++)
		{
			// if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
			// {
			fprintf(stderr, "\033[40;31m  find pthread_detched stmt  \033[0m\n");
			// fprintf(stderr, "find free stmt free same pointer\n");
			debug_gimple_stmt((callerRetTypearray)[k].stmt);
			warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
			++(*freecount);
			// 	++(*freecount);

			// 	// debug_tree((callerRetTypearray)[k].free_tree);
			// }
			// debug_tree((callerRetTypearray)[k].free_tree);
			// debug_tree (mallocStmt_tree);
		}
	}
	if (tracerelatestmt == true)
	{
		struct cgraph_node *node;
		if (mallocStmt_tree != NULL)
			dump_fucntion2(node, function_tree, mallocStmt_tree);
	}
	fucntion_level += 1;

	// checkPointerConstraint(function_tree,ptable,user_tmp);

	fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);

	// pathStack.push(function_tree);
	for (int i = 0; i < function_path_array.size(); i++)
	{

		int find = 0;
		fprintf(stderr, "\033[40;42m =======pre add _ fucntion:%s========= \033[0m\n", get_name((function_path_array)[i].next));

		for (int o = 0; o < traceStack.size(); o++)
		{
			if (traceStack.c[o] == (function_path_array)[i].next)
			{
				find = 1;
				fprintf(stderr, "\033[40;41m =======recursive_fun:%s========= \033[0m\n", get_name(traceStack.c[o]));
				//	fprintf(stderr, "				=======recursive_fun:%s=========\n", get_name(traceStack.c[o]));
			}
		}

		if (find == 0)
		{
			fprintf(stderr, "\033[40;46m =======add node_fun stack:%s========= \033[0m\n", get_name((function_path_array)[i].next));

			//fprintf(stderr, "=======add node_fun trace stack:%s=========\n", get_name((function_path_array)[i].next));
			// debug_tree((function_path_array)[i].next);
			traceStack.push((function_path_array)[i].next);
			trace_function_path((function_path_array)[i].next, fucntion_level, mallocStmt_tree, freecount);
			traceStack.pop();
		}
	}

	// fprintf(stderr, "\033[40;31m  find free stmt free same pointer count:%d \033[0m\n", freecount);
	// return freecount;
}

void tracefree_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp)
{

	cgraph_edge *e;
	FOR_EACH_DEFINED_FUNCTION(node)
	{

		int fucntion_level = 0;

		push_cfun(node->get_fun());

		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

		if (cfun == NULL)
			continue;
		enum availability avail;

		fprintf(stderr, "fucntion collect path \n");
		pathStack.push(cfun->decl);

		walk_function_path(cfun->decl, fucntion_level, ptable, user_tmp);

		pathStack.pop();
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}

void walk_function_path(tree function_tree, int fucntion_level, ptb *ptable, gimple_array *user_tmp)
{
	if (function_path_collect->get(function_tree) == NULL)
		return;
	function_path_array fun_array = *(function_path_collect->get(function_tree));

	vector<function_path> function_path_array = fun_array.function_path_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array callerFunArray = *(function_return_collect->get(function_tree));
	vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
	fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	fprintf(stderr, "\033[40;44m =======print_function_type %d  ========  \033[0m\n", callerFunArray.return_type_num);
	// fprintf(stderr, "rrrrrr%d-------\n", callerFunArray.pthread_type_num );
	if (callerFunArray.pthread_type_num == FUNCITON_THREAD)
		fprintf(stderr, "\033[40;44m =======print_pthread_type_is thread_fucntion  ========  \033[0m\n");

	fucntion_level += 1;

	int find = 0;
	int find_thread = 0;
	for (int i = 0; i < function_path_array.size(); i++)
	{
		find = 0;
		fprintf(stderr, "\033[40;42m =======pre add _ fucntion:%s========= \033[0m\n", get_name((function_path_array)[i].next));

		for (int o = 0; o < pathStack.size(); o++)
		{

			if (pathStack.c[o] == (function_path_array)[i].next)
			{
				find = 1;

				fprintf(stderr, "\033[40;41m =======recursive_fun:%s========= \033[0m\n", get_name(pathStack.c[o]));

				// pathStack.push((function_path_array)[i].next);
				break;
			}
		}
		// if(find ==1)
		// continue;

		if (find == 0)
		{
			fprintf(stderr, "\033[40;46m =======add node_fun stack:%s========= \033[0m\n", get_name((function_path_array)[i].next));

			pathStack.push((function_path_array)[i].next);
			// int find_type=0;
			if (function_return_collect->get((function_path_array)[i].next) != NULL)
			{
				function_return_array calleeFunArray = *(function_return_collect->get((function_path_array)[i].next));

				fprintf(stderr, "\033[40;44m =======print_function_type %d  ========  \033[0m\n", calleeFunArray.return_type_num);

				//thread lock set
				function_return_array find_fun_array;
				vector<return_type> ret_type_find_fun_array;
				tree getFucntionDecl = ((function_path_array)[i].next);

				if (function_return_collect->get(getFucntionDecl) == NULL)
				{
					find_fun_array.return_type_array = ret_type_find_fun_array;
				}
				else
				{
					find_fun_array = *(function_return_collect->get(getFucntionDecl));
				}
				vector<return_type> funcalleeRetTypearray = find_fun_array.return_type_array;

				for (int k = 0; k < callerRetTypearray.size(); k++)
				{
					if ((callerRetTypearray)[k].return_type_stmt_num == FUNCITON_THREAD)
					{
						int find_diff = 0;
						for (int k2 = 0; k2 < funcalleeRetTypearray.size(); k2++)
						{
							if ((callerRetTypearray)[k].return_tree == (funcalleeRetTypearray)[k2].return_tree)
								find_diff = 1;
						}
						if (find_diff == 0)
						{
							// fprintf(stderr, "\033[40;44m ======= print pthread lock stmt %d ========  \033[0m\n" ,(callerRetTypearray)[k].return_type_stmt_num);
							// debug_gimple_stmt((callerRetTypearray)[k].stmt);
							// debug_tree((callerRetTypearray)[k].return_tree);
							struct return_type ret_type;
							ret_type.stmt = (callerRetTypearray)[k].stmt;
							ret_type.return_tree = (callerRetTypearray)[k].return_tree;
							ret_type.return_type_stmt_num = FUNCITON_THREAD;

							find_fun_array.return_type_array.push_back(ret_type);
							function_return_collect->put(getFucntionDecl, find_fun_array);
							find_thread = FUNCITON_THREAD;

							//fprintf(stderr, "\033[40;44m ============================================  \033[0m\n" ,(callerRetTypearray)[k].return_type_stmt_num);
						}
					}
				}

				// fprintf(stderr, "=======print_function_type %d  ========\n", calleeFunArray.return_type_num);

				for (int k = 0; k < callerRetTypearray.size(); k++)
				{
					if (calleeFunArray.return_type_num == 2)
						if ((callerRetTypearray)[k].return_tree == (function_path_array)[i].next)
						{
							checkPointerConstraint(function_tree, ptable, user_tmp, (callerRetTypearray)[k].return_tree, 0);
							break;
						}
				}
			}
			walk_function_path((function_path_array)[i].next, fucntion_level, ptable, user_tmp);
			fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.top()));
			pathStack.pop();
		}
	}
	if (find_thread == FUNCITON_THREAD)
	{
		checkPointerConstraint(function_tree, ptable, user_tmp, NULL, FUNCITON_THREAD);
	}
	// if (check == 0)
	//
	checkPointerConstraint(function_tree, ptable, user_tmp, NULL, 0);
	// fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
}

void dump_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp)
{

	cgraph_edge *e;
	if (node == NULL)

		fprintf(stderr, "=======node_fun: =========\n");

	FOR_EACH_DEFINED_FUNCTION(node)
	{
		//   if (!gimple_has_body_p (node->decl))
		//       continue;
		int fucntion_level = 0;

		push_cfun(node->get_fun());

		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

		if (cfun == NULL)
			continue;
		//mutlple entry point
		if (!strcmp(get_name(cfun->decl), "main"))
		{
			fprintf(stderr, "\033[40;44m =======node_fun:%s========= \033[0m\n", get_name(cfun->decl));
			// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

			fprintf(stderr, "\033[40;44m fucntion collect path  \033[0m\n");
			// fprintf(stderr, "fucntion collect path \n");
			pathStack.push(cfun->decl);

			walk_function_path(cfun->decl, fucntion_level, ptable, user_tmp);
			fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.top()));
			pathStack.pop();
		}
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}
