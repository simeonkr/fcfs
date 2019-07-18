#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "init.h"
#include "dbase.h"
#include "bloom_filter.h"
#include "hash.h"
#include "util.h"

#define CONFIG_MAX_KEY 16
#define CONFIG_MAX_PATTERN 1024
#define BLOOM_FILTER_P 1e-4

static list recurse_into_dirs(list files) {
    return files;  // TODO: implement this
}

static list eliminate_subdirs(list files) {
    return files;  // TODO: implement this
}

list get_files_for_pattern(int action_type, char *pattern) {
    char *filter = strchr(pattern, ' ') + 1;
    char *filter_type = pattern;
    *(filter - 1) = '\0';
    
    list files;
    if (strcmp(filter_type, "tag") == 0) {
        files = db_tag_list_query(filter);
        
        list tags = split(filter, ' ');
        for (list_node *cur = tags.head; cur != NULL; cur = cur->next) {
            char *tag = cur->data;
            hash_table_insert(&config.tag_actions, 
                (hash_key){.p = tag}, (hash_val){.i = action_type});
        }
    }
    else if (strcmp(filter_type, "regex") == 0) {

    }
    if (action_type == ACTION_LOG)
        files = recurse_into_dirs(files);
    else
        files = eliminate_subdirs(files);
    
    return files;
}

typedef struct {
    int action_type;
    list files;
} action_pattern;

void parse_config(char *config_file) {
    // defaults
    config.db_file = "fcfs.db";
    config.log_file = "fcfs.log";

    FILE *f = fopen(config_file, "r");
    if (f == NULL) {
        f = fopen(config_file, "w");
        fclose(f);
        return;
    }
    
    char line[CONFIG_MAX_KEY + CONFIG_MAX_PATTERN];
    while (fgets(line, CONFIG_MAX_KEY + CONFIG_MAX_PATTERN, f) != NULL) {
        char *pattern = strchr(line, ' ') + 1;
        *strchr(pattern, '\n') = '\0';
        char *key = line;
        *(pattern - 1) = '\0';
        
        if (strcmp(key, "database_file") == 0) {
            config.db_file = pattern;
        }
        else if (strcmp(key, "log_file") == 0) {
            config.log_file = pattern;
        }
    }
   
    db_init();
    config.log_f = fopen(config.log_file, "a");
    
    fclose(f);
}

void parse_config_actions(char *config_file) {
    FILE *f = fopen(config_file, "r");
    
    list action_patterns = list_new();
    
    char line[CONFIG_MAX_KEY + CONFIG_MAX_PATTERN];
    while (fgets(line, CONFIG_MAX_KEY + CONFIG_MAX_PATTERN, f) != NULL) {
        char *pattern = strchr(line, ' ') + 1;
        *strchr(pattern, '\n') = '\0';
        char *key = line;
        *(pattern - 1) = '\0';
        
        int action_type;
        
        if (strcmp(key, "log") == 0) {
            action_type = ACTION_LOG;
        }
        else if (strcmp(key, "compress") == 0) {
            action_type = ACTION_COMPRESS;
        }
        else if (strcmp(key, "encrypt") == 0) {
            action_type = ACTION_ENCRYPT;
        }
        else if (strcmp(key, "backup") == 0) {
            action_type = ACTION_BACKUP;
        }
        else {
            continue;
        }
        
        action_pattern *ap = malloc(sizeof(action_pattern));
        ap->action_type = action_type;
        ap->files = get_files_for_pattern(ap->action_type, pattern);
        list_append(&action_patterns, ap);
    }
    
    int actionable_count = 0;
    config.file_flags = hash_table_new(h_string, key_string_comp);
    
    for (list_node *cur = action_patterns.head; cur != NULL; cur = cur->next) {
        action_pattern *ap = cur->data;
        for (list_node *cur_file = ap->files.head; cur_file != NULL; cur_file = cur_file->next) {
            char *file_path = (char *)cur_file->data;
            
            hash_entry *entry = hash_table_lookup_insert(
                &config.file_flags, (hash_key){.p = file_path}, 
                (hash_val){.i = 1 << ap->action_type});
            if (entry == NULL) {
                actionable_count++;
            }
            else {
                entry->val.i |= 1 << ap->action_type;
            }
        }
    }
    
    config.actionable = bloom_filter_new(h_string, actionable_count, BLOOM_FILTER_P);
    
    for (list_node *cur = action_patterns.head; cur != NULL; cur = cur->next) {
        action_pattern *ap = cur->data;
        for (list_node *cur_file = ap->files.head; cur_file != NULL; cur_file = cur_file->next) {
            char *file_path = (char *)cur_file->data;
            
            bloom_filter_insert(&config.actionable, (hash_key){.p = file_path});
        }
    }
    
    fclose(f);
}
