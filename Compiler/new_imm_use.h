void Varnew_search_imm_use(gimple_array *used_stmt, gimple *use_stmt, tree target, tree target2)
{
	// if(	gimple_code(use_stmt) == GIMPLE_CALL  && TREE_CODE(target) == ADDR_EXPR)
	// debug_gimple_stmt(use_stmt);
	// debug_tree(target);
	// fprintf(stderr, "------------------VAR_DECL : LHS2------------------\n");
	if ((gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == VAR_DECL) ||
		gimple_assign_rhs1(use_stmt) && TREE_CODE(gimple_assign_rhs1(use_stmt)) == VAR_DECL ||
		gimple_assign_rhs1(use_stmt) && TREE_CODE(gimple_assign_rhs1(use_stmt)) == ARRAY_REF ||
		gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == ARRAY_REF ||
		gimple_assign_rhs1(use_stmt) && TREE_CODE(gimple_assign_rhs1(use_stmt)) == COMPONENT_REF ||
		gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == COMPONENT_REF)

	{
		/*
		int *foo(int z)
		{
			int *b = malloc(1);
			int *p2 = malloc(1);
			p2 = &b;
			b[0] = 2;
			p2[0] = 1;
			free(p2);
			return p2;
		}
		*/
		// debug_gimple_stmt(use_stmt);
		// debug_tree(target);
		// fprintf(stderr, "------------------VAR_DECL : LHS2------------------\n");
		tree getFunctionAssignVAR;

		if ((gimple_code(use_stmt) == GIMPLE_ASSIGN))
		{
			if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == VAR_DECL)
				getFunctionAssignVAR = gimple_assign_lhs(use_stmt);
			if (gimple_assign_rhs1(use_stmt) && TREE_CODE(gimple_assign_rhs1(use_stmt)) == VAR_DECL)
				getFunctionAssignVAR = gimple_assign_rhs1(use_stmt);
			if (gimple_assign_rhs1(use_stmt))
			{
				if (TREE_CODE(gimple_assign_rhs1(use_stmt)) == ARRAY_REF)
				{
					// debug_gimple_stmt(use_stmt);
					// 	fprintf(stderr, "------------------VAR_DECL : LHS2------------------\n");
					tree second = TREE_OPERAND(gimple_assign_rhs1(use_stmt), 0);
					if (second)
					{
						getFunctionAssignVAR = second;
					}
				}
			}
			if (gimple_assign_lhs(use_stmt))
			{
				if (TREE_CODE(gimple_assign_lhs(use_stmt)) == ARRAY_REF)
				{
					// debug_gimple_stmt(use_stmt);
					// 	debug_gimple_stmt(use_stmt);
					tree second = TREE_OPERAND(gimple_assign_lhs(use_stmt), 0);
					if (second)
						getFunctionAssignVAR = second;
				}
			}
			if (gimple_assign_rhs1(use_stmt))
			{
				if (TREE_CODE(gimple_assign_rhs1(use_stmt)) == COMPONENT_REF)
				{
					// debug_gimple_stmt(use_stmt);
					// fprintf(stderr, "------------------VAR_DECL : COMPONENT_REF------------------\n");
					// debug_gimple_stmt(use_stmt);
					// debug_gimple_stmt(use_stmt);
					tree second = TREE_OPERAND(gimple_assign_rhs1(use_stmt), 0);
					tree second2 = TREE_OPERAND(gimple_assign_rhs1(use_stmt), 1);
					if (second)
					{
						if (TREE_CODE(second) == MEM_REF)
						{
							tree three = TREE_OPERAND(second, 0);
							if (three)
							{
								if (TREE_CODE(three) == SSA_NAME)
								{
									gimple *def_stmt = SSA_NAME_DEF_STMT(three);

									if (is_gimple_assign(def_stmt))

										if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
										{
											getFunctionAssignVAR = gimple_assign_rhs1(def_stmt);
										}
								}
								else if (TREE_CODE(three) == VAR_DECL)
									getFunctionAssignVAR = three;
							}
						}
						else if (TREE_CODE(second) == VAR_DECL)
						{
							getFunctionAssignVAR = second;
						}

						if (second2)
						{
							if (TREE_CODE(second2) == FIELD_DECL)
							{

								if (TREE_CODE(second) == VAR_DECL)
								{
									getFunctionAssignVAR = second;
								}

								function_assign_array assign_array;
								vector<assign_type> assign_type_array;

								if (function_assign_collect->get(second2) != NULL)
								{
									assign_array = *(function_assign_collect->get(second2));
									assign_type_array = assign_array.assign_type_array;
									if (function_assign_collect->get(second2) != NULL)
									{

										if (TREE_CODE(second) != VAR_DECL)
											for (int i = 0; i < assign_array.assign_type_array.size(); i++)
											{

												if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
												{

													if (TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
													{

														tree filed_from = TREE_OPERAND(second, 0);

														tree filed_from2 = TREE_OPERAND((assign_array.assign_type_array)[i].assign_tree, 0);

														if (TREE_CODE(filed_from2) == MEM_REF)
															filed_from2 = TREE_OPERAND(filed_from2, 0);

														if (TREE_CODE(filed_from) == MEM_REF)
															filed_from = TREE_OPERAND(filed_from, 0);

														tree treecheck;
														gimple *def_stmt;
														if (TREE_CODE(filed_from2) == SSA_NAME)
														{
															def_stmt = SSA_NAME_DEF_STMT(filed_from2);
															if (def_stmt)
																if (gimple_code(def_stmt) != GIMPLE_NOP)
																	if (gimple_assign_rhs1(def_stmt))
																		if (TREE_CODE(gimple_assign_rhs1(def_stmt)) != ERROR_MARK)
																			if (def_stmt)
																				treecheck = gimple_assign_rhs1(def_stmt);
																			else
																				treecheck = filed_from2;
														}
														else
															treecheck = filed_from2;

														gimple *def_stmt2;
														tree treecheck2;
														if (TREE_CODE(second) != VAR_DECL && TREE_CODE(filed_from) != INTEGER_CST)
														{
															gimple *def_stmt2 = SSA_NAME_DEF_STMT(filed_from);
															if (def_stmt2)
																if (gimple_code(def_stmt2) != GIMPLE_NOP)
																	if (gimple_assign_rhs1(def_stmt2))
																		if (TREE_CODE(gimple_assign_rhs1(def_stmt2)) != ERROR_MARK)
																			if (def_stmt2)
																				treecheck2 = gimple_assign_rhs1(def_stmt2);
																			else
																				treecheck2 = filed_from;
														}
														else
															treecheck2 = filed_from;

														if (!treecheck)
															treecheck = filed_from2;
														if (!treecheck2)
															treecheck2 = filed_from;

														if (treecheck2 == treecheck)
															if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
															{

																if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target2 && !check_stmtStack2((assign_array.assign_type_array)[i].stmt))
																{

																	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
																	new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
																}
															}
													}
													else if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
													{

														tree filed_from = TREE_OPERAND(second, 0);
														tree filed_from2 = TREE_OPERAND((assign_array.assign_type_array)[i].assign_tree, 0);

														if (TREE_CODE(filed_from2) == MEM_REF)
															filed_from2 = TREE_OPERAND(filed_from2, 0);

														if (TREE_CODE(filed_from) == MEM_REF)
															filed_from = TREE_OPERAND(filed_from, 0);

														tree treecheck;
														gimple *def_stmt;
														if (TREE_CODE(filed_from2) == SSA_NAME)
														{
															def_stmt = SSA_NAME_DEF_STMT(filed_from2);
															if (def_stmt)
																if (gimple_code(def_stmt) != GIMPLE_NOP)
																	if (gimple_assign_rhs1(def_stmt))
																		if (TREE_CODE(gimple_assign_rhs1(def_stmt)) != ERROR_MARK)
																			if (def_stmt)
																				treecheck = gimple_assign_rhs1(def_stmt);
																			else
																				treecheck = filed_from2;
														}
														else
															treecheck = filed_from2;

														gimple *def_stmt2;
														tree treecheck2;

														if (TREE_CODE(second) != VAR_DECL && TREE_CODE(filed_from) != INTEGER_CST)
														{
															gimple *def_stmt2 = SSA_NAME_DEF_STMT(filed_from);

															if (def_stmt2)
																if (gimple_code(def_stmt2) != GIMPLE_NOP)
																	if (gimple_assign_rhs1(def_stmt2))
																		if (TREE_CODE(gimple_assign_rhs1(def_stmt2)) != ERROR_MARK)
																			if (def_stmt2)
																				treecheck2 = gimple_assign_rhs1(def_stmt2);
																			else
																				treecheck2 = filed_from;
														}
														else
															treecheck2 = filed_from;
														if (!treecheck)
															treecheck = filed_from2;
														if (!treecheck2)
															treecheck2 = filed_from;

														if (treecheck2 == treecheck)
															if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
																if (gimple_assign_lhs((assign_array.assign_type_array)[i].stmt) != target2 && !check_stmtStack2((assign_array.assign_type_array)[i].stmt))
																{

																	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
																	new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
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
			}
			if (gimple_assign_lhs(use_stmt))
			{
				if (TREE_CODE(gimple_assign_lhs(use_stmt)) == COMPONENT_REF)
				{
					// debug_gimple_stmt(use_stmt);
					tree second = TREE_OPERAND(gimple_assign_lhs(use_stmt), 0);
					tree second2 = TREE_OPERAND(gimple_assign_lhs(use_stmt), 1);
					if (second)
					{
						tree three = TREE_OPERAND(second, 0);
						if (TREE_CODE(second) == MEM_REF)
						{
							if (three)
							{
								if (TREE_CODE(three) == SSA_NAME)
								{
									gimple *def_stmt = SSA_NAME_DEF_STMT(three);

									if (is_gimple_assign(def_stmt))

										if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
										{
											getFunctionAssignVAR = gimple_assign_rhs1(def_stmt);
										}

									if (!check_stmtStack(three))
									{
										// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
										if (three != target2 && !check_stmtStack2(use_stmt))
										{
											// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
											set_gimple_array(used_stmt, use_stmt, three, target, NULL);
											// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
											new_search_imm_use(used_stmt, three, three);
										}
									}
								}
								else if (TREE_CODE(three) == VAR_DECL)
									getFunctionAssignVAR = three;
							}
						}
						if (second2)
						{
							if (TREE_CODE(second2) == FIELD_DECL)
							{
								// debug_tree(second);
								if (TREE_CODE(second) == VAR_DECL)
								{
									getFunctionAssignVAR = second;
									// fprintf(stderr, "mappinggggggggggggFIELD_DECLggggggggggggggggggg-------\n");
								}
								function_assign_array assign_array;
								vector<assign_type> assign_type_array;

								if (function_assign_collect->get(second2) != NULL)
								{
									assign_array = *(function_assign_collect->get(second2));
									assign_type_array = assign_array.assign_type_array;
									if (function_assign_collect->get(second2) != NULL)
									{

										// debug(use_stmt);
										// fprintf(stderr, "=======fist hit========\n");
										// 	debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
										// assign_array = ret_function_varstmt(getFunctionAssignVAR);
										// fprintf(stderr, "=======print_function_var test %d========\n", assign_array.pass);
										// fprintf(stderr, "=======print_function_var %d   %d========\n", getFunctionAssignVAR, assign_array.assign_type_array.size());
										// if (TREE_CODE(second) != VAR_DECL)
										if (TREE_CODE(second) == VAR_DECL)
										{
											// fprintf(stderr, "=======fist hit========\n");
											getFunctionAssignVAR = second;
											// debug_gimple_stmt(use_stmt);
										}
										if (TREE_CODE(second) != VAR_DECL)
											for (int i = 0; i < assign_array.assign_type_array.size(); i++)
											{

												// if ((assign_array.assign_type_array)[i].stmt == use_stmt)
												// {
												// debug((assign_array.assign_type_array)[i].stmt);
												// 	continue;
												// }

												if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
												{
													// fprintf(stderr, "=======fist hit========\n");
													// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);

													if (TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
													{
														tree filed_from = TREE_OPERAND(second, 0);

														tree filed_from2 = TREE_OPERAND((assign_array.assign_type_array)[i].assign_tree, 0);

														if (TREE_CODE(filed_from2) == MEM_REF)
															filed_from2 = TREE_OPERAND(filed_from2, 0);

														if (TREE_CODE(filed_from) == MEM_REF)
															filed_from = TREE_OPERAND(filed_from, 0);

														tree treecheck;
														gimple *def_stmt;
														if (TREE_CODE(filed_from2) == SSA_NAME)
														{
															def_stmt = SSA_NAME_DEF_STMT(filed_from2);
															if (def_stmt)
																if (gimple_code(def_stmt) != GIMPLE_NOP)
																	if (gimple_assign_rhs1(def_stmt))
																		if (TREE_CODE(gimple_assign_rhs1(def_stmt)) != ERROR_MARK)
																			if (def_stmt)
																				treecheck = gimple_assign_rhs1(def_stmt);
																			else
																				treecheck = filed_from2;
														}
														else
															treecheck = filed_from2;

														gimple *def_stmt2;
														tree treecheck2;
														if (TREE_CODE(second) != VAR_DECL && TREE_CODE(filed_from) != INTEGER_CST)
														{
															gimple *def_stmt2 = SSA_NAME_DEF_STMT(filed_from);
															// if (filed_from)
															if (def_stmt2)
																if (gimple_code(def_stmt2) != GIMPLE_NOP)
																	if (gimple_assign_rhs1(def_stmt2))
																		if (TREE_CODE(gimple_assign_rhs1(def_stmt2)) != ERROR_MARK)
																			if (def_stmt2)
																				treecheck2 = gimple_assign_rhs1(def_stmt2);
																			else
																				treecheck2 = filed_from;
														}
														else
															treecheck2 = filed_from;
														if (!treecheck)
															treecheck = filed_from2;
														if (!treecheck2)
															treecheck2 = filed_from;
														// debug_tree(treecheck);
														// debug_tree(treecheck2);
														// debug_tree(filed_from);
														// debug_tree(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
														if (treecheck2 == treecheck)
															if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
															{
																// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
																if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target2 && !check_stmtStack2((assign_array.assign_type_array)[i].stmt))
																{
																	// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
																	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
																	// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
																	new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
																}
															}
													}
													else if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
													{

														tree filed_from = TREE_OPERAND(second, 0);

														tree filed_from2 = TREE_OPERAND((assign_array.assign_type_array)[i].assign_tree, 0);

														if (TREE_CODE(filed_from2) == MEM_REF)
															filed_from2 = TREE_OPERAND(filed_from2, 0);

														if (TREE_CODE(filed_from) == MEM_REF)
															filed_from = TREE_OPERAND(filed_from, 0);

														tree treecheck;
														gimple *def_stmt;
														if (TREE_CODE(filed_from2) == SSA_NAME)
														{
															def_stmt = SSA_NAME_DEF_STMT(filed_from2);
															if (def_stmt)
																if (gimple_code(def_stmt) != GIMPLE_NOP)
																	if (gimple_assign_rhs1(def_stmt))
																		if (TREE_CODE(gimple_assign_rhs1(def_stmt)) != ERROR_MARK)
																			if (def_stmt)
																				treecheck = gimple_assign_rhs1(def_stmt);
																			else
																				treecheck = filed_from2;
														}
														else
															treecheck = filed_from2;

														gimple *def_stmt2;
														tree treecheck2;

														if (TREE_CODE(filed_from) == SSA_NAME)
														{

															gimple *def_stmt2 = SSA_NAME_DEF_STMT(filed_from);

															if (def_stmt2)
																if (TREE_CODE(filed_from) != GIMPLE_NOP)
																	if (gimple_code(def_stmt2) != GIMPLE_NOP)
																		if (gimple_assign_rhs1(def_stmt2))
																			if (TREE_CODE(gimple_assign_rhs1(def_stmt2)) != ERROR_MARK)
																				if (def_stmt2)
																				{
																					// fprintf(stderr, "=======fist hi5t========\n");
																					treecheck2 = gimple_assign_rhs1(def_stmt2);
																				}
																				else
																					treecheck2 = filed_from;
														}
														else
															treecheck2 = filed_from;
														if (!treecheck)
															treecheck = filed_from2;
														if (!treecheck2)
															treecheck2 = filed_from;

														if (treecheck2 == treecheck)
														{

															if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
																if (gimple_assign_lhs((assign_array.assign_type_array)[i].stmt) != target2 && !check_stmtStack2((assign_array.assign_type_array)[i].stmt))
																{
																	// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
																	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
																	new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
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
					else if (TREE_CODE(second) == VAR_DECL)
					{
						getFunctionAssignVAR = second;
					}
					//
				}
			}
		}

		if (!check_stmtStack(gimple_assign_lhs(use_stmt)) && !check_stmtStack2((use_stmt)))
		{
			set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
		}
		else if (!check_stmtStack(gimple_assign_rhs1(use_stmt)) && !check_stmtStack2((use_stmt)))
		{
			set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);
		}

		if (getFunctionAssignVAR)
		{
			function_assign_array assign_array;
			// debug_tree(getFunctionAssignVAR);
			if (function_assign_collect->get(getFunctionAssignVAR) != NULL)
			{

				assign_array = ret_function_varstmt(getFunctionAssignVAR);

				for (int i = 0; i < assign_array.assign_type_array.size(); i++)
				{
					gimple *nowstmt = (assign_array.assign_type_array)[i].stmt;

					if (gimple_code(nowstmt) == GIMPLE_ASSIGN)
					{

						if (TREE_CODE(gimple_assign_lhs(nowstmt)) == MEM_REF)
						{

							if (!check_stmtStack(gimple_assign_lhs(nowstmt)))

							{
								if ((assign_array.assign_type_array)[i].form_tree != NULL)
								{

									set_gimple_array(used_stmt, nowstmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
								}
								else
									set_gimple_array(used_stmt, nowstmt, gimple_assign_lhs(nowstmt), target, NULL);

								tree second = TREE_OPERAND(gimple_assign_lhs(nowstmt), 0);
								if (second)
								{

									if (TREE_CODE(second) == SSA_NAME)
									{
										gimple *def_stmt = SSA_NAME_DEF_STMT(second);

										if (is_gimple_assign(def_stmt))

											if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
											{

												if (gimple_assign_lhs(def_stmt) != target2)
													new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
											}
									}
								}
							}
						}
						else if (TREE_CODE(gimple_assign_lhs(nowstmt)) == SSA_NAME)
						{

							if (!check_stmtStack(gimple_assign_lhs(nowstmt)))
							{
								if ((assign_array.assign_type_array)[i].form_tree != NULL)
								{

									set_gimple_array(used_stmt, nowstmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
								}
								else
									set_gimple_array(used_stmt, nowstmt, gimple_assign_lhs(nowstmt), target, NULL);

								if (!check_stmtStack4(nowstmt))
									if (gimple_assign_lhs(nowstmt) != target2 && !check_stmtStack2(nowstmt))

										new_search_imm_use(used_stmt, gimple_assign_lhs(nowstmt), gimple_assign_lhs(nowstmt));
							}
							if (gimple_assign_rhs1(nowstmt) && TREE_CODE(gimple_assign_rhs1(nowstmt)) == COMPONENT_REF)
							{

								if (!check_stmtStack(gimple_assign_rhs1(nowstmt)))
								{
									tree second = TREE_OPERAND(gimple_assign_rhs1(nowstmt), 0);
									if (second)
									{
										if (TREE_CODE(second) == MEM_REF)
										{
											tree three = TREE_OPERAND(second, 0);
											if (three)
											{

												if (TREE_CODE(three) == SSA_NAME)
												{
													gimple *def_stmt = SSA_NAME_DEF_STMT(three);

													if (is_gimple_assign(def_stmt))

														if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
														{

															if (gimple_assign_lhs(def_stmt) != target2)
																new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
														}
												}
											}
										}
									}
								}
							}
							else if (gimple_assign_rhs1(nowstmt) && TREE_CODE(gimple_assign_rhs1(nowstmt)) == MEM_REF)
							{
								if (!check_stmtStack(gimple_assign_rhs1(nowstmt)))

								{
									if ((assign_array.assign_type_array)[i].form_tree != NULL)
									{

										set_gimple_array(used_stmt, nowstmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
									}
									else
										set_gimple_array(used_stmt, nowstmt, gimple_assign_rhs1(nowstmt), target, NULL);

									tree second = TREE_OPERAND(gimple_assign_rhs1(nowstmt), 0);
									if (TREE_CODE(second) == MEM_REF)
									{
										gimple *def_stmt = SSA_NAME_DEF_STMT(second);

										if (is_gimple_assign(def_stmt))

											if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
											{

												if (gimple_assign_lhs(def_stmt) != target2)
													new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
											}
									}
								}
							}
							else if (gimple_assign_rhs1(nowstmt) && TREE_CODE(gimple_assign_rhs1(nowstmt)) == ARRAY_REF)
							{

								if (!check_stmtStack(gimple_assign_rhs1(nowstmt)))
								{

									if ((assign_array.assign_type_array)[i].form_tree != NULL)
									{

										set_gimple_array(used_stmt, nowstmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
									}
									else
										set_gimple_array(used_stmt, nowstmt, gimple_assign_rhs1(nowstmt), target, NULL);
								}
							}
						}
						else if (TREE_CODE(gimple_assign_rhs1(nowstmt)) == SSA_NAME)
						{
							if (!check_stmtStack(gimple_assign_rhs1(nowstmt)))
							{

								if ((assign_array.assign_type_array)[i].form_tree != NULL)
								{

									set_gimple_array(used_stmt, nowstmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
								}
								else
									set_gimple_array(used_stmt, nowstmt, gimple_assign_rhs1(nowstmt), target, NULL);

								if (gimple_assign_rhs1(nowstmt) != target2)
								{

									if (!check_stmtStack4(nowstmt))
										if (gimple_assign_rhs1(nowstmt) != target2 && !check_stmtStack2(nowstmt))
											new_search_imm_use(used_stmt, gimple_assign_rhs1(nowstmt), gimple_assign_rhs1(nowstmt));
								}
							}
							if (gimple_assign_lhs(nowstmt) && TREE_CODE(gimple_assign_lhs(nowstmt)) == COMPONENT_REF)
							{

								if (!check_stmtStack(gimple_assign_lhs(nowstmt)))
								{
									tree second = TREE_OPERAND(gimple_assign_lhs(nowstmt), 0);
									if (second)
									{
										if (TREE_CODE(second) == MEM_REF)
										{
											tree three = TREE_OPERAND(second, 0);
											if (three)
											{

												if (TREE_CODE(three) == SSA_NAME)
												{
													gimple *def_stmt = SSA_NAME_DEF_STMT(three);

													if (is_gimple_assign(def_stmt))

														if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
														{

															if (gimple_assign_lhs(def_stmt) != target2)
																new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
														}
												}
											}
										}
									}
								}
							}
							else if (gimple_assign_lhs(nowstmt) && TREE_CODE(gimple_assign_lhs(nowstmt)) == MEM_REF)
							{

								if (!check_stmtStack(gimple_assign_lhs(nowstmt)))
								{
									tree second = TREE_OPERAND(gimple_assign_lhs(nowstmt), 0);
									if (TREE_CODE(second) == MEM_REF)
									{
										gimple *def_stmt = SSA_NAME_DEF_STMT(second);

										if (is_gimple_assign(def_stmt))

											if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
											{

												if (gimple_assign_lhs(def_stmt) != target2)
													new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
											}
									}
								}
							}
							else if (TREE_CODE(gimple_assign_lhs(nowstmt)) == ARRAY_REF)
							{
								if (!check_stmtStack(gimple_assign_lhs(nowstmt)))
								{

									if ((assign_array.assign_type_array)[i].form_tree != NULL)
									{

										set_gimple_array(used_stmt, nowstmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
									}
									else
										set_gimple_array(used_stmt, nowstmt, gimple_assign_lhs(nowstmt), target, NULL);
								}
							}
						}
					}

					else if (gimple_code(nowstmt) == GIMPLE_CALL)
					{
						// 	debug_tree(getFunctionAssignVAR);
						// debug_gimple_stmt(nowstmt);
						if (gimple_call_lhs(nowstmt) || gimple_call_fndecl(nowstmt))
							if (!check_stmtStack(gimple_call_lhs(nowstmt)) && !check_stmtStack2(nowstmt))
							{
								if ((assign_array.assign_type_array)[i].form_tree != NULL)
								{

									set_gimple_array(used_stmt, nowstmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
								}
								else
									set_gimple_array(used_stmt, nowstmt, gimple_call_lhs(nowstmt), target, NULL);
								// debug_gimple_stmt(nowstmt);
								// debug_tree(gimple_call_arg(nowstmt, 0));
								// fprintf(stderr, "\n%d\n", gimple_call_num_args(nowstmt));
								int size = gimple_call_num_args(nowstmt);
								// gimple *nowstmt=nowstmt;
								for (int i = 0; i < size; i++)
								{
									if (!check_stmtStack(gimple_call_arg(nowstmt, i)))
										if (TREE_CODE(gimple_call_arg(nowstmt, i)) == SSA_NAME)
										{

											if (!check_stmtStack(gimple_call_arg(nowstmt, i)))
												if (gimple_call_arg(nowstmt, i) != target2)
													new_search_imm_use(used_stmt, gimple_call_arg(nowstmt, i), gimple_call_arg(nowstmt, i));
										}
										else if (TREE_CODE(gimple_call_arg(nowstmt, i)) == ADDR_EXPR)
										{
											tree second = TREE_OPERAND(gimple_call_arg(nowstmt, i), 0);
											// debug_tree(gimple_call_arg(nowstmt, i));
											if (TREE_CODE(second) == VAR_DECL)
											{
												// debug_gimple_stmt(nowstmt);
												// debug_tree(target);
												set_gimple_array(used_stmt, nowstmt, gimple_call_arg(nowstmt, i), target, NULL);
											}
										}
								}
							}
							else
							{
								if (!check_stmtStack2(nowstmt))
								{

									if (TREE_CODE(gimple_call_arg(nowstmt, 0)) == ADDR_EXPR)
										if (!check_stmtStack(gimple_call_arg(nowstmt, 0)))
										{

											set_gimple_array(used_stmt, nowstmt, gimple_call_arg(nowstmt, 0), target, NULL);
										}
									int size = gimple_call_num_args(nowstmt);
									for (int i = 0; i < size; i++)
									{
										if (!check_stmtStack(gimple_call_arg(nowstmt, i)))
										{

											if (TREE_CODE(gimple_call_arg(nowstmt, i)) == SSA_NAME)
											{

												if (!check_stmtStack(gimple_call_arg(nowstmt, i)))
													if (gimple_call_arg(nowstmt, i) != target2)
														new_search_imm_use(used_stmt, gimple_call_arg(nowstmt, i), gimple_call_arg(nowstmt, i));
											}
										}
									}
								}
							}
						// else{
						// 					fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
						// debug_tree(getFunctionAssignVAR);
						// 				debug_gimple_stmt(nowstmt);
						// }
					}
				}
			}
		}
	}
}
void Prenew_search_imm_use(gimple_array *used_stmt, tree target, tree target2)
{
	gimple *def_stmt = SSA_NAME_DEF_STMT(target);
	// debug_gimple_stmt(def_stmt);

	if (def_stmt)
		if (gimple_code(def_stmt) == GIMPLE_CALL)
		{
			if (gimple_call_lhs(def_stmt))
			{
				// fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
				if (gimple_call_lhs(def_stmt) && TREE_CODE(gimple_call_lhs(def_stmt)) == SSA_NAME)
				{
					if (gimple_assign_rhs1(def_stmt))
					{
						if (gimple_call_num_args(def_stmt) != 0)
						{
							for (int i = 0; i < gimple_call_num_args(def_stmt); i++)
							{
								// if (!check_stmtStack(gimple_call_arg(def_stmt, i)))
								// {
								if (TREE_CODE(gimple_call_arg(def_stmt, i)) == ADDR_EXPR || TREE_CODE(gimple_call_arg(def_stmt, i)) == VAR_DECL)
								{
									tree second;
									if (TREE_CODE(gimple_call_arg(def_stmt, i)) == ADDR_EXPR)
										second = TREE_OPERAND(gimple_call_arg(def_stmt, i), 0);
									else
										second = gimple_call_arg(def_stmt, i);

									tree getFunctionAssignVAR = second;

									function_assign_array assign_array;
									if (function_assign_collect->get(getFunctionAssignVAR) != NULL)
									{

										assign_array = ret_function_varstmt(getFunctionAssignVAR);

										for (int i = 0; i < assign_array.assign_type_array.size(); i++)
										{

											// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
											// fprintf(stderr, "------------------VAR_DECL : LHS------------------\n");
											if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
											{

												if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == MEM_REF)
												{
													// fprintf(stderr, "=======fist hit2========\n");
													// fprintf(stderr, "=======fist hit3=======\n");
													// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
													// debug_tree(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
													// debug(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
													// NULL_TREE
													if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
													// fprintf(stderr, "=======fist hit4=======\n");
													{
														if ((assign_array.assign_type_array)[i].form_tree != NULL)
														{

															// fprintf(stderr, "=======fist hit3=======\n");
															// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
															// debug_tree((assign_array.assign_type_array)[i].form_tree);
															set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
														}
														else
															set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
													}
												}
												else if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
												{
													if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
													{
														if ((assign_array.assign_type_array)[i].form_tree != NULL)
														{
															// fprintf(stderr, "=======fist hit2========\n");
															// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
															// debug_tree(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
															// debug_tree((assign_array.assign_type_array)[i].form_tree);
															// fprintf(stderr, "=======fist hit2========\n");
															// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
															set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
														}
														else
															set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
														// debug(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
														// used_stmt = used_stmt2;
														if (!check_stmtStack4((assign_array.assign_type_array)[i].stmt))
															if (gimple_assign_lhs((assign_array.assign_type_array)[i].stmt) != target2 && !check_stmtStack2((assign_array.assign_type_array)[i].stmt))
																new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
													}
													if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) && TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == COMPONENT_REF)
													{
														// fprintf(stderr, "============COMPONENT_REF==================\n");
														if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
														{
															tree second = TREE_OPERAND(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), 0);
															if (second)
															{
																if (TREE_CODE(second) == MEM_REF)
																{
																	tree three = TREE_OPERAND(second, 0);
																	if (three)
																	{

																		if (TREE_CODE(three) == SSA_NAME)
																		{
																			gimple *def_stmt = SSA_NAME_DEF_STMT(three);

																			if (is_gimple_assign(def_stmt))

																				if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
																				{

																					// if (!check_stmtStack3((assign_array.assign_type_array)[i].stmt))
																					// if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target2 && !check_stmtStack2((assign_array.assign_type_array)[i].stmt))
																					// if (!check_stmtStack3(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
																					if (gimple_assign_lhs(def_stmt) != target2)
																						new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
																					// }
																				}
																		}
																	}
																}
															}
														}
													}
													else if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) && TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == MEM_REF)
													{
														if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
														{
															tree second = TREE_OPERAND(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), 0);
															if (TREE_CODE(second) == MEM_REF)
															{
																gimple *def_stmt = SSA_NAME_DEF_STMT(second);

																if (is_gimple_assign(def_stmt))

																	if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
																	{

																		if (gimple_assign_lhs(def_stmt) != target2)
																			new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
																	}
															}
														}
													}
												}
												else if (TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
												{
													if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
													{
														// fprintf(stderr, "=======fist hit2========\n");
														// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
														if ((assign_array.assign_type_array)[i].form_tree != NULL)
														{
															// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
															// debug_tree(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
															// debug_tree((assign_array.assign_type_array)[i].form_tree);
															// fprintf(stderr, "=======fist hit2========\n");
															// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
															set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
														}
														else
															set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
														// debug(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
														// used_stmt = used_stmt2;
														// debug_tree(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
														// fprintf(stderr, "------------------VAR_DECL : LHS41------------------\n");
														if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target2)
														{
															// fprintf(stderr, "=======fist hit3========\n");
															// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
															// debug_tree(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
															// 	imm_use_iterator imm_iter2;
															// gimple *use_stmt2;
															// FOR_EACH_IMM_USE_STMT(use_stmt2, imm_iter, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt))
															// {
															// 	debug_gimple_stmt(use_stmt2);

															// }
															if (!check_stmtStack4((assign_array.assign_type_array)[i].stmt))
																if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target2 && !check_stmtStack2((assign_array.assign_type_array)[i].stmt))
																	new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
														}
													}
													if (gimple_assign_lhs((assign_array.assign_type_array)[i].stmt) && TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == COMPONENT_REF)
													{
														// fprintf(stderr, "============COMPONENT_REF==================\n");
														if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
														{
															tree second = TREE_OPERAND(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), 0);
															if (second)
															{
																if (TREE_CODE(second) == MEM_REF)
																{
																	tree three = TREE_OPERAND(second, 0);
																	if (three)
																	{

																		if (TREE_CODE(three) == SSA_NAME)
																		{
																			gimple *def_stmt = SSA_NAME_DEF_STMT(three);

																			if (is_gimple_assign(def_stmt))

																				if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
																				{

																					// if (!check_stmtStack3((assign_array.assign_type_array)[i].stmt))
																					// if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target2 && !check_stmtStack2((assign_array.assign_type_array)[i].stmt))
																					// if (!check_stmtStack3(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
																					if (gimple_assign_lhs(def_stmt) != target2)
																						new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
																					// }
																				}
																		}
																	}
																}
															}
														}
													}
													else if (gimple_assign_lhs((assign_array.assign_type_array)[i].stmt) && TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == MEM_REF)
													{
														if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
														{
															tree second = TREE_OPERAND(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), 0);
															if (TREE_CODE(second) == MEM_REF)
															{
																gimple *def_stmt = SSA_NAME_DEF_STMT(second);

																if (is_gimple_assign(def_stmt))

																	if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
																	{

																		if (gimple_assign_lhs(def_stmt) != target2)
																			new_search_imm_use(used_stmt, gimple_assign_lhs(def_stmt), gimple_assign_lhs(def_stmt));
																	}
															}
														}
													}
												}
											}

											else if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_CALL)
											{

												// tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
												// const char *name;
												// name = get_tree_code_name(TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)));
												// ADDR_EXPR
												// if (name != NULL)
												// {
												if (TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)) == ADDR_EXPR)
													if (!check_stmtStack(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)))
													{

														set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0), target, NULL);
													}
												// }
												// fprintf(stderr, "=======fist hit4========\n");
												// debug_tree(gimple_assign_lhs(use_stmt));
												// debug_tree(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0));.
												if (!check_stmtStack(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)))
												{
													if (TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)) == SSA_NAME)
													{
														// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
														if ((assign_array.assign_type_array)[i].form_tree != NULL)
														{

															set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
														}
														else
															set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0), target, NULL);
													}
												}
											}
										}
									}
								}
								else if (TREE_CODE(gimple_call_arg(def_stmt, i)) == SSA_NAME)
								{

									// debug_tree(gimple_call_arg(def_stmt, i));
									// debug_gimple_stmt(def_stmt);
									if (!check_stmtStack(gimple_call_arg(def_stmt, i)))

										new_search_imm_use(used_stmt, gimple_call_arg(def_stmt, i), gimple_call_arg(def_stmt, i));
								}
							}
						}
					}
				}
			}
		}
}

/*new_search_imm_use */
void new_search_imm_use(gimple_array *used_stmt, tree target, tree target2)
{
	imm_use_iterator imm_iter;
	use_operand_p use_p;
	gimple *use_stmt;
	gimple *gc;
	const char *name;
	gimple *def_stmt = SSA_NAME_DEF_STMT(target);
	int size = sizeof(used_stmt);
	int var_declsuperjump = 0;
	int has_single_use_flag = 0;

	levelsize += 1;

	// you can chang there max expan stmt
	// fprintf(stderr, "max expan %d   %d\n", levelsize, num_imm_uses(target));
	// if(levelsize <17)

	if (TREE_CODE(target) == SSA_NAME || TREE_CODE(target) == ADDR_EXPR)
	{
		const ssa_use_operand_t *const head = &(SSA_NAME_IMM_USE_NODE(target));
		if (TREE_CODE(target) == ADDR_EXPR)
		{
			fprintf(stderr, "GIMPLE CODE :var_declsuperjump-------\n");
			var_declsuperjump = 1;
			// debug_tree(target);
			goto has_single_use_addr_expr_jump;
		}
		if (USE_STMT(head->next))
			if (num_imm_uses(target) == 1)
			{
				has_single_use_flag = 1;
				if (target != target2)
				{

					use_stmt = USE_STMT(head->next);

					goto has_single_use_jump;
				}
			}

		if (levelsize > 1)
			if (target == now_tree)
			{
				// if (!check_stmtStack3(use_stmt))
				// goto has_single_use_jump2;
				// 	// return;
				return;
			}

		// need check tree target != null than compiler error
		if (num_imm_uses(target) && head->next != NULL)
			FOR_EACH_IMM_USE_FAST(use_p, imm_iter, target)
			{
				use_stmt = USE_STMT(use_p);
				if (!check_stmtStack4(use_stmt))
				{

					// debug_gimple_stmt(use_stmt);
					// if(gimple_code (use_stmt) == GIMPLE_LABEL){
					// 	fprintf(stderr, "--------GIMPLE LABEL -------\n");
					// 	debug_gimple_stmt(use_stmt);
					// }
					// if(target ==target2)
					// if (target2 == NULL)
					// {
					// debug_tree(target);
					// if(target == target2)
					// fprintf(stderr, "possible recivedebug module\n");
					// debug(use_stmt);
					// fprintf(stderr, "debug module2\n");
					// continue;
					// if (gimple_code(use_stmt) == GIMPLE_ASSIGN)
					// 	if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == VAR_DECL)
					// 	{
					// 		gimple *def_stmt = SSA_NAME_DEF_STMT(target2);
					// 		if (gimple_code(def_stmt) == GIMPLE_ASSIGN)
					// 		{
					// 			if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
					// 			{
					// 				fprintf(stderr, "debug mo\n");
					// 			}
					// 		}
					// 	}
					// continue;
					// }
					// gimple *def_stmt2 = SSA_NAME_DEF_STMT(target2);
					// if ( (target2 == gimple_assign_rhs1(use_stmt)))
					// 	continue;
					// if ( (target2 == gimple_assign_lhs(use_stmt)))
					// 	continue;
					// if(gimple_assign_lhs(def_stmt)==)
					// fprintf(stderr,"			FOR_EACH_IMM_USE_STMT INSIDE-------------------------------\n");

				has_single_use_jump:

					if (gimple_cond_code(use_stmt))
					{
						// debug_gimple_stmt(use_stmt);
						// fprintf(stderr, "--------GIMPLE Cond -------\n");
						if (!is_gimple_assign(use_stmt))
							if (gimple_cond_lhs(use_stmt))
							{
								// debug_gimple_stmt(use_stmt);
								// fprintf(stderr, "--------GIMPLE Cond -------\n");

								const gcond *gc = GIMPLE_CHECK2<const gcond *>(use_stmt);

								if (gimple_cond_rhs(gc) && gimple_cond_lhs(gc))
									if (!check_stmtStack(gimple_cond_rhs(gc)))
									{

										set_gimple_array(used_stmt, use_stmt, gimple_cond_lhs(gc), target, NULL);
									}
							}
					}

					if (gimple_code(use_stmt) == GIMPLE_RETURN)
					{
						// fprintf(stderr, "GIMPLE_RETURN\n");
						// fprintf(stderr, "------------------GIMPLE_RETURN : LHS------------------\n");
						tree return_tree = gimple_return_retval(as_a<greturn *>(use_stmt));
						if (!check_stmtStack(return_tree))
						{
							set_gimple_array(used_stmt, use_stmt, return_tree, return_tree, NULL);
						}
					}
					// add new rule in there warring need double check possiable loop
					else if (gimple_code(use_stmt) == GIMPLE_ASSIGN)
					{
						// fprintf(stderr, "GIMPLE ASSIGN\n");

						int ssa_name_assign = 0;
						tree gimpleassignlhs = prechecktree(gimple_assign_lhs(use_stmt));
						tree gimpleassignrhs = prechecktree(gimple_assign_rhs1(use_stmt));

						if (gimpleassignlhs && TREE_CODE(gimpleassignlhs) == SSA_NAME)
						{
							// fprintf(stderr, "-------always in therealways in therealways in there------%d--------------------\n", has_zero_uses (gimple_assign_lhs(use_stmt)));
							// debug_gimple_stmt(use_stmt);
							// debug_tree(gimple_assign_lhs(use_stmt));

							if (gimple_call_fn(use_stmt))
								if (!check_stmtStack(gimpleassignlhs) && !check_stmtStack(gimple_call_fn(use_stmt)))
								{
									// fprintf(stderr, "-------always in therealways in therealways in there----------------------------\n");
									// debug_tree(gimple_call_fn(use_stmt));
									set_gimple_array(used_stmt, use_stmt, gimpleassignlhs, target, NULL);
									ssa_name_assign = 1;

									if (gimpleassignlhs != target2 && !check_stmtStack2((use_stmt)))
										new_search_imm_use(used_stmt, gimpleassignlhs, gimpleassignlhs);

									Checknew_search_imm_use_lhs(used_stmt, use_stmt, target, target2);
								}
								else if (gimpleassignrhs && TREE_CODE(gimpleassignrhs) == SSA_NAME)
								{
									// fprintf(stderr, "--------GIMPLE goto -------\n");
									// debug_gimple_stmt(use_stmt);
									if (!check_stmtStack(gimpleassignrhs))
									{
										// fprintf(stderr, "------------------SSA_NAME : rhs ssaname------------------\n");
										// check_stmtStack2((use_stmt));
										// set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);
										if (gimpleassignrhs != target2 && !check_stmtStack2((use_stmt)))
										{

											new_search_imm_use(used_stmt, gimpleassignrhs, gimpleassignrhs);
										}
										Checknew_search_imm_use_rhs(used_stmt, use_stmt, target, target2);
									}
								}
						}
						else if (gimpleassignrhs && TREE_CODE(gimpleassignrhs) == SSA_NAME)
						{
							if (!check_stmtStack(gimpleassignrhs))
							{
								// fprintf(stderr, "------------------SSA_NAME : rhs ssaname------------------\n");

								set_gimple_array(used_stmt, use_stmt, gimpleassignrhs, target, NULL);

								if (gimpleassignrhs != target2)
								{

									new_search_imm_use(used_stmt, gimpleassignrhs, gimpleassignrhs);
								}
								Checknew_search_imm_use_rhs(used_stmt, use_stmt, target, target2);
							}
						}
						// if (!check_stmtStack3(use_stmt))
						// possiable rhs is vardecl
						if (!check_stmtStack(target))
						{
							// fprintf(stderr, "------------------SSA_NAME : rhs ssaname------------------\n");

							set_gimple_array(used_stmt, use_stmt, target, target2, NULL);

							// if (target != target2)
							// {
							// }
						}
						Varnew_search_imm_use(used_stmt, use_stmt, target, target2);

						if (gimpleassignlhs && TREE_CODE(gimpleassignlhs) == MEM_REF)
						{

							tree fundecl = TREE_OPERAND(gimpleassignlhs, 0);

							if (!check_stmtStack(gimpleassignlhs))
							{
								// fprintf(stderr, "ewwwwwwwwwwwwwwwwwwwwww");
								// debug_gimple_stmt(use_stmt);
								set_gimple_array(used_stmt, use_stmt, gimpleassignlhs, target, NULL);
							}
						}

						else if (gimpleassignlhs && TREE_CODE(gimpleassignlhs) == COMPONENT_REF)
						{
							// 	debug_gimple_stmt(use_stmt);
							if (TREE_CODE(gimpleassignrhs) == SSA_NAME)
							{
								// fprintf(stderr, "============COMPONENT_REF2==================\n");
								// debug_gimple_stmt(use_stmt);
								if (!check_stmtStack(gimpleassignrhs))
								{
									set_gimple_array(used_stmt, use_stmt, gimpleassignrhs, target, NULL);
									if (gimpleassignrhs != target2)
										new_search_imm_use(used_stmt, gimpleassignrhs, gimpleassignrhs);
								}
							}
						}

						else if (gimpleassignrhs && TREE_CODE(gimpleassignrhs) == COMPONENT_REF)
						{
							// fprintf(stderr, "============COMPONENT_REF2==================\n");
							// debug_gimple_stmt(use_stmt);
							// debug_gimple_stmt(use_stmt);
							if (TREE_CODE(gimpleassignlhs) == SSA_NAME)
							{
								// fprintf(stderr, "============COMPONENT_REF3==================\n");
								if (!check_stmtStack(gimpleassignlhs) || !check_stmtStack2((use_stmt)))
								{
									set_gimple_array(used_stmt, use_stmt, gimpleassignlhs, target, NULL);
									if (gimpleassignlhs != target2)
										new_search_imm_use(used_stmt, gimpleassignlhs, gimpleassignlhs);
								}
							}
						}
					}

					else if (gimple_code(use_stmt) == GIMPLE_PHI)
					{
						// debug_tree(gimple_assign_lhs(use_stmt));
						// debug_tree(gimple_phi_result(use_stmt));

						if (gimple_phi_result(use_stmt) && TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
						{
							if (TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
							{
								if (!check_stmtStack(gimple_phi_result(use_stmt)) && !check_stmtStack2(use_stmt))
								{
									set_gimple_array(used_stmt, use_stmt, gimple_phi_result(use_stmt), target, NULL);
									// debug_tree(gimple_phi_result(use_stmt));
									if (gimple_phi_result(use_stmt) != target2)
									{
										// if (has_zero_uses(gimple_phi_result(use_stmt)))
										//  const ssa_use_operand_t *const head = &(SSA_NAME_IMM_USE_NODE (gimple_phi_result(use_stmt)));
										//  if(head->next != NULL){
										// }
										new_search_imm_use(used_stmt, gimple_phi_result(use_stmt), gimple_phi_result(use_stmt));
									}
								}
							}
						}
					}
					else if (gimple_code(use_stmt) == GIMPLE_CALL)
					{
						// fprintf(stderr, "GIMPLE_CALL2\n");

						if (gimple_call_fn(use_stmt) == NULL)
							return;

						name = get_name(gimple_call_fn(use_stmt));
						if (name != NULL)
							if (!strcmp(name, "realloc"))
							{

								if (gimple_call_lhs(use_stmt))
									if (!check_stmtStack(gimple_call_lhs(use_stmt)))
									{

										set_gimple_array(used_stmt, use_stmt, gimple_call_lhs(use_stmt), target, NULL);

										if (TREE_CODE(gimple_call_lhs(use_stmt)) == SSA_NAME)
											new_search_imm_use(used_stmt, gimple_call_lhs(use_stmt), gimple_call_lhs(use_stmt));
									}
									else 
								 if (gimple_call_fn(use_stmt))
									{
										if (!check_stmtStack(gimple_call_arg(use_stmt,0)))
										{

											set_gimple_array(used_stmt, use_stmt,gimple_call_arg(use_stmt,0), target, NULL);

											if (TREE_CODE(gimple_call_arg(use_stmt,0)) == SSA_NAME)
												new_search_imm_use(used_stmt, gimple_call_arg(use_stmt,0), gimple_call_arg(use_stmt,0));
										}
									}
							}
							else if (!strcmp(name, "free") || !strcmp(name, "xfree"))
							{

								if (!check_stmtStack(gimple_call_fn(use_stmt)))
								{

									set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);
									if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
										new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
								}
							}

							else if (!strcmp(name, "pthread_create") || !strcmp(name, "pthread_join"))
							{

								if (!check_stmtStack(gimple_call_fn(use_stmt)))
								{
									// fprintf(stderr, "-----------------GIMPLE_CALL : precess pthread------------------\n");
									// debug(target);

								has_single_use_addr_expr_jump:
									tree second = NULL;
									int isVardecl = 0;

									if (var_declsuperjump == 1)
										if (TREE_CODE(target) == ADDR_EXPR)
										{
											second = TREE_OPERAND(target, 0);
											target = target;
											isVardecl = 1;
										}
									if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == ADDR_EXPR)
									{
										second = TREE_OPERAND(gimple_assign_rhs1(def_stmt), 0);
										second = TREE_OPERAND(second, 0);
										// debug_tree(second);
										isVardecl = 1;
									}
									else if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
									{
										second = gimple_assign_rhs1(def_stmt);
										// debug_tree(second);
										isVardecl = 1;
									}

									if (second != NULL)
										if (TREE_CODE(second) == VAR_DECL)
										{

											tree getFunctionAssignVAR = second;
											function_assign_array assign_array;
											// fprintf(stderr, "-----------------GIMPLE_CALL : VAR_DECL hit------------------\n");
											if (function_assign_collect->get(getFunctionAssignVAR) != NULL)
											{
												// debug(use_stmt);
												// fprintf(stderr, "=======fist hit========\n");
												assign_array = ret_function_varstmt(getFunctionAssignVAR);
												// fprintf(stderr, "=======print_function_var test %d========\n", assign_array.pass);
												// fprintf(stderr, "=======print_function_var %d   %d========\n", getFunctionAssignVAR, assign_array.assign_type_array.size());
												for (int i = 0; i < assign_array.assign_type_array.size(); i++)
												{

													if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
													{

														if (gimple_assign_lhs((assign_array.assign_type_array)[i].stmt) != target && TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
														{

															if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
															{

																if ((assign_array.assign_type_array)[i].form_tree != NULL)
																{

																	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
																}
																else
																	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
																// if (var_declsuperjump == 1)
																// 	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, target, target, NULL);

																new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
															}
														}
														else if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target && TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
														{

															if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
															{

																if ((assign_array.assign_type_array)[i].form_tree != NULL)
																{
																	// debug_tree((assign_array.assign_type_array)[i].form_tree);
																	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
																}
																else
																	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
																// if (var_declsuperjump == 1)
																// 	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, target, target, NULL);
																new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
															}
														}
													}

													else if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_CALL)
													{
														if (TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)) == ADDR_EXPR)
															if (!check_stmtStack(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)))
															{

																set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0), target, NULL);
															}
													}
												}
											}
										}

									// if (gimple_code(use_stmt) == GIMPLE_CALL)
									// 	name = get_name(gimple_call_fn(use_stmt));
									// 			!strcmp(name, "pthread_join")
									// debug_gimple_stmt(use_stmt);
									// debug_gimple_stmt(use_stmt);
									if (var_declsuperjump == 1)
									{
										return;
									}
									set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);
									if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
										new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
								}
							}
							else if (gimple_call_fn(use_stmt) && gimple_call_lhs(use_stmt) && TREE_CODE(gimple_call_lhs(use_stmt)) == SSA_NAME)
							{

								if (!check_stmtStack(gimple_call_lhs(use_stmt)))
								{
									set_gimple_array(used_stmt, use_stmt, gimple_call_lhs(use_stmt), target, NULL);

									if (gimple_call_lhs(use_stmt) != target2)
									{

										if (!check_stmtStack2(use_stmt))
										{

											new_search_imm_use(used_stmt, gimple_call_lhs(use_stmt), gimple_call_lhs(use_stmt));
										}
									}
									if (gimple_call_fn(use_stmt))
									{
										if (!check_stmtStack(gimple_call_fn(use_stmt)))
										{

											if (gimple_call_num_args(use_stmt) != 0)
											{
												for (int i = 0; i < gimple_call_num_args(use_stmt); i++)
												{

													if (gimple_call_arg(use_stmt, i))
														if (gimple_call_arg(use_stmt, i) != target2)
														{
															if (TREE_CODE(gimple_call_arg(use_stmt, i)) == SSA_NAME)
															{
																if (!check_stmtStack(gimple_call_arg(use_stmt, i)))
																{

																	if (gimple_call_arg(use_stmt, i) != target2)
																		new_search_imm_use(used_stmt, gimple_call_arg(use_stmt, i), gimple_call_arg(use_stmt, i));
																}
															}
														}
												}
											}
										}
									}
								}
							}
							else
							{
								if (!check_stmtStack(gimple_call_fn(use_stmt)))
								{
									// debug_gimple_stmt(use_stmt);
									// fprintf(stderr, "-------Untreated-------------------------\n");
									set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);

									if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
										new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
								}
							}
					}
				}
			}
	}
}
