#include "config.h"
#include "bloom_filter.h"
#include "hash.h"

int get_file_flags(const char *file_path) {
    hash_entry *entry;
    if (bloom_filter_lookup(&config.actionable, (hash_key){.p = file_path}) == 0)
        return 0;
    else if ((entry = hash_table_lookup(&config.file_flags, (hash_key){.p = file_path})) == NULL)
        return 0;
    else
        return entry->val.i;
}