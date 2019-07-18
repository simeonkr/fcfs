#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include "bloom_filter.h"
#include "hash.h"

struct fcfs_config {
    char *mnt_point;
    char *root_dir;
    char *db_file;
    char *log_file;
    FILE *log_f;
    bloom_filter actionable;
    hash_table file_flags;
    hash_table tag_actions;
} config;

extern struct fcfs_config config;

int get_file_flags(const char *file_path);

#define ACTION_LOG 1
#define ACTION_COMPRESS 2
#define ACTION_ENCRYPT 3
#define ACTION_BACKUP 4

#endif /* CONFIG_H */
