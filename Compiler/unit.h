bool bb_in_loop_p(basic_block bb)
{
	return bb->loop_father->header->index != 0;
}

bool bb_in_branch_p(gimple *stmt)
{
	//function* fn = DECL_STRUCT_FUNCTION(gimple_get_lhs(stmt));
	// fprintf(stderr, "backkkkkkkbackkkkkkkbackkkkkkkbackkkkkkkbackkkkkkkbackkkkkkk\n");
	// debug_gimple_stmt(stmt);
	// debug_bb(stmt->bb);
	// fprintf(stderr, "prev_bbprev_bbprev_bbprev_bbprev_bbprev_bbprev_bbprev_bb\n");
	// debug_bb(cfun->cfg->x_exit_block_ptr->prev_bb);
	// debug_bb(cfun->cfg->x_entry_block_ptr->next_bb);
	// debug_bb(stmt->bb);
	// fprintf(stderr," %d\n",dominated_by_p(CDI_DOMINATORS,stmt->bb,cfun->cfg->x_exit_block_ptr->prev_bb));

	return !dominated_by_p(CDI_DOMINATORS, stmt->bb, cfun->cfg->x_exit_block_ptr->prev_bb);
}
void init_table()
{
	//fprintf(stderr,"init_table.... \n");
	start1.target = NULL_TREE;
	start1.next = NULL;
	start1.state = POINTER_NOT_EXIST;
	ptable = &start1;

	start2.target = NULL_TREE;
	start2.next = NULL;
	start2.state = POINTER_NOT_EXIST;
	ftable = &start2;

	start3.target = NULL_TREE;
	start3.next = NULL;
	start3.state = POINTER_NOT_EXIST;
	retable = &start3;

	start4.target = NULL_TREE;
	start4.next = NULL;
	start4.state = POINTER_NOT_EXIST;
	phitable = &start4;

	start5.target = NULL_TREE;
	start5.next = NULL;
	start5.state = POINTER_NOT_EXIST;
	return_table = &start5;

	start6.target = NULL_TREE;
	start6.next = NULL;
	start6.state = POINTER_NOT_EXIST;
	use_table = &start6;

	start7.target = NULL_TREE;
	start7.next = NULL;
	start7.state = POINTER_NOT_EXIST;
	fopen_table = &start7;

	start8.target = NULL_TREE;
	start8.next = NULL;
	start8.state = POINTER_NOT_EXIST;
	locktable = &start8;

	start9.target = NULL_TREE;
	start9.next = NULL;
	start9.state = POINTER_NOT_EXIST;
	unlocktable = &start9;
}

bool Location_b(gimple *a, gimple *b, basic_block bb)
{
	gimple *gc;
	//debug_gimple_stmt(a);
	//debug_gimple_stmt(b);
	for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
	{
		gc = gsi_stmt(gsi);
		//debug_gimple_stmt(gc);
		if (gc == a)
		{
			return true;
		}
		else if (gc == b)
		{
			return false;
		}
	}
	return false;
}

bool Location_b2(gimple *a, gimple *b, tree function_tree)
{
	struct cgraph_node *node;
	basic_block bb;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		//   if (!gimple_has_body_p (node->decl))
		//       continue;

		push_cfun(DECL_STRUCT_FUNCTION(node->decl));

		// fprintf(stderr, "=====www==node_fun:%s=========\n", get_name(cfun->decl));

		if (cfun == NULL)
			continue;
		//mutlple entry point
		if (cfun->decl == function_tree)
		{

			calculate_dominance_info(CDI_DOMINATORS);

			FOR_EACH_BB_FN(bb, cfun)
			{

				for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
				{
					gimple *gc = gsi_stmt(gsi);
					if (gc == a)
					{
						pop_cfun();
						return true;
					}
					else if (gc == b)
					{
						pop_cfun();
						return false;
					}
				}
			}
		}
				pop_cfun();
	}
	return false;
}

unsigned int SDBMHash(char *str)
{
	unsigned int hash = rand() % 10;

	while (*str)
	{
		// equivalent to: hash = 65599*hash + (*str++);
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}

	return (hash & 0x7FFFFFFF);
}