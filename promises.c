/*
 * Copyright (c) 2013 Julian Haldenby
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "promises.h"
#include <assert.h>

static void remove_listeners(promise * p);
static bool check_if_all_dependant_promises_resolved( promise * p );
static void joint_promise_resolution(promise * p);
static uint8_t add_promise_to_join( promise * p, promise * add);

#define foreach(START,FINNISH) int for_each_i = START;\
							   for(;for_each_i < FINNISH; for_each_i++)

promise * promise_create(promise * p) {
	p->data = NULL;
	p->data_size = 0;
	remove_listeners(p);
	p->promise_state = PROMISE_STATE_INCOMPLETE;
	p->closure = NULL;
	p->scope_data = NULL;
	p->parent = 0;

	// NULL all the dependant promises
	foreach(0,MAX_NUMBER_OF_JOINT_PROMISES)	{
		p->join.promise_to_join[for_each_i] = NULL;
	}

	return p;
}

/*!
 * \fn void removeListeners(promise * p)
 * \brief This will remove all the listeners on p.
 * \param p A pointer to the promise whose listeners should be removed.
 *
 * \internal
 *
 * \relates promise_create 
 */
static void remove_listeners(promise * p)
{
	assert(p);

	for (int i = 0; i < PROMISE_MAX_HANDLERS; i++) {
		p->successHandlers[i] = NULL;
		p->failureHandlers[i] = NULL;
	}

	return;
}

void promise_resolve(promise * p, void * data, int32_t data_size) {
	if ( p->promise_state == PROMISE_STATE_INCOMPLETE ) {
		p->promise_state = PROMISE_STATE_SUCCESS;
		p->data = data;
		p->data_size = data_size;
		for (int i = 0; i < PROMISE_MAX_HANDLERS; i++) {
			if (p->successHandlers[i] != NULL) {
				PROMISE_CALLBACK_RESPONSE callbackFcn = p->successHandlers[i];
				callbackFcn(p,data,data_size);
			}
		}
		remove_listeners(p);
	}

	return;
}

void promise_fail(promise * p, void * data, int32_t data_size) {
	if ( p->promise_state == PROMISE_STATE_INCOMPLETE ) {
		p->promise_state = PROMISE_STATE_FAILURE;
		p->data = data;
		p->data_size = data_size;
		for (int i = 0; i < PROMISE_MAX_HANDLERS; i++) {
			if (p->failureHandlers[i] != NULL) {
				PROMISE_CALLBACK_RESPONSE callbackFcn = p->failureHandlers[i];
				callbackFcn(p,data,data_size);
			}
		}
		remove_listeners(p);
	}

	return;
}

void promise_add_listener(promise * p,PROMISE_CALLBACK_RESPONSE success,PROMISE_CALLBACK_RESPONSE failure) 
{
	if ( p->promise_state == PROMISE_STATE_INCOMPLETE ) {
		for (int i = 0; i < PROMISE_MAX_HANDLERS; i++) {
			if (p->successHandlers[i] == NULL) {
				p->successHandlers[i] = success;
				break;
			}
		}	
		for (int i = 0; i < PROMISE_MAX_HANDLERS; i++) {
			if (p->failureHandlers[i] == NULL) {
				p->failureHandlers[i] = failure;
				break;
			}
		}
	} else {
		// If the promise is already resolved then we want 
		// to immediately call the handlers
		if ( p->promise_state == PROMISE_STATE_SUCCESS ) {
			success(p,p->data,p->data_size);
		} 
		else {
			failure(p,p->data,p->data_size);
		}
	}

	return;
}

void promise_add_closure( promise * p, void * closed_data) {
	assert(p);
	p->closure = closed_data;
	return;
}

/*!
 * \fn void promise_add_scope_data( promise * p, void * scope_data)
 * \brief Add data about the scope in which the promise was invoked
 */
void promise_add_scope_data( promise * p, void * scope_data) {
	assert(p);

	p->scope_data = scope_data;
	return;
}

/*!
 * \fn void * promise_get_scope_data( promise * p)
 * \brief get the scope data that is bound to the promise
 */
void * promise_get_scope_data(const promise * p) {
	assert(p);
	return p->scope_data;
}

bool promise_is_done( const promise * p ) {
	if ( p->promise_state != PROMISE_STATE_INCOMPLETE ) {
		return true;
	} 
	else {
		return false;
	}
}

void * promise_get_closed_data( promise * p) {
	assert(p);
	return p->closure;
}

static void promise_joint_success(promise * p, void * data, int datasize) {
	joint_promise_resolution(p);
	return;
}

static void promise_joint_failure(promise * p, void * data, int datasize) {
	joint_promise_resolution(p);
	return;
}

static void joint_promise_resolution(promise * p)
{
	promise * parent_promise = p->parent;
	
	bool all_deps_resolved = check_if_all_dependant_promises_resolved(parent_promise);
	if ( all_deps_resolved ) {
		promise_resolve(parent_promise,NULL,0);
	}
	return;
}
static uint8_t add_promise_to_join( promise * p, promise * add) 
{
	uint8_t promise_idx = 0;
	foreach(0,MAX_NUMBER_OF_JOINT_PROMISES) {
		if ( p->join.promise_to_join[for_each_i] == NULL ) {
			p->join.promise_to_join[for_each_i] = add;
			promise_idx = for_each_i;
			break;
		}
	}
	return promise_idx;
}

static bool check_if_all_dependant_promises_resolved(promise * p ) {
	
	bool all_done = true;
	
	foreach(0,MAX_NUMBER_OF_JOINT_PROMISES) {
		if ( p->join.promise_to_join[for_each_i] != NULL) {
			if ( !promise_is_done(p->join.promise_to_join[for_each_i])) {
				all_done = false;
				break;
			}
		}
	}
	return all_done;
}

uint8_t promise_add_joint_promise( promise * p, promise * add_joint)
{
	uint8_t joint_idx = 0;

	// A promise can only be apart of at most 1 other joint, fail if 
	// it is attempted to be added as another
	assert(add_joint->parent == NULL);

	add_joint->parent = p;
	joint_idx = add_promise_to_join(p,add_joint);
	promise_add_listener(add_joint,promise_joint_success,promise_joint_failure);

	return joint_idx;
}