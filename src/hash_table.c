#include<stdlib.h>
#include<string.h>

#include "hash_table.h"


const int HT_SIZE = 50;

static item* new_item(const char* k, const char* v){
	item* it = malloc(sizeof(item));
	it->key = strdup(k);
	it->value = strdup(v);
	return it;
}

hash_table* ht_new(){
	hash_table* ht = malloc(sizeof(hash_table));
	
	ht->size = HT_SIZE;
	ht->count = 0;
	ht->items = calloc((size_t)ht->size, sizeof(item*));
	return ht;
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

// hash function
static int hash(const char* str, const int a, const int size){
	long hsh = 0;
	const int len_s = strlen(str);
	for(int i=0;i<len_s;i++){
		hsh += (long)pow(a,len_s - (i+1)) * str[i];
		hsh = hsh % size;
	}
	return (int)hsh;
}

// for handling collision -- double hashing
static int get_hash(const char* s, const int num_buck, const int attempt){
	const int hash_a = hash(s,HT_PRIME_1, num_buck);
	const int hash_b = hash(s,HT_PRIME_2, num_buck);
	return (hash_a + (attempt*(hash_b + 1))) % num_buck;
}



