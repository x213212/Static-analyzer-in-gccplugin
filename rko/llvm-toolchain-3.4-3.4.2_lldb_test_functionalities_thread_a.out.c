// tag-#anon#UN[SYM#tag-__pthread_mutex_s#'__data'||ARR40{S8}$S8$'__size'||S64'__align'|]
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 90
union anonymous;

// tag-__pthread_internal_list
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 75
struct __pthread_internal_list;

// tag-__pthread_mutex_s
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 92
struct __pthread_mutex_s;

// tag-pthread_attr_t
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 63
union pthread_attr_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

// pthread_create
// file /usr/include/pthread.h line 244
extern signed int pthread_create(unsigned long int *, const union pthread_attr_t *, void * (*)(void *), void *);
// pthread_join
// file /usr/include/pthread.h line 261
extern signed int pthread_join(unsigned long int, void **);
// pthread_mutex_lock
// file /usr/include/pthread.h line 773
extern signed int pthread_mutex_lock(union anonymous *);
// pthread_mutex_unlock
// file /usr/include/pthread.h line 784
extern signed int pthread_mutex_unlock(union anonymous *);
// thread1
// file main.c line 26
void * thread1(void *input);
// thread2
// file main.c line 16
void * thread2(void *input);
// thread3
// file main.c line 8
void * thread3(void *input);

struct __pthread_internal_list
{
  // __prev
  struct __pthread_internal_list *__prev;
  // __next
  struct __pthread_internal_list *__next;
};

struct __pthread_mutex_s
{
  // __lock
  signed int __lock;
  // __count
  unsigned int __count;
  // __owner
  signed int __owner;
  // __nusers
  unsigned int __nusers;
  // __kind
  signed int __kind;
  // __spins
  signed short int __spins;
  // __elision
  signed short int __elision;
  // __list
  struct __pthread_internal_list __list;
};

union anonymous
{
  // __data
  struct __pthread_mutex_s __data;
  // __size
  char __size[40l];
  // __align
  signed long int __align;
};

union pthread_attr_t
{
  // __size
  char __size[56l];
  // __align
  signed long int __align;
};


// mutex1
// file main.c line 3
union anonymous mutex1 = { .__data={ .__lock=0, .__count=(unsigned int)0, .__owner=0, .__nusers=(unsigned int)0,
    .__kind=0, .__spins=(signed short int)0,
    .__elision=(signed short int)0, .__list={ .__prev=((struct __pthread_internal_list *)NULL), .__next=((struct __pthread_internal_list *)NULL) } } };
// mutex2
// file main.c line 4
union anonymous mutex2 = { .__data={ .__lock=0, .__count=(unsigned int)0, .__owner=0, .__nusers=(unsigned int)0,
    .__kind=0, .__spins=(signed short int)0,
    .__elision=(signed short int)0, .__list={ .__prev=((struct __pthread_internal_list *)NULL), .__next=((struct __pthread_internal_list *)NULL) } } };
// mutex3
// file main.c line 5
union anonymous mutex3 = { .__data={ .__lock=0, .__count=(unsigned int)0, .__owner=0, .__nusers=(unsigned int)0,
    .__kind=0, .__spins=(signed short int)0,
    .__elision=(signed short int)0, .__list={ .__prev=((struct __pthread_internal_list *)NULL), .__next=((struct __pthread_internal_list *)NULL) } } };

// main
// file main.c line 39
signed int main()
{
  unsigned long int thread_1;
  unsigned long int thread_3;
  pthread_mutex_lock(&mutex1);
  pthread_mutex_lock(&mutex2);
  pthread_mutex_lock(&mutex3);

  pthread_mutex_unlock(&mutex3); // introduced
  pthread_mutex_unlock(&mutex2); // introduced
  pthread_mutex_unlock(&mutex1); // introduced

  pthread_create(&thread_1, (const union pthread_attr_t *)(void *)0, thread1, (void *)0);

  pthread_mutex_lock(&mutex2); // introduced
  pthread_mutex_lock(&mutex1); // introduced

  pthread_mutex_lock(&mutex3);
  pthread_create(&thread_3, (const union pthread_attr_t *)(void *)0, thread3, (void *)0);
  pthread_join(thread_1, (void **)(void *)0);
  pthread_join(thread_3, (void **)(void *)0);
  return 0;
}

// thread1
// file main.c line 26
void * thread1(void *input)
{
  unsigned long int thread_2;
  pthread_create(&thread_2, (const union pthread_attr_t *)(void *)0, thread2, (void *)0);
  pthread_mutex_lock(&mutex1);

  pthread_mutex_lock(&mutex2); // introduced

  pthread_mutex_unlock(&mutex1);
  pthread_join(thread_2, (void **)(void *)0);
  return (void *)0;
}

// thread2
// file main.c line 16
void * thread2(void *input)
{
  pthread_mutex_unlock(&mutex3);
  pthread_mutex_lock(&mutex2);
  pthread_mutex_unlock(&mutex2);
  return (void *)0;
}

// thread3
// file main.c line 8
void * thread3(void *input)
{
  pthread_mutex_unlock(&mutex2);
  pthread_mutex_unlock(&mutex1);
  return (void *)0;
}

