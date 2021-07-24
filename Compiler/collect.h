void collect_function_call(gimple *gc, cgraph_node *node, basic_block bb)
{

	location_t loc = gimple_location(gc);

	tree a;
	cgraph_node *node2;
	const char *name;
	if (is_gimple_call(gc))
		if (gimple_call_fn(gc) == NULL)
			return;

	// location_t loc;
	// loc = gimple_location(gc);
	// if( LOCATION_LINE(loc) != 561)
	// 	fprintf(stderr, "測試222%d\n\n", LOCATION_LINE(loc));
	// else
	// return ;
	// fprintf(stderr, "測試222%d\n\n", LOCATION_LINE(loc));
	// fprintf(stderr, "測試%s\n\n", LOCATION_FILE(loc));

	if (is_gimple_call(gc))
	{
		name = get_name(gimple_call_fn(gc));
		if (name != NULL)
		{
			if (is_gimple_assign(gc))
				if (gimple_assign_lhs(gc) != NULL)
					if (TREE_CODE(gimple_assign_lhs(gc)) == SSA_NAME)
					{
						fprintf(stderr, "====================POINTER_TYPE============================\n");
						debug(gimple_assign_lhs(gc));
						// debug(gimple_assign_rhs1(gc));
						// debug_tree(gimple_assign_lhs(gc));
						warning_at(gimple_location(gc), 0, "use location");
						fprintf(stderr, "====================POINTER_TYPE============================\n");
					}

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
				debug_tree(gimple_call_lhs(gc));
				set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
				warning_at(gimple_location(gc), 0, "use location");
				// if(TREE_CODE(TREE_TYPE((gimple_call_lhs(gc))))==POINTER_TYPE){
				// 	fprintf(stderr, "====================POINTER_TYPE============================\n");
				// }
			}
			else if (!strcmp(name, "pthread_create"))
			{
				// fprintf(stderr, "====================pthread_create============================\n");
				// gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_call_arg(gc, 0));
				// debug_tree(gimple_call_arg(gc, 0));
				set_ptb(bb, ptable, gimple_call_arg(gc, 0), loc, 0, gc, node);
				// if (gimple_call_arg(gc, 3) != NULL)
				// 	set_ptb(bb, ptable, gimple_call_arg(gc, 3), loc, 0, gc, node);
			}
			else if (!strcmp(name, "pthread_attr_setdetachstate"))
			{
				// fprintf(stderr, "====================pthread_attr_setdetachstate============================\n");

				// debug_tree(gimple_call_arg(gc, 0));
				// debug_tree(gimple_call_arg(gc, 1));
				tree getvardecl = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
				if (TREE_CODE(gimple_call_arg(gc, 1)) == INTEGER_CST)
				{
					// fprintf(stderr, "%d\n",	 int_cst_value (gimple_call_arg(gc, 1)));

					//printf("<units>%d</units>\n", int_cst_value (gimple_call_arg(gc, 1)));
				}
				// fprintf(stderr, "====================pthread_attr_setdetachstate============================\n");
				//   printf("<units>%d</units>\n", TREE_INT_CST_NUNITS(gimple_call_arg(gc, 1)));
				// gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_call_arg(gc, 0));
				// // debug_tree(gimple_call_arg(gc, 0));
				// set_ptb(bb, ptable, gimple_call_arg(gc, 0), loc, 0, gc, node);
				pthread_attr_array pthread_attr_array;
				// vector<attr_type> attr_type_array;

				if (pthread_attr_setdetachstates->get(getvardecl) == NULL)
				{
					// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
					// pthread_attr_array.attr_type_array = attr_type_array;
					// debug(gc);
				}
				else
				{
					// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
					// debug(gc);
					pthread_attr_array = *(pthread_attr_setdetachstates->get(getvardecl));
					// attr_type_array = pthread_attr_array.attr_type_array;
				}
				// struct attr_type attr_type;

				// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
				// debug(gc);

				pthread_attr_array.stmt = gc;
				pthread_attr_array.attr_tree = gimple_call_arg(gc, 0);
				if (TREE_CODE(gimple_call_arg(gc, 1)) == INTEGER_CST)
				{
					pthread_attr_array.attr_type_num = int_cst_value(gimple_call_arg(gc, 1));
				}

				// pthread_attr_array.attr_type_array.push_back(attr_type);
				pthread_attr_setdetachstates->put(getvardecl, pthread_attr_array);
			}
			// else if (!strcmp(name, "pthread_join"))
			// {
			// 	fprintf(stderr, "====================pthread_join============================\n");
			// 	gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_call_arg(gc, 0));
			// 	debug_tree(gimple_call_arg(gc, 0));
			// 	set_ptb(bb, ptable, gimple_call_arg(gc, 0), loc, 0, gc, node);
			// }
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
	}
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
				// 	debug_gimple_stmt(gc);
				// 	debug_tree(gimple_call_arg(gc, 0));
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

						if (TREE_CODE(gimple_call_arg(gc, 0)) == SSA_NAME)
						{
							// fprintf(stderr, "firstfirstfirstfirswwwwwwwwwwggggggggggggggggggggggggggggggg-------\n");
							// debug_tree(second);
							// second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
							function_assign_array assign_array;
							vector<assign_type> assign_type_array;
							gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_call_arg(gc, 0));
							if (def_stmt != NULL)
								if (is_gimple_assign(def_stmt))
								{
									// fprintf(stderr, "firstfqwwwwwwwwwwgggggggggggggggggggggg-------\n");
									// debug_tree(gimple_assign_rhs1(def_stmt));
									// debug_tree(gimple_assign_lhs(gc));
									// fprintf(stderr, "firstfqwwwwwwwwwwgggggggggggggggggggggg-------\n");
									if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
									{
										debug_tree(gimple_assign_rhs1(def_stmt));
										if (function_assign_collect->get(gimple_assign_rhs1(def_stmt)) == NULL)
										{
											// debug_tree(gimple_assign_rhs1(def_stmt));
											// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
											// debug((gc));
											assign_array.assign_type_array = assign_type_array;
											// debug(gc);
										}
										else
										{
											// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
											// debug((gc));
											// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
											// debug(gc);
											assign_array = *(function_assign_collect->get(gimple_assign_rhs1(def_stmt)));
											assign_type_array = assign_array.assign_type_array;
										}

										struct assign_type assign_type;

										assign_type.stmt = gc;
										assign_type.assign_tree = gimple_call_arg(gc, 0);
										assign_type.form_tree = node->get_fun()->decl;
										// debug_tree(gimple_assign_lhs(gc));
										// ret_type.reutnr_type_num = 0;
										assign_array.assign_type_array.push_back(assign_type);
										function_assign_collect->put(gimple_assign_rhs1(def_stmt), assign_array);
									}
								}

							// else if (is_gimple_call(def_stmt))
							// {
							// 	debug_tree(gimple_call_lhs(def_stmt));
							// 	fprintf(stderr, "firstfqwdggggggggg-------\n");
							// }
						}
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
			else if (!strcmp(name, "pthread_create"))
			{
				tree getFunctionAssignRHS = gimple_call_arg(gc, 0);
				getFunctionAssignRHS = TREE_OPERAND(getFunctionAssignRHS, 0);
				function_assign_array assign_array;
				vector<assign_type> assign_type_array;
				// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
				// debug(gc);
				// debug_tree(getFunctionAssignRHS);
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
				assign_type.assign_tree = gimple_call_arg(gc, 0);

				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);
				function_assign_collect->put(getFunctionAssignRHS, assign_array);

				if (gimple_call_arg(gc, 3) != NULL)
				{
					getFunctionAssignRHS = gimple_call_arg(gc, 3);
					getFunctionAssignRHS = TREE_OPERAND(getFunctionAssignRHS, 0);
					// if (getFunctionAssignRHS != NULL)
					// 	debug_tree(getFunctionAssignRHS);
					// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
					// debug(gc);
					// debug_tree(getFunctionAssignRHS);
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
					assign_type.assign_tree = gimple_call_arg(gc, 3);

					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);
					function_assign_collect->put(getFunctionAssignRHS, assign_array);
				}
			}
			else if (!strcmp(name, "pthread_detach"))
			{
				tree getFucntionDecl = (node->get_fun()->decl);

				function_pthread_detched_array fun_array;
				vector<pthread_detched_type> pthread_detched_array;
				if (function_pthread_detched_collect->get(getFucntionDecl) == NULL)
				{
					// fprintf(stderr, "%s\n",get_name (getFucntionDecl));
					fun_array.pthread_detched_array = pthread_detched_array;
				}
				else
				{
					fun_array = *(function_pthread_detched_collect->get(getFucntionDecl));
					pthread_detched_array = fun_array.pthread_detched_array;
				}

				struct pthread_detched_type pthread_detched_type;
				pthread_detched_type.stmt = gc;
				pthread_detched_type.pthread_detched_tree = gimple_call_arg(gc, 0);

				fun_array.pthread_detched_array.push_back(pthread_detched_type);
				function_pthread_detched_collect->put(getFucntionDecl, fun_array);
			}
			else if (!strcmp(name, "pthread_exit"))
			{
				tree getFucntionDecl = (node->get_fun()->decl);

				function_pthread_exit_array fun_array;
				vector<pthread_exit_type> pthread_exit_array;
				if (function_pthread_exit_collect->get(getFucntionDecl) == NULL)
				{
					// fprintf(stderr, "%s\n",get_name (getFucntionDecl));
					fun_array.pthread_exit_array = pthread_exit_array;
				}
				else
				{
					fun_array = *(function_pthread_exit_collect->get(getFucntionDecl));
					pthread_exit_array = fun_array.pthread_exit_array;
				}

				struct pthread_exit_type pthread_exit_type;
				pthread_exit_type.stmt = gc;
				pthread_exit_type.pthread_exit_tree = gimple_call_arg(gc, 0);

				fun_array.pthread_exit_array.push_back(pthread_exit_type);
				function_pthread_exit_collect->put(getFucntionDecl, fun_array);
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
						// debug_gimple_stmt(gc);
						// fprintf(stderr, "functionname %s\n", name);

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
		// if (TREE_CODE(getFunctionAssignRHS) == SSA_NAME)
		// {
		// 	if (TREE_CODE(getFunctionAssignLHS) == VAR_DECL)
		// 	{
		// 		function_assign_array assign_array;
		// 		vector<assign_type> assign_type_array;

		// 		if (function_assign_collect->get(getFunctionAssignLHS) == NULL)
		// 		{
		// 			assign_array.assign_type_array = assign_type_array;
		// 		}
		// 		else
		// 		{
		// 			assign_array = *(function_assign_collect->get(getFunctionAssignLHS));
		// 			assign_type_array = assign_array.assign_type_array;
		// 		}
		// 		tree function_return_lhstree = gimple_call_lhs(gc);
		// 		struct assign_type assign_type;

		// 		assign_type.stmt = gc;
		// 		assign_type.assign_tree = gimple_assign_lhs(gc);
		// 		// ret_type.reutnr_type_num = 0;
		// 		assign_array.assign_type_array.push_back(assign_type);

		// 		function_assign_collect->put(getFunctionAssignLHS, assign_array);
		// 	}
		// }

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
			else if (TREE_CODE(getFunctionAssignRHS) == SSA_NAME)
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

		else if (TREE_CODE(getFunctionAssignLHS) == MEM_REF)
		{

			tree first = TREE_OPERAND(gimple_assign_lhs(gc), 0);
			if (!strcmp(get_tree_code_name(TREE_CODE(first)), "addr_expr"))
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
			else if (TREE_CODE(getFunctionAssignRHS) == INTEGER_CST)
			{
				// debug_tree(gimple_assign_lhs(gc));
				tree second = NULL_TREE;
				second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
				function_assign_array assign_array;
				vector<assign_type> assign_type_array;
				if (TREE_CODE(second) == SSA_NAME)
				{
					// fprintf(stderr, "firstfirstfirstfirswwwwwwwwwwggggggggggggggggggggggggggggggg-------\n");
					// debug_tree(second);
					gimple *def_stmt = SSA_NAME_DEF_STMT(second);
					if (def_stmt != NULL)
						if (is_gimple_assign(def_stmt))
						{
							// fprintf(stderr, "firstfqwwwwwwwwwwgggggggggggggggggggggg-------\n");
							// debug_tree(gimple_assign_rhs1(def_stmt));
							// debug_tree(gimple_assign_lhs(gc));
							// fprintf(stderr, "firstfqwwwwwwwwwwgggggggggggggggggggggg-------\n");
							if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
							{
								if (function_assign_collect->get(gimple_assign_rhs1(def_stmt)) == NULL)
								{
									// debug_tree(gimple_assign_rhs1(def_stmt));
									// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
									// debug((gc));
									assign_array.assign_type_array = assign_type_array;
									// debug(gc);
								}
								else
								{
									// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
									// debug((gc));
									// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
									// debug(gc);
									assign_array = *(function_assign_collect->get(gimple_assign_rhs1(def_stmt)));
									assign_type_array = assign_array.assign_type_array;
								}

								struct assign_type assign_type;

								assign_type.stmt = gc;
								assign_type.assign_tree = gimple_assign_lhs(gc);
								assign_type.form_tree = node->get_fun()->decl;
								// debug_tree(gimple_assign_lhs(gc));
								// ret_type.reutnr_type_num = 0;
								assign_array.assign_type_array.push_back(assign_type);
								function_assign_collect->put(gimple_assign_rhs1(def_stmt), assign_array);
							}
						}

					// else if (is_gimple_call(def_stmt))
					// {
					// 	debug_tree(gimple_call_lhs(def_stmt));
					// 	fprintf(stderr, "firstfqwdggggggggg-------\n");
					// }
				}
				// debug((gc));
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

				// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
				// debug(gc);

				ptb *table1 = ptable;
				tree t2 = NULL_TREE;
				if (ptable->size >= 0)
				{
					FOR_EACH_TABLE(table1, t2)
					{

						if (!strcmp(get_tree_code_name(TREE_CODE(table1->target)), "addr_expr"))
						{
							tree second = TREE_OPERAND(table1->target, 0);
							if (getFunctionAssignRHS == second)
							{
								// debug_tree(getFunctionAssignLHS);
								// debug_tree(table1->target);
								// debug_tree(second);
								// fprintf(stderr, "okokokokokokok-------\n");
								table1->swap_target = table1->target;
								table1->swap_stmt = table1->last_stmt;
								table1->target = getFunctionAssignLHS;
								table1->last_stmt = gc;
								table1->swap_type = FUNCITON_THREAD;
								assign_type.stmt = table1->last_stmt;
								assign_type.assign_tree = table1->target;
								// ret_type.reutnr_type_num = 0;
								// assign_array.assign_type_array.push_back(assign_type);
								// function_assign_collect->put(getFunctionAssignRHS, assign_array);
								// set_ptb(bb, ptable, gimple_call_arg(gc, 0), loc, 0, gc, node);
							}
							// debug_tree(getFunctionAssignRHS);
						}
					}
				}
			}
			else if (TREE_CODE(getFunctionAssignRHS) == ARRAY_REF)
			{
				// fprintf(stderr, "sssssssssssssssssaaaaaaaaa name array_ref--------\n");
				tree second = TREE_OPERAND(getFunctionAssignRHS, 0);
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
				// debug_tree(gimple_assign_lhs(gc));
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);
				function_assign_collect->put(second, assign_array);
				// debug_tree(gimple_assign_rhs1(getFunctionAssignRHS));
			}
			// else if (TREE_CODE(getFunctionAssignRHS) == INTEGER_CST)
			// {
			// 	// debug_tree(gimple_assign_lhs(gc));
			// 	tree second = NULL_TREE;
			// 	second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
			// 	function_assign_array assign_array;
			// 	vector<assign_type> assign_type_array;
			// 	debug((gc));
			// 	if (function_assign_collect->get(second) == NULL)
			// 	{
			// 		fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
			// 		debug((gc));
			// 		assign_array.assign_type_array = assign_type_array;
			// 		// debug(gc);
			// 	}
			// 	else
			// 	{
			// 		fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
			// 		debug((gc));
			// 		// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
			// 		// debug(gc);
			// 		assign_array = *(function_assign_collect->get(second));
			// 		assign_type_array = assign_array.assign_type_array;
			// 	}

			// 	struct assign_type assign_type;

			// 	assign_type.stmt = gc;
			// 	assign_type.assign_tree = gimple_assign_lhs(gc);
			// 	// debug_tree(gimple_assign_lhs(gc));
			// 	// ret_type.reutnr_type_num = 0;
			// 	assign_array.assign_type_array.push_back(assign_type);
			// 	function_assign_collect->put(second, assign_array);
			// }
		}
		// if (TREE_CODE(getFunctionAssignLHS) == MEM_REF)
		// {
		// 	if (TREE_CODE(getFunctionAssignRHS) == INTEGER_CST)
		// 	{
		// 		// debug_tree(gimple_assign_lhs(gc));
		// 		tree second = NULL_TREE;
		// 		second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
		// 		function_assign_array assign_array;
		// 		vector<assign_type> assign_type_array;
		// 		if (TREE_CODE(second) == SSA_NAME)
		// 		{
		// 			fprintf(stderr, "firstfirstfirstfirswwwwwwwwwwggggggggggggggggggggggggggggggg-------\n");
		// 			// debug_tree(second);
		// 			gimple *def_stmt = SSA_NAME_DEF_STMT(second);
		// 			if (def_stmt != NULL)
		// 				if (is_gimple_assign(def_stmt))
		// 				{
		// 					// fprintf(stderr, "firstfqwwwwwwwwwwgggggggggggggggggggggg-------\n");
		// 					// debug_tree(gimple_assign_rhs1(def_stmt));
		// 					// debug_tree(gimple_assign_lhs(gc));
		// 					// fprintf(stderr, "firstfqwwwwwwwwwwgggggggggggggggggggggg-------\n");
		// 					if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
		// 					{
		// 						if (function_assign_collect->get(gimple_assign_rhs1(def_stmt)) == NULL)
		// 						{
		// 							// debug_tree(gimple_assign_rhs1(def_stmt));
		// 							// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
		// 							// debug((gc));
		// 							assign_array.assign_type_array = assign_type_array;
		// 							// debug(gc);
		// 						}
		// 						else
		// 						{
		// 							// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
		// 							// debug((gc));
		// 							// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
		// 							// debug(gc);
		// 							assign_array = *(function_assign_collect->get(gimple_assign_rhs1(def_stmt)));
		// 							assign_type_array = assign_array.assign_type_array;
		// 						}

		// 						struct assign_type assign_type;

		// 						assign_type.stmt = gc;
		// 						assign_type.assign_tree = gimple_assign_lhs(gc);
		// 						assign_type.form_tree = node->get_fun()->decl;
		// 						// debug_tree(gimple_assign_lhs(gc));
		// 						// ret_type.reutnr_type_num = 0;
		// 						assign_array.assign_type_array.push_back(assign_type);
		// 						function_assign_collect->put(gimple_assign_rhs1(def_stmt), assign_array);
		// 					}
		// 				}

		// 			// else if (is_gimple_call(def_stmt))
		// 			// {
		// 			// 	debug_tree(gimple_call_lhs(def_stmt));
		// 			// 	fprintf(stderr, "firstfqwdggggggggg-------\n");
		// 			// }
		// 		}
		// 		// debug((gc));
		// 	}
		// }
	}
}

void collect_FunctionMapping_Ret(tree function_tree, gimple *u_stmt, gimple_array *user_tmp, ptb *table_temp, ptb *ptable)
{

	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	if (fun_array.return_type_num == 2)
		return;
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
			// debug(table_temp->last_stmt);

			if (name != NULL)
				// 		debug(u_stmt);
				// fprintf(stderr, "%s\n",name);
				// if (!strcmp(name, "realloc") || !strcmp(name, "malloc") || !strcmp(name, "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
				// {
				// 	// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
				// 	// warning_at(gimple_location(gc), 0, "use location");
				// 	// debug(table_temp->last_stmt);
				// 	// debug(gc);

				// 	// debug_tree(table_temp->target);
				// 	// debug_tree((ret_type_array)[i].return_tree);
				// 	// gimple *def_stmt2 = SSA_NAME_DEF_STMT((ret_type_array)[i].return_tree);
				// 	// 	debug(def_stmt2);
				// 	// if (gimple_code(gc) == GIMPLE_ASSIGN)
				// 	// {
				// 	// 	debug_tree(gimple_assign_lhs(gc));
				// 	// 	if (gimple_assign_lhs(gc) != NULL)
				// 	// 		if (TREE_CODE(gimple_assign_lhs(gc)) == SSA_NAME)
				// 	// 			(ret_type_array)[i].return_tree = gimple_assign_lhs(gc);
				// 	// }

				// 	// // debug(u_stmt);
				// 	// debug(gc);

				// 	// gimple *test2 =   test;
				// 	tree getmalloclhs = table_temp->target;
				// 	// debug_tree (getmalloclhs);
				// 	if ((ret_type_array)[i].return_tree != NULL && getmalloclhs != NULL)
				// 	{

				// 		// if (gimple_code(gc) == GIMPLE_RETURN)
				// 		// {
				// 		// fprintf(stderr, "RETURN with possible malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 		// debug(table_temp->last_stmt);
				// 		// debug(gc);
				// 		// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
				// 		// warning_at(gimple_location(gc), 0, "use location");
				// 		// fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGN CALLLLLLLLLLLLLLLLLLLLLLL\n");
				// 		//比對所有 ptable 有malloc 的stmt 假設有遇到 malloc 則可能為type2
				// 		if (TREE_CODE((ret_type_array)[i].return_tree) == SSA_NAME)
				// 		{
				// 			fprintf(stderr, "RETURN with possible malloc2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 			debug(table_temp->last_stmt);
				// 			debug(gc);
				// 			if (ptr_derefs_may_alias_p(table_temp->target, (ret_type_array)[i].return_tree))
				// 			{
				// 				fprintf(stderr, "RETURN with possible malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 				// debug(table_temp->last_stmt);
				// 				// debug(gc);
				// 				// debug_tree(table_temp->target);
				// 				// debug_tree((ret_type_array)[i].return_tree);
				// 				// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
				// 				// warning_at(gimple_location(gc), 0, "use location");
				// 				// fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_successssssssssssssssss\n");
				// 				// return value related malloc stmt
				// 				// debug(test);
				// 				// debug(u_stmt);
				// 				// debug(gc);
				// 				// set_ptb( table_temp->bb, ptable,  table_temp->target,  table_temp->loc, 0,  table_temp->last_stmt,  table_temp->node);
				// 				fun_array.return_type_num = 2;

				// 				function_return_collect->put(function_tree, fun_array);
				// 				// return;
				// 				// return;
				// 			}
				// 		}

				// 		// else
				// 		// {
				// 		// 	if (function_return_collect->get((ret_type_array)[i].return_tree) == NULL)
				// 		// 		return;
				// 		// 			debug(gc);
				// 		// 	fprintf(stderr, "RETURN with possible malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 		// 	function_return_array fun_array2 = *(function_return_collect->get(function_tree));

				// 		// 	vector<return_type> ret_type_array2 = fun_array.return_type_array;
				// 		// }
				// 		// }
				// 	}
				// 	// else
				// 	// {

				// 	// 	// fun_array.return_type_num = 1;
				// 	// 	// function_return_collect->put(function_tree, fun_array);
				// 	// 	// return ;
				// 	// }
				// 	// 			if ((ret_type_array)[i].return_tree == NULL || getmalloclhs == NULL ){
				// 	// 			debug_tree((ret_type_array)[i].return_tree );
				// 	// 			debug_tree(getmalloclhs);
				// 	// fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// return;

				// 	// 			}

				// 	// debug(getmalloclhs);
				// }
				// else if (!strcmp(name, "free") || !strcmp(name, "xfree"))
				// {

				// 	// fun_array.return_type_num = 3;
				// 	// fprintf(stderr, "RETURN with free ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 	// function_return_collect->put(function_tree, fun_array);
				// 	// return;
				// }
				if (!strcmp(name, "pthread_create") || !strcmp(name, "pthread_join"))
				{
					if (!strcmp(name, "pthread_create"))
					{
						// fprintf(stderr, "%sqqqqqqqqqqqqqqq\n", name);
						pthread_attr_array pthread_attr_array;
						// vector<attr_type> attr_type_array;
						tree getvardecl = TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 1), 0);
						// debug_tree(getvardecl);
						if (pthread_attr_setdetachstates->get(getvardecl) != NULL)
						{
							// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
							// pthread_attr_array.attr_type_array = attr_type_array;
							// debug(gc);

							// fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
							// debug(gc);
							// if (fun_array.pthread_type_num == -1)
							// {
							// if (function_return_collect->get(function_tree) == NULL)
							// 	return;
							// function_return_array fun_array = *(function_return_collect->get(function_tree));

							// vector<return_type> ret_type_array = fun_array.return_type_array;
							// fprintf(stderr, "asdddddddddddddd%d-------\n", pthread_attr_array.attr_type_num);
							// // debug_tree(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0));
							// if (function_return_collect->get(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0)) == NULL)
							// 	return;
							// fun_array = *(function_return_collect->get(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0)));

							// ret_type_array = fun_array.return_type_array;
							//tree getvardecl = gimple_call_arg(table_temp->last_stmt, 2);
							// debug_tree(getvardecl);
							pthread_attr_array = *(pthread_attr_setdetachstates->get(getvardecl));
							table_temp->swap_type = FUNCITON_THREAD;
							table_temp->pthread_type = pthread_attr_array.attr_type_num;
							// fun_array.pthread_type_num =FUNCITON_THREAD;
							// function_return_collect->put(function_tree, fun_array);
							// fprintf(stderr, "rrrrrr%d-------\n", pthread_attr_array.attr_type_num);
							// }
							// attr_type_array = pthread_attr_array.attr_type_array;
						}
					}
					// fun_array.return_type_num = 3;
					// fprintf(stderr, "RETURN with free ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// function_return_collect->put(function_tree, fun_array);
					// return;
				}
		}
		else if (gimple_code(table_temp->last_stmt) == GIMPLE_ASSIGN)
		{
			const char *name;
			if (table_temp->swap_type == FUNCITON_THREAD)
			{
				name = get_name(gimple_call_fn(table_temp->swap_stmt));
				if (!strcmp(name, "pthread_create") || !strcmp(name, "pthread_join"))
				{
					if (!strcmp(name, "pthread_create"))
					{
						// fprintf(stderr, "%sqqqqqqqqqqq2qqqq\n", name);
						pthread_attr_array pthread_attr_array;
						tree getvardecl = TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 1), 0);
						// vector<attr_type> attr_type_array;
						// debug_tree(getvardecl);
						if (pthread_attr_setdetachstates->get(getvardecl) != NULL)
						{
							// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
							// pthread_attr_array.attr_type_array = attr_type_array;
							// debug(gc);

							// fprintf(stderr, "mappingggggggggggggg222ggggggggggggggggg-------\n");
							// debug(gc);
							// if (fun_array.pthread_type_num == -1)
							// {
							// if (function_return_collect->get(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0)) == NULL)
							// 	return;
							// fun_array = *(function_return_collect->get(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0)));

							// ret_type_array = fun_array.return_type_array;
							pthread_attr_array = *(pthread_attr_setdetachstates->get(getvardecl));
							table_temp->pthread_type = pthread_attr_array.attr_type_num;
							// fprintf(stderr, "rrrrsrr%d-------\n", pthread_attr_array.attr_type_num);
							// fun_array.pthread_type_num =FUNCITON_THREAD;
							// fprintf(stderr, "rrrrrr%d-------\n", pthread_attr_array.attr_type_num);
							// function_return_collect->put(function_tree, fun_array);
							// }
							// attr_type_array = pthread_attr_array.attr_type_array;
						}
						// table_temp->pthread_type_num =
						// debug(table_temp->swap_stmt);
					}
					// fun_array.return_type_num = 3;
					// fprintf(stderr, "RETURN with free ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// function_return_collect->put(function_tree, fun_array);
					// return;
				}
			}
		}
	}
}

void collect_function_return(gimple *gc, cgraph_node *node, basic_block bb)
{

	function_return_array fun_array;
	tree get_function_return_tree = gimple_return_retval(as_a<greturn *>(gc));
	vector<return_type> ret_type_array;

	if (get_function_return_tree)
	{
		fprintf(stderr, "GIMPLE_RETURN\n");
		debug_gimple_stmt(gc);
		debug_tree(get_function_return_tree);
		debug_tree(node->get_fun()->decl);
		// debug_tree(get_function_return_tree);
		fun_array.return_type_array = ret_type_array;
		struct return_type ret_type;
		ret_type.stmt = gc;
		ret_type.return_tree = get_function_return_tree;
		// ret_type.reutnr_type_num = 0;
		fun_array.return_type_array.push_back(ret_type);
		function_return_collect->put(node->get_fun()->decl, fun_array);
	}
	//debug_tree(get_function_return_tree);

	//tree test = cfun->decl;
	//var_points_to  get_function_return =*(function_return_collect->get(get_function_return_tree));
}