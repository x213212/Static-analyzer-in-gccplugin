function_assign_array ret_function_varstmt(tree function_tree)
{
	function_assign_array assign_array;
	if (function_assign_collect->get(function_tree) != NULL)
	{

		assign_array = *(function_assign_collect->get(function_tree));

		vector<assign_type> assign_type_array = assign_array.assign_type_array;
		assign_array.pass = 1;
		// debug_tree(function_tree);
		//vector<pair<fdecl,location_t>> loc;
		// fprintf(stderr, "=======prinst_function_var %d   %d========\n", function_tree, assign_type_array.size());
		// for (int i =0 ; i < assign_type_array.size() ; i++)
		// {
		// 	debug_gimple_stmt( assign_type_array[i].stmt);
		// }
		function_assign_collect->put(function_tree, assign_array);
	}
	// fprintf(stderr, "[\n");
	// if (assign_type_array.size() != 0)
	return assign_array;
	// else
	// 	return ;
}

void get_function_varstmt(tree function_tree, tree vartree)
{
	if (function_assign_collect->get(function_tree) == NULL)
		return;
	function_assign_array assign_array = *(function_assign_collect->get(function_tree));

	vector<assign_type> assign_type_array = assign_array.assign_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_var %d   %d========\n", function_tree, assign_type_array.size());
	fprintf(stderr, "[\n");
	for (int i = 0; i < assign_type_array.size(); i++)
	{

		// for (int i = 0; i < (ret_type_array).size(); i++)
		// {

		debug((assign_type_array)[i].stmt);
		fprintf(stderr, ",\n");
		// }
		// fprintf(stderr, "]\n");
		// trace_function_path(&ret_type_array);
	}
	fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGN CALLLLLLLLLLLLLLLLLLLLLLL\n");
	fprintf(stderr, "]\n");
}

void get_function_specialstmt(tree function_tree, tree vartree)
{
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_return %d   %d========\n", function_tree, ret_type_array.size());
	fprintf(stderr, "[\n");
	for (int i = 0; i < ret_type_array.size(); i++)
	{

		// for (int i = 0; i < (ret_type_array).size(); i++)
		// {

		debug((ret_type_array)[i].stmt);
		fprintf(stderr, ",\n");
		// }
		// fprintf(stderr, "]\n");
		// trace_function_path(&ret_type_array);
	}
	fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGN CALLLLLLLLLLLLLLLLLLLLLLL\n");
	fprintf(stderr, "]\n");
}
