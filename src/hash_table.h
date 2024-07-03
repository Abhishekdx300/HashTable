
typedef struct {
	char* key;
	char* value;
} item;

typedef struct {
	int size;
	int count;
	item** items;
} hash_table;

