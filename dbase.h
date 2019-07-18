#ifndef DBASE_H
#define DBASE_H

#include "list.h"

void db_init();

void db_store_tag(char *file, char *tag);

void db_remove_tag(char *file, char *tag);

list db_lookup_file_tags(char *file);

list db_tag_query(char *tag);

list db_tag_list_query(char *tag_list);

void db_close();

#endif /* DBASE_H */
