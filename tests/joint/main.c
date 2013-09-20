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

#include <promises.h>
#include <stdio.h>
#include <string.h>

#define INDEX_STACK_PROMISE_1 0
#define INDEX_STACK_PROMISE_2 1

void promise_success(struct _promise *,void *, int32_t);
void promise_failure(struct _promise *,void *, int32_t);

void promise_success(struct _promise * p ,void * data_resolved, int32_t data_size) {
	fprintf(stdout,"Joint Promise has succeded!\r\n");

	struct _promise * p_joint = NULL;
	for (int i=0; i < MAX_NUMBER_OF_JOINT_PROMISES; i++ ) {
		p_joint = p->join.promise_to_join[i];

		if ( p_joint && p_joint->data_size ) {
			fprintf(stdout,"Promise %d: %s!\r\n",i,(char *) p_joint->data);
		}
	}
}

void promise_failure(struct _promise * p ,void * data_fail, int32_t data_size) {
	fprintf(stdout,"Joint Promise has failed!\r\n");
}

int main()
{
	char data_buffer[500] = {0};
	strcpy(data_buffer,"resolution success");

	promise stack_promise;
	promise stack_promise2;

	promise joint_promise;
	/* initialize our first promise */
	promise_create(&stack_promise);

	/* intialize our second promise */
	promise_create(&stack_promise2);
	
	/* initilize our joint promise */
	promise_create(&joint_promise);

	/*  
	 *  Also if you want you can use: add_scope_data 
	 *  to add scoped data to add distinction between the 
	 *  promises.
	 * 
	 *  But the memory management for the attached scope data
	 *  must be handled by the call.
	 */
	fprintf(stdout,"stack_promise has been added with index: %u\r\n",(uint32_t) promise_add_joint_promise(&joint_promise,&stack_promise));
	fprintf(stdout,"stack_promise2  has been added with index: %u\r\n",(uint32_t) promise_add_joint_promise(&joint_promise,&stack_promise2));

	promise_add_listener(&joint_promise,promise_success,promise_failure);

	/* 
	 * When both the joint promises are resolved,
	 * then the resolved handler will be called on the joint.
	 */
	promise_resolve(&stack_promise,data_buffer, strlen(data_buffer) + 1);
	promise_resolve(&stack_promise2,NULL,0);
}
