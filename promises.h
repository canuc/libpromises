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
 
#ifndef PROMISES_H_
#define PROMISES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
	extern "C" {
#endif


#define RETURN_INT 1;

#define PROMISE_STATE_INCOMPLETE 0
#define PROMISE_STATE_SUCCESS 1
#define PROMISE_STATE_FAILURE 2

#define PROMISE_MAX_HANDLERS 5
#define MAX_NUMBER_OF_JOINT_PROMISES 5

struct _promise;

/*!
 * \type PROMISE_CALLBACK_RESPONSE
 * \brief The promise callback, callbacks must conform to this structure.
 */
typedef void (*PROMISE_CALLBACK_RESPONSE)(struct _promise *,void *, int32_t);

/*!
 * \typedef _joint_promise
 * \brief the joint promise structure will contain data pertaining to apex of the join.
 */ 
typedef struct _joint_promise {
	struct _promise * promise_to_join[MAX_NUMBER_OF_JOINT_PROMISES];
} joint_promise_t;

typedef struct _promise * resolve_promise_t;

/*!
 * \struct promise
 * \brief This type will wrap the promise's execution.
 * The promise must be initialized with \ref promise_create
 * \note You cannot have more then 1 parent promise.
 */
typedef struct _promise {
	uint8_t promise_state;
	
	void * data;
	uint8_t data_size;
	
	void * closure;
	void * scope_data;

	joint_promise_t join;
	resolve_promise_t parent;	
	PROMISE_CALLBACK_RESPONSE successHandlers[PROMISE_MAX_HANDLERS];
	PROMISE_CALLBACK_RESPONSE failureHandlers[PROMISE_MAX_HANDLERS];
} promise;


/*!
 * \fn createPromise( ) 
 * \brief initialize a promise structure with no listeners attached.
 */
promise * promise_create(promise * p);

/*!
 * \fn resolvePromise( promise * p, void * data, uint8_t data_size);
 * \brief This will resolve a promise as successful and call all the required listeners
 * \param p The promise object to resolve
 * \param data Data pointer responsible for calling back. Ownership of the data is maintained by application.
 */
void promise_resolve( promise * p, void * data, int32_t data_size );

/*!
 * \fn failPromise(promise * p, void * data, uint8_t data_size)
 * \brief fail a promise pointed to by p
 * This will fail a promise and call all the associated failure promise handlers,
 * after all the promise handlers will be removed as they will be called once per bind.
 */
void promise_fail( promise * p, void * data, int32_t data_size );

/*!
 * \fn addListener(promise * p,CALLBACK_RESPONSE success,CALLBACK_RESPONSE failure)
 * \brief adds a promise callback function to the promise. If the promise is created int
 * \pre The promise p must be initialized with: #promise_create
 * \param p The promse object to add a listener.
 * \param success A function pointer to a callback, will be called iff the promise resolves as success
 * \param failure A function pointer to a failure callback, will be called iff the promise resolves as failure
 *  If the promise is already completed then it will be called in-line, if the callback from the same thread as the resolver. The contract
 * is one of the success or failure handlers will be called.
 */
void promise_add_listener( promise * p, PROMISE_CALLBACK_RESPONSE success, PROMISE_CALLBACK_RESPONSE failure );

/*!
 * \fn void promise_add_closure( promise * p, void * closed_data)
 * \brief create a closure for the promise.
 */
void promise_add_closure( promise * p, void * closed_data);

/*!
 * \fn void promise_add_scope_data( promise * p, void * scope_data)
 * \brief Add data about the scope in which the promise was created.  
 * \param p the promise to attach scope data.
 * \param scope_data The data to attach
 * The application must retain control of the associated memory.
 */
void promise_add_scope_data( promise * p, void * scope_data);

/*!
 * \fn void * promise_get_scope_data( promise * p)
 * \brief get the scope data that is bound to the promise.
 */
void * promise_get_scope_data( const promise * p);

/*!
 * \fn bool promise_is_done( const promise * p ) 
 * \brief true if a resolution has been reached
 */
bool promise_is_done( const promise * p );

/*!
 * \fn void * promise_get_closed_data( promise * p )
 * \brief This will retrieve the closed data around the promises.
 */
void * promise_get_closed_data( promise * p);

/*!
 * This will create a joint promise. The application will still retain control of
 * the memory allocated.
 *
 * \return the index in the promise_join which has the dependant promise.
 */
uint8_t promise_add_joint_promise( promise * p, promise * add_joint);

#ifdef __cplusplus
	}
#endif

#endif //  PROMISES_H_


