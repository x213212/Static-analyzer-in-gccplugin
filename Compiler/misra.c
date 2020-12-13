
#include "gcc-plugin.h"
#include "plugin-version.h"
#include <coretypes.h>

#include "tm.h"
#include <tree.h>
#include <tree-pass.h>
#include <tree-core.h>
#include <print-tree.h>
#include <tree-ssa-alias.h>
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-inline.h"

#include "cgraph.h"
#include "gimple-walk.h"
#include "gimple-pretty-print.h"
#include "gimple-ssa.h"
#include "context.h"
#include "tree-dfa.h"
#include "attribs.h"

#include "config.h"
#include "system.h"
#include <coretypes.h>
#include "insn-constants.h"
#include "options.h"
#include "backend.h" 
#include "rtl.h"
#include "alloc-pool.h"
#include "ssa.h"
#include "diagnostic-core.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "stmt.h"
#include "ssa-iterators.h"
//#include "DFS.h"
#include "mla.h"
#include "tree-pass.h"
#include "hash-map.h"
//#include "ipa-fnsummary.h"

int plugin_is_GPL_compatible;
extern gcc::pass_manager *PASS;

struct freetable{
	tree target;
	freetable *next;
};

hash_map<function *, freetable > *fn_free;


extern void *objc_get_current_scope (void);
void cgraph_call_travel(gimple *gc,cgraph_node *node);

void hash_initialize(void);
unsigned int hash_value_compute(char *key);
void hash_search_func(char *key,int value,int level,location_t locus);
void alloc_detect(function* fun);
void print_pointer_table_new(ptb *ptable);
//void new_double_free_analysis(ptb *ptable, ptb *retable, ptb *ftable, ptb *phitable,Graph g);

void set_ptb(basic_block b,ptb *table, tree t, location_t l, int s, gimple *stmt,cgraph_node *node);
void set_ft(tree t,freetable* ft);
void check_memory_state(function* fun);
struct register_pass_info my_passinfo;
struct register_pass_info my_passinfo2;
struct register_pass_info my_passinfo3;
struct register_pass_info my_passinfo4;
struct register_pass_info my_passinfo5;
struct register_pass_info my_passinfo6;
struct register_pass_info my_passinfo7;

const pass_data my_pass_data =
{
    .type = SIMPLE_IPA_PASS,
    .name = "my_pass",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required = PROP_ssa,
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0
};
const pass_data my_pass_data2 =
{
    .type = GIMPLE_PASS,
    .name = "my_pass2",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required =( PROP_cfg | PROP_ssa ),
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0
};

const pass_data my_pass_data3 =
{
  IPA_PASS, /* type */
  "mypass3", /* name */
  OPTGROUP_INLINE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  ( TODO_dump_symtab ), /* todo_flags_finish */
};
const pass_data my_pass_data4 =
{
    .type = SIMPLE_IPA_PASS,
    .name = "my_pass4",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required =( PROP_cfg | PROP_ssa ),
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0
};
const pass_data my_pass_data5 =
{
    .type = GIMPLE_PASS,
    .name = "my_pass5",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required = 0,
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0
};
const pass_data my_pass_data6 =
{
    .type = SIMPLE_IPA_PASS,//SIMPLE_IPA_PASS,GIMPLE_PASS
    .name = "my_pass6",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required = PROP_ssa,
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0
};

const pass_data my_pass_data7 = 
{
    .type = GIMPLE_PASS,
    .name = "my_pass7",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_IPA_CONSTANT_PROP,
    .properties_required = 0,
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0
};
//gcc 4.8.0 after use class create pass
void set_ft(tree t,freetable* table){
	if(&table->target == NULL){
		table->target = t;
		table->next = NULL;
	}  
	else{
		while(table->next!=NULL){
			table=table->next;
		}
		table->next = new freetable();
		table = table->next;
		table->target = t;
		table->next = NULL;
	}
}

void call_graph(tree fndecl){
	struct function *my_function = DECL_STRUCT_FUNCTION (fndecl);
	basic_block bb;
	/*
	FOR_EACH_BB_FN (bb, my_function)
	{
		for (gimple_stmt_iterator bsi = gsi_start_bb (bb); !gsi_end_p (bsi); gsi_next (&bsi)){
			gimple *stmt = gsi_stmt (bsi);
			debug_gimple_stmt(stmt);
		}
	}*/
	FOR_EACH_BB_FN (bb, my_function)
	{
		for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			gimple *stmt = gsi_stmt (gsi);
			debug_gimple_stmt(stmt);
			if(is_gimple_call(stmt)){
				if(!gimple_call_builtin_p(stmt)){
					tree fndecl=gimple_call_fndecl(stmt);
					debug_tree(fndecl);
					//function *ffun=DECL_STRUCT_FUNCTION (fndecl);
					//call_graph(ffun);
					//allocate_struct_function (fndecl,0);
				}
			}
			//gimple *gc=a->call_stmt;
			//tree type=gimple_call_fndecl(gc);
			//debug_gimple_stmt(a->redirect_call_stmt_to_callee());
		}
	}
}
void search_ptr_use1(gimple_array *used_stmt, tree target){
  imm_use_iterator imm_iter;
  gimple *use_stmt;
  FOR_EACH_IMM_USE_STMT (use_stmt, imm_iter, target)
  {
      debug_gimple_stmt(use_stmt);
      //set_gimple_array(used_stmt, use_stmt);

      if(gimple_code(use_stmt) == GIMPLE_ASSIGN){
      if(gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt))==SSA_NAME && gimple_assign_single_p(use_stmt) && gimple_assign_rhs1(use_stmt)==target){
        search_ptr_use1(used_stmt, gimple_assign_lhs(use_stmt));
      }}

  }
}
void collect_malloc(gimple *gc,cgraph_node *node,basic_block bb,freetable* ft){
	//debug_gimple_stmt(gc);
	location_t loc = gimple_location(gc);
	//fprintf(stderr, "functionname %s\n",get_name(gimple_call_fn(gc)));
	if (!strcmp(get_name(gimple_call_fn(gc)),"free")){
		//fprintf(stderr, "free %s\n",get_name(gimple_call_fn(gc)));
		//DECL_STRUCT_FUNCTION(gimple_call_arg(gc, 0))=ofun;
		set_ptb(bb, ftable, gimple_call_arg(gc, 0), loc, 0, gc,node);
		//DECL_STRUCT_FUNCTION(gimple_call_arg(gc, 0))=cfun;
		set_ft(gimple_call_arg(gc, 0),ft);
		struct ptr_info_def *pi=SSA_NAME_PTR_INFO (gimple_call_arg(gc, 0));
		debug(&pi->pt);
		fprintf(stderr, "\n");
		//if (gimple_code (SSA_NAME_DEF_STMT (gimple_call_arg(stmt, 0))) == GIMPLE_PHI)
		//fprintf(stderr, "free phi\n");
	}else if(!strcmp(get_name(gimple_call_fn(gc)),"malloc"))
	{		
		fprintf(stderr, "malloc %s\n",get_name(gimple_call_fn(gc)));	
		//DECL_STRUCT_FUNCTION(gimple_call_lhs(gc))=ofun;
		set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc,node);
		//DECL_STRUCT_FUNCTION(gimple_call_lhs(gc))=cfun;
		struct ptr_info_def *pi=SSA_NAME_PTR_INFO (gimple_call_lhs(gc));
		debug(&pi->pt);
		fprintf(stderr, "\n");
	}else if(!strcmp(get_name(gimple_call_fn(gc)),"xmalloc"))
	{		
		fprintf(stderr, "!xmalloc %s\n",get_name(gimple_call_fn(gc)));	
		//DECL_STRUCT_FUNCTION(gimple_call_lhs(gc))=ofun;
		set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc,node);
		//DECL_STRUCT_FUNCTION(gimple_call_lhs(gc))=cfun;
		struct ptr_info_def *pi=SSA_NAME_PTR_INFO (gimple_call_lhs(gc));
		debug(&pi->pt);
		fprintf(stderr, "\n");
	}else if(gimple_code(gc) == GIMPLE_RETURN){
		fprintf(stderr, "return \n");
	}else{
		fprintf(stderr, "else \n");
	}
}

/*pass 6*/

/*
							tree a=gimple_call_fn(gc);
							fnode->put(a,node);
							
							var_points_to vpt;
							//var_points_to *vpt_p;
							//vpt_p=&vpt;
							
							vector<vector<pair<fdecl,location_t>>> may_malloc_path;
							vpt.may_malloc_path=may_malloc_path;

							tvpt->put(gimple_call_lhs (gc),vpt);
							fprintf(stderr,"vpt_p: %d gimple_call_fn :%d \n",vpt,a);
							if(a!=NULL){
								node2=*(fnode->get(a));
								fprintf(stderr,"node before :%d after: %d \n",node->decl,node2->decl);
							}
							//debug_tree(DECL_STRUCT_FUNCTION(gimple_call_fntype(gc))->decl);
*/
void collect_malloc(gimple *gc,cgraph_node *node,basic_block bb){
	//debug_gimple_stmt(gc);
	location_t loc = gimple_location(gc);
	tree a;
	cgraph_node* node2;
	const char *name;
	if(gimple_call_fn(gc)==NULL)
		return;
	name=get_name(gimple_call_fn(gc));
	if(name==NULL)
		return;
	fprintf(stderr, "functionname %s\n",name);
	if (!strcmp(name,"free") || !strcmp(name,"xfree")){
		//fprintf(stderr, "free %s\n",get_name(gimple_call_fn(gc)));
		//DECL_STRUCT_FUNCTION(gimple_call_arg(gc, 0))=ofun;
		set_ptb(bb, ftable, gimple_call_arg(gc, 0), loc, 0, gc,node);
		//DECL_STRUCT_FUNCTION(gimple_call_arg(gc, 0))=cfun;
		
		struct ptr_info_def *pi=SSA_NAME_PTR_INFO (gimple_call_arg(gc, 0));
		//debug(&pi->pt);
		fprintf(stderr, "gimple_call_arg:");
		
		debug_tree(gimple_call_arg(gc, 0));


		//if (gimple_code (SSA_NAME_DEF_STMT (gimple_call_arg(stmt, 0))) == GIMPLE_PHI)
		//fprintf(stderr, "free phi\n");
	}else if(!strcmp(name,"malloc") || !strcmp(get_name(gimple_call_fn(gc)),"calloc")|| !strcmp(name,"xmalloc")|| !strcmp(name,"strdup"))
	{		
		fprintf(stderr, "malloc %s\n",get_name(gimple_call_fn(gc)));	
		//DECL_STRUCT_FUNCTION(gimple_call_lhs(gc))=ofun;
		set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc,node);
		//if(DECL_STRUCT_FUNCTION(gimple_call_fn(gc))==NULL)
		struct ptr_info_def *pi=SSA_NAME_PTR_INFO (gimple_call_lhs(gc));
		debug(&pi->pt);
		fprintf(stderr, "\n");
	}else if(gimple_code(gc) == GIMPLE_RETURN){
		fprintf(stderr, "return \n");
	}else{
		fprintf(stderr, "else \n");
	}


	a=gimple_call_fn(gc);
	fprintf(stderr, "gimple_call_fn: put(a): %d ",a);
	debug_tree(a);
	fnode->put(a,node);
	
	var_points_to vpt;
	vector<vector<pair<fdecl,location_t>>> may_malloc_path;
	vpt.may_malloc_path=may_malloc_path;
	tvpt->put(gimple_call_lhs (gc),vpt);
	fprintf(stderr,"vpt_p: %d gimple_call_fn :%d \n",vpt,a);
	if(a!=NULL){
		node2=*(fnode->get(a));
		fprintf(stderr,"node before :%d after: %d \n",node->decl,node2->decl);
	}
}
void cgraph_call_travel(gimple *gc,cgraph_node *node){
	basic_block bb;
	cgraph_edge *e=node->get_edge(gc);
	function *testfun=e->callee->get_fun();

	fprintf(stderr,"=======fun:%s=========\n",get_name(testfun->decl));
	FOR_EACH_BB_FN (bb, testfun){
		for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			gimple *stmt = gsi_stmt (gsi);
			debug_gimple_stmt(stmt);
			//fprintf(stderr, "cgraph_call_travel \n");
			if (is_gimple_call(stmt)){
				//collect_malloc(stmt,node,bb,ft);
			}
		}
	}
	fprintf(stderr,"=======fun_end=========\n",get_name(testfun->decl));


}
struct my_pass: gimple_opt_pass
{	 
public:
    
    my_pass(): gimple_opt_pass(my_pass_data, g) {}

    virtual unsigned int execute(function* fun) override
    {
	fprintf(stderr,"=======pass:%s=========1\n",my_passinfo.reference_pass_name);
    	basic_block bb;
    	gimple *stmt;
    	gimple_stmt_iterator gsi;
    	
    	/*FOR_EACH_BB_FN(bb,fun)
      		for (gsi=gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
         		
      		}
	*/ 
	//alloc_detect(fun);
        return 0;
    }

    virtual my_pass* clone() override { return this; }
};

void DFA(gimple *gc,cgraph_node *node,basic_block bb){
	
}
struct my_pass4: gimple_opt_pass
{	 
public:
    
    my_pass4(): gimple_opt_pass(my_pass_data4, g) {}

    virtual unsigned int execute(function* fun) override
	{
		fprintf(stderr,"=======pass:%s=========4\n",my_passinfo4.reference_pass_name);
		basic_block bb;
		gimple *stmt;
		gimple_stmt_iterator gsi;
		cgraph_node *node,*mnode;
		function* main;
		function *f;
		fn_free = new hash_map<function *, freetable >;
		FOR_EACH_DEFINED_FUNCTION (node)
		{	
			freetable ft;
			ft.target=NULL;
			ft.next=NULL;
			node->get_body();
			push_cfun(node->get_fun());
			calculate_dominance_info(CDI_DOMINATORS);
			if(strcmp(get_name(cfun->decl),"main")==0){
				main=cfun;
				mnode=node;
			}
			//fn_free->put(&cfun,ft);
			//calculate_dominance_info(CDI_DOMINATORS);
			fprintf(stderr,"=======node_fun:%s=========\n",get_name(cfun->decl));
		
		

			fprintf(stderr,"=======debug_gimple_stmt(gfn)=========\n");
			gimple *gfn=cfun->gimple_body;
			debug_gimple_stmt(gfn);	
			fprintf(stderr,"=======debug_gimple_stmt(gfn)end =========\n");/*
			unsigned int arg_num=gimple_call_num_args(gfn);
			tree arg_tree;
			for(unsigned int num=0;num<arg_num;num++){
				arg_tree=gimple_call_arg(gfn,num);
				debug_tree(arg_tree);
			}*/
			FOR_EACH_BB_FN (bb, cfun)
			{
			 	/*每個BB的後一個BB的index*/
				fprintf(stderr, "now := %d\n",bb->index);
				if(bb != cfun->cfg->x_exit_block_ptr->prev_bb){
					edge e;
					edge_iterator ei;
					FOR_EACH_EDGE (e, ei, bb->succs)
					{
						//gp.addEdge(bb->index-2,e->dest->index-2);
					}
				}
				if(bb != cfun->cfg->x_entry_block_ptr->next_bb){
					fprintf(stderr, "pred := ");
					edge e;
					edge_iterator ei;
					FOR_EACH_EDGE (e, ei, bb->preds)
					{
						fprintf(stderr, " %d ", e->src->index );
					}
					fprintf(stderr, "\n");
				}
				/*每個BB的後一個BB的index*/
				if(bb != cfun->cfg->x_exit_block_ptr->prev_bb){//
					//if(EDGE_COUNT(bb->succs)>1)
						//branch_counter = branch_counter-1+EDGE_COUNT(bb->succs);
					fprintf(stderr, "succ := "); 
					edge e;
					edge_iterator ei;
					FOR_EACH_EDGE (e, ei, bb->succs)
					{
						fprintf(stderr, " %d ", e->dest->index);
						fprintf(stderr,"(%d->%d)",bb->index-2,e->dest->index-2);				
					}
				}
				for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
				{
								
					gimple *gc = gsi_stmt (gsi);
					debug_gimple_stmt(gc);	
					if (is_gimple_call(gc)){
						collect_malloc(gc,node,bb,&ft);
					}
				}
				DECL_STRUCT_FUNCTION(cfun->decl)=cfun;
				fprintf(stderr, "\n========\n");
			
			}
			pop_cfun();
		}
		node=mnode;
		for (cgraph_edge *e = node->callees; e; e = e->next_callee){
			fprintf(stderr,"=======callees=========\n");
			//debug_head(e->callee->get_fun()->decl);
			gimple *ga=e->call_stmt;
			debug_gimple_stmt(ga);
			tree a=gimple_call_fndecl(ga);
			debug_head(a);
			f=DECL_STRUCT_FUNCTION(a);
			fprintf(stderr,"function_name :%s",function_name (f));
			debug_head(a);
			if(f!=NULL){
				debug_tree(f->decl);
			}
			fprintf(stderr,"=======call_graph=========\n");
			//call_graph(e->callee->get_fun()->decl);
			fprintf(stderr,"=======call_graph end=========\n");
			fprintf(stderr,"=======callees end=========\n");
		}
		print_pointer_table_new(ftable);
		new_double_free_analysis(ptable,retable,ftable,phitable);

		//new_use_after_free_analysis(ptable, ftable);

		//new_memory_leak_analysis (ptable,ftable);
        return 0;
    }

    virtual my_pass4* clone() override { return this; }
};
class my_pass5: public gimple_opt_pass
{
public:
    my_pass5(): gimple_opt_pass(my_pass_data5, g) {}

    virtual unsigned int execute(function*) override
    {
			cgraph_node* node;
			FOR_EACH_DEFINED_FUNCTION (node)
			{
				basic_block bb;
				cgraph_edge* e;
				//fprintf(stderr,"=======inline=========\n");
				//fprintf(stderr,"=======node_fun:%s %d=========\n",get_name(node->decl),node->decl);
				//fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (node->decl));

				//fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (node->decl)));
				//node->debug();
				
				tree attr ;
  				enum availability avail;
				for (e = node->callees; e; e = e->next_callee)
    			{
  					cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
					cgraph_node *callee = e->callee->ultimate_alias_target (&avail, caller);
					tree callee_tree = callee ? DECL_FUNCTION_SPECIFIC_OPTIMIZATION (callee->decl) : NULL;
					//DECL_DISREGARD_INLINE_LIMITS (callee->decl)=1;
					if (DECL_ATTRIBUTES (callee->decl)!=NULL)
						attr = get_attribute_name (DECL_ATTRIBUTES (callee->decl));
					else{
						//if(callee->decl)
						//DECL_ATTRIBUTES (callee->decl) = tree_cons (get_identifier ("always_inline"),
                                        //NULL, DECL_ATTRIBUTES (callee->decl));
					}
					//debug_tree(attr);
					//fprintf(stderr,"=======address:%d=========\n",callee->decl);
				}
				
				
				node->debug();
				push_cfun(DECL_STRUCT_FUNCTION (node->decl));
				if(cfun==NULL)
					//fprintf(stderr,"=======NULL=========\n");
				FOR_EACH_BB_FN (bb, cfun){
					
					for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
					{
						gimple *gc = gsi_stmt (gsi);
						//debug_gimple_stmt(gc);
						
						if (is_gimple_call(gc)){

							if(gimple_call_fn(gc)==NULL)
								continue;
							name= get_name (gimple_call_fn(gc));
							if(name==NULL)
								continue;
							//fprintf(stderr, "functionname %s\n",name);
							if(!strcmp(name,"free") ||
								!strcmp(name,"xfree") ||
								!strcmp(name,"malloc") || 
								!strcmp(name,"xmalloc")||
								!strcmp(name,"calloc")||
								!strcmp(name,"xcalloc")||
								!strcmp(name,"strdup") ){
							DECL_ATTRIBUTES (node->decl) = tree_cons (get_identifier ("always_inline"),NULL, DECL_ATTRIBUTES (node->decl));
							DECL_DISREGARD_INLINE_LIMITS (node->decl)=1;
							//fprintf(stderr,"=======add_always=========\n");
							}
							
						}
					
					}
				}
				pop_cfun();
			}
			//fprintf(stderr,"=======end=========\n");
			return false;
    }
    virtual my_pass5* clone() override { return new my_pass5(); }
};

struct my_pass2: opt_pass
{	 
public:
    
    my_pass2(): opt_pass(my_pass_data2, g) {}

    virtual unsigned int execute(function* fun) override
    {	
	tree tt;
	fprintf(stderr,"%s %s\n",my_passinfo2.reference_pass_name,get_name(fun->decl));
	debug_head(fun->decl);
	struct cgraph_node *node;
    	basic_block bb,testbb;
    	gimple *stmt;
    	gimple_stmt_iterator gsi;
    	//check_memory_state(fun);
	//alloc_detect(fun);
	FOR_EACH_BB_FN (bb, fun){
	for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			
			gimple *stmt = gsi_stmt (gsi);
			debug_gimple_stmt(stmt);
			//debug_tree(gimple_get_lhs(stmt));
			location_t loc = gimple_location(stmt);
			//fprintf(stderr,"gimple_bb\n");
			//debug_tree(gimple_get_lhs(stmt));
			
			if (is_gimple_call(stmt)){
			/*FREE分析*/
				if (gimple_call_builtin_p (stmt, BUILT_IN_FREE)){
					set_ptb(bb, ftable, gimple_call_arg(stmt, 0), loc, 0, stmt);
					//if (gimple_code (SSA_NAME_DEF_STMT (gimple_call_arg(stmt, 0))) == GIMPLE_PHI)
						//fprintf(stderr, "free phi\n");
					continue;
				}
				if (gimple_call_builtin_p (stmt, BUILT_IN_MALLOC)|| gimple_call_builtin_p (stmt, BUILT_IN_CALLOC)){
					set_ptb(bb, ptable, gimple_call_lhs(stmt), loc, 0, stmt);
					fprintf(stderr, "===malloc===\n");
					//debug_tree(gimple_call_lhs(stmt));
					continue;
				}
				if (strcmp(get_name(gimple_call_fndecl(stmt)),"fopen")==0){
					//set_ptb(bb, ptable, gimple_call_lhs(stmt), loc, 0, stmt);
					debug_tree(gimple_call_lhs(stmt));
					continue;
				}
				/*
				if(){
					bitmap_move()
				}*/
				tree fndecl=gimple_call_fntype(stmt);
				debug_tree(fndecl);
				cgraph_node *fn_node = cgraph_node::get (fndecl);
				if (fn_node != NULL){
					fprintf(stderr,"test\n");
					function *testfun=fn_node->get_fun();
					tree testdecl=fun->decl;
					debug_tree(testdecl);
					
				}
			}
		}
	}
	ptb *temp=ftable;
	ptb *temp2=ptable;
	if(/*strcmp(get_name(fun->decl),"main")==0*/1){
		
		fprintf(stderr,"=====malloc=====\n");
		FOR_EACH_TABLE(temp2,tt){
			struct ptr_info_def *pi=SSA_NAME_PTR_INFO (tt);
			struct pt_solution *ppt=&pi->pt;


			//bitmap_set_bit(ppt->vars,9487);
			//bitmap_clear_bit(ppt->vars,4041);
			gimple_array *used_stmt;
			gimple_array gstart;
	    gstart.stmt = NULL;
	    used_stmt = &gstart;
			//search_ptr_use1(used_stmt,temp2->target);
			
			debug_head(temp2->target);
			//debug_tree(temp2->target);
			//ppt->escaped=1;
			debug(&pi->pt);
			fprintf(stderr,"\n ");
			if(temp2->next!=NULL){
				debug_head(temp2->next->target);
				pi=SSA_NAME_PTR_INFO (temp2->next->target);
				ppt=&pi->pt;
				debug(&pi->pt);
				tree oper=temp2->target->ssa_name.var;
				//debug_head(oper);
				fprintf(stderr,"\n ");
				//if(dominated_by_p(CDI_DOMINATORS,temp2->bb,temp2->next->bb))
					//fprintf(stderr,"BB:%d~%d\n",temp2->bb->index,temp2->next->bb->index);
			}
		}
		fprintf(stderr,"=====free=====\n");
		FOR_EACH_TABLE(temp,tt){
			
			struct ptr_info_def *pi=SSA_NAME_PTR_INFO (tt);
			struct pt_solution *ppt=&pi->pt;
			debug_head(tt);
			//debug_tree( temp->target);
			//ppt->escaped=1;
			debug(&pi->pt);
			fprintf(stderr,"\n");
			if(temp->next!=NULL){
				debug_head(temp->next->target);
				pi=SSA_NAME_PTR_INFO (temp->next->target);
				ppt=&pi->pt;
				debug(&pi->pt);
				fprintf(stderr,"\n");
				if(ptr_derefs_may_alias_p(temp->target,temp->next->target))
					fprintf(stderr,"alias\n");
			}
			fprintf(stderr,"=====END=====\n");
/*
			imm_use_iterator imm_iter;
  		gimple *use_stmt;
  		FOR_EACH_IMM_USE_STMT (use_stmt, imm_iter, tt){
					debug_gimple_stmt(use_stmt);
			}
*/

		}
	}
	fprintf(stderr,"=====end=====\n");
        return 0;
    }

    virtual my_pass2* clone() override { return this; }
};
void finish_decl(void* event, void* __unused__){
	//alloc_detect(cfun);
	tree decl = (tree) event;
	printf("%-16s in %s line %-4d column %-4d ", get_tree_code_name(TREE_CODE(decl)), DECL_SOURCE_FILE (decl), DECL_SOURCE_LINE (decl), DECL_SOURCE_COLUMN (decl));
    	printf("name: %s \n", DECL_NAME(decl) != NULL_TREE ? IDENTIFIER_POINTER(DECL_NAME(decl)) : "<unamed>");

}

static int test_always(void){
			cgraph_node* node;
			cgraph_edge* e;
			FOR_EACH_DEFINED_FUNCTION (node)
			{
				basic_block bb;
				fprintf(stderr,"=======inline=========\n");
				//push_cfun(node->get_fun());
				fprintf(stderr,"=======node_fun:%s=========\n",get_name(node->decl));
				fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (node->decl)));
				node->debug();
				push_cfun(DECL_STRUCT_FUNCTION (node->decl));
				if(cfun==NULL)
					fprintf(stderr,"=======NULL=========\n");
				
  				enum availability avail;
				for (e = node->callees; e; e = e->next_callee)
    			{
  					cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
					cgraph_node *callee = e->callee->ultimate_alias_target (&avail, caller);
					tree callee_tree = callee ? DECL_FUNCTION_SPECIFIC_OPTIMIZATION (callee->decl) : NULL;
					DECL_DISREGARD_INLINE_LIMITS (callee->decl)=1;
					
					tree attr = get_attribute_name (DECL_ATTRIBUTES (callee->decl));
					IDENTIFIER_POINTER (attr);
				}
				pop_cfun();
				
			}
			return 0;
}
namespace {
class pass_ipa_always : public ipa_opt_pass_d
{
public:
  pass_ipa_always (gcc::context *ctxt)
    : ipa_opt_pass_d (my_pass_data3, ctxt,
                      NULL, /* generate_summary */
                      NULL, /* write_summary */
                      NULL, /* read_summary */
                      NULL, /* write_optimization_summary */
                      NULL, /* read_optimization_summary */
                      NULL, /* stmt_fixup */
                      0, /* function_transform_todo_flags_start */
                      0, /* function_transform */
                      NULL) /* variable_transform */
  {}
  /* opt_pass methods: */
  virtual unsigned int execute (function *) { return test_always (); }
  virtual pass_ipa_always* clone() override { return this; }
}; // class pass_ipa_inline
}
ipa_opt_pass_d *
make_pass_ipa_always (gcc::context *ctxt)
{
  return new pass_ipa_always (ctxt);
}

/*
struct my_pass3: opt_pass
{	 
public:
    
    my_pass3(): opt_pass(my_pass_data3, g) {}

    virtual unsigned int execute(function* ) override
    {	
			cgraph_node* node;
			FOR_EACH_DEFINED_FUNCTION (node)
			{
				
				fprintf(stderr,"=======inline=========\n");
				//ofun=node->get_fun();
				push_cfun(node->get_fun());
				if (cfun==NULL)
					break;

				if(strcmp(get_name(cfun->decl),"main")==0)
					ffun=cfun;
				//if(strcmp(get_name(ofun->decl),"main")==0){
				fprintf(stderr,"=======node_fun:%s=========\n",get_name(cfun->decl));
				fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (get_attribute_name(cfun->decl)));
				node->debug();
			}
    }

    virtual my_pass3* clone() override { return this; }
};
*/
struct my_pass6: opt_pass
{	 
public:
    
    my_pass6(): opt_pass(my_pass_data6, g) {}

    virtual unsigned int execute(function *test) override
    {	
			
			fprintf(stderr,"=======pass:%s=========\n",my_passinfo6.reference_pass_name);
			//fprintf (stderr, "now:%s\n",get_name(fun->decl));
			struct cgraph_node *node;
			struct var_points_to vpt;
			tree ptr;
			unsigned i;
			function *ofun;
			function *fn;
			basic_block bb;
			ipa=true;
			/*
			FOR_EACH_DEFINED_FUNCTION (node)
			{
				
				push_cfun(node->get_fun());
				if(cfun==NULL)
					continue;
				if(strstr(get_name(cfun->decl),"realloc")!=NULL){
					pop_cfun();
					continue;
				}
				if(MAIN_NAME_P(DECL_NAME (node->decl)))
					fprintf(stderr,"=======main=========\n");
				fprintf(stderr,"=======function:%s=========\n",get_name(cfun->decl));
				debug_head(cfun->decl);
				//fprintf(stderr,"=======node:%s=========\n",get_name(cfun->decl));
				//calculate_dominance_info(CDI_DOMINATORS);
				//calculate_dominance_info(CDI_POST_DOMINATORS);
				alloc_detect();
				//free_dominance_info (CDI_POST_DOMINATORS);
				pop_cfun();
			}*/
			tvpt = new hash_map<tree,var_points_to >;
			fDFS = new hash_map<cgraph_node *, Graph >;
			fnode=new hash_map<tree, cgraph_node *>;
			init_table();
			/*
			FOR_EACH_DEFINED_FUNCTION (node)
			{
				push_cfun(node->get_fun());
				 // Compute the points-to sets for pointer SSA_NAMEs.  
				FOR_EACH_VEC_ELT (*cfun->gimple_df->ssa_names, i, ptr)
				{
					if (ptr	&& POINTER_TYPE_P (TREE_TYPE (ptr))){
						debug_head(ptr);
						fprintf(stderr,"ptr address %d \n",ptr);
					}
				}
				//Graph DFS;
				//DFS.init_Graph(cfun->cfg->x_last_basic_block);

				fprintf(stderr,"=======node_fun:%s %d =========\n",get_name(cfun->decl),cfun->decl->function_decl.f->decl);
				debug_tree(cfun->decl->function_decl.f->decl);
				cfun->decl->function_decl.f->decl;
				for (cgraph_edge *e = node->callers; e; e = e->next_callee){
					fprintf(stderr,"=======callees=========\n");
					debug_head(e->caller->get_fun()->decl);
					gimple *ga=e->call_stmt;
					debug_gimple_stmt(ga);
					
				}
				FOR_EACH_BB_FN (bb, cfun)
				{

					calculate_dominance_info(CDI_DOMINATORS);
					fprintf(stderr, "now := %d\n",bb->index);
					fprintf(stderr, "dfs_num_in  := %d\n",bb->dom[0]->dfs_num_in);
					fprintf(stderr, "dfs_num_out := %d\n",bb->dom[0]->dfs_num_out);
					fprintf(stderr, "loop father := %d\n",bb->loop_father->header->index);
					

					if(bb != cfun->cfg->x_exit_block_ptr->prev_bb){
						edge e;
						edge_iterator ei;
						FOR_EACH_EDGE (e, ei, bb->succs)
						{
							//DFS.addEdge(bb->index,e->dest->index);
				
						}
					}
					if(bb != cfun->cfg->x_entry_block_ptr->next_bb){
						fprintf(stderr, "pred := ");
						edge e;
						edge_iterator ei;
						FOR_EACH_EDGE (e, ei, bb->preds)
						{
							fprintf(stderr, " %d ", e->src->index );
						}
						fprintf(stderr, "\n");
					}
					if(bb != cfun->cfg->x_exit_block_ptr->prev_bb){//
						//if(EDGE_COUNT(bb->succs)>1)
							//branch_counter = branch_counter-1+EDGE_COUNT(bb->succs);
						fprintf(stderr, "succ := "); 
						edge e;
						edge_iterator ei;
						FOR_EACH_EDGE (e, ei, bb->succs)
						{
							fprintf(stderr, " %d ", e->dest->index);
							fprintf(stderr,"(%d->%d)",bb->index,e->dest->index);				
						}
					}
					fprintf(stderr, "\n");
					for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
					{
						gimple *gc = gsi_stmt (gsi);
						debug_gimple_stmt(gc);					
					}
					DECL_STRUCT_FUNCTION(cfun->decl)=cfun;
					fprintf(stderr, "\n========\n");
				}
				fDFS->put(node,DFS);
				pop_cfun();
			}
			*/
			FOR_EACH_DEFINED_FUNCTION (node)
			{
				
				if(!ipa)
					init_table();
				//ofun=node->get_fun();
				push_cfun(node->get_fun());
				debug_tree(node->decl);
				if (cfun==NULL)
					continue;
					
				if(strcmp(get_name(cfun->decl),"main")==0)
					ffun=cfun;
				//if(strcmp(get_name(ofun->decl),"main")==0){
				fprintf(stderr,"=======node_fun:%s=========\n",get_name(cfun->decl));
				//fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (get_attribute_name(cfun->decl)));
				node->debug();

				Graph DFS;
				DFS.init_Graph(cfun->cfg->x_last_basic_block);
				calculate_dominance_info(CDI_DOMINATORS);

				FOR_EACH_BB_FN (bb, cfun){
					
					if(bb != cfun->cfg->x_exit_block_ptr->prev_bb){
						edge e;
						edge_iterator ei;
						fprintf(stderr,"node:= %d \n succs:=",bb->index);
						FOR_EACH_EDGE (e, ei, bb->succs)
						{
							DFS.addEdge(bb->index,e->dest->index);
							fprintf(stderr,"%d",e->dest->index);
						}
						if(!DFS.exist_succ(bb)){
												
						}
						fprintf(stderr,"\n");
					}

					for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
					{
						gimple *gc = gsi_stmt (gsi);
						debug_gimple_stmt(gc);
						if (is_gimple_call(gc)){
							collect_malloc(gc,node,bb);
							
						}
						if(gimple_code (gc)==GIMPLE_PHI){
							var_points_to vpt;
							vector<vector<pair<fdecl,location_t>>> may_malloc_path;
							vpt.may_malloc_path=may_malloc_path;
							fprintf(stderr,"address phi %d \n",gimple_phi_result (gc));
							tvpt->put(gimple_phi_result (gc),vpt);
						}
					
					}
				}
				fDFS->put(node,DFS);
				fprintf(stderr,"=======node_fun_end=========\n");
				//}else{

				//}
				fprintf(stderr,"%d %d---------------\n",ptable->fun,ftable->fun);
				fprintf(stderr,"malloc_table---------------\n");
				print_pointer_table_new(ptable);
				fprintf(stderr,"free_table---------------\n");
				print_pointer_table_new(ftable);
				if(!ipa){
					new_memory_leak_analysis (ptable,ftable);
					new_double_free_analysis(ptable,retable,ftable,phitable);
					new_use_after_free_analysis(ptable, ftable);
				}				
				//node->get_body();
				
				pop_cfun();
			}
			fprintf(stderr,"=======pass end=========\n");
			
			fprintf(stderr,"=======analysis=========\n");
			if(ipa){
				//new_memory_leak_analysis (ptable,ftable);
				//new_double_free_analysis(ptable,retable,ftable,phitable);
				new_use_after_free_analysis(ptable, ftable);
			}
			fprintf(stderr,"=======analysis end=========\n");	
			FOR_EACH_DEFINED_FUNCTION (node)
			{
				push_cfun(node->get_fun());
				//free_dominance_info (CDI_DOMINATORS);		
				pop_cfun();
			}
			//push_cfun(test);
			return 0;
    }
    virtual my_pass6* clone() override { return this; }
};
void *plugin_callback_cgraph (void *gcc_data, void *user_data)
{
	//cgraph_node *node=(cgraph_node)gcc_data;
	//for (node = cgraph_nodes; node; node = node->next)
  return NULL;
}


int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    fp = fopen ("file.txt", "a");
	fprintf(fp,"test\n");
	if (!plugin_default_version_check(version, &gcc_version)){
		printf("incompatible gcc/plugin versions\n");
		return 1;
	}
 
	my_passinfo.pass=new my_pass();
	my_passinfo.reference_pass_name="materialize-all-clones"; 
	my_passinfo.ref_pass_instance_number=0;// 0=true
	my_passinfo.pos_op=PASS_POS_INSERT_AFTER;

    
	my_passinfo2.pass=new my_pass2();
	my_passinfo2.reference_pass_name="rtl_dce"; 
	my_passinfo2.ref_pass_instance_number=1;
	my_passinfo2.pos_op=PASS_POS_INSERT_AFTER;    	
	
	my_passinfo3.pass=make_pass_ipa_always(g);
	my_passinfo3.reference_pass_name="inline";
	my_passinfo3.ref_pass_instance_number=0;
	my_passinfo3.pos_op=PASS_POS_INSERT_BEFORE;

	my_passinfo4.pass=new my_pass4();
	my_passinfo4.reference_pass_name="alias";
	my_passinfo4.ref_pass_instance_number=0;
	my_passinfo4.pos_op=PASS_POS_INSERT_AFTER;

	my_passinfo5.pass=new my_pass5();
	my_passinfo5.reference_pass_name="einline";
	my_passinfo5.ref_pass_instance_number=0;
	my_passinfo5.pos_op=PASS_POS_INSERT_BEFORE;


	my_passinfo6.pass=new my_pass6();
	my_passinfo6.reference_pass_name="pta";
	//my_passinfo6.reference_pass_name="alias";
	my_passinfo6.ref_pass_instance_number=0;
	my_passinfo6.pos_op=PASS_POS_INSERT_AFTER;

	my_passinfo7.pass=make_pass_ipa_pta(g);
	my_passinfo7.reference_pass_name="free-fnsummary";
	my_passinfo7.ref_pass_instance_number=0;
	my_passinfo7.pos_op=PASS_POS_INSERT_AFTER;

	const char * const plugin_name = plugin_info->base_name;

	//register_callback("test", PLUGIN_PASS_MANAGER_SETUP, NULL, &my_passinfo);
	//register_callback("test2", PLUGIN_PASS_MANAGER_SETUP, NULL, &my_passinfo2);
	//register_callback("test3", PLUGIN_PASS_MANAGER_SETUP, NULL, &my_passinfo3);
	//register_callback("test4", PLUGIN_PASS_MANAGER_SETUP, NULL, &my_passinfo4);
	register_callback("test5", PLUGIN_PASS_MANAGER_SETUP, NULL, &my_passinfo5);
	register_callback("test6", PLUGIN_PASS_MANAGER_SETUP, NULL, &my_passinfo6);
	//register_callback("test7", PLUGIN_PASS_MANAGER_SETUP, NULL, &my_passinfo7);
	//register_callback (plugin_name,PLUGIN_ALL_IPA_PASSES_START,(plugin_callback_func) plugin_callback_cgraph,NULL);
	//register_callback(plugin_name, PLUGIN_FINISH_PARSE_FUNCTION, finish_decl, NULL);
	//register_callback("ruletest", PLUGIN_FINISH, &decl_func, NULL);
    
    return 0;
}
