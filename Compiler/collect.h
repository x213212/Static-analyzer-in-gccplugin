
void collect_function_call(gimple *gc, cgraph_node *node, basic_block bb)
{

	location_t loc = gimple_location(gc);
	tree a;
	cgraph_node *node2;
	const char *name;

	if (gimple_call_fn(gc) == NULL)
		return;

	name = get_name(gimple_call_fn(gc));
	if (name != NULL)
		if (!strcmp(name, "free") || !strcmp(name, "xfree"))
		{
			// addr_expr free (&a)
			if (!strcmp(get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0))), "addr_expr"))
			{
				// debug_gimple_stmt(gc);
				// debug_tree(gimple_call_arg(gc, 0));
				// fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
			}
			else
			{
				// debug_tree(gimple_call_arg(gc, 0));
				set_ptb(bb, ftable, gimple_call_arg(gc, 0), loc, 0, gc, node);
			}
			// tree getreturnFucntionDecl = TREE_OPERAND(get_function_return_tree, 0);
			// tree getFucntionDecl = (node->get_fun()->decl);

			// vector<free_type> free_type_array;
			// function_free_array fun_array;
			// if (function_free_collect->get(getFucntionDecl) == NULL)
			// {
			// 	// fprintf(stderr, "%s\n",get_name (getFucntionDecl));
			// 	fun_array.free_type_array = free_type_array;
			// }
			// else
			// {
			// 	fun_array = *(function_free_collect->get(getFucntionDecl));
			// 	free_type_array = fun_array.free_type_array;
			// }

			// struct free_type free_type;
			// free_type.stmt = gc;
			// free_type.free_tree = gimple_call_arg(gc, 0);
			// // ret_type.return_tree = get_function_return_tree;

			// fun_array.free_type_array.push_back(free_type);
			// function_free_collect->put(node->get_fun()->decl, fun_array);
		}

		else if (!strcmp(name, "realloc") || !strcmp(name, "malloc") || !strcmp(get_name(gimple_call_fn(gc)), "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
		{
			// debug_tree(gimple_call_lhs(gc));
			set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
		}

	// warning_at(gimple_location(gc), 0, "use location");

	// else if (!strcmp(name, "pthread_mutex_unlock"))
	// {
	// 	fprintf(stderr, "================================================\n");
	// 	debug(gc);
	// 	debug_tree(gimple_call_arg(gc, 0));
	// 	fprintf(stderr, "================================================\n");
	// 	tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
	// 	debug_tree(second);
	// 	set_ptb(bb, unlocktable, second, loc, 0, gc, node);
	// }
}

void collect_FunctionMapping_Assign(gimple *gc, cgraph_node *node, basic_block bb)
{
	location_t loc = gimple_location(gc);
	tree a;
	cgraph_node *node2;
	const char *name;
	//https://doc.ecoscentric.com/gnutools/doc/gccint/Expression-trees.html#Expression-trees
	//https://doc.ecoscentric.com/gnutools/doc/gccint/Storage-References.html
	if (gimple_code(gc) == GIMPLE_CALL)
	{
		// if (gimple_call_fn(gc) == NULL)
		// 	return;

		// fprintf(stderr, "GIMPLE_CALL\n");
		name = get_name(gimple_call_fn(gc));

		// fprintf(stderr, "--------------------collect_FunctionMapping------------------\n");
		// fprintf(stderr, "%s\n", name);

		// debug(gc);
		tree get_function_return_tree = gimple_call_fn(gc);
		// debug_tree(get_function_return_tree);
		if (TREE_CODE(get_function_return_tree) == SSA_NAME)
			return;

		if (name != NULL)
			if (!strcmp(name, "free") || !strcmp(name, "xfree"))
			{

				//fprintf(stderr , "%s",get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0))));
				// fprintf(stderr, "--------------------qweqweqwe------------------\n");
				name = get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0)));

				// fprintf(stderr, "--------------------collect_FunctionMapping------------------\n");
				// debug_tree(get_function_return_tree);
				// debug_tree(gimple_call_arg(gc, 0));
				// fprintf(stderr, "--------------------wmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm------------------\n");
				// if (function_return_collect->get(node->get_fun()->decl) == NULL)
				// {

				// 	fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
				// 	return;
				// }

				if (name != NULL)
					if (!strcmp(name, "addr_expr"))
					{
						// fprintf(stderr, "白癡展開-------\n");
						tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
						{
							// if (TREE_CODE(second) == VAR_DECL)
							// {
							// 	debug_tree(second);
							// }
							// debug_tree(gimple_assign_lhs(gc));

							function_assign_array assign_array;
							vector<assign_type> assign_type_array;

							if (function_assign_collect->get(second) == NULL)
							{
								// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
								assign_array.assign_type_array = assign_type_array;
								// debug(gc);
							}
							else
							{
								// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
								// debug(gc);
								assign_array = *(function_assign_collect->get(second));
								assign_type_array = assign_array.assign_type_array;
							}

							struct assign_type assign_type;

							assign_type.stmt = gc;
							assign_type.assign_tree = gimple_call_arg(gc, 0);
							// ret_type.reutnr_type_num = 0;
							assign_array.assign_type_array.push_back(assign_type);
							function_assign_collect->put(second, assign_array);
						}
					}
					else if (TREE_CODE(gimple_call_arg(gc, 0)) == SSA_NAME)
					{
						//free(ptr_ssaname)
						// fprintf(stderr, "free stmt arg isssa _name-------\n");
						// debug_tree(gimple_call_arg(gc, 0));

						tree getFucntionDecl = (node->get_fun()->decl);

						vector<free_type> free_type_array;
						function_free_array fun_array;
						if (function_free_collect->get(getFucntionDecl) == NULL)
						{
							// fprintf(stderr, "%s\n",get_name (getFucntionDecl));
							fun_array.free_type_array = free_type_array;
						}
						else
						{
							fun_array = *(function_free_collect->get(getFucntionDecl));
							free_type_array = fun_array.free_type_array;
						}

						struct free_type free_type;
						free_type.stmt = gc;
						free_type.free_tree = gimple_call_arg(gc, 0);

						fun_array.free_type_array.push_back(free_type);
						function_free_collect->put(getFucntionDecl, fun_array);
					}
				// tree getFucntionDecl = (node->get_fun()->decl);
				// debug_tree(getFucntionDecl);
				// vector<return_type> ret_type_array;
				// function_return_array fun_array;
				// if (function_return_collect->get(getFucntionDecl) == NULL)
				// {
				// 	fun_array.return_type_array = ret_type_array;
				// }
				// else
				// {
				// 	fun_array = *(function_return_collect->get(getFucntionDecl));
				// 	ret_type_array = fun_array.return_type_array;
				// }
				// tree function_return_lhstree = gimple_call_arg(gc, 0);
				// struct return_type ret_type;
				// ret_type.stmt = gc;
				// ret_type.return_tree = function_return_lhstree;
				// ret_type.reutnr_type_num = 0;
				// fun_array.return_type_array.push_back(ret_type);
				// function_return_collect->put(node->get_fun()->decl, fun_array);
			}
			else if (!strcmp(name, "pthread_mutex_lock") || !strcmp(name, "pthread_mutex_unlock"))
			{
				// fprintf(stderr, "======================pthread_mutex_lock unlock==========================\n");
				// debug(gc);

				// debug_tree(gimple_call_arg(gc, 0));
				// fprintf(stderr, "================================================\n");
				// // tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
				// // debug_tree(second);
				// // set_ptb(bb, locktable, second, loc, 0, gc, node);
				function_return_array fun_array;
				vector<return_type> ret_type_array;
				tree getFucntionDecl = (node->get_fun()->decl);
				if (function_return_collect->get(getFucntionDecl) == NULL)
				{
					fun_array.return_type_array = ret_type_array;
				}
				else
				{
					fun_array = *(function_return_collect->get(getFucntionDecl));
					// ret_type_array = fun_array.return_type_array;
				}
				// fprintf(stderr, "======================pthread_mutex_lock unlock==========================\n");

				struct return_type ret_type;
				ret_type.stmt = gc;
				ret_type.return_tree = gimple_call_arg(gc, 0);
				ret_type.return_type_stmt_num = FUNCITON_THREAD;

				fun_array.return_type_array.push_back(ret_type);
				function_return_collect->put(node->get_fun()->decl, fun_array);
			}
			else
			{

				tree getreturnFucntionDecl = TREE_OPERAND(get_function_return_tree, 0);
				if (function_return_collect->get(getreturnFucntionDecl) == NULL)
				{
					// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					return;
				}
				tree getFucntionDecl = (node->get_fun()->decl);
				// debug_tree(getFucntionDecl);
				vector<return_type> ret_type_array;
				function_return_array fun_array;

				if (function_return_collect->get(getFucntionDecl) == NULL)
				{
					fun_array.return_type_array = ret_type_array;
				}
				else
				{
					fun_array = *(function_return_collect->get(getFucntionDecl));
					ret_type_array = fun_array.return_type_array;
				}

				struct return_type ret_type;
				ret_type.stmt = gc;
				ret_type.return_tree = getreturnFucntionDecl;

				fun_array.return_type_array.push_back(ret_type);
				function_return_collect->put(node->get_fun()->decl, fun_array);

				// debug(gc);
				if (gimple_call_num_args(gc) != 0)
				{
					name = get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0)));
					if (name != NULL)
						if (!strcmp(name, "addr_expr"))
						{
							function_assign_array assign_array;
							vector<assign_type> assign_type_array;

							if (function_assign_collect->get(gimple_call_fn(gc)) == NULL)
							{
								// fprintf(stderr, "other fucntion call addexpr-------\n");
								// debug(gc);
								// debug_tree(gimple_call_fn(gc));
								// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
								assign_array.assign_type_array = assign_type_array;
								// debug(gc);
							}
							else
							{
								// fprintf(stderr, "other fucntion call addexpr-------\n");
								// debug(gc);
								// debug_tree(gimple_call_fn(gc));
								assign_array = *(function_assign_collect->get(gimple_call_fn(gc)));
								assign_type_array = assign_array.assign_type_array;
							}

							struct assign_type assign_type;

							assign_type.stmt = gc;
							assign_type.assign_tree = gimple_call_arg(gc, 0);
							// ret_type.reutnr_type_num = 0;
							assign_array.assign_type_array.push_back(assign_type);
							tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
							// debug_tree(second);
							if (TREE_CODE(second) == VAR_DECL)
							{
								// debug_tree(second);
								// fprintf(stderr, "other fucntion call addexpr2-------\n");
								function_assign_collect->put(second, assign_array);
							}
						}
				}
				// fprintf(stderr, "------------w--------wwwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwww------------------\n");
				// 分歧點
				// foo()
				// p1 = foo2()
				//fix  function != null
				// fprintf(stderr, "%d\n", gimple_call_num_args(gc));
				// if (gimple_call_num_args(gc) != 0)
				// 	debug_tree(gimple_call_arg(gc, 0));
				// gimple_call_num_args
				// if(call_tree != NULL)
				// debug_tree(gimple_call_arg(gc, 0));
				// fprintf(stderr, "--------------------wwwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwww------------------\n");

				// debug_tree(gimple_call_lhs(gc));
				// fprintf(stderr, "--------------------wwwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwww------------------\n");

				// debug_tree(gimple_call_fn(gc));
				// fprintf(stderr, "--------------------wwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwwww------------------\n");
				tree checklhstree = gimple_call_lhs(gc);
				// if lhs tree is null this is fucntion call
				if (checklhstree != NULL)
				{
					if (TREE_CODE(gimple_call_lhs(gc)) == SSA_NAME)
						set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
				}
				else if (gimple_code(gc) == GIMPLE_CALL)
				{
					name = get_name(gimple_call_fn(gc));
					if (name != NULL)
					{
						// fprintf(stderr, "functionname %s\n", name);
						// fprintf(stderr, "functionname %s\n", name);
						// fprintf(stderr, "functionname %s\n", name);
						// fprintf(stderr, "functionname %s\n", name);
						// fprintf(stderr, "functionname %s\n", name);
						// fprintf(stderr, "functionname %s\n", name);
						// fprintf(stderr, "functionname %s\n", name);
						// fprintf(stderr, "functionname %s\n", name);

						// fprintf(stderr, "functionname %s\n", name);
						// fprintf(stderr, "functionname %s\n", name);
						// if (
						// 	!strcmp(name, "malloc") ||
						// 	!strcmp(name, "xmalloc") ||
						// 	!strcmp(name, "calloc") ||
						// 	!strcmp(name, "xcalloc") ||
						// 	!strcmp(name, "strdup"))
						// {

						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
						// debug_tree(gimple_call_num_args(gimple_call_arg(gc, 0)));
						if (gimple_call_num_args(gc) == 0)
							set_ptb(bb, ptable, gimple_call_fn(gc), loc, 0, gc, node);
						else
							set_ptb(bb, ptable, gimple_call_arg(gc, 0), loc, 0, gc, node);
						// }
					}
				}

				// set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
			}
		// debug_tree(function_return_lhstree);
		// fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
	}
	else if (gimple_code(gc) == GIMPLE_ASSIGN)
	{
		// if (TREE_CODE(gimple_assign_lhs(gc)) == VAR_DECL== NULL)
		// fprintf(stderr, "GIMPLE_ASSIGN\n");

		if ((gimple_assign_lhs(gc) == NULL) || (gimple_assign_rhs1(gc)) == NULL)
			return;
		// fprintf(stderr, "GIMPLE ASSIGN\n");
		// fprintf(stderr, "測試fuck %d\n", TREE_STATIC(gimple_assign_lhs(gc)));

		// debug(gc);
		// debug_tree(gimple_assign_lhs(gc));
		// debug_tree(gimple_assign_rhs1(gc));
		// struct assign_type tmp ;
		// tmp.stmt = gc;
		// tmp.assign_tree =gimple_assign_lhs(gc);
		///MEM[(int *)&b] = 1;
		// tree getFunctionAssignSSANAME = gimple_assign_rhs1(gc);
		// tree getFunctionAssignVAR = gimple_assign_lhs(gc);
		tree getFunctionAssignRHS = gimple_assign_rhs1(gc);
		tree getFunctionAssignLHS = gimple_assign_lhs(gc);
		if (TREE_CODE(getFunctionAssignRHS) == SSA_NAME)
		{
			if (TREE_CODE(getFunctionAssignLHS) == VAR_DECL)
			{
				function_assign_array assign_array;
				vector<assign_type> assign_type_array;

				if (function_assign_collect->get(getFunctionAssignLHS) == NULL)
				{
					assign_array.assign_type_array = assign_type_array;
				}
				else
				{
					assign_array = *(function_assign_collect->get(getFunctionAssignLHS));
					assign_type_array = assign_array.assign_type_array;
				}
				tree function_return_lhstree = gimple_call_lhs(gc);
				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);

				function_assign_collect->put(getFunctionAssignLHS, assign_array);
			}
		}

		if (TREE_CODE(getFunctionAssignLHS) == VAR_DECL)
		{
			if (!strcmp(get_tree_code_name(TREE_CODE(getFunctionAssignRHS)), "addr_expr"))
			{
				tree second = TREE_OPERAND(getFunctionAssignRHS, 0);
				if (TREE_CODE(second) == VAR_DECL)
				{
					/*
					執行緒

					pthread_mutex_lock(&mLock);
					int **ppData = malloc(10);
					int *pData = malloc(20);
					int data2 = 0;

					ppData = &pData;
					pData = &data2;
					*ppData[1] = 10;
					**ppData = 10; 
					pthread_mutex_unlock(&mLock);
					pthread_exit(NULL); // 離開子執行緒
					*/
					/*
						//pData = &data2;
					*/
					function_assign_array assign_array;
					vector<assign_type> assign_type_array;

					if (function_assign_collect->get(getFunctionAssignLHS) == NULL)
					{
						assign_array.assign_type_array = assign_type_array;
					}
					else
					{
						assign_array = *(function_assign_collect->get(getFunctionAssignLHS));
						assign_type_array = assign_array.assign_type_array;
					}
					tree function_return_lhstree = gimple_call_lhs(gc);
					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = gimple_assign_lhs(gc);
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);

					function_assign_collect->put(getFunctionAssignLHS, assign_array);
					// fprintf(stderr, "--------------------xxxxxxxxxxxxxxxxxxxxxxx------------------\n");
				}
			}
		}

		if (TREE_CODE(getFunctionAssignLHS) == MEM_REF)
		{

			tree first = TREE_OPERAND(gimple_assign_lhs(gc), 0);
			if (!strcmp(get_tree_code_name(TREE_CODE(first)), "addr_expr"))
			// if (!strcmp( get_tree_code_name(first), "addr_expr"))
			// if (!strcmp(get_tree_code_name(first, "addr_expr")))
			{
				// debug_gimple_stmt(gc);
				// debug_tree(gimple_call_arg(gc, 0));
				// fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
				tree second = TREE_OPERAND(first, 0);
				// debug_tree(second);
				// debug_tree(gimple_assign_lhs(gc));

				function_assign_array assign_array;
				vector<assign_type> assign_type_array;

				if (function_assign_collect->get(second) == NULL)
				{
					// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
					assign_array.assign_type_array = assign_type_array;
					// debug(gc);
				}
				else
				{
					// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
					// debug(gc);
					assign_array = *(function_assign_collect->get(second));
					assign_type_array = assign_array.assign_type_array;
				}

				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);
				function_assign_collect->put(second, assign_array);

				// function_assign_collect->put(second, gc);
			}
		}
		else if (TREE_CODE(getFunctionAssignLHS) == SSA_NAME)
		{
			/*
			執行緒

			pthread_mutex_lock(&mLock);
			int **ppData = malloc(10);
			int *pData = malloc(20);
			int data2 = 0;

			ppData = &pData;
			pData = &data2;
			*ppData[1] = 10;
			**ppData = 10; 
			pthread_mutex_unlock(&mLock);
			pthread_exit(NULL); // 離開子執行緒
			*/
			if (TREE_CODE(getFunctionAssignRHS) == MEM_REF)
			{
				tree first = TREE_OPERAND(getFunctionAssignRHS, 0);
				if (!strcmp(get_tree_code_name(TREE_CODE(first)), "addr_expr"))
				{
					// debug_gimple_stmt(gc);
					// debug_tree(gimple_call_arg(gc, 0));
					// fprintf(stderr, "sssssssssssssssssaaaaaaaaa name addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
					tree second = TREE_OPERAND(first, 0);
					// debug_tree(second);
					function_assign_array assign_array;
					vector<assign_type> assign_type_array;

					if (function_assign_collect->get(second) == NULL)
					{
						// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
						assign_array.assign_type_array = assign_type_array;
						// debug(gc);
					}
					else
					{
						// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
						// debug(gc);
						assign_array = *(function_assign_collect->get(second));
						assign_type_array = assign_array.assign_type_array;
					}

					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = gimple_assign_lhs(gc);
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);
					function_assign_collect->put(second, assign_array);
				}
			}

			else if (TREE_CODE(getFunctionAssignRHS) == VAR_DECL)
			{
				function_assign_array assign_array;
				vector<assign_type> assign_type_array;

				if (function_assign_collect->get(getFunctionAssignRHS) == NULL)
				{
					// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
					assign_array.assign_type_array = assign_type_array;
					// debug(gc);
				}
				else
				{
					// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
					// debug(gc);
					assign_array = *(function_assign_collect->get(getFunctionAssignRHS));
					assign_type_array = assign_array.assign_type_array;
				}
				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);
				function_assign_collect->put(getFunctionAssignRHS, assign_array);
			}
		}
	}
}

void collect_FunctionMapping_Ret(tree function_tree, gimple *u_stmt, gimple_array *user_tmp, ptb *table_temp, ptb *ptable)
{

	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	// fprintf(stderr, "=======print_function_return %d   %d========\n", function_tree, ret_type_array.size());

	for (int i = 0; i < ret_type_array.size(); i++)
	{
		// fprintf(stderr, "[\n");

		// debug((ret_type_array)[i].stmt);

		if (!strcmp(get_tree_code_name(TREE_CODE(table_temp->target)), "addr_expr"))
		{
			tree second = TREE_OPERAND(table_temp->target, 0);
			gimple *def_stmt = SSA_NAME_DEF_STMT(table_temp->target);
			// debug(def_stmt);
			// debug_tree(second);
			table_temp->target = second;
			table_temp->last_stmt = def_stmt;
			// fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
		}
		// else
		// 	debug(table_temp->last_stmt);
		// debug_tree(user_tmp->target);
		// debug(u_stmt);
		gimple *gc = (ret_type_array)[i].stmt;
		// _4=PHI<_>
		// return _4;
		if (gimple_code(u_stmt) == GIMPLE_PHI)
		{

			if ((ret_type_array)[i].return_tree == gimple_phi_result(u_stmt))
			{
				// debug((ret_type_array)[i].stmt);
				// debug(u_stmt);

				// tree fundecl = TREE_OPERAND(gimple_assign_lhs(u_stmt), 0);
				// gimple *def_stmt = SSA_NAME_DEF_STMT(u_stmt);
				// debug(def_stmt);
				set_gimple_array(user_tmp, table_temp->last_stmt, (ret_type_array)[i].return_tree, (ret_type_array)[i].return_tree, gc);
				return;
			}
		}
		else if (gimple_code(table_temp->last_stmt) == GIMPLE_CALL)
		{
			const char *name;
			name = get_name(gimple_call_fn(table_temp->last_stmt));
			if (name != NULL)
				// 		debug(u_stmt);
				// fprintf(stderr, "%s\n",name);
				if (!strcmp(name, "realloc") || !strcmp(name, "malloc") || !strcmp(name, "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
				{

					// debug(table_temp->last_stmt);
					// // debug(u_stmt);
					// debug(gc);

					// gimple *test2 =   test;
					tree getmalloclhs = table_temp->target;
					// debug_tree (getmalloclhs);
					if ((ret_type_array)[i].return_tree != NULL && getmalloclhs != NULL)
					{

						if (gimple_code(gc) == GIMPLE_RETURN)
						{
							// fprintf(stderr, "RETURN with possible malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
							// fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGN CALLLLLLLLLLLLLLLLLLLLLLL\n");
							//比對所有 ptable 有malloc 的stmt 假設有遇到 malloc 則可能為type2
							if (TREE_CODE((ret_type_array)[i].return_tree) == SSA_NAME)
								if (ptr_derefs_may_alias_p(table_temp->target, (ret_type_array)[i].return_tree))
								{
									// fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_successssssssssssssssss\n");
									// return value related malloc stmt
									// debug(test);
									// debug(u_stmt);
									// debug(gc);
									// set_ptb( table_temp->bb, ptable,  table_temp->target,  table_temp->loc, 0,  table_temp->last_stmt,  table_temp->node);
									fun_array.return_type_num = 2;

									function_return_collect->put(function_tree, fun_array);
									// return;
									return;
								}
						}
					}
					else
					{
						// fun_array.return_type_num = 1;
						// function_return_collect->put(function_tree, fun_array);
						// return ;
					}
					// 			if ((ret_type_array)[i].return_tree == NULL || getmalloclhs == NULL ){
					// 			debug_tree((ret_type_array)[i].return_tree );
					// 			debug_tree(getmalloclhs);
					// fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// return;

					// 			}

					// debug(getmalloclhs);
				}
			if (!strcmp(name, "free") || !strcmp(name, "xfree"))
			{
				// fun_array.return_type_num = 3;
				// fprintf(stderr, "RETURN with free ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// function_return_collect->put(function_tree, fun_array);
				// return;
			}
		}
	}
}


void collect_function_return(gimple *gc, cgraph_node *node, basic_block bb)
{

	// fprintf(stderr, "GIMPLE_RETURN\n");
	function_return_array fun_array;
	tree get_function_return_tree = gimple_return_retval(as_a<greturn *>(gc));
	vector<return_type> ret_type_array;

	fun_array.return_type_array = ret_type_array;
	struct return_type ret_type;
	ret_type.stmt = gc;
	ret_type.return_tree = get_function_return_tree;
	// ret_type.reutnr_type_num = 0;
	fun_array.return_type_array.push_back(ret_type);
	function_return_collect->put(node->get_fun()->decl, fun_array);

	//debug_tree(get_function_return_tree);

	//tree test = cfun->decl;
	//var_points_to  get_function_return =*(function_return_collect->get(get_function_return_tree));
}