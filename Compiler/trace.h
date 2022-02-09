static unsigned long now_fucntion = 0;
static gimple *now_laststmt;
static gimple *now_relatelaststmt;
static unsigned long now_laststmtid = 0;
static unsigned long now_relatelaststmtid = 0;
static int fistconunt = 0;

void trace_fucntion_relate_stmt(cgraph_node *node, tree function_tree, tree mallocStmt_tree)
{

	cgraph_edge *e;
	basic_block bb;
	tree t;
	if (node == NULL)
		return;
	// fprintf(stderr, "=======node_fun: =========\n");
	// debug(now_laststmt);
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		// if (!gimple_has_body_p (node->decl))
		//    continue;
		int fucntion_level = 0;
		push_cfun(node->get_fun());
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		if (cfun == NULL)
		{
			pop_cfun();
			continue;
		}
		// mutlple entry point
		if (cfun->decl == function_tree)
		{
			now_relatelaststmt = NULL;
			/*calculate dominator tree*/
			calculate_dominance_info(CDI_DOMINATORS);

			/*create DFS graph, Algorithm 1 and 2*/
			fprintf(stderr, "\033[40;36m ======= node_fun:%s========= \033[0m\n", get_name(function_tree));
			fprintf(stderr, "\033[40;36m ======= find relate stmt with %s ========= \033[0m\n", get_name(mallocStmt_tree));

			fprintf(stderr, "\033[40;36m ======= relate stmt argument:%s ========= \033[0m\n", get_name(mallocStmt_tree));
			int seccount = 0;
			FOR_EACH_BB_FN(bb, cfun)
			{
				if (!(tracerelatestmt && freemod))
					fistconunt = 0;

				for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
				{
					gimple *gc = gsi_stmt(gsi);
					const char *name;
					if (is_gimple_call(gc))
					{
						name = get_name(gimple_call_fn(gc));
						if (name != NULL)
							if (!strcmp(name, "pthread_exit"))
							{
								debug_gimple_stmt(gc);
							}
					}

					if (is_gimple_assign(gc))
					{
						struct ptr_info_def *pi1, *pi2, *pi3, *pi4;
						pi1 = SSA_NAME_PTR_INFO(mallocStmt_tree);
						struct pt_solution *pt1 = &pi1->pt;
						if (pt1 && relatemod && debugmod)
							if (TREE_CODE(mallocStmt_tree) == SSA_NAME)
							{
								if (gimple_assign_lhs(gc) != NULL)
								{
									if (TREE_CODE(gimple_assign_lhs(gc)) == SSA_NAME || TREE_CODE(gimple_assign_lhs(gc)) == MEM_REF)
									{
										pi2 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));
										struct pt_solution *pt2 = &pi2->pt;

										if (pt2)
										{
											if (gimple_assign_rhs1(gc) != NULL)
											{
												if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
												{

													if (gimple_assign_rhs1(gc) != NULL)
													{

														if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
														{

															if (TREE_CODE(gimple_assign_lhs(gc)) == ADDR_EXPR)
															{

																if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_rhs1(gc)))
																{
																	debug_tree(gimple_assign_rhs1(gc));
																	fprintf(stderr, "\n ================== warring ================== \n");

																	fprintf(stderr, "\033[40;35m this pointer possible  reference other address (rhs)\033[0m\n");
																	fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
																	fprintf(stderr, "\n ================== warring ================== \n");
																}
															}
														}

														if (TREE_CODE(gimple_assign_lhs(gc)) == SSA_NAME && TREE_CODE(gimple_assign_rhs1(gc)) == INTEGER_CST)
														{

															continue;
														}
														if (TREE_CODE(gimple_assign_lhs(gc)) == MEM_REF && TREE_CODE(gimple_assign_rhs1(gc)) == INTEGER_CST)
														{

															// fprintf(stderr, "\n ================== warring GLOBOBLBOLBO================== \n");
															tree first = TREE_OPERAND(gimple_assign_lhs(gc), 0);
															gimple *def_stmt = SSA_NAME_DEF_STMT(first);
															gimple *def_stmt2 = SSA_NAME_DEF_STMT(mallocStmt_tree);
															if (def_stmt && def_stmt2)
															{

																pi4 = SSA_NAME_PTR_INFO(first);
																struct pt_solution *pt1 = &pi4->pt;
																struct pt_solution *pt2 = &pi1->pt;
																if (!pt1 || pt1->anything)
																	continue;
																//
																if (is_gimple_assign(def_stmt) && is_gimple_assign(def_stmt2))
																	if (!strcmp(get_tree_code_name(TREE_CODE(gimple_assign_rhs1(def_stmt))), "<invalid tree code>"))
																	{

																		if (!strcmp(get_tree_code_name(TREE_CODE(gimple_assign_rhs1(def_stmt2))), "<invalid tree code>"))
																		{
																			if (!pt1->nonlocal && !pt2->nonlocal)
																			{
																				if (gimple_assign_rhs1(def_stmt2) != NULL)
																					if (!is_global_var(gimple_assign_rhs1(def_stmt2)))
																					{

																						if (pt1)
																							if (gimple_assign_rhs1(def_stmt) != NULL)
																								if (is_global_var(gimple_assign_rhs1(def_stmt)))
																								{
																									if (!ptr_derefs_may_alias_p(mallocStmt_tree, first))
																									{
																										continue;
																									}
																								}
																					}
																					else
																					{
																						continue;
																					}
																			}
																			else
																			{
																				continue;
																			}
																		}
																	}
															}
														}
														else if (TREE_CODE(gimple_assign_rhs1(gc)) == VAR_DECL)
														{

															if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
															{
																if (is_global_var(gimple_assign_rhs1(gc)))
																{
																	continue;
																}
															}
														}
														else if (TREE_CODE(gimple_assign_rhs1(gc)) == SSA_NAME)
														{

															pi3 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));
															pi4 = SSA_NAME_PTR_INFO(gimple_assign_rhs1(gc));
															struct pt_solution *pt3 = &pi3->pt;
															struct pt_solution *pt4 = &pi4->pt;
															if (!pt3 || !pt4)
																continue;
															else if (!ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_rhs1(gc)))
																continue;
														}
													}

													fprintf(stderr, "\033[40;36m ======= relate node_fun argument:%s========= \033[0m\n", get_name(mallocStmt_tree));
													fprintf(stderr, "\033[40;36m ======= relate gimple_assign_lhs:%s========= \033[0m\n", gimple_assign_lhs(gc));
													// ready add dot graph
													fprintf(stderr, "\n\n\n\n");
													unsigned long x = rand();
													if (freemod == true)
													{
														if (seccount == 0)
														{
															fprintf(stderr, "dot graph arrow");
														}
														else
														{
															if (now_laststmt != NULL)
															{

																fprintf(stderr, "dot graph relate stm2 start ID : %lu stmt(free) :", now_laststmtid);
																debug(now_laststmt);
																warning_at(gimple_location(now_laststmt), 0, "use location");
															}
															fprintf(stderr, "dot graph relate en1\n\n");
														}
														fprintf(stderr, "dot graph start relate for1");
													}
													else
													{

														if (fistconunt == 0)
														{
															fprintf(stderr, "dot graph start relate form");
															fistconunt++;
														}
														else
														{
															if (now_laststmt != NULL)
															{
																fprintf(stderr, "dot graph relate stm2 start ID : %lu stmt(free) :", now_laststmtid);
																debug(now_laststmt);
																warning_at(gimple_location(now_laststmt), 0, "use location");
																fprintf(stderr, "dot graph relate en1\n\n");
																now_laststmt = gc;
																now_laststmtid = x;
															}
															fprintf(stderr, "dot graph start relate for1");
														}
													}
													fistconunt++;
													fprintf(stderr, "ID : %lu\n", now_fucntion);
													fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(gc)->index);
													fprintf(stderr, "dot graph end relate end\n\n");

													// print address error
													warning_at(gimple_location(gc), 0, "use location");
													fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(LHS) :", x);
													debug(gc);
													// check_bbinfo2(gimple_bb(gc));
													warning_at(gimple_location(gc), 0, "use location");
													debug(gimple_assign_lhs(gc));
													fprintf(stderr, "dot graph relate end\n\n");
													// ready add dot graph
													now_relatelaststmt = gc;
													now_relatelaststmtid = x;
													now_laststmtid = x;

													fprintf(stderr, "\n ================== warring ================== \n");
													fprintf(stderr, "\033[40;35m this pointer possible  reference other address \033[0m\n");
													fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
													fprintf(stderr, "\n ================== warring ================== \n");
												}
											}
										}
									}
								}
							}

						if (TREE_CODE(gimple_assign_lhs(gc)) == ARRAY_REF || TREE_CODE(mallocStmt_tree) == SSA_NAME)
						{
							tree second = NULL_TREE;
							second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
							if (TREE_CODE(gimple_assign_lhs(gc)) == ARRAY_REF)
								second = TREE_OPERAND(gimple_assign_lhs(gc), 0);

							// debug_tree(second);
							// debug_tree(mallocStmt_tree);
							if (second != NULL_TREE)
								if ((TREE_CODE(mallocStmt_tree) == VAR_DECL) && TREE_CODE(second) == VAR_DECL)
									if ((second == mallocStmt_tree))
									{
										fprintf(stderr, "\033[40;36m ======= relate node_fun argument:%s========= \033[0m\n", get_name(function_tree));
										fprintf(stderr, "\033[40;36m ======= relate gimple_assign_lhs:%s========= \033[0m\n", gimple_assign_lhs(gc));
										debug_gimple_stmt(gc);
										warning_at(gimple_location(gc), 0, "use location");
										fprintf(stderr, "\n ================== warring ================== \n");
										fprintf(stderr, "\033[40;35m this pointer possible  reference other address \033[0m\n");
										fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");

										fprintf(stderr, "\n ================== warring ================== \n");
									}
							if ((second == NULL_TREE) && (TREE_CODE(mallocStmt_tree) == SSA_NAME))
							{
								if ((TREE_CODE(gimple_assign_rhs1(gc)) == SSA_NAME))
								{

									gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_rhs1(gc));

									if (def_stmt != NULL)
										if ((gimple_code(def_stmt) != GIMPLE_PHI) && ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)) && ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_rhs1(gc)))
										{
											fprintf(stderr, "\033[40;36m ======= relate node_fun argument:%s========= \033[0m\n", get_name(function_tree));
											fprintf(stderr, "\033[40;36m ======= relate gimple_assign_lhs:%s========= \033[0m\n", gimple_assign_lhs(gc));
											debug_gimple_stmt(gc);
											warning_at(gimple_location(gc), 0, "use location");

											fprintf(stderr, "\n ================== warring ================== \n");
											fprintf(stderr, "\033[40;35m this pointer possible  reference other address \033[0m\n");
											fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
											fprintf(stderr, "\n ================== warring ================== \n");
										}
								}
							}
						}
					}
					else if (is_gimple_call(gc) != NULL)
					{

						struct ptr_info_def *pi1, *pi2, *pi3, *pi4;

						pi1 = SSA_NAME_PTR_INFO(mallocStmt_tree);

						struct pt_solution *pt1 = &pi1->pt;
						// now_relatelaststmt = NULL;
						if (pt1 && relatemod && debugmod)
							if (gimple_call_num_args(gc) != 0)
							{
								const char *name;
								name = get_name(gimple_call_fn(gc));
								if (gimple_call_lhs(gc) != NULL)
									if (TREE_CODE(gimple_call_lhs(gc)) == SSA_NAME || TREE_CODE(gimple_call_arg(gc, 0)) == SSA_NAME || ((!strcmp(get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0))), "addr_expr"))))
										if (name != NULL)
										{
											if (!strcmp(name, "pthread_mutex_lock") || !strcmp(name, "pthread_mutex_unlock") || !strcmp(name, "pthread_exit"))
											{
												continue;
											}
											pi2 = SSA_NAME_PTR_INFO(gimple_call_arg(gc, 0));
											struct pt_solution *pt2 = &pi2->pt;

											if (pt2)
											{
												if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_call_arg(gc, 0)))
												{
													unsigned long x = rand();
													if (freemod == true)
													{
														if (seccount == 0)
														{
															fprintf(stderr, "dot graph arrow");
														}
														else
														{
															if (now_laststmt != NULL)
															{
																fprintf(stderr, "dot graph relate stm2 start ID : %lu stmt(free) :", now_laststmtid);
																debug(now_laststmt);
																warning_at(gimple_location(now_laststmt), 0, "use location");
															}
															fprintf(stderr, "dot graph relate en1\n\n");
														}
														fprintf(stderr, "dot graph start relate for1");
													}
													else
													{

														if (fistconunt == 0)
														{
															fprintf(stderr, "dot graph start relate form");
															fistconunt++;
														}
														else
														{
															if (now_laststmt != NULL)
															{
																fprintf(stderr, "dot graph relate stm2 start ID : %lu stmt(free) :", now_laststmtid);
																debug(now_laststmt);
																warning_at(gimple_location(now_laststmt), 0, "use location");
																fprintf(stderr, "dot graph relate en1\n\n");
																now_laststmt = gc;
																now_laststmtid = x;
															}
															fprintf(stderr, "dot graph start relate for1");
														}
													}

													fistconunt++;

													// debug_tree(gimple_call_arg(gc, 0));
													fprintf(stderr, "ID : %lu\n", now_fucntion);
													fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(gc)->index);
													fprintf(stderr, "dot graph end relate end\n\n");
													fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(call) :", x);
													debug(gc);
													// check_bbinfo2(gimple_bb(gc));
													now_relatelaststmt = gc;
													now_relatelaststmtid = x;
													now_laststmtid = x;
													warning_at(gimple_location(gc), 0, "use location");
													debug(gimple_call_arg(gc, 0));
													fprintf(stderr, "dot graph relate end\n\n");
												}
											}
										}
							}
					}
				}
			}

			vector<relate_type> relate_type_array;
			function_relate_array fun_array;
			if (now_relatelaststmt != NULL)
			{
				if (function_relate_collect->get(mallocStmt_tree) == NULL)
				{
					fun_array.relate_type_array = relate_type_array;
				}
				else
				{
					fun_array = *(function_relate_collect->get(mallocStmt_tree));
					relate_type_array = fun_array.relate_type_array;
				}

				struct relate_type relate_type;
				relate_type.stmt = now_relatelaststmt;
				relate_type.relate_tree = mallocStmt_tree;
				relate_type.relate_funtree = function_tree;
				relate_type.now_fucntion = now_fucntion;
				relate_type.now_stmt = now_laststmtid;

				fun_array.relate_type_array.push_back(relate_type);
				function_relate_collect->put(mallocStmt_tree, fun_array);
			}
		}
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}

int trace_function_path(tree function_tree, int fucntion_level, tree mallocStmt_tree, int *freecount)
{
	if (function_path_collect->get(function_tree) == NULL)
	{
		return 0;
	}
	now_fucntion = rand();
	function_path_array fun_array = *(function_path_collect->get(function_tree));
	vector<function_path> function_path_array = fun_array.function_path_array;
	const char *name;
	fistconunt = 0;

	fprintf(stderr, "\033[40;44m =======trace_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);

	if (retmod)
		if (function_return_collect->get(function_tree) != NULL && fucntion_level == RET_HEAP_OBJECT)
		{
			// function_pthread_exit_array callerFunArray = *(function_pthread_exit_collect->get(function_tree));
			function_return_array callerFunArray = *(function_return_collect->get(function_tree));

			vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
			for (int k = 0; k < callerRetTypearray.size(); k++)
			{

				if ((callerRetTypearray)[k].return_tree)
				{
					// debug_points_to_info_for(mallocStmt_tree);
					if (TREE_CODE(mallocStmt_tree) != INTEGER_CST && TREE_CODE(mallocStmt_tree) != FUNCTION_DECL && TREE_CODE((callerRetTypearray)[k].return_tree) != FUNCTION_DECL)
						if (TREE_CODE((callerRetTypearray)[k].return_tree) != INTEGER_CST)
						{
							struct ptr_info_def *pi;
							pi = SSA_NAME_PTR_INFO(mallocStmt_tree);
							struct pt_solution *pt = &pi->pt;
							if (pi)
								if (ptr_derefs_may_alias_p((callerRetTypearray)[k].return_tree, mallocStmt_tree))
								{
									fprintf(stderr, "\033[40;31m  find return stmt  \033[0m\n");
									fprintf(stderr, "\033[40;31m  this fucntion return heap-object  \033[0m\n");
									debug_gimple_stmt((callerRetTypearray)[k].stmt);
									warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
								}
						}
				}
			}
		}
	if (mallocStmt_tree != NULL_TREE && fucntion_level != DISABLE_TREACE)
	{
		if (function_free_collect->get(function_tree) != NULL)
		{
			function_free_array callerFunArray = *(function_free_collect->get(function_tree));
			vector<free_type> callerRetTypearray = callerFunArray.free_type_array;
			hash_map<tree, int> *function_basicblock_collect = new hash_map<tree, int>;
			for (int k = 0; k < callerRetTypearray.size(); k++)
			{
				function_basicblock_collect->put(gimple_block((callerRetTypearray)[k].stmt), 0);
			}
			for (int k = 0; k < callerRetTypearray.size(); k++)
			{
				debug_gimple_stmt((callerRetTypearray)[k].stmt);
				if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
				{
					fprintf(stderr, "\033[40;31m  find free stmt free same pointer \033[0m\n");
					debug_gimple_stmt((callerRetTypearray)[k].stmt);
					warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
					++(*freecount);
					if (freemod && debugmod)
					{
						if (*(function_basicblock_collect)->get(gimple_block((callerRetTypearray)[k].stmt)) == 0)
						{
							function_basicblock_collect->put(gimple_block((callerRetTypearray)[k].stmt), 1);
							fprintf(stderr, "dot graph start relate form ");
						}
						else
						{
							fprintf(stderr, "dot graph start relate form ");
						}
						fprintf(stderr, "ID : %lu\n", now_fucntion);
						fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb((callerRetTypearray)[k].stmt)->index);
						fprintf(stderr, "dot graph end relate end\n\n");
						now_laststmt = (callerRetTypearray)[k].stmt;
						// ready add dot graph
						unsigned long x = rand();
						now_laststmtid = x;
						// ready add dot graph
						fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(free) :", x);
						debug((callerRetTypearray)[k].stmt);
						warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
						fprintf(stderr, "dot graph relate end\n\n");
						// ready add dot graph
						fprintf(stderr, "dot graph target color desc");
						fprintf(stderr, "green");
						fprintf(stderr, "dot graph target color desend\n\n");
						// ready add dot graph
						fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(free) :", x);
						debug((callerRetTypearray)[k].stmt);
						warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
						fprintf(stderr, "dot graph relate end\n\n");
					}
				}
			}
			delete function_basicblock_collect;
		}
	}

	if (function_pthread_detched_collect->get(function_tree) != NULL && fucntion_level == -1 && fucntion_level != DISABLE_TREACE)
	{
		function_pthread_detched_array callerFunArray = *(function_pthread_detched_collect->get(function_tree));
		vector<pthread_detched_type> callerRetTypearray = callerFunArray.pthread_detched_array;
		for (int k = 0; k < callerRetTypearray.size(); k++)
		{

			fprintf(stderr, "\033[40;31m  find pthread_detched stmt  \033[0m\n");
			debug_gimple_stmt((callerRetTypearray)[k].stmt);
			warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
			*freecount = *freecount + 1;
			if (pthread_detachedmod)
			{
				fprintf(stderr, "dot graph start relate form ");
				fprintf(stderr, "ID : %lu\n", now_fucntion);
				fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb((callerRetTypearray)[k].stmt)->index);
				fprintf(stderr, "dot graph end relate end\n\n");
				// ready add dot graph
				now_laststmt = (callerRetTypearray)[k].stmt;
				// ready add dot graph
				unsigned long x = rand();
				now_laststmtid = x;
				// ready add dot graph
				fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(pthread_detched)) :", x);
				debug((callerRetTypearray)[k].stmt);
				warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
				// debug(gimple_assign_lhs((callerRetTypearray)[k].stmt));
				fprintf(stderr, "dot graph relate end\n\n");
			}
		}
	}

	if (function_pthread_exit_collect->get(function_tree) != NULL && fucntion_level == -1 && fucntion_level != DISABLE_TREACE)
	{
		function_pthread_exit_array callerFunArray = *(function_pthread_exit_collect->get(function_tree));
		vector<pthread_exit_type> callerRetTypearray = callerFunArray.pthread_exit_array;
		for (int k = 0; k < callerRetTypearray.size(); k++)
		{

			fprintf(stderr, "\033[40;31m  find pthread_exit stmt  \033[0m\n");
			debug_gimple_stmt((callerRetTypearray)[k].stmt);
			warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
			if (pthread_exitmod)
			{
				fprintf(stderr, "dot graph start relate form ");

				fprintf(stderr, "ID : %lu\n", now_fucntion);
				fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb((callerRetTypearray)[k].stmt)->index);
				fprintf(stderr, "dot graph end relate end\n\n");
				// ready add dot graph
				now_laststmt = (callerRetTypearray)[k].stmt;
				// ready add dot graph
				unsigned long x = rand();
				now_laststmtid = x;
				// ready add dot graph
				fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(pthread_exit) :", x);
				debug((callerRetTypearray)[k].stmt);
				warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
				// debug(gimple_assign_lhs((callerRetTypearray)[k].stmt));
				fprintf(stderr, "dot graph relate end\n\n");
			}
		}
	}

	//避開 pthread
	if (tracerelatestmt == true && fucntion_level != -1 && fucntion_level != DISABLE_TREACE)
	{
		struct cgraph_node *node;
		if (mallocStmt_tree != NULL)
		{

			struct ptr_info_def *pi1, *pi2, *pi3;
			pi1 = SSA_NAME_PTR_INFO(mallocStmt_tree);
			struct pt_solution *pt1 = &pi1->pt;
			if (pt1 && pt1->null)
			{
				trace_fucntion_relate_stmt(node, function_tree, mallocStmt_tree);
			}
		}
	}
	if (fucntion_level != -1 && fucntion_level != DISABLE_TREACE)
		fucntion_level += 1;

	if (function_path_array.size() == 0)
	{
		if (function_return_collect->get(function_tree) != NULL)
		{
			function_return_array callerFunArray = *(function_return_collect->get(function_tree));
			if (function_return_collect->get(function_tree) != NULL && callerFunArray.return_type_num == 2)
				fprintf(stderr, "%s this fucntion return value is heap-object \n", get_name(function_tree));
		}
	}

	function_return_array *callerFunArray;

	callerFunArray = function_return_collect->get(function_tree);
	vector<return_type> callerRetTypearray;
	function_return_array calleeFunArray;

	if (callerFunArray)
		for (int i = 0; i < function_path_array.size(); i++)
		{

			int find = 0;
			fprintf(stderr, "\033[40;42m =======pre add _ fucntion:%s========= \033[0m\n", get_name((function_path_array)[i].next));

			for (int o = 0; o < traceStack.size(); o++)
			{
				if (traceStack[o] == (function_path_array)[i].next)
				{
					find = 1;
					fprintf(stderr, "\033[40;41m =======recursive_fun2:%s========= \033[0m\n", get_name(traceStack[o]));
					break;
				}
			}

			if ((function_path_array)[i].next)
				if (function_return_collect->get((function_path_array)[i].next) != NULL)
				{

					callerRetTypearray = callerFunArray->return_type_array;
					calleeFunArray = *(function_return_collect->get((function_path_array)[i].next));
					function_return_array find_fun_array;
					vector<return_type> ret_type_find_fun_array;
					tree getFucntionDecl = ((function_path_array)[i].next);

					if (function_return_collect->get(getFucntionDecl) == NULL)
					{
						find_fun_array.return_type_array = ret_type_find_fun_array;
					}
					else
					{
						find_fun_array = *(function_return_collect->get(getFucntionDecl));
					}

					for (int k = 0; k < callerRetTypearray.size(); k++)
					{
						if (calleeFunArray.return_type_num == 2)
							if ((callerRetTypearray)[k].return_tree == (function_path_array)[i].next)
							{
								fprintf(stderr, "%s this fucntion return value is heap-object \n", get_name((function_path_array)[i].next));
								fprintf(stderr, "%s this fucntion return value is heap-object %s\n", get_name(function_tree), get_name((callerRetTypearray)[k].return_tree));
								if (fucntion_level == RET_HEAP_OBJECT)
									(*freecount) = EFFECTIVE;
							}
					}
				}
			if (find == 0)
			{
				fprintf(stderr, "\033[40;46m =======add node_fun stack:%s========= \033[0m\n", get_name((function_path_array)[i].next));
				traceStack.push_back((function_path_array)[i].next);
				trace_function_path((function_path_array)[i].next, fucntion_level, mallocStmt_tree, freecount);
				traceStack.pop_back();
			}
		}
}

void tracefree_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp)
{
	cgraph_edge *e;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		int fucntion_level = 0;
		push_cfun(node->get_fun());
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		if (cfun == NULL)
		{
			pop_cfun();
			continue;
		}
		enum availability avail;

		fprintf(stderr, "fucntion collect path \n");
		pathStack.push_back(cfun->decl);
		walk_function_path(cfun->decl, fucntion_level, ptable, user_tmp);
		pathStack.pop_back();
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}

void walk_function_path(tree function_tree, int fucntion_level, ptb *ptable, gimple_array *user_tmp)
{
	if (function_path_collect->get(function_tree) == NULL)
		return;

	function_path_array fun_array = *(function_path_collect->get(function_tree));
	vector<function_path> function_path_array = fun_array.function_path_array;
	function_return_array callerFunArray;
	vector<return_type> callerRetTypearray;

	fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	if (function_return_collect->get(function_tree) != NULL)
	{
		callerFunArray = *(function_return_collect->get(function_tree));
		callerRetTypearray = callerFunArray.return_type_array;
		fprintf(stderr, "\033[40;44m =======print_function_type %d  ========  \033[0m\n", callerFunArray.return_type_num);

		if (callerFunArray.pthread_type_num == FUNCITON_THREAD)
			fprintf(stderr, "\033[40;44m =======print_pthread_type_is thread_fucntion  ========  \033[0m\n");
	}

	fucntion_level += 1;

	int find = 0;
	int find_thread = 0;
	for (int i = 0; i < function_path_array.size(); i++)
	{
		find = 0;
		fprintf(stderr, "\033[40;42m =======pre add _ fucntion:%s========= \033[0m\n", get_name((function_path_array)[i].next));

		for (int o = 0; o < pathStack.size(); o++)
		{
			if (pathStack[o] == (function_path_array)[i].next)
			{
				find = 1;
				fprintf(stderr, "\033[40;41m =======recursive_fun:%s========= \033[0m\n", get_name(pathStack[o]));
				break;
			}
		}
		if (find == 0)
		{
			pathStack.push_back((function_path_array)[i].next);
			if (function_return_collect->get((function_path_array)[i].next) != NULL)
			{
				function_return_array calleeFunArray = *(function_return_collect->get((function_path_array)[i].next));
				fprintf(stderr, "\033[40;44m =======print_function_type %d  ========  \033[0m\n", calleeFunArray.return_type_num);
				// thread lock set
				function_return_array find_fun_array;
				vector<return_type> ret_type_find_fun_array;
				tree getFucntionDecl = ((function_path_array)[i].next);

				if (function_return_collect->get(getFucntionDecl) == NULL)
					find_fun_array.return_type_array = ret_type_find_fun_array;
				else
					find_fun_array = *(function_return_collect->get(getFucntionDecl));

				vector<return_type> funcalleeRetTypearray = find_fun_array.return_type_array;

				for (int k = 0; k < callerRetTypearray.size(); k++)
				{
					if ((callerRetTypearray)[k].return_type_stmt_num == FUNCITON_THREAD)
					{
						int find_diff = 0;
						for (int k2 = 0; k2 < funcalleeRetTypearray.size(); k2++)
						{
							if ((callerRetTypearray)[k].return_tree == (funcalleeRetTypearray)[k2].return_tree)
								find_diff = 1;
						}
						if (find_diff == 0)
						{

							struct return_type ret_type;
							ret_type.stmt = (callerRetTypearray)[k].stmt;
							ret_type.return_tree = (callerRetTypearray)[k].return_tree;
							ret_type.return_type_stmt_num = FUNCITON_THREAD;

							find_fun_array.return_type_array.push_back(ret_type);
							function_return_collect->put(getFucntionDecl, find_fun_array);
							find_thread = FUNCITON_THREAD;
						}
					}
				}
			}
			walk_function_path((function_path_array)[i].next, fucntion_level, ptable, user_tmp);
			fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.back()));
			pathStack.pop_back();
		}
	}

	if (find_thread == FUNCITON_THREAD)
		checkPointerConstraint(function_tree, ptable, user_tmp, NULL, FUNCITON_THREAD);
	else
		checkPointerConstraint(function_tree, ptable, user_tmp, NULL, 0);
}

void dump_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp)
{

	cgraph_edge *e;
	if (node == NULL)

		fprintf(stderr, "=======node_fun: =========\n");

	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!gimple_has_body_p(node->decl))
			continue;
		int fucntion_level = 0;

		push_cfun(node->get_fun());

		// fprintf(stderr, "=======node_fun:=========\n");
		// debug_tree(node->get_fun()->decl);
		if (node->get_fun()->decl == NULL)
		{
			pop_cfun();
			continue;
		}
		// mutlple entry point
		// if (!strcmp(get_name(cfun->decl), "main"))

		fprintf(stderr, "\033[40;44m =======node_fun:%s========= \033[0m\n", get_name(cfun->decl));
		fprintf(stderr, "\033[40;44m fucntion collect path  \033[0m\n");

		pathStack.push_back(cfun->decl);
		// debug_tree(node->get_fun()->decl );
		walk_function_path(cfun->decl, fucntion_level, ptable, user_tmp);
		fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.back()));
		pathStack.pop_back();

		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}
