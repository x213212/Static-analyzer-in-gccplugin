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

		print_function_path(cfun->decl, fucntion_level, ptable, user_tmp);

		pathStack.pop();
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}