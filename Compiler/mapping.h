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

							collect_FunctionMapping_Ret(table_temp->node->get_fun()->decl, u_stmt, user_tmp, table_temp, ptable);
						}
					}

				if (gimple_code(table_temp->last_stmt) == GIMPLE_CALL)
				{

					const char *name;
					name = get_name(gimple_call_fn(table_temp->last_stmt));

					if (name != NULL)

						if (!strcmp(name, "realloc") || !strcmp(name, "malloc") || !strcmp(name, "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
						{

							debug_tree(table_temp->node->get_fun()->decl);
							if (function_return_collect->get(table_temp->node->get_fun()->decl) == NULL)
								continue;
							function_return_array fun_array = *(function_return_collect->get(table_temp->node->get_fun()->decl));

							vector<return_type> ret_type_array = fun_array.return_type_array;

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
											// debug((ret_type_array)[i].stmt);
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