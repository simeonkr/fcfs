#include <stdio.h>
#include "tag.h"
#include "dbase.h"
#include "fs_update.h"

void tag(char *file, char *tag) {
    db_store_tag(file, tag);
    fs_tag_sig(file, tag);
}

void untag(char *file, char *tag) {
    db_remove_tag(file, tag);
    fs_untag_sig(file, tag);
}

void list_tags(char *file) {
    list tags = db_lookup_file_tags(file);
    for (list_node *tag_node = tags.head; tag_node != NULL; tag_node = tag_node->next) {
        printf("%s\n", (char*)tag_node->data);
    }
}

void list_files(char *tag) {
    list files = db_tag_query(tag);
    for (list_node *file_node = files.head; file_node != NULL; file_node = file_node->next) {
        printf("%s\n", (char*)file_node->data);
    }  
}
