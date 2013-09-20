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

void promise_success(struct _promise *,void *, int32_t);
void promise_failure(struct _promise *,void *, int32_t);

void promise_success(struct _promise * p ,void * data_resolved, int32_t data_size) {
	fprintf(stdout,"Promise has succeded! %d\r\n",data_size);
	if ( data_size ) {
		fprintf(stdout,"resolved with data: %s!\r\n", (char *) data_resolved );
	}
}

void promise_failure(struct _promise * p ,void * data_fail, int32_t data_size) {
	fprintf(stdout,"Promise has failed!\r\n");
}

int main()
{
	char data_buffer[500] = {0};
	strcpy(data_buffer,"resolution success");

	promise stack_promise;
	promise_create(&stack_promise);
	promise_add_listener(&stack_promise,promise_success,promise_failure);

	promise_resolve(&stack_promise, data_buffer, strlen(data_buffer) + 1);
}
