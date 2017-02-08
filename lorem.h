#ifndef 	LOREM_H_
#define 	LOREM_H_

#include 	<stdlib.h>
#include 	<stdio.h>
#include 	<pthread.h>
#include 	<unistd.h>
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include 	<fcntl.h>
#include 	<string.h>

#define 	SIZEOFBUFFER 20000

typedef struct s_data
{
	pthread_mutex_t data_mutex;
	char 	*word;
	struct 	s_data *prec;
	struct 	s_data *next;
	struct 	s_data *controller;
	int 	wordCount;
} 			t_data;

typedef struct s_threadIntell {
	t_data	**my_data;
	int 	fd;
	char 	buffer[SIZEOFBUFFER];
} 			t_threadIntell;

#endif 		/*!LOREM_H_*/