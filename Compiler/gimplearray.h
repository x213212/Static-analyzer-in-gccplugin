/*set allocation and deallocation table*/
void set_ptb(basic_block b, ptb *table, tree t, location_t l, int s, gimple *stmt, cgraph_node *node)
{

	if (table->target == NULL_TREE)
	{
		table->bb = b;
		table->target = t;
		table->next = NULL;
		table->loc = l;
		table->state = s;
		table->may_realloc = false;
		table->last_stmt = stmt;
		table->node = node;
		table->inbranch = bb_in_branch_p(stmt);
		table->fun = node->get_fun();
		if (gimple_code(stmt) == GIMPLE_PHI)
		{
			//fprintf(stderr, "add phi stmt ");debug_head(table->target);
		}
		table->removed = false;
		table->size = table->size + 1;
	}
	else
	{
		// fprintf(stderr, "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
		// debug_tree(t);
		bool same = false;
		// ptb *tmp =table;
		while (table->next != NULL)
		{
			if (table->target == t)
			// if (table->stmt == used_stmt && LOCATION_LINE(gimple_location(table->stmt)) == LOCATION_LINE(gimple_location(used_stmt)))
			{
				same = true;
				// fprintf(stderr, "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
				
			}
		
				table = table->next;
		}
		// if( table->next == NULL){
		// 		fprintf(stderr, "22222wwwwwwwwww\n");
		// // table = table->next;
		// }
		if (!same)
		{
			// fprintf(stderr, "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
			// debug_tree(t);
			table->next = new ptb();
			table = table->next;
			table->bb = b;
			table->target = t;
			table->next = NULL;
			table->loc = l;
			table->state = s;
			table->may_realloc = false;
			table->last_stmt = stmt;
			table->node = node;
			table->inbranch = bb_in_branch_p(stmt);
			table->fun = node->get_fun();
			if (gimple_code(stmt) == GIMPLE_PHI)
			{
				//fprintf(stderr, "add phi stmt ");debug_head(table->target);
				table->is_phi_stmt = true;
			}
			else
				table->is_phi_stmt = false;
			table->removed = false;
			table->size = table->size + 1;
		}
	}
}

void set_gimple_array(gimple_array *table, gimple *used_stmt, tree fucntion, tree target, gimple *ret_stmt)
{
	// fprintf(stderr, "set_gimple_array----------------\n");
	// 	gimple_array *user_tmp ;
	// 	user_tmp = tvpt2->get(a);
	// 	if(user_tmp != NULL)
	// 	return ;
	// bool find = false;
	// gimple *u_stmt;
	// gimple_array table_tmp = table;
	// FOR_EACH_USE_TABLE(table_tmp, u_stmt) 			{
	// 	if(table_tmp->aptr == a)
	// 		fprintf(stderr , "222222222222222\n");
	// }
	// debug_tree(a);

	if (table->stmt == NULL)
	{
		table->stmt = used_stmt;
		table->ret_stmt = ret_stmt;
		table->aptr = fucntion;
		table->target = target;
		table->size = 1;
		table->next = NULL;
	}
	else
	{
		bool same = false;
		int size_tmp = 0;
		while (table->next != NULL)
		{
			//  fprintf(stderr, "set_gimple_array----------------\n");
			//  debug(table->stmt);
			//  debug(used_stmt);
			if (table->stmt == used_stmt)
			// if (table->stmt == used_stmt && LOCATION_LINE(gimple_location(table->stmt)) == LOCATION_LINE(gimple_location(used_stmt)))
			{

				same = true;
				break;
			}
			size_tmp = size_tmp + 1;
			table = table->next;
		}
		if (!same)
		{
			table->next = new gimple_array();
			table = table->next;
			table->stmt = used_stmt;
			table->ret_stmt = ret_stmt;
			table->target = target;
			table->size = size_tmp;
			table->aptr = fucntion;
			table->next = NULL;
		}
	}
}
