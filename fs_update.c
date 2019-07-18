#include "bloom_filter.h"
#include "hash.h"
#include "config.h"

void fs_tag_sig(char *file, char *tag) {
}

void fs_untag_sig(char *file, char *tag) {
}

void fs_tag(char *file, char *tag) {
    hash_entry *tag_entry = hash_table_lookup(&config.tag_actions, (hash_key){.p = tag});
    if (tag_entry != NULL) {
        int flags = get_file_flags(file);
        if (flags == 0) {
            bloom_filter_insert(&config.actionable, (hash_key){.p = file});
        }
        hash_table_insert(&config.file_flags, 
            (hash_key){.p = file}, (hash_val){.i = flags | tag_entry->val.i});
    }
}

void fs_untag(char *file, char *tag) {
    /* this is more complicated: before disabling an action must lookup all tags
       for file to make sure there isn't another tag associated with action */
}
