/*set allocation and deallocation table*/
void set_ptb(basic_block b, ptb *table, tree t, location_t l, int s, gimple *stmt, cgraph_node *node)
{

	if (table->target == NULL_TREE)
	{
		pointtablecount++;
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
			// fprintf(stderr, "add phi stmt ");debug_head(table->target);
		}
		table->removed = false;
		table->size = table->size + 1;
	}
	else
	{
		// fprintf(stderr, "\n======================================================================\n");
		// debug_tree(t);
		bool same = false;
		// ptb *tmp =table;
		while (table->next != NULL)
		{
			if (table->target == t)
			// if (table->stmt == used_stmt && LOCATION_LINE(gimple_location(table->stmt)) == LOCATION_LINE(gimple_location(used_stmt)))
			{
				same = true;
				// fprintf(stderr, "\n======================================================================\n");
			}
			table = table->next;
		}

		if (!same)
		{
			pointtablecount++;
			// fprintf(stderr, "\n======================================================================\n");
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
				// fprintf(stderr, "add phi stmt ");debug_head(table->target);
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
		// check_stmtStack4(used_stmt)
		while (table->next )
		{
	
			//  debug(table->stmt);
			//  debug_tree(target);
			//  if (table == NULL)
			//  break;

			if (table->stmt == used_stmt)
			{
				same = true;
				// break;
			}

			size_tmp = size_tmp + 1;
			table = table->next;
		}
		// if(!	check_stmtStack4(used_stmt))
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
