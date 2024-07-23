#include<stdlib.h>
#include<string.h>
#include<math.h>

#include "hash_table.h"


const int HT_INITIAL_BASE_SIZE = 50;
const int HT_PRIME_1 = 211;
const int HT_PRIME_2 = 139;
static item DELETED_ITEM = {NULL, NULL};


static item* new_item(const char* k, const char* v){
	item* it = malloc(sizeof(item));
	it->key = strdup(k);
	it->value = strdup(v);
	return it;
}

static hash_table* ht_new_sized(const int base_size){
	hash_table* ht = malloc(sizeof(hash_table));
	ht->base_size = base_size;
	ht->size = next_prime(ht->base_size);

	ht->count = 0;
	ht->items = calloc((size_t)ht->size,sizeof(item*));
	return ht;
}

hash_table* ht_new(){
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void del_item(item* it){
	free(it->key);
	free(it->value);
	free(it);
}

void del_hash_table(hash_table* ht){
	for(int i=0;i< ht->size;i++){
		item* item = ht->items[i];
		if(item!=NULL) del_item(item);
	}
	free(ht->items);
	free(ht);	
}

// polynomial rolling hash function - 
// https://www.geeksforgeeks.org/string-hashing-using-polynomial-rolling-hash-function/
static int hash(const char* str, const int a, const int size){
	long hsh = 0;
	const int len_s = strlen(str);
	for(int i=0;i<len_s;i++){
		hsh += (long)pow(a,len_s - (i+1)) * str[i];
		hsh = hsh % size;
	}
	return (int)hsh;
}

// for handling collision -- double hashing -
// https://www.geeksforgeeks.org/double-hashing/
static int get_hash(const char* s, const int num_buck, const int attempt){
	const int hash_a = hash(s,HT_PRIME_1, num_buck);
	const int hash_b = hash(s,HT_PRIME_2, num_buck);
	return (hash_a + (attempt*(hash_b + 1))) % num_buck;
}


void ht_insert(hash_table* ht, const char* key, const char* value){
	
	// resize
	const int load = ht->count*100/ht->size;
	if(load > 70){
		ht_resize_up(ht);
	}
	
	
	item* it = new_item(key,value);
	int index = get_hash(it->key,ht->size,0);
	
	int attempt = 1;
	item* cur_item = ht->items[index];
	
	while(cur_item!=NULL){
		if(cur_item != &DELETED_ITEM){
			if(strcmp(cur_item->key,key)==0){
				del_item(cur_item);
				ht->items[index]= it;
				return;
			}
		}
		index = get_hash(it->key,ht->size,attempt);
		cur_item = ht->items[index];
		attempt++;
	}
	ht->items[index] = it;
	ht->count++;
}

char* ht_search(hash_table* ht, const char* key){
	int index = get_hash(key,ht->size,0);
	item* it = ht->items[index];
	
	int attempt = 1;
	while(it!=NULL){
		if(it!= &DELETED_ITEM)
			if(strcmp(it->key,key)==0) return it->value;
		
		index = get_hash(key, ht->size, attempt);
		it = ht->items[index];
		attempt++;
	}
	return NULL;
}


void ht_delete(hash_table* ht, const char* key){

	// resize
	const int load = ht->count*100/ht->size;
	if(load < 10){
		ht_resize_down(ht);
	}
	
	int index = get_hash(key,ht->size,0);
	item* item = ht->items[index];
	
	int attempt = 1;
	while(item !=NULL){
		if(item != &DELETED_ITEM){
			if(strcmp(item->key,key)==0){
				del_item(item);
				ht->items[index] = &DELETED_ITEM;
			}
		}
	index = get_hash(key,ht->size,attempt);
	item = ht->items[index];
	attempt++;
	}
	ht->count--;
}

static void ht_resize(hash_table* ht, const int base_size){
	if(base_size < HT_INITIAL_BASE_SIZE) return;

	hash_table* new_ht = ht_new_sized(base_size);
	for(int i=0;i<ht->size;i++){
		item* it = ht->items[i];
		if(it != NULL && it != &DELETED_ITEM){
			ht_insert(new_ht, it->key, it->value);
		}
	}

	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;

	const int tmp_size = ht->size;
	ht->size = new_ht->size;

	item** tmp_items = ht->items;
	ht->items = new_ht->items;
	new_ht->items = tmp_items;

	del_hash_table(new_ht);
}

static void ht_resize_up(hash_table* ht){
	const int new_size = ht->base_size*2;
	ht_resize(ht,new_size);
}

static void ht_resize_down(hash_table* ht){
	const int new_size = ht->base_size/2;
	ht_resize(ht,new_size);
}