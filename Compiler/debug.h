void printfBasicblock()
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	basic_block bb;
	//
	struct cgraph_node *node;
	const char *name;

	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// debug_tree(cfun->decl);
		if (cfun == NULL)
			continue;
		name = get_name(cfun->decl);
		if (name != NULL)
		{

			fprintf(stderr, "=======Mapping node_fun:%s=========\n", get_name(cfun->decl));
			// debug_tree(cfun->decl);
			// debug_tree(cfun->decl);
		}
		calculate_dominance_info(CDI_DOMINATORS);
		FOR_EACH_BB_FN(bb, cfun)
		{
			debug_bb(bb);

			fprintf(stderr, "=======is loop:%d=========\n", bb_in_loop_p(bb));
		}
		pop_cfun();
	}
}

void print_function_path(vector<return_type> *path)
{
	fprintf(stderr, "[\n");
	for (int i = 0; i < (*path).size(); i++)
	{
		debug((*path)[i].stmt);
		fprintf(stderr, ",\n");
	}
	fprintf(stderr, "]\n");
	//fprintf(stderr, "	function ->%s in loc %d \n", IDENTIFIER_POINTER(DECL_NAME((*path)[i].first.fndecl)), LOCATION_LINE((*path)[i].second));
}

void print_function_return2(tree function_tree)
{
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_return %d   %d========\n", function_tree, ret_type_array.size());
	fprintf(stderr, "[\n");
	for (int i = 0; i < ret_type_array.size(); i++)
	{

		// for (int i = 0; i < (ret_type_array).size(); i++)
		// {
		debug((ret_type_array)[i].stmt);
		fprintf(stderr, ",\n");
		// }
		// fprintf(stderr, "]\n");
		// print_function_path(&ret_type_array);
	}
	fprintf(stderr, "]\n");
}

void print_function_return(tree function_tree)
{
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_return %d   %d========\n", function_tree, ret_type_array.size());
	fprintf(stderr, "[\n");
	for (int i = 0; i < ret_type_array.size(); i++)
	{

		// for (int i = 0; i < (ret_type_array).size(); i++)
		// {
		debug((ret_type_array)[i].stmt);
		fprintf(stderr, ",\n");
		// }
		// fprintf(stderr, "]\n");
		// print_function_path(&ret_type_array);
	}
	fprintf(stderr, "]\n");
}

void printfunctionCollect2(ptb *ptable, gimple_array *user_tmp)
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	basic_block bb;
	//
	struct cgraph_node *node;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		// debug_tree(cfun->decl);
		if (cfun == NULL)
			continue;
		calculate_dominance_info(CDI_DOMINATORS);
		print_function_return2(node->get_fun()->decl);
		pop_cfun();
	}
}
void printfunctionCollect(ptb *ptable, gimple_array *user_tmp)
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	basic_block bb;
	//
	struct cgraph_node *node;
	const char *name;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		// debug_tree(cfun->decl);
		if (cfun == NULL)
			continue;
		name = get_name(cfun->decl);
		if (name != NULL)
		{
			fprintf(stderr, "=======Mapping node_fun:%s=========\n", get_name(cfun->decl));
			// debug_tree(cfun->decl);
			// debug_tree(cfun->decl);
		}
		calculate_dominance_info(CDI_DOMINATORS);
		print_function_return(node->get_fun()->decl);
		pop_cfun();
	}
}

void printfPointerConstraint(ptb *ptable, gimple_array *user_tmp)
{
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	//

	FOR_EACH_TABLE(table_temp, t)
	{
		fprintf(stderr, "------------------- stmt -----------------------\n");
		// debug_tree(table_temp->target);
		if (table_temp->target != NULL)
		{
			// debug(table_temp->last_stmt);

			location_t loc = gimple_location(table_temp->last_stmt);
			fprintf(stderr, "------------------------------------------\n");
			warning_at(loc, 0, "loc");

			debug_tree(table_temp->target);
			fprintf(stderr, "------------------------------------------\n");
			user_tmp = treeGimpleArray->get(table_temp->target);
			start.stmt = NULL;
			used_stmt = &start;
			if (user_tmp != NULL)
			{
				fprintf(stderr, " \n Pointer to set  is size %d :[ \n", user_tmp->size);
				if (user_tmp->size > 0)
					FOR_EACH_USE_TABLE(user_tmp, u_stmt)
					{
						fprintf(stderr, "------------------------------------------\n");
						if (user_tmp->ret_stmt != NULL)
						{

							// debug(user_tmp->ret_stmt);

							location_t loc = gimple_location(user_tmp->ret_stmt);
							warning_at(loc, 0, "loc");
						}
						else
						{
							// debug(u_stmt);
							location_t loc = gimple_location(u_stmt);
							warning_at(loc, 0, "loc");
						}
						// user_tmp->
					}
			}
			else
			{
				fprintf(stderr, " \n Pointer to set  is size 0 :[ \n");
			}
			fprintf(stderr, "] \n");
		}
	}
}

void printfPointerConstraint2(ptb *ftable, gimple_array *user_tmp)
{
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	//

	FOR_EACH_TABLE(table_temp, t)
	{
		fprintf(stderr, "\n ------------------------------------------\n");
		// debug_tree(table_temp->target);
		if (table_temp->target != NULL)
		{
			debug_tree(table_temp->target);
			user_tmp = treeGimpleArray->get(table_temp->target);
			start.stmt = NULL;
			used_stmt = &start;
			if (user_tmp != NULL)
			{
				fprintf(stderr, " \n Pointer to set  is size %d :[ \n", user_tmp->size);
				if (user_tmp->size > 0)
					FOR_EACH_USE_TABLE(user_tmp, u_stmt)
					{

						if (user_tmp->ret_stmt != NULL)
						{

							debug(user_tmp->ret_stmt);
						}
						else
						{
							debug(u_stmt);
							// if (gimple_code(u_stmt) == GIMPLE_RETURN)
							// {

							// }
						}
						// user_tmp->
					}
				fprintf(stderr, "] \n");
			}
			else
			{
				fprintf(stderr, " \n Pointer to set  is size 0 :[ \n");
				fprintf(stderr, "] \n");
			}
		}
	}
}

void print_function_path(tree function_tree, int fucntion_level, ptb *ptable, gimple_array *user_tmp)
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
	if(callerFunArray.pthread_type_num ==FUNCITON_THREAD)
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
			print_function_path((function_path_array)[i].next, fucntion_level, ptable, user_tmp);
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

			print_function_path(cfun->decl, fucntion_level, ptable, user_tmp);
			fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.top()));
			pathStack.pop();
		}
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}
