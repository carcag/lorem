#include 	"lorem.h"

int 		counter;

int 		pushInList(t_data **pointer, char *word) {
	t_data 	*elem;

	if ((elem = malloc(sizeof(t_data))) == NULL)
		return (-1);
	elem->word = word;
	elem->wordCount = 1;
	elem->prec = *pointer;
	elem->next = NULL;
	pthread_mutex_init(&(elem->data_mutex), NULL);
	if (*pointer == NULL) {
		elem->controller = elem;
	}
	else {
		(*pointer)->next = elem;
		elem->controller = (*pointer)->controller;
	}
	*pointer = elem;
	return (0);
}

char 		*popInList(t_data **pointer) {
	char 	*word;
	t_data 	*tmp;

	if (!*pointer)
		return (NULL);
	tmp = (*pointer)->prec;
	word = (*pointer)->word;
	free(*pointer);
	*pointer = tmp;
	return (word);
}

t_data 		*searchInList(t_data **pointer, char *word) {
	t_data 	*tmp;

	tmp = *pointer;
	while (tmp != NULL) {
//		printf("|%s| != |%s|\n", word, tmp->word);
		if (strcmp(tmp->word, word) == 0){
			return (tmp);
		}
		tmp = tmp->prec;
	}
	return (NULL);
}

void 		clearDaList(t_data **pointer) {
	t_data 	*tmp;

	*pointer = (*pointer)->controller;
	while (*pointer) {
		tmp = (*pointer)->next;
		free((*pointer)->word);
		free(*pointer);
		*pointer = tmp;
	}
}

void 		*my_treater(void *data) {
	int 	i;
	int 	j;
	int 	k;
	int 	temp;
	char 	*word;
	t_data 	*tmp;
	t_threadIntell *tr;

	tr = (t_threadIntell*) data;
	i = 0;
	j = 0;
	k = 0;
	while (tr->buffer[i]) {
		while (tr->buffer[i] == ' ' || tr->buffer[i] == '\n')
			i++;
		j = i;
		while (tr->buffer[i] != ' ' && tr->buffer[i] != '\n' && tr->buffer[i] != 0) 
			i++;
		if ((word = malloc(sizeof(char) * (i - j) + 1)) == NULL)
			return;
		temp = i - j;
		strncpy(word, &(tr->buffer[j]), temp);
		j = i;
		if (*tr->my_data)
			pthread_mutex_lock(&(*tr->my_data)->data_mutex);
		if ((tmp = searchInList(tr->my_data, word)) != NULL)
			tmp->wordCount ++;
		else
			pushInList(tr->my_data, word);
		pthread_mutex_unlock(&(*tr->my_data)->data_mutex);
	}
}

void 		my_threader(t_threadIntell *tr) {
	pthread_t 	thread;
	counter++;
	pthread_create(&thread, NULL, my_treater, (void*) tr);
	pthread_join(thread, NULL);
}

int 		my_reader(t_data **my_data) {
	t_threadIntell tr;
	int 	fd;


	memset(tr.buffer, 0, SIZEOFBUFFER);
	fd = open("loremIpsum", O_RDWR);
	while (read(fd, tr.buffer, SIZEOFBUFFER) > 0) {
		tr.fd = fd;
		tr.my_data = my_data;
		my_threader(&tr);
	}
	printf("counter >>>> %d\n", counter);
	return (0);
}

int 		my_result(t_data **data) {
	t_data 	*tmp;

	tmp = *data;
	if (!tmp)
		return (-1);
	while (tmp != NULL) {
		printf("%s : %d \n", tmp->word, tmp->wordCount);
		tmp = tmp->prec;
	}
	return (0);
}

int 		main(int ac, char *av) {
	t_data 	*my_data = NULL;

	my_reader(&my_data);
	my_result(&my_data);
	clearDaList(&my_data);

	return (0);
}