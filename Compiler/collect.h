void collect_function_call(gimple *gc, cgraph_node *node, basic_block bb)
{

	location_t loc = gimple_location_safe(gc);

	tree a;
	cgraph_node *node2;
	const char *name;
	if (is_gimple_call(gc))
		if (gimple_call_fn(gc) == NULL)
			return;
	// location_t loc;
	// loc = gimple_location(gc);
	// if( LOCATION_LINE(loc) != 561)
	//		fprintf(stderr, "file line%d\n\n", LOCATION_LINE(loc));
	// else
	// 		return ;
	// fprintf(stderr, "file line\n\n", LOCATION_LINE(loc));

	if (vscode_extensionmod)
	{
		int find = 0;
		if (vscode_extensionIgonefreemod)
		{
			if (is_gimple_call(gc))
			{

				name = get_name(gimple_call_fn(gc));
				if (name != NULL)
				{
					if (!strcmp(name, "free") || !strcmp(name, "xfree"))
					{
						find = 1;
						fprintf(stderr, "set breakpoint free stmt igone\n");
						warning_at(gimple_location_safe(gc), 0, "use location");
					}
				}
			}
		}
		for (int i = 0; i < vbreakpoint.size(); i++)
		{

			if (LOCATION_FILE(loc))
			{
				size_t found = vbreakpoint[i].name.find(LOCATION_FILE(loc));
				if (found)
					if (vbreakpoint[i].line == LOCATION_LINE(loc))
					{
						debug_gimple_stmt(gc);
						fprintf(stderr, "set breakpoint %s %d\n", vbreakpoint[i].name.c_str(), vbreakpoint[i].line);
						find = 1;
					}
			}
		}
		if (find == 0)
			return;
	}
	if (is_gimple_call(gc))
	{
		name = get_name(gimple_call_fn(gc));
		if (name != NULL)
		{

			// if (gimple_call_lhs(gc))
			// {
			// debug(gimple_call_lhs(gc));

			// }
			if (gimple_call_fndecl(gc))
			{
				const char *name2;
				name2 = get_name(gimple_call_fndecl(gc));
				if (strcmp(name2, "realloc") && strcmp(name2, "malloc") && strcmp(get_name(gimple_call_fn(gc)), "calloc") && strcmp(name2, "xcalloc") && strcmp(name2, "xmalloc") && strcmp(name2, "strdup") && strcmp(name2, "xstrdup") && strcmp(name2, "free") && strcmp(name2, "printf"))
				{

					// debug_tree(gimple_call_fndecl(gc)); // GIMPLE_FREE_COUNT++;;
					// if gimple type is addr_expr memory direct all memory
					if ((gimple_call_num_args(gc) != 0))
					{
						for (int i = 0; i < gimple_call_num_args(gc); i++)
						{
							tree second = NULL_TREE;
							if (TREE_CODE(gimple_call_arg(gc, i)) == ADDR_EXPR)
								second = TREE_OPERAND(gimple_call_arg(gc, i), 0);
							else if (TREE_CODE(gimple_call_arg(gc, i)) == SSA_NAME)
							{
								second = gimple_call_arg(gc, i);
							}
							if (second)
							{
								// debug_tree(second);
								set_ptb(bb, ptable, second, loc, 0, gc, node);
							}
						}
					}
					// addr_expr free (&a)
					// ADDR_EXPR
					// if (TREE_CODE(gimple_call_arg(gc, 0)) == ADDR_EXPR)
					// {
					// 	// debug_gimple_stmt(gc);
					// 	// debug_tree(gimple_call_arg(gc, 0));
					fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr------%s--\n", name2);
					// }
					// else
					// {
					// 	// debug_tree(gimple_call_arg(gc, 0));
					// 	set_ptb(bb, ftable, gimple_call_arg(gc, 0), loc, 0, gc, node);
					// }
				}
			}
			if (!strcmp(name, "free") || !strcmp(name, "xfree"))
			{
				GIMPLE_FREE_COUNT++;
				// fprintf(stderr, "===================free===========================\n");
				// debug_gimple_stmt(gc);
				// addr_expr free (&a)
				// ADDR_EXPR
				if (TREE_CODE(gimple_call_arg(gc, 0)) == ADDR_EXPR)
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
			}

			else if (!strcmp(name, "realloc") || !strcmp(name, "malloc") || !strcmp(get_name(gimple_call_fn(gc)), "calloc") || !strcmp(name, "xcalloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup") || !strcmp(name, "xstrdup"))
			{
				GIMPLE_MALLOC_COUNT++;

				// debug_tree(gimple_call_lhs(gc));
				set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
				// warning_at(gimple_location(gc), 0, "use location");
				// if(TREE_CODE(TREE_TYPE((gimple_call_lhs(gc))))==POINTER_TYPE){
				// fprintf(stderr, "====================POINTER_TYPE============================\n");
				// }
			}
			else if (!strcmp(name, "pthread_create"))
			{
				// fprintf(stderr, "====================pthread_create============================\n");
				// gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_call_arg(gc, 0));
				// debug_tree(gimple_call_arg(gc, 0));
				set_ptb(bb, ptable, gimple_call_arg(gc, 0), loc, IS_PTHREAD_FUCNTION, gc, node);
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
					// printf("<units>%d</units>\n", int_cst_value (gimple_call_arg(gc, 1)));
				}
				// fprintf(stderr, "====================pthread_attr_setdetachstate============================\n");
				// printf("<units>%d</units>\n", TREE_INT_CST_NUNITS(gimple_call_arg(gc, 1)));
				// gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_call_arg(gc, 0));
				//  debug_tree(gimple_call_arg(gc, 0));
				// set_ptb(bb, ptable, gimple_call_arg(gc, 0), loc, 0, gc, node);
				pthread_attr_array pthread_attr_array;
				// vector<attr_type> attr_type_array;

				if (pthread_attr_setdetachstates->get(getvardecl) == NULL)
				{

					// pthread_attr_array.attr_type_array = attr_type_array;
					// debug(gc);
				}
				else
				{

					// debug(gc);
					pthread_attr_array = *(pthread_attr_setdetachstates->get(getvardecl));
					// attr_type_array = pthread_attr_array.attr_type_array;
				}

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
			else if (gimple_call_lhs(gc) != NULL && gimple_call_fn(gc) != NULL)
				if (TREE_CODE(gimple_call_lhs(gc)) == SSA_NAME)
				{
					set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
					// fprintf(stderr, "====================POINTER_TYPE============================\n");
					// debug(gimple_call_lhs(gc));
					// debug(gimple_assign_rhs1(gc));
					// debug_tree(gimple_assign_lhs(gc));
					// warning_at(gimple_location(gc), 0, "use location");
					// fprintf(stderr, "====================POINTER_TYPE============================\n");
				}
		}
	}
}

void collect_FunctionMapping_Assign(gimple *gc, cgraph_node *node, basic_block bb, ptb *ptable)
{
	location_t loc = gimple_location_safe(gc);
	tree a;
	cgraph_node *node2;
	const char *name;

	// 	fprintf(stderr, "--------------------collect_FunctionMapping_Assign start------------------\n");
	// https://doc.ecoscentric.com/gnutools/doc/gccint/Expression-trees.html#Expression-trees
	// https://doc.ecoscentric.com/gnutools/doc/gccint/Storage-References.html
	if (gimple_code(gc) == GIMPLE_CALL)
	{

		// fprintf(stderr, "GIMPLE_CALL\n");
		name = get_name(gimple_call_fn(gc));

		tree get_function_return_tree = gimple_call_fn(gc);
		// debug_tree(get_function_return_tree);
		if (TREE_CODE(get_function_return_tree) == SSA_NAME)
			return;

		if (name != NULL)

			if (strcmp(name, "printf"))
				if (!strcmp(name, "free") || !strcmp(name, "xfree") || !strcmp(name, "realloc"))
				{

					if (TREE_CODE(gimple_call_arg(gc, 0)) == ADDR_EXPR)
					{

						tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
						{

							function_assign_array assign_array;
							vector<assign_type> assign_type_array;

							if (function_assign_collect->get(second) == NULL)
							{

								// debug(gc);
								assign_array.assign_type_array = assign_type_array;
							}
							else
							{

								// debug(gc);
								assign_array = *(function_assign_collect->get(second));
								assign_type_array = assign_array.assign_type_array;
							}

							struct assign_type assign_type;

							assign_type.stmt = gc;
							assign_type.assign_tree = gimple_call_arg(gc, 0);
							assign_type.form_tree = node->get_fun()->decl;
							// ret_type.reutnr_type_num = 0;
							assign_array.assign_type_array.push_back(assign_type);
							function_assign_collect->put(second, assign_array);
						}
					}
					else if (TREE_CODE(gimple_call_arg(gc, 0)) == SSA_NAME)
					{
						// free(ptr_ssaname)
						// debug_gimple_stmt(gc);

						//  debug_tree(gimple_call_arg(gc, 0));

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
							// fprintf(stderr, "SSA_NAME\n");
							// debug((gc));
							// debug_tree(second);
							// second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
							function_assign_array assign_array;
							vector<assign_type> assign_type_array;
							gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_call_arg(gc, 0));
							if (def_stmt != NULL)
								if (is_gimple_assign(def_stmt))
								{

									if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
									{
										debug_tree(gimple_assign_rhs1(def_stmt));
										if (function_assign_collect->get(gimple_assign_rhs1(def_stmt)) == NULL)
										{

											// debug(gc);
											assign_array.assign_type_array = assign_type_array;
										}
										else
										{

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
						}
					}
				}
				else if (!strcmp(name, "exit"))
				{
					function_return_array fun_array;
					tree get_function_arg_tree = gimple_call_arg(gc, 0);
					vector<return_type> ret_type_array;

					// fprintf(stderr, "GIMPLE_exit\n");
					if (get_function_return_tree)
					{
						// debug_gimple_stmt(gc);
						// debug_tree(get_function_return_tree);
						// debug_tree(node->get_fun()->decl);
						// debug_tree(get_function_return_tree);
						// fun_array.return_type_array = ret_type_array;
						struct return_type ret_type;
						ret_type.stmt = gc;
						ret_type.return_tree = get_function_return_tree;
						// ret_type.reutnr_type_num = 0;
						global_ret_type_array.push_back(ret_type);
						fun_array.return_type_array.push_back(ret_type);
						function_return_collect->put(node->get_fun()->decl, fun_array);
					}
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

					// debug(gc);
					// debug_tree(getFunctionAssignRHS);
					if (function_assign_collect->get(getFunctionAssignRHS) == NULL)
					{

						// debug(gc);
						assign_array.assign_type_array = assign_type_array;
					}
					else
					{

						// debug(gc);
						assign_array = *(function_assign_collect->get(getFunctionAssignRHS));
						assign_type_array = assign_array.assign_type_array;
					}
					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = gimple_call_arg(gc, 0);
					assign_type.form_tree = node->get_fun()->decl;
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);
					function_assign_collect->put(getFunctionAssignRHS, assign_array);

					if (gimple_call_arg(gc, 3) != NULL)
					{
						getFunctionAssignRHS = gimple_call_arg(gc, 3);
						getFunctionAssignRHS = TREE_OPERAND(getFunctionAssignRHS, 0);
						// if (getFunctionAssignRHS != NULL)
						// 	debug_tree(getFunctionAssignRHS);

						// debug(gc);
						// debug_tree(getFunctionAssignRHS);
						if (function_assign_collect->get(getFunctionAssignRHS) == NULL)
						{

							// debug(gc);
							assign_array.assign_type_array = assign_type_array;
						}
						else
						{

							// debug(gc);
							assign_array = *(function_assign_collect->get(getFunctionAssignRHS));
							assign_type_array = assign_array.assign_type_array;
						}
						struct assign_type assign_type;

						assign_type.stmt = gc;
						assign_type.assign_tree = gimple_call_arg(gc, 3);
						assign_type.form_tree = node->get_fun()->decl;
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

					debug_gimple_stmt(gc);
					for (int i = 0; i < gimple_call_num_args(gc); i++)
					{
						// ADDR_EXPR

						// get vardecl
						tree getVARDECLtree;
						// name = get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0)));
						// if (name != NULL)

						if (TREE_CODE(gimple_call_arg(gc, i)) == ADDR_EXPR || TREE_CODE(gimple_call_arg(gc, i)) == VAR_DECL)
						{
							// debug_tree(gimple_call_arg(gc, i));
							if (TREE_CODE(gimple_call_arg(gc, i)) == ADDR_EXPR)
								getVARDECLtree = TREE_OPERAND(gimple_call_arg(gc, i), 0);
							else
								getVARDECLtree = gimple_call_arg(gc, i);

							// debug_tree(getVARDECLtree);

							function_assign_array assign_array;
							vector<assign_type> assign_type_array;

							if (function_assign_collect->get(getVARDECLtree) == NULL)
							{

								// debug(gc);
								// debug_tree(gimple_call_fn(gc));

								assign_array.assign_type_array = assign_type_array;
								// debug(gc);
							}
							else
							{

								// debug(gc);
								// debug_tree(gimple_call_fn(gc));
								assign_array = *(function_assign_collect->get(getVARDECLtree));
								assign_type_array = assign_array.assign_type_array;
							}
							// fprintf(stderr, "check vardecl-------\n");
							// debug_tree(getVARDECLtree);
							struct assign_type assign_type;

							assign_type.stmt = gc;
							assign_type.assign_tree = gimple_call_arg(gc, i);
							assign_type.form_tree = node->get_fun()->decl;
							// ret_type.reutnr_type_num = 0;
							assign_array.assign_type_array.push_back(assign_type);
							// tree second = TREE_OPERAND(gimple_call_arg(gc, i), 0);
							// debug_tree(second);

							function_assign_collect->put(getVARDECLtree, assign_array);
						}
					}

					tree getVARDECLtree;
					// name = get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0)));
					// if (name != NULL)
					if (gimple_call_num_args(gc) != 0)
						if (TREE_CODE(gimple_call_arg(gc, 0)) == ADDR_EXPR)
						{
							function_assign_array assign_array;
							vector<assign_type> assign_type_array;

							if (function_assign_collect->get(gimple_call_fn(gc)) == NULL)
							{
								// fprintf(stderr, "other fucntion call addexpr-------\n");
								// debug(gc);
								// debug_tree(gimple_call_fn(gc));

								assign_array.assign_type_array = assign_type_array;
							}
							else
							{
								// fprintf(stderr, "other fucntion call addexpr-------\n");

								assign_array = *(function_assign_collect->get(gimple_call_fn(gc)));
								assign_type_array = assign_array.assign_type_array;
							}

							struct assign_type assign_type;

							assign_type.stmt = gc;
							assign_type.assign_tree = gimple_call_arg(gc, 0);
							assign_type.form_tree = node->get_fun()->decl;
							// ret_type.reutnr_type_num = 0;
							assign_array.assign_type_array.push_back(assign_type);
							tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
							// debug_tree(second);
							if (TREE_CODE(second) == VAR_DECL)
							{

								function_assign_collect->put(second, assign_array);
							}
						}

					tree checklhstree = gimple_call_lhs(gc);

					if (gimple_code(gc) == GIMPLE_CALL)
					{
						if (vscode_extensionmod)
						{
							int find = 0;
							for (int i = 0; i < vbreakpoint.size(); i++)
							{
								size_t found = vbreakpoint[i].name.find(LOCATION_FILE(loc));
								if (found)
									if (vbreakpoint[i].line == LOCATION_LINE(loc))
									{
										debug_gimple_stmt(gc);
										fprintf(stderr, "set breakpoint %s %d\n", vbreakpoint[i].name.c_str(), vbreakpoint[i].line);
										find = 1;
									}
							}
							if (find == 0)
								return;
						}
						name = get_name(gimple_call_fn(gc));
						if (name != NULL)
						{

							if (gimple_code(gc) == GIMPLE_CALL)
								if (gimple_call_lhs(gc))
									if (TREE_CODE(gimple_call_lhs(gc)) == SSA_NAME)
										set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);

							if (gimple_call_num_args(gc) != 0)
							{
								for (int i = 0; i < gimple_call_num_args(gc); i++)
								{
									if (TREE_CODE(gimple_call_arg(gc, i)) == SSA_NAME)
										set_ptb(bb, ptable, gimple_call_arg(gc, i), loc, 0, gc, node);
								}
							}
						}
					}

					tree getreturnFucntionDecl = TREE_OPERAND(get_function_return_tree, 0);
					if (getreturnFucntionDecl)
					{
						if (function_return_collect->get(getreturnFucntionDecl) == NULL)
						{

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
					}
				}
	}
	else if (gimple_code(gc) == GIMPLE_ASSIGN)
	{

		// if (TREE_CODE(gimple_assign_lhs(gc)) == VAR_DECL== NULL)
		// fprintf(stderr, "GIMPLE_ASSIGN\n");

		if ((gimple_assign_lhs(gc) == NULL) || (gimple_assign_rhs1(gc)) == NULL)
			return;

		tree getFunctionAssignRHS = gimple_assign_rhs1(gc);
		tree getFunctionAssignLHS = gimple_assign_lhs(gc);

		if (TREE_CODE(getFunctionAssignLHS) == VAR_DECL)
		{

			if (TREE_CODE(getFunctionAssignRHS) == ADDR_EXPR)
			{
				tree second = TREE_OPERAND(getFunctionAssignRHS, 0);
				if (TREE_CODE(second) == VAR_DECL)
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
					assign_type.form_tree = node->get_fun()->decl;
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);

					function_assign_collect->put(getFunctionAssignLHS, assign_array);
				}
			}
			else if (TREE_CODE(getFunctionAssignRHS) == SSA_NAME)
			{

				// debug_gimple_stmt(gc);
				// debug_tree(getFunctionAssignLHS);
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
				tree function_return_lhstree = gimple_assign_rhs1(gc);
				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_rhs1(gc);
				assign_type.form_tree = node->get_fun()->decl;
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);

				function_assign_collect->put(getFunctionAssignLHS, assign_array);
			}
		}
		else if (TREE_CODE(getFunctionAssignRHS) == VAR_DECL)
		{

			if (TREE_CODE(getFunctionAssignLHS) == SSA_NAME)
			{

				// debug_gimple_stmt(gc);
				function_assign_array assign_array;
				vector<assign_type> assign_type_array;

				if (function_assign_collect->get(getFunctionAssignRHS) == NULL)
				{
					assign_array.assign_type_array = assign_type_array;
				}
				else
				{
					assign_array = *(function_assign_collect->get(getFunctionAssignRHS));
					assign_type_array = assign_array.assign_type_array;
				}
				tree function_return_lhstree = gimple_assign_lhs(gc);
				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				assign_type.form_tree = node->get_fun()->decl;
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);

				function_assign_collect->put(getFunctionAssignRHS, assign_array);
			}
		}
		else if (TREE_CODE(getFunctionAssignLHS) == MEM_REF)
		{

			tree first = TREE_OPERAND(gimple_assign_lhs(gc), 0);
			if (TREE_CODE(first) == ADDR_EXPR)
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

					// debug(gc);
					assign_array.assign_type_array = assign_type_array;
				}
				else
				{

					// debug(gc);
					assign_array = *(function_assign_collect->get(second));
					assign_type_array = assign_array.assign_type_array;
				}

				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				assign_type.form_tree = node->get_fun()->decl;
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);
				function_assign_collect->put(second, assign_array);

				// function_assign_collect->put(second, gc);
			}
			if (TREE_CODE(getFunctionAssignRHS) == INTEGER_CST)
			{

				// debug_gimple_stmt(gc);
				// debug_tree(gimple_assign_lhs(gc));
				tree second = NULL_TREE;
				second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
				function_assign_array assign_array;
				vector<assign_type> assign_type_array;
				if (TREE_CODE(second) == SSA_NAME)
				{

					// debug_tree(second);

					gimple *def_stmt = SSA_NAME_DEF_STMT(second);
					if (def_stmt != NULL)
						if (is_gimple_assign(def_stmt))
						{

							// debug_tree(gimple_assign_rhs1(def_stmt));
							// debug_tree(gimple_assign_lhs(gc));

							if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
							{
								if (function_assign_collect->get(gimple_assign_rhs1(def_stmt)) == NULL)
								{
									// debug_tree(gimple_assign_rhs1(def_stmt));

									// debug((gc));
									assign_array.assign_type_array = assign_type_array;
								}
								else
								{

									// debug(gc);
									assign_array = *(function_assign_collect->get(gimple_assign_rhs1(def_stmt)));
									assign_type_array = assign_array.assign_type_array;
								}

								struct assign_type assign_type;

								assign_type.stmt = gc;
								assign_type.assign_tree = gimple_assign_lhs(gc);
								assign_type.form_tree = node->get_fun()->decl;
								// debug_tree(node->get_fun()->decl);
								// debug_gimple_stmt(gc);
								// debug_tree(gimple_assign_lhs(gc));
								// ret_type.reutnr_type_num = 0;
								assign_array.assign_type_array.push_back(assign_type);
								function_assign_collect->put(gimple_assign_rhs1(def_stmt), assign_array);
							}
						}
				}
			}
		}
		else if (TREE_CODE(getFunctionAssignLHS) == SSA_NAME)
		{

			if (TREE_CODE(getFunctionAssignRHS) == MEM_REF)
			{
				tree first = TREE_OPERAND(getFunctionAssignRHS, 0);
				if (TREE_CODE(first) == ADDR_EXPR)
				{
					// debug_gimple_stmt(gc);
					// debug_tree(gimple_call_arg(gc, 0));

					tree second = TREE_OPERAND(first, 0);
					// debug_tree(second);
					function_assign_array assign_array;
					vector<assign_type> assign_type_array;

					if (function_assign_collect->get(second) == NULL)
					{

						// debug(gc);
						assign_array.assign_type_array = assign_type_array;
					}
					else
					{

						// debug(gc);
						assign_array = *(function_assign_collect->get(second));
						assign_type_array = assign_array.assign_type_array;
					}

					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = gimple_assign_lhs(gc);
					assign_type.form_tree = node->get_fun()->decl;
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

					// debug(gc);
					assign_array.assign_type_array = assign_type_array;
				}
				else
				{

					// debug(gc);
					assign_array = *(function_assign_collect->get(getFunctionAssignRHS));
					assign_type_array = assign_array.assign_type_array;
				}
				struct assign_type assign_type;

				// debug(gc);

				ptb *table1 = ptable;
				tree t2 = NULL_TREE;
				if (ptable->size >= 0)
				{
					FOR_EACH_TABLE(table1, t2)
					{

						if (TREE_CODE(table1->target) == ADDR_EXPR)
						{
							tree second = TREE_OPERAND(table1->target, 0);
							if (getFunctionAssignRHS == second)
							{
								// debug_tree(getFunctionAssignLHS);
								// debug_tree(table1->target);
								// debug_tree(second);

								table1->swap_target = table1->target;
								table1->swap_stmt = table1->last_stmt;
								table1->target = getFunctionAssignLHS;
								table1->last_stmt = gc;
								table1->swap_type = FUNCITON_THREAD;
								assign_type.stmt = table1->last_stmt;
								assign_type.assign_tree = table1->target;
								assign_type.form_tree = node->get_fun()->decl;
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

					// debug(gc);
					assign_array.assign_type_array = assign_type_array;
				}
				else
				{

					// debug(gc);
					assign_array = *(function_assign_collect->get(second));
					assign_type_array = assign_array.assign_type_array;
				}

				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				assign_type.form_tree = node->get_fun()->decl;
				// debug_tree(gimple_assign_lhs(gc));
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);
				function_assign_collect->put(second, assign_array);
				// debug_tree(gimple_assign_rhs1(getFunctionAssignRHS));
			}
			else if (TREE_CODE(getFunctionAssignRHS) == COMPONENT_REF)
			{

				// debug_gimple_stmt(gc);
				tree second = TREE_OPERAND(getFunctionAssignRHS, 0);
				tree second2 = TREE_OPERAND(getFunctionAssignRHS, 1);
				if (second)
				{
					if (TREE_CODE(second) == MEM_REF)
					{
						tree three = TREE_OPERAND(second, 0);
						if (three)
						{
							// loc = gimple_location(gc);
							// if (gimple_location(gc))
							// debug_tree(three);
							// if (LOCATION_LINE(loc) == 104 || LOCATION_LINE(loc) == 105)
							// {
							// fprintf(stderr, "============COMPONENT_REF==================\n");
							// debug_tree(gimple_assign_rhs1(gc));
							if (TREE_CODE(three) == SSA_NAME)
							{
								gimple *def_stmt = SSA_NAME_DEF_STMT(three);

								if (is_gimple_assign(def_stmt))

									if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
									{
										function_assign_array assign_array;
										vector<assign_type> assign_type_array;

										if (function_assign_collect->get(gimple_assign_rhs1(def_stmt)) == NULL)
										{

											// debug(gc);
											assign_array.assign_type_array = assign_type_array;
										}
										else
										{

											// debug(gc);
											assign_array = *(function_assign_collect->get(gimple_assign_rhs1(def_stmt)));
											assign_type_array = assign_array.assign_type_array;
										}

										struct assign_type assign_type;

										assign_type.stmt = gc;
										assign_type.assign_tree = gimple_assign_lhs(def_stmt);
										assign_type.form_tree = node->get_fun()->decl;
										// debug_tree(gimple_assign_lhs(gc));
										// ret_type.reutnr_type_num = 0;
										assign_array.assign_type_array.push_back(assign_type);
										function_assign_collect->put(gimple_assign_rhs1(def_stmt), assign_array);
										// debug_tree(gimple_assign_rhs1(def_stmt));
										// debug_gimple_stmt(def_stmt);
									}
							}
						}
					}
					else if (TREE_CODE(second) == VAR_DECL)
					{

						function_assign_array assign_array;
						vector<assign_type> assign_type_array;

						// debug_gimple_stmt(gc);
						if (function_assign_collect->get(second) == NULL)
						{
							// debug(gc);
							assign_array.assign_type_array = assign_type_array;
						}
						else
						{

							// debug(gc);
							assign_array = *(function_assign_collect->get(second));
							assign_type_array = assign_array.assign_type_array;
						}

						struct assign_type assign_type;

						assign_type.stmt = gc;
						assign_type.assign_tree = second;
						assign_type.form_tree = node->get_fun()->decl;
						// debug_tree(gimple_assign_lhs(gc));
						// ret_type.reutnr_type_num = 0;
						assign_array.assign_type_array.push_back(assign_type);
						function_assign_collect->put(second, assign_array);
						// debug_tree(gimple_assign_rhs1(def_stmt));
						// debug_gimple_stmt(def_stmt);
					}
					else if (TREE_CODE(second) == COMPONENT_REF)
					{

						tree three = TREE_OPERAND(getFunctionAssignLHS, 0);
						if (three)
						{
							if (TREE_CODE(three) == MEM_REF)
							{
								tree four = TREE_OPERAND(second, 0);
								if (four)
								{
									if (TREE_CODE(four) == SSA_NAME)
									{
										gimple *def_stmt = SSA_NAME_DEF_STMT(four);

										if (is_gimple_assign(def_stmt))

											if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
											{
												function_assign_array assign_array;
												vector<assign_type> assign_type_array;

												if (function_assign_collect->get(gimple_assign_rhs1(def_stmt)) == NULL)
												{

													// debug(gc);
													assign_array.assign_type_array = assign_type_array;
												}
												else
												{

													// debug(gc);
													assign_array = *(function_assign_collect->get(gimple_assign_rhs1(def_stmt)));
													assign_type_array = assign_array.assign_type_array;
												}

												struct assign_type assign_type;

												assign_type.stmt = gc;
												assign_type.assign_tree = gimple_assign_lhs(def_stmt);
												assign_type.form_tree = node->get_fun()->decl;
												// debug_tree(gimple_assign_lhs(gc));
												// ret_type.reutnr_type_num = 0;
												assign_array.assign_type_array.push_back(assign_type);
												function_assign_collect->put(gimple_assign_rhs1(def_stmt), assign_array);
												// debug_tree(gimple_assign_rhs1(def_stmt));
												// debug_gimple_stmt(def_stmt);
											}
									}
								}
							}
						}
					}
				}
				if (second2)
				{

					//  fprintf(stderr, "mappinggggggggggggFIELD_DECLggggggggggggggggggg-------\n");
					// 	debug_gimple_stmt(gc);
					// 	debug_tree( gimple_assign_lhs(gc));
					if (TREE_CODE(second2) == FIELD_DECL)
					{

						function_assign_array assign_array;
						vector<assign_type> assign_type_array;

						if (function_assign_collect->get(second2) == NULL)
						{

							// debug(gc);
							assign_array.assign_type_array = assign_type_array;
						}
						else
						{

							// debug(gc);
							assign_array = *(function_assign_collect->get(second2));
							assign_type_array = assign_array.assign_type_array;
						}

						struct assign_type assign_type;

						assign_type.stmt = gc;
						assign_type.assign_tree = getFunctionAssignRHS;
						assign_type.form_tree = node->get_fun()->decl;
						// debug_tree(gimple_assign_lhs(gc));
						// ret_type.reutnr_type_num = 0;
						assign_array.assign_type_array.push_back(assign_type);
						function_assign_collect->put(second2, assign_array);
					}
				}
			}
		}
		else if (TREE_CODE(getFunctionAssignLHS) == COMPONENT_REF)
		{

			// debug_gimple_stmt(gc);
			// debug_tree(getFunctionAssignLHS);
			tree second = TREE_OPERAND(getFunctionAssignLHS, 0);
			tree second2 = TREE_OPERAND(getFunctionAssignLHS, 1);
			if (second)
			{
				if (TREE_CODE(second) == MEM_REF)
				{
					tree three = TREE_OPERAND(second, 0);
					if (three)
					{
						// loc = gimple_location(gc);
						// if (gimple_location(gc))
						// debug_tree(three);
						// if (LOCATION_LINE(loc) == 104 || LOCATION_LINE(loc) == 105)
						// {
						// fprintf(stderr, "============COMPONENT_REF==================\n");
						// debug_tree(gimple_assign_rhs1(gc));
						if (TREE_CODE(three) == SSA_NAME)
						{
							gimple *def_stmt = SSA_NAME_DEF_STMT(three);

							if (is_gimple_assign(def_stmt))

								if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
								{
									function_assign_array assign_array;
									vector<assign_type> assign_type_array;

									if (function_assign_collect->get(gimple_assign_rhs1(def_stmt)) == NULL)
									{

										// debug(gc);
										assign_array.assign_type_array = assign_type_array;
									}
									else
									{

										// debug(gc);
										assign_array = *(function_assign_collect->get(gimple_assign_rhs1(def_stmt)));
										assign_type_array = assign_array.assign_type_array;
									}

									struct assign_type assign_type;

									assign_type.stmt = gc;
									assign_type.assign_tree = gimple_assign_lhs(def_stmt);
									assign_type.form_tree = node->get_fun()->decl;
									// debug_tree(gimple_assign_lhs(gc));
									// ret_type.reutnr_type_num = 0;
									assign_array.assign_type_array.push_back(assign_type);
									function_assign_collect->put(gimple_assign_rhs1(def_stmt), assign_array);
									// debug_tree(gimple_assign_rhs1(def_stmt));
									// debug_gimple_stmt(def_stmt);
								}
						}
					}
				}
				else if (TREE_CODE(second) == VAR_DECL)
				{

					function_assign_array assign_array;
					vector<assign_type> assign_type_array;

					if (function_assign_collect->get(second) == NULL)
					{

						// debug(gc);
						assign_array.assign_type_array = assign_type_array;
					}
					else
					{

						// debug(gc);
						assign_array = *(function_assign_collect->get(second));
						assign_type_array = assign_array.assign_type_array;
					}

					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = second;
					assign_type.form_tree = node->get_fun()->decl;
					// debug_tree(gimple_assign_lhs(gc));
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);
					function_assign_collect->put(second, assign_array);
					// debug_tree(gimple_assign_rhs1(def_stmt));
					// debug_gimple_stmt(def_stmt);
				}
			}
			if (second2)
			{
				if (TREE_CODE(second2) == FIELD_DECL)
				{

					function_assign_array assign_array;
					vector<assign_type> assign_type_array;

					if (function_assign_collect->get(second2) == NULL)
					{

						// debug(gc);
						assign_array.assign_type_array = assign_type_array;
					}
					else
					{

						// debug(gc);
						assign_array = *(function_assign_collect->get(second2));
						assign_type_array = assign_array.assign_type_array;
					}

					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = getFunctionAssignLHS;
					assign_type.form_tree = node->get_fun()->decl;
					// debug_tree(gimple_assign_lhs(gc));
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);
					function_assign_collect->put(second2, assign_array);
				}
			}
		}
		else if (TREE_CODE(getFunctionAssignLHS) == ARRAY_REF)
		{

			debug_gimple_stmt(gc);
			// debug_tree(getFunctionAssignLHS);
			tree second = TREE_OPERAND(getFunctionAssignLHS, 0);
			// debug_tree(second);

			function_assign_array assign_array;
			vector<assign_type> assign_type_array;

			if (function_assign_collect->get(second) == NULL)
			{

				// debug(gc);
				assign_array.assign_type_array = assign_type_array;
			}
			else
			{

				// debug(gc);
				assign_array = *(function_assign_collect->get(second));
				assign_type_array = assign_array.assign_type_array;
			}

			struct assign_type assign_type;

			assign_type.stmt = gc;
			assign_type.assign_tree = gimple_assign_rhs1(gc);
			assign_type.form_tree = node->get_fun()->decl;
			// debug_tree(gimple_assign_lhs(gc));
			// ret_type.reutnr_type_num = 0;
			assign_array.assign_type_array.push_back(assign_type);
			function_assign_collect->put(second, assign_array);
			// debug_tree(gimple_assign_rhs1(getFunctionAssignRHS));
		}
	}
	// 	fprintf(stderr, "--------------------collect_FunctionMapping_Assign end------------------\n");
}

void collect_function_return(gimple *gc, cgraph_node *node, basic_block bb)
{

	function_return_array fun_array;
	tree get_function_return_tree;
	if (gimple_return_retval(as_a<greturn *>(gc)))
		get_function_return_tree = gimple_return_retval(as_a<greturn *>(gc));
	else
		get_function_return_tree = NULL_TREE;
	vector<return_type> ret_type_array;

	// debug_gimple_stmt(gc);
	// fprintf(stderr, "GIMPLE_RETURN\n");
	// debug_tree(get_function_return_tree);
	// debug_tree(node->get_fun()->decl);
	// warning_at(gimple_location_safe(gc), 0, "use location");

	fun_array.return_type_array = ret_type_array;
	struct return_type ret_type;
	ret_type.stmt = gc;
	ret_type.return_tree = get_function_return_tree;
	// global
	global_ret_type_array.push_back(ret_type);

	fun_array.return_type_array.push_back(ret_type);
	function_return_collect->put(node->get_fun()->decl, fun_array);
}