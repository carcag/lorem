#include 	"lorem.h"
#include 	"time.h"

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
		if (strcmp(tmp->word, word) == 0){
			pthread_mutex_lock(&tmp->data_mutex);
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
	int 	occur;
	int 	i;
	int 	j;
	int 	k;
	char 	*word;
	t_data 	*tmp;
	t_threadIntell *tr;

	tr = (t_threadIntell*) data;
	occur = tr->occur;
	i = 0;
	j = 0;
	k = 0;
	while (tr->buffer[occur][i]) {
		while (tr->buffer[occur][i] == ' ' || tr->buffer[occur][i] == '\n')
			i++;
		j = i;
		while (tr->buffer[occur][i] != ' ' && tr->buffer[occur][i] != '\n' && tr->buffer[occur][i] != 0)
			i++;
		if ((word = malloc(sizeof(char) * (i - j) + 1)) == NULL)
			return;
		strncpy(word, &(tr->buffer[occur][j]), i - j);
		if ((tmp = searchInList(tr->my_data, word)) != NULL) {
			tmp->wordCount++;
			pthread_mutex_unlock(&tmp->data_mutex);
		}
		else
			pushInList(tr->my_data, word);
		i++;
	}
}

void 		my_threader(t_threadIntell *tr) {
	pthread_t 	thread[5];
	int 		i;

	i = 0;
	if ((tr->buffer = malloc(sizeof(char*) * 6)) == NULL)
		return;
	while (i < 4) {
		tr->buffer[i] = malloc(sizeof(char) * (tr->sb.st_size / 4 + 1));
		i++;
	} 
	tr->buffer[i] = malloc(sizeof(char) * (tr->sb.st_size % 4 + 1));
	tr->buffer[i+1] = NULL;
	i = 0;
	tr->start = 0;
	while (i < 4){
		memcpy(tr->buffer[i], &tr->map[tr->start], tr->sb.st_size / 4);
		tr->start = (tr->sb.st_size / 4) * (i + 1);
		tr->buffer[i][(tr->sb.st_size / 4) + 1] = 0;
		tr->occur = i;
		pthread_create(&thread[i], NULL, my_treater, (void*) tr);
		usleep(100);
		i++;
	}
	tr->start = tr->sb.st_size / 4 * 4;
	tr->end = tr->sb.st_size;
	memcpy(tr->buffer[4], &tr->map[tr->start], tr->sb.st_size % 4);
	tr->buffer[4][(tr->sb.st_size % 4) + 1] = 0;
	tr->end = tr->start + tr->sb.st_size % 4;
	tr->occur = 4;
	my_treater((void*) tr);
	i = 0;
	while (i < 4) {
		pthread_join(thread[i], NULL);
		i++;
	}
}

int 		my_reader(t_data **my_data) {
	t_threadIntell tr;
	int 	fd;

	tr.my_data = my_data;
	if ((fd = open("gen.txt", O_RDWR)) == -1)
		return (-1);
	if (fstat(fd, &tr.sb) == -1) 
		return (-2);
	if (!S_ISREG(tr.sb.st_mode))
		return (-3);
	if ((tr.map = mmap(0, tr.sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
		return (-4);
	my_threader(&tr);

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
	if (my_result(&my_data) == -1) {
		printf("error in my_result\n");
		return (-1);
	}
	clearDaList(&my_data);

	return (0);
}