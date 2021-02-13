/*   
 *   File: bst_howley.c
 *   Author: Balmau Oana <oana.balmau@epfl.ch>, 
 *  	     Zablotchi Igor <igor.zablotchi@epfl.ch>, 
 *  	     Tudor David <tudor.david@epfl.ch>
 *   Description: Shane V Howley and Jeremy Jones. 
 *   A non-blocking internal binary search tree. SPAA 2012
 *   bst_howley.c is part of ASCYLIB
 *
 * Copyright (c) 2014 Vasileios Trigonakis <vasileios.trigonakis@epfl.ch>,
 * 	     	      Tudor David <tudor.david@epfl.ch>
 *	      	      Distributed Programming Lab (LPD), EPFL
 *
 * ASCYLIB is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "bst_howley.h"
#include "../../../genmc/include/genmc.h"

__thread ssmem_allocator_t* alloc;

const sval_t val_mask = ~(0x3);

node_t* create_node(skey_t key, sval_t value, int initializing) {
    volatile node_t * new_node;
#if GC == 1
    if (unlikely(initializing)) {
        new_node = (volatile node_t*) ssalloc_aligned(CACHE_LINE_SIZE, sizeof(node_t));
    } else {
        new_node = (volatile node_t*) ssmem_alloc(alloc, sizeof(node_t));
    }
#else
    new_node = (volatile node_t*) malloc(sizeof(node_t));
#endif
    if (new_node==NULL) {
        assert(0);
    }
	atomic_store_explicit(&new_node->key, key, memory_order_seq_cst);
	atomic_store_explicit(&new_node->value, value, memory_order_seq_cst);
	atomic_store_explicit(&new_node->op, NULL, memory_order_seq_cst);
	atomic_store_explicit(&new_node->right, NULL, memory_order_seq_cst);
	atomic_store_explicit(&new_node->left, NULL, memory_order_seq_cst);

    asm volatile("" ::: "memory");
    return (node_t*) new_node;
}

operation_t* alloc_op() {
    volatile operation_t * new_op;
#if GC == 1
    new_op = (volatile operation_t*) ssmem_alloc(alloc, sizeof(operation_t));
#else
    new_op = (volatile operation_t*) malloc(sizeof(operation_t));
#endif
    if (new_op==NULL) {
        assert(0);
    }
    return (operation_t*) new_op;
}

node_t* bst_initialize() {

	// node_t* root = (node_t*) ssalloc(sizeof(node_t));
	node_t* root = create_node(0,0,1);

	// assign minimum key to the root, actual tree will be 
	// the right subtree of the root

	return root;
}


sval_t bst_contains(skey_t k, node_t* root){
	
	node_t* pred;
	node_t* curr;
	operation_t* pred_op;
	operation_t* curr_op;
    sval_t res = bst_find(k, &pred, &pred_op, &curr, &curr_op, root, root);
    if (res & val_mask) return res;
    return 0;
}

sval_t bst_find(skey_t k, node_t** pred, operation_t** pred_op, node_t** curr, operation_t** curr_op, node_t* aux_root, node_t* root){

	sval_t result;
	skey_t curr_key;
	node_t* next;
	node_t* last_right;
	operation_t* last_right_op;

retry:
	// PARSE_TRY();

	result = NOT_FOUND_R;
	*curr = aux_root;
	*curr_op = atomic_load_explicit(&(*curr)->op, memory_order_seq_cst);

	if(GETFLAG(*curr_op) != STATE_OP_NONE){
#ifdef __tile__
    MEM_BARRIER;
#endif
		if (aux_root == root){
			bst_help_child_cas((operation_t*)UNFLAG(*curr_op), *curr, root);
			goto retry;
		} else {
			return ABORT;
		}
	}


	next = atomic_load_explicit(&(*curr)->right, memory_order_seq_cst);
	last_right = *curr;
	last_right_op = *curr_op;

	while (!ISNULL(next)){
		*pred = *curr;
		*pred_op = *curr_op;
		*curr = next;
		*curr_op = atomic_load_explicit(&(*curr)->op, memory_order_seq_cst);


		if(GETFLAG(*curr_op) != STATE_OP_NONE){
			bst_help(*pred, *pred_op, *curr, *curr_op, root);
			goto retry;
		}
		curr_key = atomic_load_explicit(&(*curr)->key, memory_order_seq_cst);
		if(k < curr_key){
			result = NOT_FOUND_L;
			next = atomic_load_explicit(&(*curr)->left, memory_order_seq_cst);
		} else if(k > curr_key) {
			result = NOT_FOUND_R;
			next = atomic_load_explicit(&(*curr)->right, memory_order_seq_cst);
			last_right = *curr;
			last_right_op = *curr_op;
		} else{
			result = atomic_load_explicit(&(*curr)->value, memory_order_seq_cst);
			break;
		}
	}
	
	if ((!(result & val_mask)) && (last_right_op != atomic_load_explicit(&last_right->op, memory_order_seq_cst))) {
		goto retry;
	}

	if (atomic_load_explicit(&(*curr)->op, memory_order_seq_cst) != *curr_op){
		goto retry;
	}

	return result;
} 
  
#define MAX_ITER 2

bool_t bst_add(skey_t k,sval_t v,  node_t* root){

	node_t* pred;
	node_t* curr;
	node_t* new_node = NULL;
	operation_t* pred_op;
	operation_t* curr_op;
	operation_t* cas_op;
	sval_t  result;

	int cnt = 0;
	while(TRUE) {
		__VERIFIER_assume(++cnt < MAX_ITER);
		result = bst_find(k, &pred, &pred_op, &curr, &curr_op, root, root);
		if (result & val_mask) {
#if GC == 1
            if (new_node!=NULL) {
                ssmem_free(alloc,new_node);
            }
#endif
			return FALSE;
		}
        
        if (new_node == NULL) {
		    new_node = create_node(k,v,0);
        }

		bool_t is_left = (result == NOT_FOUND_L);
		node_t* old;
		if (is_left) {
			old = atomic_load_explicit(&curr->left, memory_order_seq_cst);
		} else {
			old = atomic_load_explicit(&curr->right, memory_order_seq_cst);
		}

		cas_op = alloc_op();
		cas_op->child_cas_op.is_left = is_left;
		cas_op->child_cas_op.expected = old;
		cas_op->child_cas_op.update = new_node;

#if defined(__tile__)
		MEM_BARRIER;
#endif
		operation_t * expected = curr_op;
		if (atomic_compare_exchange_weak_explicit(&curr->op, &expected, (operation_t *)FLAG(cas_op, STATE_OP_CHILDCAS), memory_order_seq_cst, memory_order_seq_cst)) {

			bst_help_child_cas(cas_op, curr, root);
#if GC == 1
            //if (UNFLAG(curr_op)!=0) ssmem_free(alloc,(void*)UNFLAG(curr_op));
#endif
			return TRUE;
		} else {
#if GC == 1
            ssmem_free(alloc,cas_op);
#endif
        }
	}
}

void bst_help_child_cas(operation_t* op, node_t* dest, node_t* root){

	node_t * expected = op->child_cas_op.expected;
	if (op->child_cas_op.is_left) {
		atomic_compare_exchange_weak_explicit(&(dest->left), &expected, op->child_cas_op.update, memory_order_seq_cst, memory_order_seq_cst);
	} else {
		atomic_compare_exchange_weak_explicit(&(dest->right), &expected, op->child_cas_op.update, memory_order_seq_cst, memory_order_seq_cst);
	}
#ifdef __tile__
    MEM_BARRIER;
#endif
	operation_t * expectedOp = (operation_t *) FLAG(op, STATE_OP_CHILDCAS);
	atomic_compare_exchange_weak_explicit(&(dest->op), &expectedOp, (operation_t *)FLAG(op, STATE_OP_NONE), memory_order_seq_cst, memory_order_seq_cst);
}

sval_t bst_remove(skey_t k, node_t* root){

	node_t* pred;
	node_t* curr;
	node_t* replace;
    sval_t val;
	operation_t* pred_op;
	operation_t* curr_op;
	operation_t* replace_op;
	operation_t* reloc_op=NULL;

	int cnt = 0;
	while(TRUE) {
		__VERIFIER_assume(++cnt < MAX_ITER);
        sval_t res = bst_find(k, &pred, &pred_op, &curr, &curr_op, root, root);
		if (!(res & val_mask)) {
#if GC == 1
            //if (reloc_op!=NULL) ssmem_free(alloc,reloc_op);
#endif
			return 0;
		}

		if (ISNULL(atomic_load_explicit(&curr->right, memory_order_seq_cst)) || ISNULL(atomic_load_explicit(&curr->left, memory_order_seq_cst))) { // node has less than two children
			operation_t * expected = curr_op;
			if (atomic_compare_exchange_weak_explicit(&(curr->op), &expected, (operation_t *) FLAG(curr_op, STATE_OP_MARK), memory_order_seq_cst, memory_order_seq_cst)) {
				bst_help_marked(pred, pred_op, curr, root);
#if GC == 1
                //if (reloc_op!=NULL) ssmem_free(alloc,reloc_op);
                if (UNFLAG(curr->op)!=0) ssmem_free(alloc,(void*)UNFLAG(curr->op));
                ssmem_free(alloc,curr);
#endif
				return res;
			}
		} else { // node has two children
			val = bst_find(k, &pred, &pred_op, &replace, &replace_op, curr, root);
			if ((val == ABORT) || (atomic_load_explicit(&curr->op, memory_order_seq_cst) != curr_op)) {
				continue;
			} 
            
            //if (reloc_op==NULL) {
			    reloc_op = alloc_op(); 
            //}
			atomic_store_explicit(&reloc_op->relocate_op.state, STATE_OP_ONGOING, memory_order_seq_cst);
			reloc_op->relocate_op.dest = curr;
			reloc_op->relocate_op.dest_op = curr_op;
			reloc_op->relocate_op.remove_key = k;
			reloc_op->relocate_op.remove_value = res;
			reloc_op->relocate_op.replace_key = atomic_load_explicit(&replace->key, memory_order_seq_cst);
			reloc_op->relocate_op.replace_value = atomic_load_explicit(&replace->value, memory_order_seq_cst);

#if defined(__tile__)
			MEM_BARRIER;
#endif
			operation_t * expected = replace_op;
			if (atomic_compare_exchange_weak_explicit(&(replace->op), &expected, (operation_t *) FLAG(reloc_op, STATE_OP_RELOCATE), memory_order_seq_cst, memory_order_seq_cst)) {
#if GC == 1
                if (UNFLAG(replace_op)!=0) ssmem_free(alloc,(void*)UNFLAG(replace_op));
#endif
				if (bst_help_relocate(reloc_op, pred, pred_op, replace, root)) {
                    //if (UNFLAG(replace->op)!=0) ssmem_free(alloc,(void*)UNFLAG(replace->op));
#if GC == 1
                    //ssmem_free(alloc,replace);
#endif
					return res;
				}
			} else {
#if GC == 1
               ssmem_free(alloc,reloc_op);
             //   reloc_op=NULL;
#endif
            }
		}
	}
}

bool_t bst_help_relocate(operation_t* op, node_t* pred, operation_t* pred_op, node_t* curr, node_t* root){

	int seen_state = atomic_load_explicit(&op->relocate_op.state, memory_order_seq_cst);
	if (seen_state == STATE_OP_ONGOING) {
		// VCAS in original implementation
		operation_t* seen_op = op->relocate_op.dest_op;
		atomic_compare_exchange_weak_explicit(&(op->relocate_op.dest->op), &seen_op, (operation_t *) FLAG(op, STATE_OP_RELOCATE), memory_order_seq_cst, memory_order_seq_cst);
		if ((seen_op == op->relocate_op.dest_op) || (seen_op == (operation_t *)FLAG(op, STATE_OP_RELOCATE))){
			int expected = STATE_OP_ONGOING;
			atomic_compare_exchange_weak_explicit(&(op->relocate_op.state), &expected, STATE_OP_SUCCESSFUL, memory_order_seq_cst, memory_order_seq_cst);
			seen_state = STATE_OP_SUCCESSFUL;
            if (seen_op == op->relocate_op.dest_op) {
#if GC == 1
                if (UNFLAG(seen_op)!=0) ssmem_free(alloc,(void*)UNFLAG(seen_op));
#endif
            } 
		} else {
			// VCAS in original implementation
			seen_state = STATE_OP_ONGOING;
			atomic_compare_exchange_weak_explicit(&(op->relocate_op.state), &seen_state, STATE_OP_FAILED, memory_order_seq_cst, memory_order_seq_cst);
		}
	}

	if (seen_state == STATE_OP_SUCCESSFUL) {
		skey_t expectedKey = op->relocate_op.remove_key;
		atomic_compare_exchange_weak_explicit(&(op->relocate_op.dest->key), &expectedKey, op->relocate_op.replace_key, memory_order_seq_cst, memory_order_seq_cst);
		sval_t expectedVal = op->relocate_op.remove_value;
		atomic_compare_exchange_weak_explicit(&(op->relocate_op.dest->value), &expectedVal, op->relocate_op.replace_value, memory_order_seq_cst, memory_order_seq_cst);
		operation_t * expectedOp = (operation_t*) FLAG(op, STATE_OP_RELOCATE);
		atomic_compare_exchange_weak_explicit(&(op->relocate_op.dest->op), &expectedOp, (operation_t *) FLAG(op, STATE_OP_NONE), memory_order_seq_cst, memory_order_seq_cst);
	}

	bool_t result = (seen_state == STATE_OP_SUCCESSFUL);
	if (op->relocate_op.dest == curr) {
		return result;
	}

	operation_t * expectedOp = (operation_t *)FLAG(op, STATE_OP_RELOCATE);
	atomic_compare_exchange_weak_explicit(&(curr->op), &expectedOp, (operation_t *) FLAG(op, result ? STATE_OP_MARK : STATE_OP_NONE), memory_order_seq_cst, memory_order_seq_cst);
	if (result) {
		if (op->relocate_op.dest == pred) {
			pred_op = (operation_t *)FLAG(op, STATE_OP_NONE);
		}
		bst_help_marked(pred, pred_op, curr, root);
	}
	return result;
}

void bst_help_marked(node_t* pred, operation_t* pred_op, node_t* curr, node_t* root){

	node_t* new_ref;
	if (ISNULL(atomic_load_explicit(&curr->left, memory_order_seq_cst))) {
		if (ISNULL(atomic_load_explicit(&curr->right, memory_order_seq_cst))) {
			new_ref = (node_t*)SETNULL(curr);
		} else {
			new_ref = atomic_load_explicit(&curr->right, memory_order_seq_cst);
		}
	} else {
		new_ref = atomic_load_explicit(&curr->left, memory_order_seq_cst);
	}
	operation_t* cas_op = alloc_op();
	cas_op->child_cas_op.is_left = (curr == atomic_load_explicit(&pred->left, memory_order_seq_cst));
	cas_op->child_cas_op.expected = curr;
	cas_op->child_cas_op.update = new_ref;

#ifdef __tile__
    MEM_BARRIER;
#endif
	operation_t * expectedOp = pred_op;
	if (atomic_compare_exchange_weak_explicit(&(pred->op), &expectedOp, (operation_t *) FLAG(cas_op, STATE_OP_CHILDCAS), memory_order_seq_cst, memory_order_seq_cst)) {
		bst_help_child_cas(cas_op, pred, root);
#if GC == 1
        if (UNFLAG(pred_op)!=0) ssmem_free(alloc,(void*)UNFLAG(pred_op));
#endif
	} else {
#if GC == 1
        ssmem_free(alloc,cas_op);
#endif
    }
}

void bst_help(node_t* pred, operation_t* pred_op, node_t* curr, operation_t* curr_op, node_t* root ){
	

	if (GETFLAG(curr_op) == STATE_OP_CHILDCAS) {
		bst_help_child_cas((operation_t*)UNFLAG(curr_op), curr, root);
	} else if (GETFLAG(curr_op) == STATE_OP_RELOCATE) {
		bst_help_relocate((operation_t*)UNFLAG(curr_op), pred, pred_op, curr, root);
	} else if (GETFLAG(curr_op) == STATE_OP_MARK) {
		bst_help_marked(pred, pred_op, curr, root);
	}
}

unsigned long bst_size_rec(volatile node_t* node) {
  if (ISNULL((node_t*) node)) {
    return 0;
  } else if (GETFLAG(atomic_load_explicit(&node->op, memory_order_seq_cst)) != STATE_OP_MARK) {
    return 1 + bst_size_rec(atomic_load_explicit(&node->right, memory_order_seq_cst)) + bst_size_rec(atomic_load_explicit(&node->left, memory_order_seq_cst));
  } else {
    return bst_size_rec(atomic_load_explicit(&node->right, memory_order_seq_cst)) + bst_size_rec(atomic_load_explicit(&node->left, memory_order_seq_cst));
  }
}

unsigned long bst_size(node_t* node) {
    return bst_size_rec(node) - 1; //do not count the root
}
