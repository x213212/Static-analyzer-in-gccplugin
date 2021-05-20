
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
							// if (user_tmp->target != NULL)
							// if (!strcmp(get_tree_code_name(TREE_CODE(user_tmp->target)), "addr_expr"))
							// {
							// 	// debug_tree(user_tmp->target);
							// 	// fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");

							// else
							// 	// debug_tree(user_tmp->target);
							// 	debug(u_stmt);
							struct cgraph_node *node;
							FOR_EACH_DEFINED_FUNCTION(node)
							{
								// if (!ipa)
								// 	init_table();
								push_cfun(node->get_fun());
								// if (strcmp(get_name(cfun->decl), "main") == 0)
								if (cfun == NULL)
									continue;

								// debug_tree(cfun->decl);
								// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
								calculate_dominance_info(CDI_DOMINATORS);
								// fprintf(stderr, "oooooooooooooooooooooooooooooooooooooooo\n", get_name(cfun->decl));

								collect_FunctionMapping_Ret(cfun->decl, u_stmt, user_tmp, table_temp, ptable);
								pop_cfun();
							}
						}

						// user_tmp->
					}
			}
			// else
			// {
			// 	fprintf(stderr, " \n Pointer to set  is size 0 :[ \n");
			// 	fprintf(stderr, "] \n");
			// }
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