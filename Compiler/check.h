int check_stmtStack(tree target)
{
	for (int o = 0; o < new_gimpletree_array.size(); o++)
	{

		if (new_gimpletree_array[o] == target)
		{

			return 1;
		}
	}
	// fprintf(stderr, "stmt sisze%d\n",size);
	int size = sizeof(tree);
	totalsize += size;
	new_gimpletree_array.push_back(target);
	return 0;
}
int check_stmtStack2(gimple *stmt)
{

	for (int o = 0; o < new_gimple_array.size(); o++)
	{

		if (new_gimple_array[o] == stmt)
		{

			return 1;
		}
	}
	int size = sizeof(stmt);
	totalsize += size;
	new_gimple_array.push_back(stmt);
	return 0;
}
int check_stmtStack3(gimple *stmt)
{

	for (int o = 0; o < new_gimple_array.size(); o++)
	{

		if (new_gimple_array[o] == stmt)
		{

			return 1;
		}
	}
	int size = sizeof(stmt);
	totalsize += size;

	return 0;
}
int check_stmtStack4(tree target)
{
	for (int o = 0; o < new_gimpletree_array.size(); o++)
	{

		if (new_gimpletree_array[o] == target)
		{

			return 1;
		}
	}
	int size = sizeof(tree);
	totalsize += size;

	return 0;
}
// void Checknew_search_imm_use(gimple_array *used_stmt, gimple *use_stmt, tree target, tree target2) __attribute__((noinline));
void Checknew_search_imm_use_lhs(gimple_array *used_stmt, gimple *use_stmt, tree target, tree target2)
{

	gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_lhs(use_stmt));

	if (def_stmt)
	{
		if (gimple_assign_lhs(def_stmt))
		{

			if (TREE_CODE(gimple_assign_lhs(def_stmt)) == ARRAY_REF)
			{
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			}
			else if (TREE_CODE(gimple_assign_lhs(def_stmt)) == COMPONENT_REF)
			{
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			}
			else if (TREE_CODE(gimple_assign_lhs(def_stmt)) == MEM_REF)
			{
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			}
		}
	}
}
void Checknew_search_imm_use_rhs(gimple_array *used_stmt, gimple *use_stmt, tree target, tree target2)
{

	gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_rhs1(use_stmt));

	if (def_stmt)
	{
		if (gimple_assign_rhs1(def_stmt))
		{

			if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == ARRAY_REF)
			{
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			}
			else if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == COMPONENT_REF)
			{
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			}
			else if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == MEM_REF)
			{
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			}
		}
	}
}
/*addr_expr preprocess*/
tree gettree;
tree prechecktree(tree tree)
{
	if (tree)
		if (TREE_CODE(tree) == ADDR_EXPR)
		{
			gettree = TREE_OPERAND(tree, 0);
			if (gettree && TREE_CODE(gettree) != ERROR_MARK)
				return gettree;
		}
	return tree;
}