#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_STRING_SIZE	256

/* --- START IMPLEMENT QUEUE HERE --- */

typedef struct queue_t queue_t;
struct queue_t
{
	/* Dimensiunea maxima a cozii */
	unsigned int max_size;
	/* Dimensiunea cozii */
	unsigned int size;
	/* Dimensiunea in octeti a tipului de date stocat in coada */
	unsigned int data_size;
	/* Indexul de la care se vor efectua operatiile de front si dequeue */
	unsigned int read_idx;
	/* Indexul de la care se vor efectua operatiile de enqueue */
	unsigned int write_idx;
	/* Bufferul ce stocheaza elementele cozii */
	void **buff;
};

queue_t *
q_create(unsigned int data_size, unsigned int max_size)
{
    queue_t *queue = malloc(sizeof(queue_t));
    queue->size = 0;
    queue->read_idx = 0;
    queue->write_idx = 0;
    queue->data_size = data_size;
    queue->max_size = max_size;
    queue->buff = malloc(max_size * sizeof(void *));

	return queue;
}

/*
 * Functia intoarce numarul de elemente din coada al carei pointer este trimis
 * ca parametru.
 */
unsigned int
q_get_size(queue_t *q)
{
	if(!q)
        exit(-1);
	return q->size;
}

/*
 * Functia intoarce 1 daca coada este goala si 0 in caz contrar.
 */
unsigned int
q_is_empty(queue_t *q)
{
	if(q->size == 0)
        return 1;

	return 0;
}

/* 
 * Functia intoarce primul element din coada, fara sa il elimine.
 */
void *
q_front(queue_t *q)
{
    if(!q && !q->buff)
        exit(-1);
    if(q->size != 0)
        return q->buff[q->read_idx];

	return NULL;
}

/*
 * Functia scoate un element din coada. Se va intoarce 1 daca operatia s-a
 * efectuat cu succes (exista cel putin un element pentru a fi eliminat) si
 * 0 in caz contrar.
 */
int
q_dequeue(queue_t *q)
{
	if(!q)
        return 0;
    if(q->size == 0 || !q->buff)
        return 0;
    free(q->buff[q->read_idx]);
    q->read_idx = (q->read_idx + 1) % q->max_size;
    q->size--;

    return 1;
}

/* 
 * Functia introduce un nou element in coada. Se va intoarce 1 daca
 * operatia s-a efectuat cu succes (nu s-a atins dimensiunea maxima) 
 * si 0 in caz contrar.
 */
int
q_enqueue(queue_t *q, void *new_data)
{
	if(!q || q->size == q->max_size)
        return 0;
    
    q->buff[q->write_idx] = malloc(q->data_size);
    if(!q->buff[q->write_idx])
        return 0;
    memcpy(q->buff[q->write_idx], new_data, q->data_size);
    q->write_idx = (q->write_idx + 1) % q->max_size;

    q->size++;

	return 1;
}

/*
 * Functia elimina toate elementele din coada primita ca parametru.
 */
void
q_clear(queue_t *q)
{
    if(!q)
        exit(-1);
    while(q->size)
        q_dequeue(q);
}

/*
 * Functia elibereaza toata memoria ocupata de coada.
 */
void
q_free(queue_t *q)
{
    q_clear(q);
    free(q->buff);
    free(q);
}

typedef struct stack_t stack_t;
struct stack_t
{
	queue_t *q1, *q2;
};

stack_t *
st_create(unsigned int data_size)
{
	stack_t *stack = malloc(sizeof(stack_t));
    if(!stack)
        exit(-1);

    stack->q1 = q_create(data_size, 10);
    stack->q2 = q_create(data_size, 10);

    return stack;
}

int st_get_size(stack_t *st)
{
	return st->q1->size;
}

/*
 * Functia intoarce 1 daca stiva al carei pointer este trimis
 * ca parametru este goala si 0 in caz contrar.
 */
unsigned int
st_is_empty(stack_t *st)
{
	if(st->q1->size != 0)
	    return 0;
    return 1;
}

/*
 * Functia intoarce elementul din vârful stivei al carei pointer este trimis
 * ca parametru fără a efectua nicio modificare asupra acesteia.
 */
void *
st_peek(stack_t *st)
{
    if(!st || !st->q1)
        exit(-1);
    if(st->q1->size == 0)
	    return NULL;

    return st->q1->buff[st->q1->read_idx];
}

/*
 * Functia elimina elementul din vârful stivei al carei pointer este trimis
 * ca parametru. Atentie! Este responsabilitatea acestei functii de a elibera
 * memoria pentru elementul eliminat.
 */
void
st_pop(stack_t *st)
{
	q_dequeue(st->q1);
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se adauga un nou element in
 * vârful stivei al carei pointer este trimis ca parametru.
 */
void
st_push(stack_t *st, void *new_data)
{
    if(!st)
        exit(-1);
	q_enqueue(st->q2, new_data);
    while(!st_is_empty(st)){
        q_enqueue(st->q2, st_peek(st));
        st_pop(st);
    }
    while(!q_is_empty(st->q2)){
        q_enqueue(st->q1, q_front(st->q2));
        q_dequeue(st->q2);
    }
}

/*
 * Functia elimina toate elementele stivei al carei pointer este trimis
 * ca parametru.
 */
void
st_clear(stack_t *st)
{
	while(!st_is_empty(st))
        st_pop(st);
}

/*
 * Functia elibereaza memoria folosita de toate elementele, iar la
 * sfarsit elibereaza memoria folosita de structura stivei.
 */
void
st_free(stack_t *st)
{
    q_free(st->q1);
    q_free(st->q2);
    free(st);
}

int main()
{
    
    stack_t *stack;
    int is_int = 0;
    int is_string = 0;

    while (1) {
        char command[MAX_STRING_SIZE], added_elem[MAX_STRING_SIZE];
        long nr;

        scanf("%s", command);

        if (strncmp(command, "create_stack_str", 16) == 0) {
            stack = st_create(MAX_STRING_SIZE);
            is_string = 1;
        }

        if (strncmp(command, "create_stack_int", 16) == 0) {
            stack = st_create(sizeof(int));
            is_int = 1;
        }

        if (strncmp(command, "push", 4) == 0) {
          
            if(is_int) {
                scanf("%ld", &nr);
                st_push(stack, &nr);
            } else if(is_string) {
                scanf("%s", added_elem);
                st_push(stack, added_elem);
            } else {
                printf("Create a stack first!\n");
                exit(0);
            }
        }

        if (strncmp(command, "pop", 3) == 0) {
            if(!is_int && !is_string) {
                printf("Create a stack first!\n");
                exit(0);
            }

            st_pop(stack);
        }

        if (strncmp(command, "peek", 4) == 0) {
            if(!is_int && !is_string) {
                printf("Create a stack first!\n");
                exit(0);
            }

            if (is_int == 1) {
                int val = *(int *) st_peek(stack);
                printf("%d\n", val);
            }

            if (is_string == 1) {
                char *val_str = (char *) st_peek(stack);
                printf("%s\n", val_str);
            }
        }

        if (strncmp(command, "empty", 5) == 0) {
            if(!is_int && !is_string) {
                printf("Create a stack first!\n");
                exit(0);
            }

            if (st_is_empty(stack)) {
                printf("empty\n");
            } else {
                printf("not empty\n");
            }
        }

        if (strncmp(command, "free", 4) == 0) {
            if(!is_int && !is_string) {
                printf("Create a stack first!\n");
                exit(0);
            }
            st_clear(stack);
            st_free(stack);
            break;
        }
    }

    return 0;
}
