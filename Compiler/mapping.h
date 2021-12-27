void FunctionStmtMappingRet(ptb *ptable, ptb *ftable, gimple_array *user_tmp)
{
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	FOR_EACH_TABLE(table_temp, t)
	{
		if (table_temp->target != NULL)
		{
			user_tmp = treeGimpleArray->get(table_temp->target);
			if (user_tmp != NULL)
			{

				const char *name;
				name = get_name(gimple_call_fn(table_temp->last_stmt));

				if (function_return_collect->get(table_temp->node->get_fun()->decl) == NULL)
					continue;
				function_return_array fun_array = *(function_return_collect->get(table_temp->node->get_fun()->decl));
				if (fun_array.return_type_num == 2)
					continue;

				vector<return_type> ret_type_array = fun_array.return_type_array;

				if (TREE_CODE(table_temp->target) == ADDR_EXPR)
				{
					tree second = TREE_OPERAND(table_temp->target, 0);
					gimple *def_stmt = SSA_NAME_DEF_STMT(table_temp->target);
					// debug(def_stmt);
					// debug_tree(second);
					table_temp->target = second;
					table_temp->last_stmt = def_stmt;
				}
				for (int i = 0; i < ret_type_array.size(); i++)
				{

					if (gimple_code(table_temp->last_stmt) == GIMPLE_CALL)
					{
						if (name != NULL)
						{
							if (!strcmp(name, "realloc") || !strcmp(name, "malloc") || !strcmp(name, "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
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
											fun_array.return_type_num = 2;
											function_return_collect->put(table_temp->node->get_fun()->decl, fun_array);
										}
									}
								}
							}
							else if (!strcmp(name, "pthread_create") || !strcmp(name, "pthread_join"))
							{
								if (!strcmp(name, "pthread_create"))
								{
									pthread_attr_array pthread_attr_array;
									tree getvardecl = TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 1), 0);
									if (pthread_attr_setdetachstates->get(getvardecl) != NULL)
									{

										pthread_attr_array = *(pthread_attr_setdetachstates->get(getvardecl));
										table_temp->swap_type = FUNCITON_THREAD;
										table_temp->pthread_type = pthread_attr_array.attr_type_num;
									}
								}
							}
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

									pthread_attr_array pthread_attr_array;
									tree getvardecl = TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 1), 0);

									if (pthread_attr_setdetachstates->get(getvardecl) != NULL)
									{

										pthread_attr_array = *(pthread_attr_setdetachstates->get(getvardecl));
										table_temp->pthread_type = pthread_attr_array.attr_type_num;
									}
								}
							}
						}
					}
					else if (gimple_code(table_temp->last_stmt) == GIMPLE_PHI)
					{

						if ((ret_type_array)[i].return_tree == gimple_phi_result(table_temp->last_stmt))
						{

							set_gimple_array(user_tmp, table_temp->last_stmt, (ret_type_array)[i].return_tree, (ret_type_array)[i].return_tree, (ret_type_array)[i].stmt);
							continue;
						}
					}
				}
			}
		}
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

		if (!gimple_has_body_p(node->decl))
			continue;
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
					collect_FunctionMapping_Assign(gc, node, bb, ptable);
				}
			}
		}
		pop_cfun();
	}
}