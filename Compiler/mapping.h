void FunctionStmtMappingRet(ptb *ptable, ptb *ftable, gimple_array *user_tmp)
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	//

	FOR_EACH_TABLE(table_temp, t)
	{
		// fprintf(stderr, "\n --------------------------ssssss----------------\n");
		if (table_temp->target != NULL)
		{
			// debug_tree(table_temp->target);

			user_tmp = treeGimpleArray->get(table_temp->target);
			if (user_tmp != NULL)
			{

				// fprintf(stderr, " \n Pointer to set  is size %d :[ \n", user_tmp->size);
				if (user_tmp->size > 0)
					FOR_EACH_USE_TABLE(user_tmp, u_stmt)
					{
						if (u_stmt != NULL)
						{
							// if (function_return_collect->get(table_temp->node->get_fun()->decl) == NULL)
							// 	continue;
							// function_return_array fun_array = *(function_return_collect->get(table_temp->node->get_fun()->decl));

							// vector<return_type> ret_type_array = fun_array.return_type_array;
							// if (fun_array.return_type_num == 2)
							// 	continue;
							// if (gimple_code(u_stmt) == GIMPLE_PHI)
							// {

							// 	if ((ret_type_array)[i].return_tree == gimple_phi_result(u_stmt))
							// 	{
							// 		// debug((ret_type_array)[i].stmt);
							// 		// debug(u_stmt);

							// 		// tree fundecl = TREE_OPERAND(gimple_assign_lhs(u_stmt), 0);
							// 		// gimple *def_stmt = SSA_NAME_DEF_STMT(u_stmt);
							// 		// debug(def_stmt);
							// 		set_gimple_array(user_tmp, table_temp->last_stmt, (ret_type_array)[i].return_tree, (ret_type_array)[i].return_tree, gc);
							// 		continue;
							// 	}
							// }

							// if (user_tmp->target != NULL)
							// if (!strcmp(get_tree_code_name(TREE_CODE(user_tmp->target)), "addr_expr"))
							// {
							// fprintf(stderr, "=======node_fu---------------=========\n");
							// debug_gimple_stmt(u_stmt);

							// else
							// 	// debug_tree(user_tmp->target);
							// 	debug(u_stmt);
							// struct cgraph_node *node;
							// FOR_EACH_DEFINED_FUNCTION(node)
							// {
							// 	// if (!ipa)
							// 	// 	init_table();
							// 	push_cfun(node->get_fun());
							// 	// if (strcmp(get_name(cfun->decl), "main") == 0)
							// 	if (cfun == NULL)
							// 		continue;
							// debug_tree(cfun->decl);
							// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
							// calculate_dominance_info(CDI_DOMINATORS);
							// if (table_temp->node->get_fun()->decl == node->get_fun()->decl)
							// {
							// fprintf(stderr, "oooooooooooooooooooooooooooooooooooooooo%s\n", get_name(table_temp->node->get_fun()->decl));

							collect_FunctionMapping_Ret(table_temp->node->get_fun()->decl, u_stmt, user_tmp, table_temp, ptable);
							// }
							// pop_cfun();
						}
					}

				// user_tmp->

				// fprintf(stderr, "=======node_fu---------------=========\n");
				// debug_gimple_stmt(table_temp->last_stmt);

				if (gimple_code(table_temp->last_stmt) == GIMPLE_CALL)
				{
					// fprintf(stderr, "RETURN with possible mallo3~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
					// fprintf(stderr, "RETURN with possible mall3~~~~~~~~~~~~~~~~~~~~~\n");
					// debug_gimple_stmt(table_temp->last_stmt);
					// debug_tree(table_temp->target);
					// fprintf(stderr, "RETURN with possible mall3~~~~~~~~~~~~~~~~~\n");
					// fprintf(stderr, "RETURN with possible mal3~~~~~~~~~~~~~~~~~~~~~~~\n");
					const char *name;
					name = get_name(gimple_call_fn(table_temp->last_stmt));
					// debug(table_temp->last_stmt);
					// fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_successssssssssssssssss%s\n", name);
					if (name != NULL)
						// 		debug(u_stmt);
						// fprintf(stderr, "%s\n",name);
						// fprintf(stderr, "RETURN with possible malloc3 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
						if (!strcmp(name, "realloc") || !strcmp(name, "malloc") || !strcmp(name, "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
						{
							// fprintf(stderr, "RETURN with possible malloc5 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
							debug_tree(table_temp->node->get_fun()->decl);
							if (function_return_collect->get(table_temp->node->get_fun()->decl) == NULL)
								continue;
							function_return_array fun_array = *(function_return_collect->get(table_temp->node->get_fun()->decl));

							vector<return_type> ret_type_array = fun_array.return_type_array;
							// fprintf(stderr, "RETURN with possible mallo66~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%d\n",fun_array.return_type_num);
							if (fun_array.return_type_num == 2)
								continue;
							for (int i = 0; i < ret_type_array.size(); i++)
							{
								if ((ret_type_array)[i].return_tree != NULL && table_temp->target != NULL)
								{

									if (TREE_CODE((ret_type_array)[i].return_tree) == SSA_NAME)
									{
										// fprintf(stderr, "RETURN with possible malloc2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
										// debug(table_temp->last_stmt);
										// debug((ret_type_array)[i].stmt);
										if (ptr_derefs_may_alias_p(table_temp->target, (ret_type_array)[i].return_tree))
										{
											// fprintf(stderr, "RETURN with possible malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
											// debug(table_temp->last_stmt);
											// debug(gc);
											// debug_tree(table_temp->target);
											// debug_tree((ret_type_array)[i].return_tree);
											// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
											// warning_at(gimple_location(gc), 0, "use location");
											// fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_successssssssssssssssss\n");
											// return value related malloc stmt
											// debug(test);
											// debug(u_stmt);
											// debug(gc);
											// set_ptb( table_temp->bb, ptable,  table_temp->target,  table_temp->loc, 0,  table_temp->last_stmt,  table_temp->node);
											fun_array.return_type_num = 2;

											function_return_collect->put(table_temp->node->get_fun()->decl, fun_array);
											// return;
											// return;
										}
									}
								}
								// fprintf(stderr, "RETURN with possible malloc3~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
								// debug_gimple_stmt(u_stmt);
								// debug_tree(table_temp->target);
								// if (gimple_code(u_stmt) == GIMPLE_RETURN)
								// {
								// 	tree get_function_return_tree = gimple_return_retval(as_a<greturn *>(u_stmt));
								// 	fprintf(stderr, "RETURN with possible malloc2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
								// 	debug(table_temp->last_stmt);
								// 	debug(u_stmt);
								// 	if (get_function_return_tree)
								// 		if (ptr_derefs_may_alias_p(table_temp->target, get_function_return_tree))
								// 		{
								// 			fprintf(stderr, "RETURN with possible malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

								// 			fun_array.return_type_num = 2;

								// 			function_return_collect->put(table_temp->node->get_fun()->decl, fun_array);
								// 			// return;
								// 			// return;
								// 		}
								// }
							}
							// if ((ret_type_array)[i].return_tree != NULL && table_temp->target != NULL)
							// {
						}
				}
			}
		}
		// else
		// {
		// 	fprintf(stderr, " \n Pointer to set  is size 0 :[ \n");
		// 	fprintf(stderr, "] \n");
		// }
	}
}
void FunctionStmtMappingAssign(ptb *ptable, gimple_array *user_tmp)
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
		if (cfun == NULL)
			continue;
		// fprintf(stderr, "=======Mapping node_fun:%s=========\n", get_name(cfun->decl));
		// debug_tree(cfun->decl);
		calculate_dominance_info(CDI_DOMINATORS);
		FOR_EACH_BB_FN(bb, cfun)
		{
			// debug_bb(bb);

			for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *gc = gsi_stmt(gsi);
				if (is_gimple_call(gc) || is_gimple_assign(gc))
				{
					/*collect malloc and free information*/
					// collect_function_call(gc, node, bb);
					collect_FunctionMapping_Assign(gc, node, bb);

					// fprintf(stderr, "add collect_function_call\n");
					// }else if (is_gimple_assign(gc)){
					// 	collect_FunctionMapping_Assign(gc, node, bb);
				}
				// }
			}
		}
		pop_cfun();
	}
}