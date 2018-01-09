// Author: Benjamin Lenington
// UW Email: lenington@wisc.edu
// Course: CS537 - Intro to Operating Systems
// Project: P4A - Scalable Web Server


// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c

#include "cs537.h"
#include "request.h"
#include "server.h"

#define ARG_COUNT 4


int main(int argc, char *argv[])
{
	// Fetch and validate server arguments
  int port_number, thread_count, buffer_count;
	getargs(&port_number, &thread_count, &buffer_count, argc, argv);
	if ((thread_count <= 0) || (port_number < 0) || (buffer_count <= 0)) {
		fprintf(stderr, "ERROR: Invalid arguments (cannot be negative values)");
		exit(1);
	}

	// Configure buffers
	full_count = 0;
	buffer_cur = 0;
	buffer_fill = 0;
	max_buffer_count = buffer_count;

	// Allocate the buffer
	current_buffer = (int*)malloc(buffer_count * sizeof(int));

	// Create the specified number server worker threads
	pthread_t **pthreads = malloc(thread_count * sizeof(pthreads));
	int current;
	while (current < thread_count) {
		pthreads[current] = (pthread_t*) malloc(sizeof(pthread_t));
		Pthread_create(pthreads[current], NULL, threadHelper, NULL);
		current++;
	}

	int port_fd = Open_listenfd(port_number);
	for (;;) {
    struct sockaddr_in client_address;
		int clientlen = sizeof(client_address);
		int connection_fd = Accept(port_fd, (SA *)&client_address, (socklen_t *)&clientlen);

		// Critical secion: request lock
		Pthread_mutex_lock(&lock);

		while (max_buffer_count == full_count) {
			Pthread_cond_wait(&empty, &lock);
		}
    full_count = (full_count + 1);

		current_buffer[buffer_fill] = connection_fd;
    int tmp_count = buffer_fill + 1;
		buffer_fill = tmp_count % max_buffer_count;
		
		Pthread_cond_signal(&fill);
		Pthread_mutex_unlock(&lock);
	}
}

// Fetch and parse arguments
void getargs(int * port_number, int * thread_count, int * buffer_count, int argc, char *argv[]) {

	// Validate argument count
	if (argc < ARG_COUNT) {
		fprintf(stderr, "ERROR: Invalid arguments (too few provided)");
    fprintf(stderr, "Usage: %s <port_number> <thread_count> <buffer_count>\n", argv[0]);
		exit(1);
	}
  if (argc > ARG_COUNT) {
		fprintf(stderr, "ERROR: Invalid arguments (too many provided)");
    fprintf(stderr, "Usage: %s <port_number> <thread_count> <buffer_count>\n", argv[0]);
		exit(1);
	}

	  // Parse arguments and save values
	  (* port_number) = atoi(argv[1]);
	  (* thread_count) = atoi(argv[2]);
	  (* buffer_count) = atoi(argv[3]);
}

void * threadHelper(void * void_arg) {
	for (;;) {
		// Enter critical section: request lock
		Pthread_mutex_lock(&lock);

		while (full_count == 0) {
			Pthread_cond_wait(&fill, &lock);
		}
    full_count = (full_count - 1);

		int buffer_fd = current_buffer[buffer_cur];
    int tmp_count = (buffer_cur + 1);
		buffer_cur = tmp_count % max_buffer_count;

		Pthread_cond_signal(&empty);
		Pthread_mutex_unlock(&lock);
		requestHandle(buffer_fd);
		Close(buffer_fd);
	}
}