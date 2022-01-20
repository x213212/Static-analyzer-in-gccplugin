int check_stmtStack(tree target)
{
	for (int o = new_gimpletree_array.size()-1; o >= 0; o--)
		if (new_gimpletree_array[o] == target)
			return 1;

	int size = sizeof(tree);
	totalsize += size;
	new_gimpletree_array.push_back(target);
	return 0;
}
int check_bbStack(basic_block target)
{
	for (int o = new_path_array.size()-1; o >= 0; o--)
		if (new_path_array[o] == target)
			return 1;

	new_path_array.push_back(target);
	return 0;
}
int check_stmtStack2(gimple *stmt)
{
	for (int o =new_gimple_array.size()-1; o >=0; o--)
		if (new_gimple_array[o] == stmt)
			return 1;

	int size = sizeof(stmt);
	totalsize += size;
	new_gimple_array.push_back(stmt);
	return 0;
}
int check_stmtStack3(tree target)
{
	for (int o = new_gimpletree_array.size()-1; o >=0; o--)
		if (new_gimpletree_array[o] == target)
			return 1;

	// int size = sizeof(tree);
	// totalsize += size;

	return 0;
}
int check_stmtStack4(gimple *stmt)
{
	for (int o = new_gimple_array.size()-1; o >=0; o--)
		if (new_gimple_array[o] == stmt)
			return 1;
	// int size = sizeof(stmt);
	// totalsize += size;

	return 0;
}

void Checknew_search_imm_use_lhs(gimple_array *used_stmt, gimple *use_stmt, tree target, tree target2)
{

	gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_lhs(use_stmt));

	if (def_stmt)
		if (gimple_assign_lhs(def_stmt))
		{
			if (TREE_CODE(gimple_assign_lhs(def_stmt)) == ARRAY_REF)
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			else if (TREE_CODE(gimple_assign_lhs(def_stmt)) == COMPONENT_REF)
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			else if (TREE_CODE(gimple_assign_lhs(def_stmt)) == MEM_REF)
				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
		}
}
void Checknew_search_imm_use_rhs(gimple_array *used_stmt, gimple *use_stmt, tree target, tree target2)
{

	gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_rhs1(use_stmt));

	if (def_stmt)

		if (gimple_assign_rhs1(def_stmt))
		{
			if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == ARRAY_REF)

				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);

			else if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == COMPONENT_REF)

				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);

			else if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == MEM_REF)

				Varnew_search_imm_use(used_stmt, def_stmt, target, target2);
			
			else if (TREE_CODE(target) == VAR_DECL)

				Varnew_search_imm_use(used_stmt, use_stmt, target, target2);
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

void check_bbinfo(cgraph_node *m, basic_block bb)
{
	if (pathconstraint)
	{
		int find_path_constraint = 0;
		fprintf(stderr, "=======================Path Constaint===========================\n\n");
		for (int o = 0; o < symbolicExecution.size(); o++)
		{

			if (syminfo->get(symbolicExecution[o]) != NULL)
			{
				struct symbolicinfo *symbolicinfotmp = syminfo->get(symbolicExecution[o]);
				push_cfun(symbolicinfotmp->node->get_fun());
				calculate_dominance_info(CDI_DOMINATORS);

				if (symbolicinfotmp->cond_truebranch == bb || dominated_by_p(CDI_DOMINATORS, bb, symbolicinfotmp->cond_truebranch))
				{
					if (gimple_location_safe(symbolicinfotmp->cond_stmt), 0, "use location")
						debug_gimple_stmt(symbolicinfotmp->cond_stmt);
					warning_at(gimple_location_safe(symbolicinfotmp->cond_stmt), 0, "use location");

					find_path_constraint++;
				}
				if (symbolicinfotmp->cond_falsebranch == bb || dominated_by_p(CDI_DOMINATORS, bb, symbolicinfotmp->cond_falsebranch))
				{
					if (gimple_location_safe(symbolicinfotmp->cond_stmt), 0, "use location")
						debug_gimple_stmt(symbolicinfotmp->cond_stmt);
					warning_at(gimple_location_safe(symbolicinfotmp->cond_stmt), 0, "use location");

					find_path_constraint++;
				}
				pop_cfun();
			}
		}

		if (!find_path_constraint)
			fprintf(stderr, "=======================NO find Path Constaint===========================\n\n");
		else
			fprintf(stderr, "=======================Path Constaint=====%d======================\n\n", find_path_constraint);
		fprintf(stderr, "=======================Path Constaint===========================\n\n");
	}
}
