
#include "config.h"
#include "system.h"
#include <coretypes.h>
#include "insn-constants.h"
//#include "config/i386/i386.h"
#include "is-a.h"
#include "options.h"
#include "backend.h" 
#include "rtl.h"
#include "tree-pass.h"
#include "tree.h"
#include "gimple.h"
#include "alloc-pool.h"
#include "ssa.h"
#include "cgraph.h"
#include "tree-pretty-print.h"
#include "diagnostic-core.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "stmt.h"
#include "gimple-iterator.h"
#include "tree-into-ssa.h"
#include "tree-dfa.h"
#include "params.h"
#include "gimple-walk.h"
#include "varasm.h"
#include "options.h"
#include "gimple-pretty-print.h"
#include "print-tree.h"
#include <string.h>
#include "intl.h"
#include "dominance.h"
#include <et-forest.h>
#include <cgraph.h>
#include "DFS.h"
#include "cfgloop.h"
#include <vector>
#define FOR_EACH_TABLE(TABLE,TAR)\
	for((TAR)=((TABLE)==NULL)?NULL:(TABLE)->target;\
	(TAR)!=NULL;\
	(TABLE)=(TABLE)->next,(TAR)=((TABLE)==NULL)?NULL:(TABLE)->target)
#define FOR_EACH_USE_TABLE(USE,STMT)\
	for((STMT)=((USE)==NULL)?NULL:(USE)->stmt;(USE)!=NULL;(USE)=(USE)->next,(STMT)=((USE)==NULL)?NULL:(USE)->stmt)


struct ptb{
  tree target;
  location_t loc;
  ptb *next;
  int state;
  bool may_realloc, is_phi_stmt;
  gimple *last_stmt, *dealloc_stmt;
  basic_block bb;
  cgraph_node *node;
  gimple* caller;
  function *fun;
  bool removed;
  bool inbranch;
};

struct gimple_array
{
  gimple *stmt;
	tree aptr;
  gimple_array *next;
};


bool ipa_pta=false;
bool developer=false;
int traverse_counter;
function* ffun;
int function_number=0;

/*state*/
unsigned int POINTER_NOT_EXIST        	= 0;
unsigned int POINTER_STATE_IS_FREE    	= 1;
unsigned int POINTER_STATE_IS_NORMAL  	= 2;
unsigned int POINTER_STATE_IS_MALLOC  	= 3;
unsigned int POINTER_STATE_IS_FILE    	= 4;
unsigned int POINTER_STATE_MAY_IS_FREE	= 5;

const unsigned int POINTER_NOT        = 0;
const unsigned int POINTER_MAY        = 1;
const unsigned int POINTER_MUST       = 2;


bool ALLOC_IS_FREE  = true;
bool ALLOC_NOT_FREE = false;
bool ipa=false;
gimple *final_stmt;
gimple_stmt_iterator final_gsi;
location_t l_stmt;

ptb start1, start2, start3, start4, start5, start6,start7; 
ptb *ptable, *retable, *ftable, *phitable,*return_table,*use_table,*fopen_table;

void double_free_analysis   (ptb *ptable, ptb *retable, ptb *ftable, ptb *phitable, int bb_loop[][2], int loop_i, int branch[][2], int branch_i);
void use_after_free_analysis(ptb *ptable, ptb *retable, ptb *ftable, ptb *phitable, int bb_loop[][2], int loop_i, int branch[][2], int branch_i);
void memory_leak_analysis   (ptb *ptable, ptb *retable, ptb *ftable, ptb *phitable,ptb *return_table, int bb_loop[][2], int loop_i, int branch[][2], int branch_i);
void loop_check(ptb *ftable, int bb_loop[][2], int loop_index);
void print_pointer_table_new(ptb *ptable);
bool cgraph_path_tracker(ptb *p1,ptb *p2);


hash_map<cgraph_node*, Graph > *fDFS;

hash_map<tree, cgraph_node *> *fnode;
FILE * fp;   

void get_member(ptb *table, tree t, ptb *result){
  while(table->target!=NULL_TREE){
    if(table->target==t){
      *result = *table;
      break;
      //return table;
    }
    else if(table->next!=NULL)
      table =table->next;
    else{
      result = NULL;
      break;
    }
  }
}
void do_realloc(ptb *table, tree t){
  while(table->target!=NULL_TREE ){
    if(ptr_derefs_may_alias_p(table->target, t))
    {
      table->may_realloc=true;
    }
    if(table->next != NULL){
      table = table->next;
    }
    else
      break;
  }
}
bool bb_in_loop_p(basic_block bb){
	return bb->loop_father->header->index!=0;
}
bool bb_in_branch_p(ptb *table){
	fprintf(stderr,"bb_in_branch_p :%d %d \n",table->last_stmt->bb->index,cfun->cfg->x_exit_block_ptr->prev_bb->index);
	return !dominated_by_p(CDI_DOMINATORS,table->fun->cfg->x_exit_block_ptr->prev_bb,table->bb);
}
bool bb_in_branch_p(gimple* stmt){
	//function* fn = DECL_STRUCT_FUNCTION(gimple_get_lhs(stmt));
	fprintf(stderr,"bb_in_branch_p :%d %d \n",stmt->bb->index,cfun->cfg->x_exit_block_ptr->prev_bb->index);
	return !dominated_by_p(CDI_DOMINATORS,stmt->bb,cfun->cfg->x_exit_block_ptr->prev_bb);
}


bool check_fn_param_used_2(tree parm, function *fn){
  bool result=false;
  tree a = DECL_ARGUMENTS(fn->decl);

  if(a != NULL_TREE && get_name (parm))
  for (tree arg = a; arg; arg = DECL_CHAIN (arg)){
   //  fprintf(stderr, "parameter %s->tree-%s\n", get_name(arg), get_name (parm));
    if(!get_name(arg))
      continue;
    if(strcmp(get_name(arg),get_name(parm))==0)
    { 
    //  fprintf(stderr, ">>>>>>>>>>>>> %s\n", get_name(TREE_OPERAND (parm, 0)));
      result = true;
      break;
    }
  }

  return result;
}
void update_location(ptb *table, gimple *stmt) {
  table->loc = gimple_location(stmt);
}

int search_table(ptb *table, tree t, bool action, int new_state, gimple *stmt){
/*  while(table->target!=NULL_TREE ){
    if(table->target==t)
      return table->state;
    else if(table->next!=NULL)
      table =table->next;
  }*/
  
  //fprintf(stderr, "search_table start:\n");
  debug_head(t);

  int result = POINTER_NOT_EXIST;
  struct ptr_info_def *pi1, *pi2;
 /* if(table->target == t)
    return true;*/
  if(TREE_CODE(t)==SSA_NAME){
    pi2 = SSA_NAME_PTR_INFO (t);}
  else 
    return result;
  //if (!pi2)
  //  return result;


  while(table->target!=NULL_TREE){
    if(TREE_CODE(table->target)==SSA_NAME)
      pi1 = SSA_NAME_PTR_INFO (table->target);

    if (!pi1){
      if(table->next != NULL)
        table = table->next;
      else
        return result;
//      break;
    }
    //fprintf(stderr,"alloc and phi's stmt:%d\n",ptr_derefs_may_alias_p(table->target, t));
    if(ptr_derefs_may_alias_p(table->target, t)
      || pt_solutions_intersect (&pi1->pt, &pi2->pt) )
    {
      if(!action) {
        if(!table->may_realloc)
          return table->state;
        else if(table->may_realloc && table->target == t)
          warning_at(table->loc, 0, "Undefined behavior: %s is used after reallocate", get_name(table->target));
        else if(table->next != NULL) {
          result = table->state;
 //         table = table->next;       
        }
      }
      else {
       /* if(table->state == POINTER_STATE_IS_FREE && new_state == POINTER_STATE_IS_FREE){
          //warning_at(table->loc, 0, "%s is double-free (MISRA-C Rule 22.2)", get_name(table->target));
          warning_at(l_stmt, 0, "%s is double-free (MISRA-C Rule 22.2)", get_name(table->target));
        }
        else if(table->state == POINTER_STATE_IS_MALLOC){
          table->state = POINTER_STATE_IS_FREE;
          fprintf(stderr, "Pointer \"");
          fprintf(stderr, "\" is free; now\n");
         //  fprintf(stderr, "Pointer -");
         // debug_head(table->target);
         // fprintf(stderr, "is free now\n");
        }
        else if(table->state == POINTER_STATE_IS_MALLOC || table->state == POINTER_STATE_IS_FREE){
          if(new_state == POINTER_STATE_IS_FREE){
            fprintf(stderr, "%s is free now\n", get_name(table->target));//debug_head(table->target);
          }
          else if(new_state == POINTER_STATE_IS_MALLOC){
            fprintf(stderr, "%s is realloc now\n", get_name(table->target));//debug_head(table->target);
          }
          table->state = new_state;
        }
        else
          ;*/
        if(!table->may_realloc){
          if(table->state == POINTER_STATE_IS_FREE){
          //  fprintf(stderr, "1 ");
            result = POINTER_STATE_IS_FREE;
            if(get_name(t))
            warning_at(l_stmt, 0, 
              "MISRA-C Rule Violated: Rule 22.2: %s is double-free by %s.",
              get_name(table->target), get_name(t));
          //debug_head(table->target);
            if(get_name(table->target))
            warning_at (gimple_location(stmt), 0, 
                        "%s is deallocated here", get_name(table->target));
           
            gimple_stmt_iterator remove_stmt = gsi_for_stmt( (table->dealloc_stmt));
            fprintf(stderr, "remove stmt: \n" );debug_gimple_stmt(gsi_stmt(remove_stmt));

            basic_block modified_bb = gimple_bb (table->dealloc_stmt);
            gsi_remove (&remove_stmt, true);
           // if (gsi_remove (&remove_stmt, true))
              unlink_stmt_vdef (table->dealloc_stmt);
              //bitmap_set_bit (BITMAP_ALLOC (NULL), modified_bb->index);
          //  bitmap_set_bit (BITMAP_ALLOC (NULL), modified_bb->index);
          // bitmap_set_bit (need_eh_cleanup, bb->index);
        //    if (need_ssa_update_p (cfun))
         //     update_ssa (TODO_update_ssa);
          }
          else if(table->state == POINTER_STATE_IS_MALLOC){
            table->state = POINTER_STATE_IS_FREE;
            result = POINTER_STATE_IS_MALLOC;
            table->dealloc_stmt = stmt;
            fprintf(stderr, "table->dealloc_stmt: " );debug_head(table->target);debug_gimple_stmt(table->dealloc_stmt);
          }
        }
     //   else if(table->may_realloc) {
        else {
          if(table->target == t) {
          //  fprintf(stderr, "2 ");
            //warning_at(table->loc, 0, "Undefined behavior: %s is used after reallocate", get_name(table->target));
            warning_at(l_stmt, 0, "MISRA-C Rule Violated: Rule 22.2: %s is reallocate before, this may cause double-free", get_name(table->target));
            return POINTER_STATE_IS_FREE;
          }
        }
      }
    }
    if(table->next != NULL){
      table = table->next;
    }
    else
      return result;
  }
  return result;
}

int search_table(ptb *table, tree t){
  int result=POINTER_NOT_EXIST;
    struct ptr_info_def *pi1, *pi2;
 /* if(table->target == t)
    return true;*/
  if(TREE_CODE(t)==SSA_NAME)
    pi2 = SSA_NAME_PTR_INFO (t);
  else 
    return result;
  if (!pi2)
    return result;

  while(table->target!=NULL_TREE ){
    if(TREE_CODE(table->target)==SSA_NAME)
      pi1 = SSA_NAME_PTR_INFO (table->target);

    if (!pi1){
      if(table->next != NULL)
        table = table->next;
      else
        return POINTER_NOT_EXIST;
//      break;
    }

    if(ptr_derefs_may_alias_p(table->target, t)
      || pt_solutions_intersect (&pi1->pt, &pi2->pt) )
    {
     // if(!action){
       // fprintf(stderr, "table->state: %d\n", table->state);
      //  result = table->state;
     // }
      //else 
      {
        if(table->state == POINTER_STATE_IS_FREE){
         // fprintf(stderr, "Pointer \"%s\" is double-free\n", get_name(table->target));
         // fprintf(stderr, "Pointer \"");
         // fprintf(stderr, "\" is double-free\n");
          warning_at(table->loc, 0, "%s is double-free (MISRA-C Rule 22.2)", get_name(table->target));
        }
        else if(table->state == POINTER_STATE_IS_MALLOC){
          table->state = POINTER_STATE_IS_FREE;
          fprintf(stderr, "Pointer \"");
          fprintf(stderr, "\" is free now\n");
         //  fprintf(stderr, "Pointer -");
         // debug_head(table->target);
         // fprintf(stderr, "is free now\n");
        }
        else
          ;
        if(table->next != NULL)
          table = table->next;       
        else 
          return POINTER_NOT_EXIST;
      }
    }
    else if(table->next != NULL){
      table = table->next;
    }
    else
      return POINTER_NOT_EXIST;
  }
  return POINTER_NOT_EXIST;
}
bool search_phi(ptb *target1, ptb *target2, ptb *phitable){
  ptb *temp=phitable;
  bool result =false;
  bool loop = true;
  basic_block phi_bb = NULL;
  edge e = NULL;
  if(TREE_CODE(target1->target) == SSA_NAME){
    while(temp->target != NULL_TREE && ptr_derefs_may_alias_p(target1->target, temp->target) && loop){
       for(int i=0;i<gimple_phi_num_args(temp->last_stmt);i++){
        //fprintf(stderr, "%d ", gimple_phi_arg_edge(temp->last_stmt, i)->src->index);
        if(TREE_CODE(TREE_TYPE(gimple_phi_arg_def(temp->last_stmt, i))) == METHOD_TYPE 
        || TREE_CODE(TREE_TYPE(gimple_phi_arg_def(temp->last_stmt, i))) == FUNCTION_TYPE
        || TREE_CODE(TREE_TYPE(gimple_phi_arg_def(temp->last_stmt, i))) == RECORD_TYPE
        || !(TREE_CODE(gimple_phi_arg_def(temp->last_stmt, i)) == SSA_NAME))
          continue;
        phi_bb = gimple_bb(SSA_NAME_DEF_STMT (gimple_phi_arg_def(temp->last_stmt, i)));
        if(!result && phi_bb == target1->bb){
          if(ptr_derefs_may_alias_p(gimple_phi_arg_def(temp->last_stmt, i), target1->target))
            result = false;
          else
            result = true;
          loop=false;
          break;
        }
        else
          phi_bb=NULL;
        edge e = gimple_phi_arg_edge (as_a <gphi *>(temp->last_stmt), i);
        if(!result && (e->src == target1->bb)){
          if(ptr_derefs_may_alias_p(gimple_phi_arg_def(temp->last_stmt, i), target1->target))
            result = false;
          else
            result = true;
          loop=false;
          break;
        }
        else
          e=NULL;
      /*  if(phi_bb!=NULL
          && phi_bb != ffun->cfg->x_entry_block_ptr->next_bb
          && phi_bb != ffun->cfg->x_exit_block_ptr->prev_bb)
        fprintf(stderr, "%d ", phi_bb->index);
        debug_head(gimple_phi_arg_def(temp->target, i));
        if(search_table(ptable, gimple_phi_result(phi), false, 0, temp->target) == POINTER_STATE_IS_MALLOC)
          set_ptb(bb, ptable, gimple_phi_result(phi), gimple_location (temp->target), POINTER_STATE_IS_MALLOC, phi);*/
      }
      if(!result && temp->next!=NULL)
        temp = temp->next;
      else
        break;
    }
    if(TREE_CODE(target2->target) ==SSA_NAME && temp->target != NULL_TREE && ptr_derefs_may_alias_p(target2->target, temp->target) && !result && (phi_bb != NULL || e != NULL)){
      for(int i=0;i<gimple_phi_num_args(temp->last_stmt);i++){
       // fprintf(stderr, "%d ", gimple_phi_arg_edge(temp->last_stmt, i)->src->index);
        if(TREE_CODE(TREE_TYPE(gimple_phi_arg_def(temp->last_stmt, i))) == METHOD_TYPE 
        || TREE_CODE(TREE_TYPE(gimple_phi_arg_def(temp->last_stmt, i))) == FUNCTION_TYPE
        || TREE_CODE(TREE_TYPE(gimple_phi_arg_def(temp->last_stmt, i))) == RECORD_TYPE
        || !(TREE_CODE(gimple_phi_arg_def(temp->last_stmt, i)) == SSA_NAME))
          continue;
        phi_bb = gimple_bb(SSA_NAME_DEF_STMT (gimple_phi_arg_def(temp->last_stmt, i)));
        if(phi_bb!=NULL && phi_bb == target2->bb){
          if(ptr_derefs_may_alias_p(gimple_phi_arg_def(temp->last_stmt, i), target2->target))
            result = false;
          else
            result = true;
          loop=false;
          break;
        }
        edge e = gimple_phi_arg_edge (as_a <gphi *>(temp->last_stmt), i);
        if(e!=NULL && !result && (e->src == target2->bb)){
          if(ptr_derefs_may_alias_p(gimple_phi_arg_def(temp->last_stmt, i), target2->target))
            result = false;
          else
            result = true;
          loop=false;
          break;
        }
      }
    }
  }
  return result;
}
void set_gimple_array(gimple_array *table, gimple *used_stmt,tree a){
  if(table->stmt==NULL){
    table->stmt = used_stmt;
	table->aptr=a;
    table->next=NULL;
  }
  else{
    bool same=false;
    while(table->next != NULL){
      if(table->stmt == used_stmt && LOCATION_LINE(gimple_location(table->stmt)) == LOCATION_LINE(gimple_location(used_stmt))){
        same=true;
        break;
      }
      table = table->next;
    }
    if(!same){
      table->next = new gimple_array();
      table = table->next;
      table->stmt = used_stmt;
		table->aptr=a;
      table->next = NULL;
    }
  }
}
void search_alias_ptr_use(gimple_array *used_stmt, tree target){
	imm_use_iterator imm_iter;
	gimple_array *used2=used_stmt;
	gimple *use_stmt; 
	gimple *gc;
	FOR_EACH_IMM_USE_STMT (use_stmt, imm_iter, target)
	{
		debug_gimple_stmt(use_stmt);
		fprintf(stderr,"%s \n",gimple_code_name[gimple_code (use_stmt)]);
		if(gimple_code(use_stmt) == GIMPLE_ASSIGN){
			debug_tree(gimple_assign_lhs(use_stmt));
			if(gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt))==SSA_NAME && gimple_assign_single_p(use_stmt)){
				set_gimple_array(used_stmt, use_stmt,target);
				search_alias_ptr_use(used_stmt, gimple_assign_lhs(use_stmt));
			}
		} 
		else if(gimple_code(use_stmt) == GIMPLE_PHI){
			if(gimple_phi_result(use_stmt) && TREE_CODE(gimple_phi_result(use_stmt))==SSA_NAME){
				debug_gimple_stmt(use_stmt);
				debug_tree(gimple_phi_result(use_stmt));
				bool exist=false;
				FOR_EACH_USE_TABLE(used2,gc){

					fprintf(stderr,"gc : %d  use :%d \n",gc,use_stmt);
					debug_gimple_stmt(gc);
					if(gc==use_stmt){
						exist=true;
					}
				}
				if(!exist){
					set_gimple_array(used_stmt, use_stmt,target);
					search_alias_ptr_use(used_stmt, gimple_phi_result(use_stmt));
				}
			}
			
		}else{
			set_gimple_array(used_stmt, use_stmt,target);
		}
		
  }
}

void search_ptr_use(gimple_array *used_stmt, tree target){
  imm_use_iterator imm_iter;
  gimple *use_stmt;

       use_operand_p imm_use_p;
       imm_use_iterator iterator;
	//debug_tree(target);
  FOR_EACH_IMM_USE_STMT (use_stmt, imm_iter, target)
  {
 //   if(TREE_CODE(target)==SSA_NAME){
      debug_gimple_stmt(use_stmt);
      //debug_tree(gimple_call_arg (use_stmt,0));
      fprintf(stderr,"do\n");
      set_gimple_array(used_stmt, use_stmt,target);
      debug_tree(target);
      fprintf(stderr,"do2\n");
      //debug_tree(TREE_OPERAND(target,1));
      if (gimple_call_arg(use_stmt,0)){
	fprintf(stderr,"gimple_call_arg(use_stmt,0)\n");
	//debug_tree(gimple_assign_rhs1(SSA_NAME_DEF_STMT (gimple_call_arg(use_stmt,0))));
      }
      if (gimple_assign_rhs1(use_stmt)){
	//fprintf(stderr,"gimple_assign_rhs1(use_stmt)\n");
	//debug_tree(gimple_assign_rhs1(use_stmt));
      }
      //if(gimple_code(use_stmt)!=GIMPLE_PHI && gimple_assign_load_p(use_stmt) && gimple_assign_single_p(use_stmt) ){
      if(gimple_code(use_stmt) == GIMPLE_ASSIGN){
      if(gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt))==SSA_NAME && gimple_assign_single_p(use_stmt) && gimple_assign_rhs1(use_stmt)==target){
     //   fprintf(stderr, "single assign: ");
        search_ptr_use(used_stmt, gimple_assign_lhs(use_stmt));
      }}
      /*if(gimple_assign_single_p(use_stmt) && gimple_assign_lhs(use_stmt)==target){
        if(TREE_CODE(gimple_assign_rhs1(use_stmt))==SSA_NAME){
          fprintf(stderr, "single assign: ");
          search_alias_ptr_use(used_stmt, gimple_assign_rhs1(use_stmt));
        }
      }*/
      // warning_at(gimple_location(use_stmt), 0,"immediate use: %s", get_name(ptable->target));
  //  }
  }
}
void trace_phi(tree origin, tree phi_arg){
  /*for (gphi_iterator gsi = gsi_start_phis (gimple_bb (SSA_NAME_DEF_STMT (gimple_phi_arg (phi, i)))); !gsi_end_p (gsi); gsi_next (&gsi))
  {
    gphi *phi = gsi.phi ();
    trace_phi(origin, phi);
  }*/
 // gphi *phi = SSA_NAME_DEF_STMT (phi_arg);
  if(get_name (phi_arg) && TREE_CODE(phi_arg) == SSA_NAME
    && gimple_phi_result (SSA_NAME_DEF_STMT (phi_arg)) == phi_arg
    && gimple_code (SSA_NAME_DEF_STMT (phi_arg)) == GIMPLE_PHI){
    gimple *phi = SSA_NAME_DEF_STMT (phi_arg);
    fprintf(stderr, "phi %s arg %d\n", get_name(phi_arg), gimple_phi_num_args(phi));
    for(int i=0;i<gimple_phi_num_args (phi); i++){
      if(!get_name(gimple_phi_arg_def (phi, i)))
        break;
      if(TREE_CODE(TREE_TYPE(gimple_phi_arg_def (phi, i))) == METHOD_TYPE 
        || TREE_CODE(TREE_TYPE(gimple_phi_arg_def (phi, i))) == FUNCTION_TYPE
        || TREE_CODE(TREE_TYPE(gimple_phi_arg_def (phi, i))) == RECORD_TYPE)
          break;
      if(!gimple_phi_arg_def (phi, i) && !get_name (gimple_phi_arg_def (phi, i)) && TREE_CODE (gimple_phi_arg_def (phi, i)) != SSA_NAME)
        break;
      if(gimple_code (SSA_NAME_DEF_STMT (gimple_phi_arg_def (phi, i))) == GIMPLE_PHI){
        trace_phi(origin, gimple_phi_arg_def (phi, i));
      }
      else{
        if(ptr_derefs_may_alias_p(origin, gimple_phi_arg_def (phi, i))){
          fprintf(stderr, "Alias:\n" );
          debug_head(origin);
          debug_head(gimple_phi_arg_def (phi, i));
        }
        else{
          fprintf(stderr, "Not:\n" );
          debug_head(origin);
          debug_head(gimple_phi_arg_def (phi, i));
        }
      }
    }
  }
}

void trace_bb_succ(basic_block start, int end, int succ_path[]){
  int i;
//for(i=0;succ_path[i]!=0;i++);

  edge e;
  edge_iterator ei;
  FOR_EACH_EDGE(e, ei, start->succs){
    bool exist=false;
    if(e->dest!=NULL && e->dest->index!=end){
     // if(*count<n_basic_blocks_for_fn(ffun))
      for(i=0;i < n_basic_blocks_for_fn(ffun) && succ_path[i]!=0;i++){
        if(e->dest->index == succ_path[i]){
          exist=true;
        }
      }
      if(i < n_basic_blocks_for_fn(ffun) && exist==false){
        succ_path[i] = e->dest->index;
      //  i++;
      }
      if(e->dest != ffun->cfg->x_exit_block_ptr->prev_bb
        && EDGE_COUNT(e->dest->succs)!=0 
        /*&& EDGE_SUCC(e->dest, 0)->dest!= ffun->cfg->x_exit_block_ptr->prev_bb */
        && EDGE_SUCC(e->dest, 0)->dest->index!=end){
        trace_bb_succ(EDGE_SUCC(e->dest, 0)->dest, end, succ_path);
        //for(i=0;succ_path[i]!=0;i++);
      }
    }
  }
}
void add_free_stmt(ptb *ptable, tree target){
  /*if(true)
    ;
  else */if(TREE_CODE(TREE_TYPE(target)) != METHOD_TYPE 
     && TREE_CODE(TREE_TYPE(target)) != FUNCTION_TYPE
     && TREE_CODE(TREE_TYPE(target)) != RECORD_TYPE
     && TREE_CODE(target) == SSA_NAME 
     && get_name(target)){
  tree fn;
            fn = builtin_decl_implicit (BUILT_IN_FREE);
           // fn = make_ssa_name(fn);
            
           // gimple *new_call = gimple_build_call (fn, 1, gimple_assign_rhs1 (stmt));
            gcall *new_call = gimple_build_call (fn, 1, target);
            //gcall *new_call;
          //  gimple_call_set_fn (new_call, fn);
            gimple_call_set_arg (new_call, 0, target);
            gimple_set_location (new_call, gimple_location(final_stmt));
            gimple_set_vuse (new_call, gimple_vuse (final_stmt));
            gimple_set_vdef (new_call, gimple_vdef (final_stmt));
           // if (gimple_vdef (new_call) && TREE_CODE (gimple_vdef (new_call)) == SSA_NAME)
           //   SSA_NAME_DEF_STMT (gimple_vdef (new_call)) = new_call;
           // gimple_set_vuse (new_call, gimple_vuse (stmt));

            gimple_set_uid (new_call, gimple_uid (final_stmt));
                  
            //if(bb){
            //  if (!gsi_end_p (gsi))
            //    gsi_insert_after (&gsi, new_call, GSI_NEW_STMT);
            //  else
                gsi_insert_before (&final_gsi, new_call, GSI_SAME_STMT);
            //}

            if (need_ssa_update_p (ffun))
              update_ssa (TODO_update_ssa);
            //debug_gimple_stmt(new_call);
          }
 // search_table(ptable, target, true, POINTER_STATE_IS_FREE, new_call);
}
void remove_pointer_stmt(gimple *stmt){
  gimple_stmt_iterator remove_stmt;
  /*if(true)
    ;
  else*/ if(gimple_code(stmt)==GIMPLE_CALL){
    remove_stmt = gsi_for_stmt(stmt);
	warning_at(gimple_location (stmt),0,"remove stmt");
    basic_block modified_bb = gimple_bb (stmt);
    gsi_remove (&remove_stmt, true);
    unlink_stmt_vdef (stmt);
  }
  /*else if(gimple_code(stmt)==GIMPLE_PHI){
    remove_stmt = gsi_after_labels (gimple_bb(stmt));
    fprintf(stderr, "remove stmt: " );debug_gimple_stmt(gsi_stmt(remove_stmt));
    basic_block modified_bb = gimple_bb (stmt);
    gsi_remove (&remove_stmt, true);
    unlink_stmt_vdef (stmt);
  }*/
}

bool bb_in_loop_p(int bb, int bb_loop[][2], int loop_index){
  bool result = false;
  for(int i=0; i<loop_index; i++){
    if(bb>=bb_loop[i][0] && bb<=bb_loop[i][1]){
      result = true;
      break;
    }
  }

  return result;
}
bool bb_in_same_loop_alloc_free(basic_block m,basic_block f){
	if(m->loop_father->header->index ==f->loop_father->header->index)
		if(m->loop_father->header->index!=0)
			return true;
	return false;
}
bool bb_in_same_loop_alloc_free(int alloc_bb,int free_bb, int bb_loop[][2], int loop_index){
	bool result = false;
	for(int i=0; i<loop_index; i++){
		if(free_bb>=bb_loop[i][0] && free_bb<=bb_loop[i][1] && alloc_bb>=bb_loop[i][0] && alloc_bb<=bb_loop[i][1]){
			result = true;
			break;
		}
	}
	return result;
}
void set_ptb(basic_block b,ptb *table, tree t, location_t l, int s, gimple *stmt,cgraph_node *node){
	if(table->target == NULL_TREE){
		table->bb = b;
		table->target = t;
		table->next = NULL;
		table->loc = l;
		table->state = s;
		table->may_realloc = false;
		table->last_stmt = stmt;
		table->node=node;
		table->inbranch=bb_in_branch_p(stmt);
		table->fun=node->get_fun();
		if(gimple_code (stmt) == GIMPLE_PHI){
			//fprintf(stderr, "add phi stmt ");debug_head(table->target);
		}
		table->removed=false;
	}  
	else{
		while(table->next!=NULL){
			table=table->next;
		}
		table->next = new ptb();
		table = table->next;
		table->bb = b;
		table->target = t;
		table->next = NULL;
		table->loc = l;
		table->state = s;
		table->may_realloc = false;
		table->last_stmt = stmt;
		table->node=node;
		table->inbranch=bb_in_branch_p(stmt);
		table->fun=node->get_fun();
		if(gimple_code (stmt) == GIMPLE_PHI){
			//fprintf(stderr, "add phi stmt ");debug_head(table->target);
			table->is_phi_stmt = true;
		}
		else
			table->is_phi_stmt = false;
		table->removed=false;
	}
}
void set_ptb(basic_block b,ptb *table, tree t, location_t l, int s, gimple *stmt){
	if(table->target == NULL_TREE){
		table->bb = b;
		table->target = t;
		table->next = NULL;
		table->loc = l;
		table->state = s;
		table->may_realloc = false;
		table->last_stmt = stmt;
		table->fun=cfun;
		if(gimple_code (stmt) == GIMPLE_PHI){
			//fprintf(stderr, "add phi stmt ");debug_head(table->target);
		}
		table->removed=false;
	}  
	else{
		while(table->next!=NULL){
			table=table->next;
		}
		table->next = new ptb();
		table = table->next;
		table->bb = b;
		table->target = t;
		table->next = NULL;
		table->loc = l;
		table->state = s;
		table->may_realloc = false;
		table->last_stmt = stmt;
		table->fun=cfun;
		if(gimple_code (stmt) == GIMPLE_PHI){
			//fprintf(stderr, "add phi stmt ");debug_head(table->target);
			table->is_phi_stmt = true;
		}
		else
			table->is_phi_stmt = false;
		table->removed=false;
	}
}
void new_use_after_free_analysis(ptb *ptable, ptb *ftable){
	
  	ptb *table1 = ptable;
  	ptb *table3 = ftable;
	bool to_remove=false;
	tree t;
	struct ptr_info_def* pi;
	struct pt_solution* pt;
	fprintf(stderr,"=====new_use_after_free_analysis,===== \n");
  	FOR_EACH_TABLE(table3,t){
		if(table3->removed || !TREE_CODE(t)==SSA_NAME )
			continue;
		pi= SSA_NAME_PTR_INFO (table3->target);
		pt=&pi->pt;
		//if(pt->null)
			//continue;
		//fprintf(stderr,"---free_table-----\n");
		//debug_head(table3->target);
		gimple_array start;
		start.stmt=NULL;
		gimple_array *used_stmt=&start;

		search_alias_ptr_use(used_stmt, table3->target);
		tree t2;
		FOR_EACH_TABLE(table1,t2){
			if(TREE_CODE(table1->target)!=SSA_NAME)
				continue;
			if(ptr_derefs_may_alias_p(table3->target, table1->target)){
				search_alias_ptr_use(used_stmt, table1->target);
			}
		}
		table1 = ptable;


		//pointer1
		gimple *u_stmt;
		fprintf(stderr,"=====use table: \n");
		debug_head(table3->target);
		FOR_EACH_USE_TABLE(used_stmt,u_stmt){
			debug_gimple_stmt(u_stmt);
			if(gimple_code (used_stmt->stmt) ==GIMPLE_PHI )
				continue;
			if( gimple_call_builtin_p (used_stmt->stmt, BUILT_IN_FREE))
				continue;
			//fprintf(stderr,"-----  %d ~ %d \n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
			fprintf(stderr,"free line : %d ,use line : %d \n",LOCATION_LINE((table3->loc)),LOCATION_LINE(gimple_location(used_stmt->stmt)));
			fprintf(stderr,"free bb : %d ,use bb : %d \n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
			if(table3->bb->index == gimple_bb(used_stmt->stmt)->index){
				if(LOCATION_LINE((table3->loc))<=LOCATION_LINE(gimple_location(used_stmt->stmt))){
					//warning_at(table3->loc, 0, "");
					debug_head(table3->target);
					//fprintf(stderr,"--index : %d %d---\n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
					if(!table3->removed){
						warning_at(table3->loc, 0, "Use after free error!: free location");
						warning_at(gimple_location(used_stmt->stmt), 0, "use location");///ppp
						//fprintf(fp,"%s use after free\n",gimple_filename (table3->last_stmt));
						//remove_pointer_stmt(table3->last_stmt);
						//table3->removed=true;
					}
				}
			}else if(fDFS->get(table3->node)->is_succ(table3->bb,gimple_bb(used_stmt->stmt))){
				//fprintf(stderr,"----- is_succ\n");
				if(!table3->removed){
					warning_at(table3->loc, 0, "Use after free error!: free location");
					warning_at(gimple_location(used_stmt->stmt), 0, "use location");
					//fprintf(fp,"%s use after free \n",DECL_SOURCE_FILE (table3->fun->decl));
					//remove_pointer_stmt(table3->last_stmt);
					//table3->removed=true;
				}
			}
		}	
		fprintf(stderr,"=====use table end \n");
	}
}
void use_after_free_analysis(ptb *ptable, ptb *retable, ptb *ftable, ptb *phitable, int bb_loop[][2], int loop_i, int branch[][2], int branch_i){
 // fprintf(stderr, "start use_after_free_analysis\n");
  ptb *table1 = ptable;
  ptb *table2 = phitable;
  ptb *table3 = ftable;
  bool to_remove=false;
//  fprintf(stderr, "table3: ");debug_head(table3->target);
/*if(ptable!=NULL && ptable->target!=NULL_TREE)
  if(TREE_CODE(ptable->target)==SSA_NAME){
  fprintf(stderr, "ptable dddddddd: ");debug_head(ptable->target);
}*/
  /*********only one free*************/
  /****multiple free in for(table3; (table3->target!=NULL_TREE && !table3->removed && table3->next!=NULL); table3=table3->next)***/
  if(table3->target!=NULL_TREE && !table3->removed && table3->next==NULL && TREE_CODE(table3->target)==SSA_NAME && !bb_in_loop_p(table3->bb->index, bb_loop, loop_i)){
    gimple_array start;
    start.stmt=NULL;
    gimple_array *used_stmt=&start;
    
  //  fprintf(stderr, "table3: ");debug_head(table3->target);
    search_alias_ptr_use(used_stmt, table3->target);
    for(table1; table1->target!=NULL_TREE && table1->next!=NULL; table1=table1->next){
      if(TREE_CODE(table1->target)==SSA_NAME)
        continue;
    //  fprintf(stderr, "pointer target: ");debug_head(table1->target);
      if(ptr_derefs_may_alias_p(table3->target, table1->target)){
   //     fprintf(stderr, "pointer use: ");debug_head(table1->target);
        search_alias_ptr_use(used_stmt, table1->target);
      }
    }//fprintf(stderr, "last use: ");debug_gimple_stmt(used_stmt->stmt); 
    if(table1->target!=NULL_TREE)
      if(ptr_derefs_may_alias_p(table3->target, table1->target)){
   //     fprintf(stderr, "pointer use: ");debug_head(table1->target);
        search_alias_ptr_use(used_stmt, table1->target);
      }
    table1 = ptable;

    basic_block join_bb = NULL;
    bool find_branch = false;
    bool do_phi = true;

    for(table2; table2->target!=NULL_TREE && table2->next!=NULL && !find_branch; table2=table2->next){
      if(TREE_CODE(table2->target)!=SSA_NAME)
        continue;
      if(ptr_derefs_may_alias_p(table2->target, table3->target)){
        if(!find_branch){
          for(int i=0; i<gimple_phi_num_args(table2->last_stmt); i++){
            if(gimple_phi_arg_edge(as_a <gphi *>(table2->last_stmt), i)->src->index == table3->bb->index){
              join_bb = table3->bb;
              find_branch=true;
              if(ptr_derefs_may_alias_p(gimple_phi_arg_def(as_a <gphi *>(table2->last_stmt), i), table3->target)){
              }
              else{
                do_phi = false;
                break;
              }
            }
          }
        }
        if(do_phi){
            search_alias_ptr_use(used_stmt, table2->target);
        }
      }
    }
    if(table2->target != NULL_TREE && TREE_CODE(table2->target)==SSA_NAME){
   //   fprintf(stderr, "phi target: ");debug_head(table2->target);
      if(ptr_derefs_may_alias_p(table2->target, table3->target)){
      //  fprintf(stderr, "phi use: ");debug_head(table2->target);
        if(!find_branch){
          for(int i=0; i<gimple_phi_num_args(table2->last_stmt); i++){
            if(gimple_phi_arg_edge(as_a <gphi *>(table2->last_stmt), i)->src->index == table3->bb->index){
              join_bb = table2->bb;
              find_branch=true;
              if(ptr_derefs_may_alias_p(gimple_phi_arg_def(as_a <gphi *>(table2->last_stmt), i), table3->target)){
                //search_alias_ptr_use(used_stmt, table2->target);
                //do_phi=true;
              }
              else{
                do_phi = false;
                break;
              }
            }
          //  else{
          //    search_alias_ptr_use(used_stmt, table2->target);
          //    break;
          //  }
          }
        }
        if(do_phi){
          search_alias_ptr_use(used_stmt, table2->target);
        }
      }
    }
    table2 = phitable;

    int succ_in_branch[n_basic_blocks_for_fn(cfun)]={0};
    int end_bb=0;
    for(int i=0; i<branch_i;i++){
      if(bb_in_loop_p(table3->bb->index, bb_loop, loop_i))
        break;
      if(table3->bb->index <= branch[i][0])
        continue;
      else if(table3->bb->index > branch[i][0] && table3->bb->index < branch[i][1]){
        if(end_bb==0)
          end_bb=branch[i][1];
        else{
          if(end_bb<branch[i][1])
            end_bb=branch[i][1];
        }
        trace_bb_succ(table3->bb, branch[i][1], succ_in_branch);
      }
    }
    for(used_stmt; used_stmt->stmt!=NULL && used_stmt->next!=NULL;used_stmt = used_stmt->next){
     // if(used_stmt->stmt == table3->last_stmt)
      if(gimple_code (used_stmt->stmt) ==GIMPLE_PHI || gimple_call_builtin_p (used_stmt->stmt, BUILT_IN_FREE))
        continue;
      if(table3->bb->index == gimple_bb(used_stmt->stmt)->index){
        if(LOCATION_LINE((table3->loc))<=LOCATION_LINE(gimple_location(used_stmt->stmt))){
     //     fprintf(stderr, "UAF check 0\n" );
          warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
          if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
        }
        else;
      }
      else if(succ_in_branch[0]!=0){
        int usi = gimple_bb(used_stmt->stmt)->index;
        if(usi>=end_bb){
          warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
          if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
        }
        else{
          for(int i=0;succ_in_branch[i]!=0;i++){
            if(usi == succ_in_branch[i]){
              warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
              if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
              break;
            }
          }
        }
      }
      else if(join_bb != NULL && join_bb->index <= gimple_bb(used_stmt->stmt)->index){
   //     fprintf(stderr, "UAF check 1\n" );
        warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
      }
      else if(join_bb != NULL && join_bb->index > gimple_bb(used_stmt->stmt)->index){
       // if(EDGE_SUCC(table3->bb, 0)->dest == gimple_bb(used_stmt->stmt))
       //   warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        edge e;
        edge_iterator ei;
        FOR_EACH_EDGE (e, ei, table3->bb->succs)
        {
         // fprintf(stderr, " %d ", e->dest->index);
          if(e->dest == gimple_bb(used_stmt->stmt)){
      //      fprintf(stderr, "UAF check 2\n" );
            warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
            if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
          }
        }
      }
      else if(!find_branch && LOCATION_LINE(table3->loc)<LOCATION_LINE(gimple_location(used_stmt->stmt))){
    //    fprintf(stderr, "UAF check 3\n" );
        warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
      }
    //  fprintf(stderr, "UAF check no %d-%d %d-%d\n", table3->bb->index, LOCATION_LINE(table3->loc), gimple_bb(used_stmt->stmt)->index,LOCATION_LINE(gimple_location(used_stmt->stmt)));
    }
    if(used_stmt->stmt!=NULL)
    {
      if(gimple_code (used_stmt->stmt) ==GIMPLE_PHI || gimple_call_builtin_p (used_stmt->stmt, BUILT_IN_FREE))
        ;
      if(gimple_bb(used_stmt->stmt) && table3->bb->index == gimple_bb(used_stmt->stmt)->index){
        if(LOCATION_LINE((table3->loc))<=LOCATION_LINE(gimple_location(used_stmt->stmt))){
    //      fprintf(stderr, "UAF check 0\n" );
          warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
          if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
        }
        else;
      }
      else if(succ_in_branch[0]!=0){
        int usi = gimple_bb(used_stmt->stmt)->index;
        if(usi>=end_bb){
          warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
          if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
        }
        else{
          for(int i=0;succ_in_branch[i]!=0;i++){
            if(usi == succ_in_branch[i]){
              warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
              if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
              ;
            }
          }
        }
      }
      else if(join_bb != NULL && join_bb->index <= gimple_bb(used_stmt->stmt)->index){
    //    fprintf(stderr, "UAF check 1\n" );
        warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
      }
      else if(join_bb != NULL && join_bb->index > gimple_bb(used_stmt->stmt)->index){
       // if(EDGE_SUCC(table3->bb, 0)->dest == gimple_bb(used_stmt->stmt))
       //   warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        edge e;
        edge_iterator ei;
        FOR_EACH_EDGE (e, ei, table3->bb->succs)
        {
         // fprintf(stderr, " %d ", e->dest->index);
          if(e->dest == gimple_bb(used_stmt->stmt)){
      //      fprintf(stderr, "UAF check 2\n" );
            warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
            if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
          }
        }
      }
      else if(!find_branch && LOCATION_LINE(table3->loc)<LOCATION_LINE(gimple_location(used_stmt->stmt))){
    //    fprintf(stderr, "UAF check 3\n" );
        warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
      }
    }
  }

  for(table3; (table3->target!=NULL_TREE/* && !table3->removed */&& table3->next!=NULL); table3=table3->next){
    to_remove=false;
    if(table3->removed || TREE_CODE(table3->target)!=SSA_NAME)
      continue;
    gimple_array start;
    start.stmt=NULL;
    gimple_array *used_stmt=&start;
  //  fprintf(stderr, "table3: ");debug_head(table3->target);
    search_alias_ptr_use(used_stmt, table3->target);
    for(table1; table1->target!=NULL_TREE && table1->next!=NULL; table1=table1->next){
      if(TREE_CODE(table1->target)!=SSA_NAME)
        continue;
   //   fprintf(stderr, "pointer target: ");debug_head(table1->target);
      if(ptr_derefs_may_alias_p(table3->target, table1->target)){
   //     fprintf(stderr, "pointer use: ");debug_head(table1->target);
        search_alias_ptr_use(used_stmt, table1->target);
      }
    }//fprintf(stderr, "last use: ");debug_gimple_stmt(used_stmt->stmt); 
    if(table1->target!=NULL_TREE && TREE_CODE(table1->target)==SSA_NAME)
      if(ptr_derefs_may_alias_p(table3->target, table1->target)){
     //   fprintf(stderr, "pointer use: ");debug_head(table1->target);
        search_alias_ptr_use(used_stmt, table1->target);
      }
    table1 = ptable;

    basic_block join_bb = NULL;
    bool find_branch = false;
    bool do_phi = true;

    for(table2; table2->target!=NULL_TREE && table2->next!=NULL && !find_branch; table2=table2->next){
      if(TREE_CODE(table2->target)!=SSA_NAME)
        continue;
     // fprintf(stderr, "phi target: ");debug_head(table2->target);
      if(ptr_derefs_may_alias_p(table2->target, table3->target)){
     //   fprintf(stderr, "phi use: ");debug_head(table2->target);
        if(!find_branch){
          for(int i=0; i<gimple_phi_num_args(table2->last_stmt); i++){
            if(gimple_phi_arg_edge(as_a <gphi *>(table2->last_stmt), i)->src->index == table3->bb->index){
              join_bb = table3->bb;
              find_branch=true;
              if(ptr_derefs_may_alias_p(gimple_phi_arg_def(as_a <gphi *>(table2->last_stmt), i), table3->target)){
                //search_alias_ptr_use(used_stmt, table2->target);
                //do_phi=true;
                ;
              }
              else{
                do_phi = false;
                break;
              }
            }
          //  else{
          //    search_alias_ptr_use(used_stmt, table2->target);
          //    break;
          //  }
          }
        }
        if(do_phi){
            search_alias_ptr_use(used_stmt, table2->target);
        }
      }
    }
    if(table2->target != NULL_TREE && TREE_CODE(table2->target)==SSA_NAME){
    //  fprintf(stderr, "phi target: ");debug_head(table2->target);
      if(ptr_derefs_may_alias_p(table2->target, table3->target)){
    //    fprintf(stderr, "phi use: ");debug_head(table2->target);
        if(!find_branch){
          for(int i=0; i<gimple_phi_num_args(table2->last_stmt); i++){
            if(gimple_phi_arg_edge(as_a <gphi *>(table2->last_stmt), i)->src->index == table3->bb->index){
              join_bb = table2->bb;
              find_branch=true;
              if(ptr_derefs_may_alias_p(gimple_phi_arg_def(as_a <gphi *>(table2->last_stmt), i), table3->target)){
                //search_alias_ptr_use(used_stmt, table2->target);
                //do_phi=true;
              }
              else{
                do_phi = false;
                break;
              }
            }
          //  else{
          //    search_alias_ptr_use(used_stmt, table2->target);
          //    break;
          //  }
          }
        }
        if(do_phi){
          search_alias_ptr_use(used_stmt, table2->target);
        }
      }
    }
    table2 = phitable;

    int succ_in_branch[n_basic_blocks_for_fn(cfun)]={0};
    int end_bb=0;
    for(int i=0; i<branch_i;i++){
      if(bb_in_loop_p(table3->bb->index, bb_loop, loop_i))
        break;
      if(table3->bb->index <= branch[i][0])
        continue;
      else if(table3->bb->index > branch[i][0] && table3->bb->index < branch[i][1]){
        if(end_bb==0)
          end_bb=branch[i][1];
        else{
          if(end_bb<branch[i][1])
            end_bb=branch[i][1];
        }
        trace_bb_succ(table3->bb, branch[i][1], succ_in_branch);
      }
    }
    for(used_stmt; used_stmt->stmt!=NULL && used_stmt->next!=NULL;used_stmt = used_stmt->next){
     // if(used_stmt->stmt == table3->last_stmt)
      if(gimple_code (used_stmt->stmt) ==GIMPLE_PHI || gimple_call_builtin_p (used_stmt->stmt, BUILT_IN_FREE))
        continue;
      if(table3->bb->index == gimple_bb(used_stmt->stmt)->index){
        if(LOCATION_LINE((table3->loc))<=LOCATION_LINE(gimple_location(used_stmt->stmt))){
    //      fprintf(stderr, "UAF check 0\n" );
          warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
          if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
        }
        else;
      }
      else if(succ_in_branch[0]!=0){
        int usi = gimple_bb(used_stmt->stmt)->index;
        if(usi>=end_bb){
          warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
          if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
        }
        else{
          for(int i=0;succ_in_branch[i]!=0;i++){
            if(usi == succ_in_branch[i]){
              warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
              if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
              break;
            }
          }
        }
      }
      else if(join_bb != NULL && join_bb->index <= gimple_bb(used_stmt->stmt)->index){
     //   fprintf(stderr, "UAF check 1\n" );
        warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
      }
      else if(join_bb != NULL && join_bb->index > gimple_bb(used_stmt->stmt)->index){
       // if(EDGE_SUCC(table3->bb, 0)->dest == gimple_bb(used_stmt->stmt))
       //   warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        edge e;
        edge_iterator ei;
        FOR_EACH_EDGE (e, ei, table3->bb->succs)
        {
         // fprintf(stderr, " %d ", e->dest->index);
          if(e->dest == gimple_bb(used_stmt->stmt)){
       //     fprintf(stderr, "UAF check 2\n" );
            warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
            if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
          }
        }
      }
      else if(!find_branch && LOCATION_LINE(table3->loc)<LOCATION_LINE(gimple_location(used_stmt->stmt))){
    //    fprintf(stderr, "UAF check 3\n" );
        warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
      }
   //   fprintf(stderr, "UAF check no %d-%d %d-%d\n", table3->bb->index, LOCATION_LINE(table3->loc), gimple_bb(used_stmt->stmt)->index,LOCATION_LINE(gimple_location(used_stmt->stmt)));
    }
    if(used_stmt->stmt!=NULL)
    {
      if(gimple_code (used_stmt->stmt) ==GIMPLE_PHI || gimple_call_builtin_p (used_stmt->stmt, BUILT_IN_FREE))
        continue;
      if(table3->bb->index == gimple_bb(used_stmt->stmt)->index){
        if(LOCATION_LINE((table3->loc))<=LOCATION_LINE(gimple_location(used_stmt->stmt))){
     //     fprintf(stderr, "UAF check 0\n" );
          warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
          if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
        }
        else;
      }
      else if(succ_in_branch[0]!=0){
        int usi = gimple_bb(used_stmt->stmt)->index;
        if(usi>=end_bb){
          warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
          if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
        }
        else{
          for(int i=0;succ_in_branch[i]!=0;i++){
            if(usi == succ_in_branch[i]){
              warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
              if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
              break;
            }
          }
        }
      }
      else if(join_bb != NULL && join_bb->index <= gimple_bb(used_stmt->stmt)->index){
    //    fprintf(stderr, "UAF check 1\n" );
        warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error!");
        if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
      }
      else if(join_bb != NULL && join_bb->index > gimple_bb(used_stmt->stmt)->index){
       // if(EDGE_SUCC(table3->bb, 0)->dest == gimple_bb(used_stmt->stmt))
       //   warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error");
        edge e;
        edge_iterator ei;
        FOR_EACH_EDGE (e, ei, table3->bb->succs)
        {
         // fprintf(stderr, " %d ", e->dest->index);
          if(e->dest == gimple_bb(used_stmt->stmt)){
       //     fprintf(stderr, "UAF check 2\n" );
            warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error!");
            if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
          }
        }
      }
      else if(!find_branch && LOCATION_LINE(table3->loc)<LOCATION_LINE(gimple_location(used_stmt->stmt))){
      //  fprintf(stderr, "UAF check 3\n" );
        warning_at(gimple_location(used_stmt->stmt), 0, "Used-after-free error!");
        if(!table3->removed){
      remove_pointer_stmt(table3->last_stmt);
      table3->removed=true;
    }
      }
    }
  }
}

void new_memory_leak_analysis (ptb *ptable, ptb *ftable){
	fprintf(stderr, "start new memory_leak_analysis\n");
	ptb *table1 = ptable;
	ptb *table3 = ftable;
	tree t,t2;
	int may=0;
	int must=0;
	int total=0;
	struct ptr_info_def* pi;
	struct pt_solution* pt;

	struct ptr_info_def *pi1;
	struct pt_solution *pt1;
		
	struct ptr_info_def *pi2;
	struct pt_solution *pt2;

	struct ptr_info_def *pi3;
	struct pt_solution *pt3;
  //phase1 check every alloc pair with one free
	FOR_EACH_TABLE(table1,t){
		if(TREE_CODE(TREE_TYPE(t)) == METHOD_TYPE 
		|| TREE_CODE(TREE_TYPE(t)) == FUNCTION_TYPE
		|| TREE_CODE(TREE_TYPE(t)) == RECORD_TYPE
		|| !(TREE_CODE(t) == SSA_NAME)){
			continue;
		}
		debug_head(t);
		pi1=SSA_NAME_PTR_INFO (table1->target);  // may nil binutils_leak_9.c:
		pt1=&pi1->pt;
		//if(MAIN_NAME_P(DECL_NAME (table1->node->decl)))
		if(pt1==NULL)
			continue;
		FOR_EACH_TABLE(table3,t2){
			if(table3->removed){
				continue;
			}
			if(t2==NULL){
				break;
			}
			if(TREE_CODE(table3->target)!=SSA_NAME){
				continue;
			}
			pi2=SSA_NAME_PTR_INFO (table3->target);
			pt2=&pi2->pt;
			//if(pt->null)
				//continue;
				//if(table1->state == POINTER_STATE_IS_FREE)
				//	warning_at(table1->loc, 0,"Memory Leak error");
			if(pt2->vars==NULL)
				continue;
			if(bitmap_intersect_p(pt1->vars,pt2->vars)){
				if(bb_in_loop_p(table1->bb) && !bb_in_same_loop_alloc_free(table1->bb,table3->bb)){
					warning_at(table1->loc, 1, "May Memory-Leak Error!: This alloc statement is in a loop which may cause a memory-leak error.");
					break;
				}

				warning_at(table1->loc,0,"");
				fprintf(stderr,"malloc ");debug(pt1);
				debug_head(t);
				
				warning_at(table3->loc,0,"");
				fprintf(stderr,"free ");debug(pt2);
				debug_head(t2);
		
				fprintf(stderr,"\n\n");
				
				gimple *def_stmt = SSA_NAME_DEF_STMT (table3->target);
				debug_tree(table3->target);	
				if(table3->inbranch){
					if(table1->state != POINTER_STATE_IS_FREE){
						fprintf(stderr,"inbranch \n");
						table1->state = POINTER_STATE_MAY_IS_FREE;
					}
				}else{
					debug_gimple_stmt(def_stmt);
					if(gimple_code (def_stmt) == GIMPLE_PHI){
						for(unsigned i=0;i<gimple_phi_num_args(def_stmt);i++){
							tree argu=gimple_phi_arg_def(def_stmt,i);
							fprintf(stderr,"phi_argu:%d ",i);
							debug_tree(argu);
							pi3=get_ptr_info (argu);
							pt3=&pi3->pt;
							if(pt3->vars==NULL)
								continue;
							if(!bitmap_intersect_p(pt2->vars,pt3->vars)){
								table1->state = POINTER_STATE_MAY_IS_FREE;
							}
						}
					}else{
						table1->state = POINTER_STATE_IS_FREE;
					}
				}
			}
		}
		if(table1->state != POINTER_STATE_IS_FREE){
			if(table1->state != POINTER_STATE_MAY_IS_FREE){
				warning_at(table1->loc, 0,"Memory Leak Error!");
				must++;
				fprintf(fp,"%s Memory Leak Error \n",DECL_SOURCE_FILE (table1->fun->decl)); //ppp
				//add_free_stmt(table1, table1->target);
				table1->state = POINTER_STATE_IS_FREE;
			}
			else{
				warning_at(table1->loc, 0,"May Memory Leak Error!");
				may++;
				fprintf(fp,"%s Memory Leak Error \n",DECL_SOURCE_FILE (table1->fun->decl));
				table1->state = POINTER_STATE_IS_FREE;

			}
		}
		fprintf(fp,"malloc: %d ",table1->loc);
		total++;
		table3 = ftable;
	}//phase1 end
	table1 = ptable;
	table3 = ftable;
	if(ptable->target!=NULL)
		//fprintf(fp,"%s Memory Leak Error \n",DECL_SOURCE_FILE (table1->fun->decl));
		fprintf(fp,"%s Memory Leak Error \n		Total:%d\n		May:%d\n		Must:%d\n",DECL_SOURCE_FILE (ptable->fun->decl),total,may,must);
	else
		fprintf(fp,"Memory Leak Error \n		Total:%d\n		May:%d\n		Must:%d\n",total,may,must);
  //phase2 check direct free for every alloc
	/*
	FOR_EACH_TABLE(table1,t){
		if(TREE_CODE(table1->target)!=SSA_NAME || table1->state == POINTER_STATE_IS_FREE)
		{
			continue;
		}

		gimple_array start;
		gimple* stmt;
		start.stmt=NULL;
		gimple_array *used_stmt=&start;
		//search_alias_ptr_use(used_stmt, table1->target);

		int now_bb = table1->bb->index;
		location_t now_loc=table1->loc; //gimple_location
		gimple* now_g;
		//search direct free
		FOR_EACH_USE_TABLE(used_stmt,stmt){
			//gimple_bb(SSA_NAME_DEF_STMT (gimple_phi_arg_def(phi, i))
			if(gimple_bb(stmt)->index > now_bb){
				now_bb=gimple_bb(stmt)->index;
				if(LOCATION_LINE(gimple_location(stmt))>LOCATION_LINE(now_loc)){
					now_loc=gimple_location(stmt);
					now_g=stmt;
				}
			}
		}
		//warning_at(gimple_location(now_g),0,"May Memory Leak Error: if %s is not free properly, it should add 'free(%s)' after line %d.",get_name(table1->target),get_name(table1->target),LOCATION_LINE(gimple_location(now_g)));
	}*/
}


void loop_check(ptb *ftable, int bb_loop[][2], int loop_index){
  ptb *temp = ftable;
  if(loop_index!=0)
  while(temp->target!=NULL_TREE && TREE_CODE(temp->target)==SSA_NAME){
    for(int i=0;i<loop_index;i++){
      if(temp->bb->index >= bb_loop[i][0] && temp->bb->index <= bb_loop[i][1]){
        if(TREE_CODE(TREE_TYPE(temp->target)) == METHOD_TYPE 
        || TREE_CODE(TREE_TYPE(temp->target)) == FUNCTION_TYPE
        || TREE_CODE(TREE_TYPE(temp->target)) == RECORD_TYPE
        || !(TREE_CODE(temp->target) == SSA_NAME))
          continue;
        //if(gimple_code(SSA_NAME_DEF_STMT(temp->target))==GIMPLE_PHI)
     //   if(!temp->removed)
     //     continue;
        if(!SSA_NAME_DEF_STMT(temp->target))
          continue;
        basic_block t = gimple_bb(SSA_NAME_DEF_STMT(temp->target));
        if(t==NULL)
          continue;
        int index = t->index;
        if(!(index >= bb_loop[i][0] && index <=  bb_loop[i][1])){
          warning_at(temp->loc, 0, "May double-free");
        //  debug_gimple_stmt(SSA_NAME_DEF_STMT(temp->target));
          continue;
        }
      }
    }
    if(temp->next!=NULL)
      temp = temp->next;
    else
      break;
  }
}


struct fdecl{
	tree fndecl;
	tree fn;
};

struct var_points_to{
	vector<vector<pair<fdecl,location_t>>> may_malloc_path;
	tree decl;
};

hash_map<tree,var_points_to> *tvpt;
bool path_intersection_tvpt(vector<pair<fdecl,location_t>> path,vector<pair<fdecl,location_t>> path2){
	vector<pair<fdecl,location_t>> temp;
	int ii=0;
	if (path.size()<path2.size()){
		temp=path2;
		path2=path;
		path=temp;
	}
	for(int i=0;i<path.size();){
		if(path.size()-i<path2.size())
			return false;

		fprintf(stderr,"=path_intersection_tvpt= ii=%d i=%d\n",ii,i);
		fprintf(stderr,"=path1ptr  %s  %d=\n",IDENTIFIER_POINTER (DECL_NAME(path[i+ii].first.fndecl)),LOCATION_LINE(path[i+ii].second));
		fprintf(stderr,"=path2ptr  %s  %d=\n",IDENTIFIER_POINTER (DECL_NAME(path2[ii].first.fndecl)),LOCATION_LINE(path2[ii].second));

		if(path[i+ii].first.fndecl == path2[ii].first.fndecl){
			if(path[i+ii].second == path2[ii].second){
				if(ii==path2.size()-1)
					return true;
				ii++;
				continue;
			}
		}
		if(ii>0)
			ii=0;
		return false;
	}
	return false;
}
bool same_path(tree t1,tree t2){
	if(tvpt->get(t1)==NULL || tvpt->get(t2)==NULL)
		return false;
	var_points_to vpt=*(tvpt->get(t1));
	var_points_to vpt2=*(tvpt->get(t2));
	int ii=0;
	vector<vector<pair<fdecl,location_t>>> path=vpt.may_malloc_path;
	vector<vector<pair<fdecl,location_t>>> path2=vpt2.may_malloc_path;

	for(int i=0;i<path.size();i++){
		for(int j=0;j<path2.size();j++){
			if (path_intersection_tvpt(path[i],path2[j])){
				return true;
			}
		}
	}
	return false;
	
	
}
bool path_intersection(vector<pair<fdecl,location_t>> *path,tree t,location_t loc){
	for(int i=0;i<(*path).size();i++){
		if((*path)[i].first.fndecl == t  && (*path)[i].second==loc){
			fprintf(stderr,"intersection\n");
			return true;
		}
	}
	return false;
}

void tracer_caller(cgraph_node *dest,cgraph_node *current,vector<pair<fdecl ,location_t>> *path);

void search_all_path(ptb *free_t,ptb *malloc_t){
	vector<pair<fdecl,location_t>> path;
	tracer_caller(free_t->node,malloc_t->node,&path);
}
void print_path(vector<pair<fdecl ,location_t>> *path){
	fprintf(stderr,"malloc from ->\n");
	for(int i=0;i<(*path).size();i++)
		fprintf(stderr,"	function ->%s in loc %d \n",IDENTIFIER_POINTER (DECL_NAME((*path)[i].first.fndecl)),LOCATION_LINE((*path)[i].second));
	
}
void print_tvpt(tree a){
	if(tvpt->get(a)==NULL)
		return ;
	var_points_to vpt=*(tvpt->get(a));
	vector<vector<pair<fdecl,location_t>>> path=vpt.may_malloc_path;
	vector<pair<fdecl,location_t>> loc;
	fprintf(stderr,"			=======print_tvpt %d   %d========\n",a,path.size());
	for(int i=0;i<path.size();i++){
		print_path(&path[i]);
	}

}
var_points_to path_merge(var_points_to lhs,var_points_to rhs){
	for(int i=0;i<rhs.may_malloc_path.size();i++){
		lhs.may_malloc_path.push_back(rhs.may_malloc_path[i]);
	}
	return lhs;
}
void test(tree target){
	tree a,tt;
	var_points_to vpt,lhs,rhs;
	gimple_array start;
	gimple *stmt;
	start.stmt=NULL;
	gimple_array *used_stmt=&start;
	search_alias_ptr_use(used_stmt, target);

	FOR_EACH_USE_TABLE(used_stmt,stmt){
		//gimple_bb(SSA_NAME_DEF_STMT (gimple_phi_arg_def(phi, i))
		if(gimple_code(stmt) == GIMPLE_ASSIGN ){
			a=gimple_assign_lhs(stmt);
			rhs=*(tvpt->get(target));

			tvpt->put(a,rhs);
			//put_all_use(a,rhs);
		}
		if(gimple_code(stmt) == GIMPLE_PHI){
			a=gimple_phi_result(stmt);
			debug_tree(a);
			fprintf(stderr,"address: %d\n",a);
			lhs=*(tvpt->get(a));
			rhs=*(tvpt->get(target));

			lhs=path_merge(lhs,rhs);
			tvpt->put(a,lhs);
			//put_all_use(a,rhs);
		} 
	}

}
int asd =2;
void tracer_caller(cgraph_node *dest,cgraph_node *current,vector<pair<fdecl ,location_t>> *path){
	fprintf(stderr,"======dest:%s current:%s\n",function_name (DECL_STRUCT_FUNCTION(dest->decl)),function_name (DECL_STRUCT_FUNCTION(current->decl)));
	//debug_tree(current->decl);
	cgraph_node *ftn;
	cgraph_node* node2;
	for (cgraph_edge *e = current->callers; e; e = e->next_caller){
		fdecl fd;
		gimple *ga=e->call_stmt;
		debug_gimple_stmt(ga);

		tree a=gimple_call_fn(ga);
		//debug_tree(a);
		fd.fn=a;
		tree b=gimple_call_fndecl(ga);
		fd.fndecl=b;
		//fprintf(stderr,"gimple_call_fn :%d \n",a);
		function *f;
		f=DECL_STRUCT_FUNCTION(gimple_call_fndecl(ga));

		location_t loc;
		loc=gimple_location(ga);
		
		//debug_head(a);
		if(f!=NULL){

			
			//fprintf(stderr,"function_name :%s \n",function_name (f));
			if(!path_intersection(path,b,loc)){
				vector<pair<fdecl ,location_t>> next=*path;

				next.push_back(make_pair(fd,loc));
				//next.push_back(make_pair(f->decl,loc));
				//fprintf(stderr,"f:%s loc:%d \n",IDENTIFIER_POINTER (DECL_NAME(next[0].first)),LOCATION_LINE(next[0].second));
				//print_path(&next);
				fprintf(stderr,"get(a) address: %d head:",a);
				debug_head(a);
				node2=*(fnode->get(a));
				ftn=node2;
				//fprintf(stderr,"f:%s loc:%d \n",IDENTIFIER_POINTER (DECL_NAME(next[0].first)),LOCATION_LINE(next[0].second));
				//fprintf(stderr,"dest %s %d , current %s %d , loc %d \n",function_name (DECL_STRUCT_FUNCTION(dest->decl)),dest->decl ,function_name (DECL_STRUCT_FUNCTION(ftn->decl)), ftn->decl,LOCATION_LINE(loc));
				if(dest->decl == ftn->decl){
					tree lhs=gimple_call_lhs (ga);
					print_path(&next);
					debug_head(gimple_call_lhs (ga));
					fprintf(stderr,"address :%d \n",gimple_call_lhs (ga));

					var_points_to a=*(tvpt->get(lhs));
					a.may_malloc_path.push_back(next);
					tvpt->put(lhs,a);
					//test(lhs);
					fprintf(stderr,"var_point_to address %d \n",a);
					//debug_gimple_stmt(ga);
					//debug_tree(gimple_call_lhs(ga));
					//record_ptr()
					continue;
					//break; //problem
				}
				if(gimple_call_lhs(ga)!=NULL){
					tree lhs=gimple_call_lhs(ga);
					gimple_array start;
					gimple* stmt;
					start.stmt=NULL;
					gimple_array *used_stmt=&start;
					search_alias_ptr_use(used_stmt, lhs);
					FOR_EACH_USE_TABLE(used_stmt,stmt){
						//gimple_bb(SSA_NAME_DEF_STMT (gimple_phi_arg_def(phi, i))
						if(gimple_code(stmt) == GIMPLE_RETURN){
							tracer_caller(dest,ftn,&next);
						}
					}
				}
			}
		}
	}

}
void tracer_callee(ptb *free,ptb *free2){
	
	for (cgraph_edge *e = free->node->callers; e; e = e->next_caller){
		function *f;
		gimple *ga=e->call_stmt;
		debug_gimple_stmt(ga);
		tree a=gimple_call_fndecl(ga);
		debug_head(a);
		f=DECL_STRUCT_FUNCTION(a);
		fprintf(stderr,"function_name :%s",function_name (f));
		debug_head(a);
		if(f!=NULL){
			debug_tree(f->decl);
			if(1)
			ptb caller;
			
		}
	}

}
void new_double_free_analysis(ptb *ptable, ptb *retable, ptb *ftable, ptb *phitable){
	
	fprintf(stderr, "start new double_free_analysis\n");
	ptb *malloc_t=ptable;
  	ptb *relloc_t=retable;
  	ptb *free_t=ftable;
  	ptb *free3_t=ftable;
  	ptb *free2_t;
	tree t,t2,t3;

	int may=0;
	int must=0;
	int total=0;
	gimple_array start;
	start.stmt=NULL;
	gimple_array *used_stmt=&start;
	struct ptr_info_def* pi;
	struct pt_solution* pt;
	struct ptr_info_def* pi2;
	struct pt_solution* pt2;
	//search_alias_ptr_use(used_stmt, malloc_t->target);
	FOR_EACH_TABLE(free_t,t){
			debug_head(t);
			total++;
    		if(TREE_CODE(free_t->target)!=SSA_NAME){
      			if(free_t->next!=NULL){
        			continue;
      			}
      			else{
        			break;
      			}
    		}
			pi= SSA_NAME_PTR_INFO (free_t->target);
			pt=&pi->pt;
			//debug(pt);
			if(pt->vars==NULL){
				
				fprintf(stderr,"points to is null\n");
				continue;
			}
			if(free_t->bb->loop_father->header->index!=0){
				warning_at(free_t->loc,0,"May Double Free Error!: This free statement is in a loop which may cause a double-free error.");
				may++;
				fprintf(fp,"%s May Double Free Error: This free statement is in a loop which may cause a double-free error.\n",DECL_SOURCE_FILE (ffun->decl));
				//continue;
			}
			//fprintf(stderr,"=======loop after========\n");
      		if(free_t->next!=NULL){
				//fprintf(stderr,"=======free2_t=free_t->next========\n");
        		free2_t=free_t->next;
      		}else{
				//fprintf(stderr,"=======break========\n");
				break;
			}
			//fprintf(stderr,"=======ftable2 before========\n");
    		FOR_EACH_TABLE(free2_t,t2){
				fprintf(stderr,"	");
				debug_head(t2);
      			if(TREE_CODE(free2_t->target)!=SSA_NAME){
        			if(free2_t->next!=NULL){
            				continue;
        			}
        			else{
						break;
        			}
      			}
				pi2= SSA_NAME_PTR_INFO (free2_t->target);
				pt2=&pi2->pt;
				//debug(pt2);
				//if(pt->null)
					//continue;
				unsigned int errorm=POINTER_NOT;
				if(free_t->bb->loop_father->header->index!=0){
					//fprintf(stderr,"bb_loop_header_p is exist\n");
					//warning_at(free_t->loc,0,"May Double-Free Error: This free statement is in a loop which may cause a double-free error.");
				}
				
				fprintf(stderr,"	=======search ftable2========\n");
				errorm=POINTER_NOT;
				if(bitmap_intersect_p(pt->vars,pt2->vars)){
					fprintf(stderr,"	=======bitmap_intersect_p========\n");
					
					if(free_t->fun==free2_t->fun){
						push_cfun(free_t->fun);
						calculate_dominance_info(CDI_DOMINATORS);
						dominated_by_p(CDI_DOMINATORS,free2_t->bb,free_t->bb);
						dominated_by_p(CDI_DOMINATORS,free_t->bb,free_t->fun->cfg->x_exit_block_ptr->prev_bb);
						if (dominated_by_p(CDI_DOMINATORS,free_t->bb,free_t->fun->cfg->x_exit_block_ptr->prev_bb) ^ dominated_by_p(CDI_DOMINATORS,free2_t->bb,free2_t->fun->cfg->x_exit_block_ptr->prev_bb)){
							errorm=POINTER_MAY;
						}
						else if (dominated_by_p(CDI_DOMINATORS,free_t->bb,free_t->fun->cfg->x_exit_block_ptr->prev_bb) && dominated_by_p(CDI_DOMINATORS,free2_t->bb,free2_t->fun->cfg->x_exit_block_ptr->prev_bb)){
							errorm=POINTER_MUST;
						}
						else if (!dominated_by_p(CDI_DOMINATORS,free_t->bb,free_t->fun->cfg->x_exit_block_ptr->prev_bb) && !dominated_by_p(CDI_DOMINATORS,free2_t->bb,free2_t->fun->cfg->x_exit_block_ptr->prev_bb)){
							if(fDFS->get(free_t->node)->is_succ(free_t->bb,free2_t->bb)){
								errorm=POINTER_MAY;
							}
						}
						free_dominance_info (CDI_DOMINATORS);
						pop_cfun();
						fprintf(stderr,"		=======search ptable %d %d========\n",free_t->fun,free2_t->fun);
						//do interprocedural allocation check
						if(ipa){
							FOR_EACH_TABLE(malloc_t,t3){
								if(ptr_derefs_may_alias_p(free_t->target,malloc_t->target)){
									if(free_t->node!=malloc_t->node){
										
										fprintf(stderr,"			=======free:");
										debug_head(free_t->target);
										fprintf(stderr,"			=======malloc:");
										debug_head(malloc_t->target);
										fprintf(stderr,"			=======search_all_path ========\n");
										search_all_path(free_t,malloc_t);
										fprintf(stderr,"			=======print_tvpt %d========\n",free_t->target);
										debug_head(free_t->target);
										print_tvpt(free_t->target);
										fprintf(stderr,"			=======print_tvpt %d========\n",free2_t->target);
										debug_head(free2_t->target);
										print_tvpt(free2_t->target);
										if(!same_path(free_t->target,free2_t->target))
											errorm=POINTER_NOT;  //problem
										}
								}
							}
						}
						malloc_t=ptable;
						fprintf(stderr,"		=======search ptable end %d========\n",errorm);
						
						
						fprintf(stderr,"	=======search ftable2 end========\n");
					}
					else{
						errorm=POINTER_MAY;
					}
					switch(errorm){
						case POINTER_MUST:
							warning_at(free_t->loc,0,"Double free error!");
							warning_at(free2_t->loc,0,"");
							fprintf(fp,"%s Double-free must\n",DECL_SOURCE_FILE (ffun->decl));
							
							must++;
							break;
						case POINTER_MAY:
							warning_at(free_t->loc,0,"May Double free error!");
							warning_at(free2_t->loc,0,"");
							fprintf(fp,"%s Double-free may\n",DECL_SOURCE_FILE (ffun->decl));
							may++;
							break;
						default:						
							break;
					}
					FOR_EACH_TABLE(malloc_t,t3){
						pi2= SSA_NAME_PTR_INFO (malloc_t->target);
						pt2=&pi2->pt;
						if(bitmap_intersect_p(pt->vars,pt2->vars)){
							warning_at(malloc_t->loc,0,"malloc location \n");
						}
					}
					malloc_t=ptable;
				/*
				fprintf(stderr,"\n%d:%d\n",free_t->bb->index,free2_t->bb->index);
				warning_at(free_t->loc,0,"loc: %d",free_t->bb->index);
				warning_at(free2_t->loc,0,"loc: %d",free2_t->bb->index);
				if (dominated_by_p(CDI_DOMINATORS,free2_t->bb,free_t->bb)){
					if()
					warning_at(free_t->loc,0,"Double-free %d",free_t->bb->index);
					warning_at(free2_t->loc,0,"Double-free %d",free2_t->bb->index);
					debug_head(free_t->target);
					struct pt_solution *pt=&pi1->pt;
					//debug(&pi1->pt);
					FILE *file;
					dump_decl_set (stderr,pt->vars);
					fprintf(stderr,"\n");
					debug_head(free2_t->target);
					debug(&pi2->pt);
					fprintf(stderr,"\n");
					if(pt_solutions_intersect (&pi1->pt,&pi2->pt)){
						
						gimple_array *used_stmt;
						gimple_array gstart;
	      					gstart.stmt = NULL;
	      					used_stmt = &gstart;
	      					//search_ptr_use(used_stmt, free_t->target);
						fprintf(stderr,"~.~\n");
	      					//search_ptr_use(used_stmt, free2_t->target);
					}
					fprintf(stderr,"----------------\n");
					break;
				}
				if(et_below(n2,n1)){
					warning_at(free_t->loc,0,"May Double-free %d",free_t->bb->index);
					warning_at(free2_t->loc,0,"May Double-free %d",free2_t->bb->index);
				}
				if(g.is_succ(free_t->bb,free2_t->bb)){
					warning_at(free_t->loc,0,"May Double-free %d",free_t->bb->index);
					warning_at(free2_t->loc,0,"May Double-free %d",free2_t->bb->index);
				}
				*/
			}
    	}
	}
	if(ffun!=NULL)
		//fprintf(fp,"%s Memory Leak Error \n",DECL_SOURCE_FILE (table1->fun->decl));
		fprintf(fp,"%s Double free Error \n		Total free:%d\n		May df:%d\n		Must df:%d\n",DECL_SOURCE_FILE (ffun->decl),total,may,must);
	else
		fprintf(fp,"Double free Error \n		Total free:%d\n		May df:%d\n		Must df:%d\n",total,may,must);
}
void double_free_analysis(ptb *ptable, ptb *retable, ptb *ftable, ptb *phitable, int bb_loop[][2], int loop_i, int branch[][2], int branch_i){
//check every free pointer is alloc
 // loop_check(ftable, bb_loop);
 // fprintf(stderr, "start double-free-analysis\n");
  ptb *temp1=ptable;
  ptb *temp2=retable;
  ptb *freet=ftable;
  ptb *temp =ftable;
//check every free stmt points to a storage
  while(freet->target!=NULL_TREE){
    if(TREE_CODE(freet->target)!=SSA_NAME){
      if(freet->next!=NULL){
        freet=freet->next;
        temp1=ptable;
        continue;
      }
      else{
        temp1=ptable;
        break;
      }
    }

    while(temp1->target!=NULL_TREE){
      if(TREE_CODE(temp1->target)!=SSA_NAME){

        //remove_pointer_stmt(freet->last_stmt);
        if(temp1->next!=NULL){
            temp1 = temp1->next;
            continue;
        }
        else{
          warning_at(freet->loc, 0, "Undefined behavior: %s doesn't point to a storage", get_name(freet->target));
          freet->removed = true;
          remove_pointer_stmt(freet->last_stmt);
        }
   //     freet = temp->next;
      }
      if(ptr_derefs_may_alias_p(temp1->target, freet->target)){
        if(bb_in_loop_p(freet->bb->index, bb_loop, loop_i)){
          if(gimple_bb(SSA_NAME_DEF_STMT(freet->target)) != freet->bb)
            warning_at(freet->loc, 0, "May Double-Free Error!: This free statement is in a loop which may cause a double-free error.");
        }
        /*fprintf(stderr, "test:\n");
        if(TREE_CODE(temp1->target)==SSA_NAME)
        debug_head(temp1->target);
        debug_head(freet->target);*/
        if(freet->next!=NULL){
          temp =freet;
          freet=freet->next;
          temp1=ptable;
        }
        else{
          temp1=ptable;
          break;
        }
      }
      else if(temp1->next!=NULL){
        temp1 = temp1->next;
      }
      else{
        warning_at(freet->loc, 0, "Undefined behavior: %s doesn't point to a storage", get_name(freet->target));
        freet->removed=true;
        remove_pointer_stmt(freet->last_stmt);
 /*       gimple_stmt_iterator remove_stmt;
  if(gimple_code(freet->last_stmt)==GIMPLE_CALL){
    remove_stmt = gsi_for_stmt(freet->last_stmt);
    fprintf(stderr, "remove stmt: " );debug_gimple_stmt(gsi_stmt(remove_stmt));
    basic_block modified_bb = gimple_bb (freet->last_stmt);
    gsi_remove (&remove_stmt, true);
  }
  else if(gimple_code(freet->last_stmt)==GIMPLE_PHI){
    remove_stmt = gsi_after_labels (gimple_bb(freet->last_stmt));
    fprintf(stderr, "remove stmt: " );debug_gimple_stmt(gsi_stmt(remove_stmt));
    basic_block modified_bb = gimple_bb (freet->last_stmt);
    gsi_remove (&remove_stmt, true);
  }*/
 // if(remove_stmt != NULL){
 //   fprintf(stderr, "remove stmt: " );debug_gimple_stmt(gsi_stmt(remove_stmt));
 //   basic_block modified_bb = gimple_bb (freet->last_stmt);
 //   gsi_remove (&remove_stmt, true);}
        if(freet==ftable){
          if(freet->next!=NULL)
            freet=freet->next;
            //ftable = freet->next;
          else{
            freet->removed=true;
          }
            //ftable = NULL;
     //     freet = temp->next;
        //  remove_pointer_stmt(freet->last_stmt);
        }
        else if(freet->next==NULL){
          temp->next = NULL;
        }
        else{
          temp->next = freet->next;
        }
       /* gimple_stmt_iterator remove_stmt = gsi_for_stmt(freet->stmt);
        fprintf(stderr, "remove stmt: " );debug_gimple_stmt(gsi_stmt(remove_stmt));
        basic_block modified_bb = gimple_bb (freet->stmt);
        gsi_remove (&remove_stmt, true);
        unlink_stmt_vdef (freet->stmt);*/
      }
    }
    if(freet->next!=NULL){
      temp =freet;
      freet=freet->next;
      temp1=ptable;
    }
    else{
      temp1=ptable;
      break;
    }
  }
//  if(ptable!=NULL && ptable->target!=NULL_TREE)
//    fprintf(stderr, "ptable: ");debug_head(ptable->target);
//Do realloc check

//search alias free stmt
  freet=ftable;
  //temp =freet->next;if(TREE_CODE(  (temp1->target) )==SSA_NAME && TREE_CODE(  (temp2->target) )==SSA_NAME){
  //      pi1 = SSA_NAME_PTR_INFO (temp1->target);
  //      pi2 = SSA_NAME_PTR_INFO (temp2->target);
  //    }
  
  while(freet->target!=NULL_TREE){
 //   fprintf(stderr, "0\n");
    if(freet->removed || TREE_CODE(freet->target)!=SSA_NAME || bb_in_loop_p(freet->bb->index, bb_loop, loop_i)){
      if(freet->next!=NULL){
        freet = freet->next;
  //      fprintf(stderr, "1 ");
   //     debug_head(freet->target);
        continue;
      }
      else {
  //      fprintf(stderr, "2 ");
  //        debug_head(freet->target);
        break;
      }
    }

    if(freet->next!=NULL)
      temp = freet->next;
    else 
      break;

    while(temp->target!=NULL_TREE){
  //    fprintf(stderr, "00\n");
      if(temp->removed || TREE_CODE(temp->target)!=SSA_NAME || bb_in_loop_p(temp->bb->index, bb_loop, loop_i)){
        if(temp->next!=NULL){
          temp = temp->next;
 //         fprintf(stderr, "3 ");
 //         debug_head(temp->target);
           continue;
        }
        else{
  //        fprintf(stderr, "4 ");debug_head(temp->target);
          break;
        }
      }
    //  if(bb_in_loop_p(table3->bb->index, bb_loop, loop_i))
    //    continue;
      if(ptr_derefs_may_alias_p(freet->target, temp->target)){
        if(freet->bb == temp->bb){
         // if(freet->loc.line <= temp->loc.line){
         // fprintf(stderr, "line %d %d\n", LOCATION_LINE(freet->loc), LOCATION_LINE(temp->loc));
          if(LOCATION_LINE(freet->loc) <= LOCATION_LINE(temp->loc)){
            if(!freet->last_stmt)
              break;

            warning_at(freet->loc, 0, "Double-free1");
            warning_at(temp->loc, 0, "Double-free1");
            if(!freet->removed)
            remove_pointer_stmt(freet->last_stmt);
            freet->removed=true;
            break;
          }
          else{
            warning_at(temp->loc, 0, "Double-free2");
            warning_at(freet->loc, 0, "Double-free2");          
            remove_pointer_stmt(temp->last_stmt);
            temp->removed=true;

            break;
            }
            
          }
        
        /*else{
          if(freet->bb == ffun->cfg->x_entry_block_ptr->next_bb){
            warning_at(freet->loc, 0, "Double-free"); 
            warning_at(temp->loc, 0, "Double-free");         
            remove_pointer_stmt(freet->last_stmt);
            freet->removed=true;
          }
          else if(temp->bb == ffun->cfg->x_entry_block_ptr->next_bb){
            warning_at(temp->loc, 0, "Double-free"); 
            warning_at(freet->loc, 0, "Double-free");         
            remove_pointer_stmt(temp->last_stmt);
            temp->removed=true;
          }*//*if(freet->bb == ffun->cfg->x_exit_block_ptr->prev_bb){
            warning_at(temp->loc, 0, "Double-free"); 
            warning_at(freet->loc, 0, "Double-free");         
            remove_pointer_stmt(temp->last_stmt);
            temp->removed=true;
          }
          else if(temp->bb == ffun->cfg->x_exit_block_ptr->prev_bb){
            warning_at(freet->loc, 0, "Double-free"); 
            warning_at(temp->loc, 0, "Double-free");         
            remove_pointer_stmt(freet->last_stmt);
            freet->removed=true;
          }*/
          else{
            int succ_in_branch[n_basic_blocks_for_fn(ffun)]={0};
            int end_bb=0;
            for(int i=0; i<branch_i;i++){
            //  if(bb_in_loop_p(table3->bb->index, bb_loop, loop_i))
            //    continue;
              if(freet->bb->index <= branch[i][0])
                break;
              else if(freet->bb->index > branch[i][0] && freet->bb->index < branch[i][1]){
                if(temp->bb->index>branch[i][0] && temp->bb->index<branch[i][1]){
                  if(end_bb==0)
                    end_bb=branch[i][1];
                  else{
                    if(end_bb<branch[i][1])
                      end_bb=branch[i][1];
                  }
                  if(end_bb!=0){
                    break;
                  }
                }
              }
            }
            if(end_bb!=0){
              bool result=false;
              if(freet->bb->index < temp->bb->index){
                trace_bb_succ(freet->bb, end_bb, succ_in_branch);
                for(int j=0;succ_in_branch[j]!=0;j++){
                  if(temp->bb->index == succ_in_branch[j]){
                    result = true;break;
                  }
                }
                if(result){
                  warning_at(freet->loc, 0, "Double-free3");   
                  warning_at(temp->loc, 0, "Double-free3"); 
                  temp->removed=true;
                  remove_pointer_stmt(freet->last_stmt);
                  break;
                }
              }
              else{
                trace_bb_succ(temp->bb, end_bb, succ_in_branch);
                for(int j=0;succ_in_branch[j]!=0;j++){
                  if(freet->bb->index == succ_in_branch[j]){
                    result = true;break;
                  }
                }
                if(result){
                  warning_at(temp->loc, 0, "Double-free4"); 
                  warning_at(freet->loc, 0, "Double-free4");  
                  temp->removed=true; 
                  remove_pointer_stmt(temp->last_stmt);
                  break;
                }
              }
            }
            //search phi for the edge to the free stmt then
         //   ptb *phi1, *phi2;
          //  else if(LOCATION_LINE(freet->loc) <= LOCATION_LINE(temp->loc)){
            else if(freet->bb->index < temp->bb->index){
              //if(!single_succ_p (PRED_EDGE(freet->bb,0)) || !single_pred_p(SUCC_EDGE(freet->bb, 0)))//branch judgement
              if(!search_phi(freet, temp, phitable)){
       //         fprintf(stderr, "error type\n");
                warning_at(freet->loc, 0, "Double-free5"); 
                warning_at(temp->loc, 0, "Double-free5"); 
                if(TREE_CODE(TREE_TYPE(freet->target)) != METHOD_TYPE 
                && TREE_CODE(TREE_TYPE(freet->target)) != FUNCTION_TYPE
                && TREE_CODE(TREE_TYPE(freet->target)) != RECORD_TYPE
                && (TREE_CODE(freet->target) == SSA_NAME)
                && !freet->removed)
                  {
                    fprintf(stderr, "do remove\n");
               // if(!freet->removed && freet->last_stmt)  
              // if(freet->last_stmt!=NULL)      
                remove_pointer_stmt(freet->last_stmt);
                freet->removed=true;
                break;
                }
              }
              
            }
            else if(freet->bb->index > temp->bb->index){
              if(!search_phi(temp, freet, phitable)){
                
                warning_at(temp->loc, 0, "Double-free6"); 
                warning_at(freet->loc, 0, "Double-free6");   
               /* if(TREE_CODE(TREE_TYPE(temp->target)) == METHOD_TYPE 
                || TREE_CODE(TREE_TYPE(temp->target)) == FUNCTION_TYPE
                || TREE_CODE(TREE_TYPE(temp->target)) == RECORD_TYPE
                || !(TREE_CODE(temp->target) == SSA_NAME))
                  continue; 
                if(!temp->removed )   */   
                remove_pointer_stmt(temp->last_stmt);
                temp->removed=true;
                break;
              }
            }
          }
        }
      
      if(temp->next!=NULL)
        temp = temp->next;
      else
        break;
    }
    if(freet->next!=NULL)
      freet = freet->next;
    else
      break;
  }
 // 
}
void print_pointer_table_new(ptb *ptable){
	ptb *temp1=ptable;
	while(temp1->target!=NULL_TREE ){
		if(get_name(temp1->target)!=NULL){
			debug_head(temp1->target);
			warning_at(temp1->loc,0, "%s", get_name(temp1->target));
			struct ptr_info_def *pi=SSA_NAME_PTR_INFO (temp1->target);
			struct pt_solution *pt=&pi->pt;
			debug(pt);
			
			fprintf(stderr,"address:%d \n",temp1->target);
		}
		else {
			debug_head(temp1->target);
			warning_at(temp1->loc,0, "NULL");
			struct ptr_info_def *pi=SSA_NAME_PTR_INFO (temp1->target);
			struct pt_solution *pt=&pi->pt;
			debug(pt);
			fprintf(stderr,"address:%d \n",temp1->target);
		}
		if(temp1->next!=NULL)
			temp1=temp1->next;
		else break;
	}
}
bool cgraph_path_tracker(ptb *p1,ptb *p2){
	fprintf(stderr,"=======orginal=========\n");
	function *f1,*f2;
	f1=p1->node->get_fun();
	f2=p2->node->get_fun();
	if(f1==f2){
		fprintf(stderr,"f1:  ");
		debug_head(f1->decl);
		fprintf(stderr,"\nf2:  ");
		debug_head(f2->decl);
		fprintf(stderr,"\n  ");
		if(et_below(p1->bb->dom[0],p2->bb->dom[0])){
			fprintf(stderr,"=======cgraph_path_tracker_end=========\n");
			return true;}
	}
	for (cgraph_edge *e = p1->node->callers; e; e = e->next_caller){
		fprintf(stderr,"=======f1 callers=========\n");/*
		f1=e->get_fun();
		f2=p2->node->get_fun();
		fprintf(stderr,"f1: %s \nf2: %s\n",f1->decl->get_name(),f1->decl->get_name());
		if(e->get_fun()==p2->node->get_fun())
			if(et_below())*/
		debug_head(e->caller->get_fun()->decl);
		debug_head(e->callee->get_fun()->decl);
	}
	fprintf(stderr,"=======cgraph_path_tracker_end=========\n");
	return true;
}
bool interdom_p(tree a,tree b){
	function *f1=DECL_STRUCT_FUNCTION (a);
	function *f2=DECL_STRUCT_FUNCTION (b);
	
	
	return true;

}
void init_table(){
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
}

void always_inline_free_malloc(ptb p1){
	/*
	tree caller_tree = DECL_FUNCTION_SPECIFIC_OPTIMIZATION (caller->decl);

	DECL_DISREGARD_INLINE_LIMITS(callee->decl)=1;
	for (cgraph_edge *e = p1->node->callees; e; e = e->next_callee){
		struct cgraph_node *callee = e->callee->ultimate_alias_target ();
	}*/
}
void alloc_detect(){
  /*
  bb_file bbtable[n_basic_blocks_for_fn(ffun)];
  for (int i = 0; i < n_basic_blocks_for_fn(ffun); ++i)
  {
    bbtable[i].bb = NULL;
    bbtable[i].path_size = i+1;
    bbtable[i].pred_path[i+1] ={-1};
  }*/
	ffun=cfun;
	//fprintf(stderr,"BB:%d\n",ffun->cfg->x_last_basic_block);
	int loop_index=0;
	basic_block bb;
	int bb_loop[n_basic_blocks_for_fn(ffun)][2]={0};
	int branch_clique[n_basic_blocks_for_fn(ffun)+2][2]={0};
	int branch_i = 0; 
	int stack[n_basic_blocks_for_fn(ffun)][2], stack_i=0; 
	Graph g; 
	g.init_Graph(ffun->cfg->x_last_basic_block-2);
	init_table();
	if(ipa_pta)
		init_table();
	cgraph_node *node=cgraph_node::get (ffun->decl);

	FOR_EACH_BB_FN (bb, ffun)
	{
	 	/*每個BB的前一個BB的index*/
		fprintf(stderr, "======bb %d start======\n", bb->index); //
		// set_bb_file(bbtable, bb);
		if(bb != ffun->cfg->x_entry_block_ptr->next_bb){
			//fprintf(stderr, "pred := ");
			edge e;
			edge_iterator ei;
			FOR_EACH_EDGE (e, ei, bb->preds)
			{
				fprintf(stderr, " %d ", e->src->index );
			}
			//fprintf(stderr, "\n");
		}
		/*每個BB的後一個BB的index*/
		if(bb != ffun->cfg->x_exit_block_ptr->prev_bb){//
			//if(EDGE_COUNT(bb->succs)>1)
				//branch_counter = branch_counter-1+EDGE_COUNT(bb->succs);
			fprintf(stderr, "succ := "); 
			edge e;
			edge_iterator ei;
			FOR_EACH_EDGE (e, ei, bb->succs)
			{
				fprintf(stderr, " %d ", e->dest->index);
				g.addEdge(bb->index-2,e->dest->index-2);
				//fprintf(stderr,"(%d->%d)",bb->index-2,e->dest->index-2);
				if(e->dest->index <= bb->index){
					bb_loop[loop_index][0]=e->dest->index;
					bb_loop[loop_index][1]=bb->index;
					loop_index++;
					//fprintf(stderr, " %d -> %d is loop", e->dest->index, bb->index);
					//break;
				}
				
			}
			//fprintf(stderr, "\n");
		}
		//fprintf(stderr, "BRANCH_EDGE:%d\n",BRANCH_EDGE(bb));
		//if(EDGE_COUNT(bb->succs)>1)
			//fprintf(stderr, "fallthru edge %d\n", FALLTHRU_EDGE(bb)->dest->index);
		
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
					set_ptb(bb, ftable, gimple_call_arg(stmt, 0), loc, POINTER_STATE_IS_FREE, stmt,node);
					//fprintf(stderr, "free_bb_index:%d\n",bb->index);
					//if (gimple_code (SSA_NAME_DEF_STMT (gimple_call_arg(stmt, 0))) == GIMPLE_PHI)
						//fprintf(stderr, "free phi\n");
				}
			/*MALLOC分析*/
				if (gimple_call_builtin_p (stmt, BUILT_IN_MALLOC)|| gimple_call_builtin_p (stmt, BUILT_IN_CALLOC)){
					set_ptb(bb, ptable, gimple_call_lhs(stmt), loc, POINTER_STATE_IS_MALLOC, stmt,node);
					//debug_tree(gimple_call_lhs(stmt));
				}
			/*REALLOC分析*/
				if (gimple_call_builtin_p (stmt, BUILT_IN_REALLOC))
					set_ptb(bb, retable, gimple_call_arg(stmt, 0), loc, POINTER_STATE_IS_MALLOC, stmt,node);
				}else{}
				/*return分析*/
				if(gimple_code(stmt) == GIMPLE_RETURN){
					if(gimple_return_retval (as_a <greturn *>(stmt))!=NULL){
						if(TREE_CODE(TREE_TYPE(gimple_return_retval (as_a <greturn *>(stmt))))==POINTER_TYPE){
							set_ptb(bb, return_table, gimple_return_retval (as_a <greturn *>(stmt)), loc, POINTER_STATE_IS_MALLOC, stmt,node);
						}
					}
				//fprintf(stderr,"GIMPLE_RETURN \n");
				//debug_tree(gimple_return_retval (as_a <greturn *>(stmt)));
				}
			/*fopen分析*//*
			if(is_gimple_call(stmt)){
				//fprintf(stderr,"gimple_call_fn :%s\n",get_name(gimple_call_fn(stmt)));
				if(strcmp(get_name(gimple_call_fn(stmt)),"fopen")==0){
					set_ptb(bb, fopen_table, gimple_call_arg(stmt, 0), loc, POINTER_STATE_IS_FILE, stmt);
					//fprintf(stderr,"fopen\n");
				}
				//debug_tree(gimple_call_fn(stmt));
			}*/

			final_gsi = gsi;
			final_stmt = stmt;
		}
		/*訪問所有擁有phi的BB*/
		for (gphi_iterator gsi = gsi_start_phis (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			gphi *phi = gsi.phi ();
			//debug_gimple_stmt(phi);/*印出擁有phi的BB*/
			for(int i=0;i<gimple_phi_num_args(phi);i++){
				//fprintf(stderr, "~%d ", gimple_phi_arg_edge(phi, i)->src->index);/*印出phi決定的兩個變數的變數的BB的index*/
				if(TREE_CODE(TREE_TYPE(gimple_phi_arg_def(phi, i))) == METHOD_TYPE 
				|| TREE_CODE(TREE_TYPE(gimple_phi_arg_def(phi, i))) == FUNCTION_TYPE
				|| TREE_CODE(TREE_TYPE(gimple_phi_arg_def(phi, i))) == RECORD_TYPE
				|| !(TREE_CODE(gimple_phi_arg_def(phi, i)) == SSA_NAME))
					continue;
				basic_block phi_bb = gimple_bb(SSA_NAME_DEF_STMT (gimple_phi_arg_def(phi, i)));
				//if(phi_bb!=NULL	&& phi_bb != ffun->cfg->x_entry_block_ptr->next_bb&& phi_bb != ffun->cfg->x_exit_block_ptr->prev_bb);
				//fprintf(stderr, "-%d ", phi_bb->index);/*印出phi決定的兩個變數的變數的BB的index*/
				//debug_head(gimple_phi_arg_def(phi, i));/*印出phi決定的兩個變數*/
			}
			if(search_table(ptable, gimple_phi_result(phi), false, 0, phi) == POINTER_STATE_IS_MALLOC){
          			set_ptb(bb, phitable, gimple_phi_result(phi), gimple_location (phi), POINTER_STATE_IS_MALLOC, phi);
			}
		}
		
	}
	
	FOR_EACH_BB_FN (bb, ffun)
	{
		//fprintf(stderr, "1 %d %d\n",n_basic_blocks_for_fn(ffun), branch_i);
		if(bb_in_loop_p(bb->index, bb_loop, loop_index))
			continue;
		//fprintf(stderr, "2 %d %d\n",bb->index, branch_i);
		for (gphi_iterator gsi = gsi_start_phis (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			gphi *phi = gsi.phi ();
			if (virtual_operand_p (gimple_phi_result (phi))){
				//fprintf(stderr, "find virtual\n");    //
				branch_clique[branch_i][1] = bb->index;
				int get_e = EDGE_COUNT(bb->preds);
				while(stack_i>0 && get_e != 1){
					//fprintf(stderr, "get_e:%d ", get_e);
					get_e=get_e-stack[stack_i-1][1]+1;
					stack_i--;
				}
				branch_clique[branch_i][0] = stack[stack_i][0];
				//fprintf(stderr, "\nbranch bb start to end  %d-> %d\n", branch_clique[branch_i][0],branch_clique[branch_i][1]);
				branch_i++;
			}
		}//fprintf(stderr, "test\n");
		//fprintf(stderr, "3 %d %d\n",bb->index, branch_i);
		if(bb != ffun->cfg->x_exit_block_ptr->prev_bb){
			if(EDGE_COUNT(bb->succs)>1){
				stack[stack_i][0]=bb->index;
				stack[stack_i][1]=EDGE_COUNT(bb->succs);
				//fprintf(stderr, "start branch bb :%d succs:%d\n", stack[stack_i][0],stack[stack_i][1]);
				stack_i++;
			}
		}
		//fprintf(stderr, "4 %d %d\n",bb->index, branch_i);
	}
	
 	fprintf(stderr,"=============table==============\n");
 	fprintf(stderr,"print_pointer_table :ptable\n");
	print_pointer_table_new(ptable);
	//fprintf(stderr,"print_pointer_table :retable\n");
	//print_pointer_table_new(retable);
	fprintf(stderr,"print_pointer_table :ftable\n");
	print_pointer_table_new(ftable);
	fprintf(stderr,"print_pointer_table :phitable\n");
	print_pointer_table_new(phitable);
	fprintf(stderr,"print_pointer_table :return_table\n");
	print_pointer_table_new(return_table);
	fprintf(stderr,"print_pointer_table :FOPEN_table\n");
	print_pointer_table_new(fopen_table);
 	fprintf(stderr,"=============table==============\n");
	
	/*
	if(name!=main){
		set_all_ptb();
	}*/
	
	//loop_check(ftable, bb_loop, loop_index);
	//double_free_analysis(ptable, retable, ftable, phitable, bb_loop, loop_index, branch_clique, branch_i);
	//new_double_free_analysis(ptable, retable, ftable, phitable);
	//use_after_free_analysis(ptable, retable, ftable, phitable, bb_loop, loop_index, branch_clique, branch_i);
	//new_use_after_free_analysis(ptable, ftable);
	new_memory_leak_analysis (ptable,ftable);
	//memory_leak_analysis(ptable, retable, ftable, phitable,return_table, bb_loop, loop_index, branch_clique, branch_i);
	function_number+=1;
	//fprintf(stderr, "Analysis Finish : %d \n",function_number);
	

}

