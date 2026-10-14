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
		ptb *head = table;
		while (table != NULL)
		{
			// De-duplicate collection of the same event, including the tail.
			// The same tree at a different statement is a different lifetime
			// event and must not be collapsed.
			if (table->target == t && table->last_stmt == stmt &&
				table->state == s)
				return;
			if (table->next == NULL)
				break;
			table = table->next;
		}

		if (table != NULL)
		{
			pointtablecount++;
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
			table->size = head->size;
			head->size++;
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
		gimple_array *head = table;
		while (table != NULL)
		{
			if (table->stmt == used_stmt && table->aptr == fucntion &&
				table->target == target && table->ret_stmt == ret_stmt)
				return;
			if (table->next == NULL)
				break;
			table = table->next;
		}
		table->next = new gimple_array();
		table = table->next;
		table->stmt = used_stmt;
		table->ret_stmt = ret_stmt;
		table->target = target;
		table->size = head->size;
		table->aptr = fucntion;
		table->next = NULL;
		head->size++;
	}
}
