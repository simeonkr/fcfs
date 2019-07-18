#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sqlite3.h>
#include "dbase.h"
#include "config.h"
#include "list.h"

int QUERY_SIZE = 1000 + PATH_MAX;

sqlite3 *db;

static int callback(void *res, int argc, char **argv, char **col_names) {
    for (int i = 0; i < argc; i++) {
        char *argvi = malloc(strlen(argv[i]) + 1);
        strncpy(argvi, argv[i], strlen(argv[i]) + 1);
        list_append(res, argvi);
    }
    return 0;
}

// TODO: use sqlite3_prepare
static list db_query(char *query) {
    list res = list_new();
    char *errmsg;
    
    int rc = sqlite3_exec(db, query, callback, &res, &errmsg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        exit(EXIT_FAILURE);
    }
    
    return res;
}

void db_init() { 
    int rc = sqlite3_open(config.db_file, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    
    char *query = "CREATE TABLE IF NOT EXISTS files ("
                  " file_id INTEGER PRIMARY KEY AUTOINCREMENT," 
                  " file_path TEXT NOT NULL UNIQUE ON CONFLICT IGNORE"
                  ");\n"
                  "CREATE INDEX IF NOT EXISTS file_paths ON files(file_path);"
                  "CREATE TABLE IF NOT EXISTS tags ("
                  " tag_id INTEGER PRIMARY KEY AUTOINCREMENT," 
                  " tag_name TEXT NOT NULL UNIQUE ON CONFLICT IGNORE"
                  ");\n"
                  "CREATE INDEX IF NOT EXISTS tag_names ON tags(tag_name);"
                  "CREATE TABLE IF NOT EXISTS attributes ("
                  " attribute_id INTEGER PRIMARY KEY AUTOINCREMENT," 
                  " attribute_name TEXT NOT NULL"
                  ");\n"
                  "CREATE INDEX IF NOT EXISTS attribute_names ON attributes(attribute_name);"
                  "CREATE TABLE IF NOT EXISTS file_tag_map ("
                  " file_id INTEGER," 
                  " tag_id INTEGER,"
                  " FOREIGN KEY(file_id) REFERENCES files(file_id),"
                  " FOREIGN KEY(tag_id) REFERENCES tags(tag_id),"
                  " PRIMARY KEY(file_id, tag_id)"
                  ");\n"
                  "CREATE TABLE IF NOT EXISTS file_attribute_map ("
                  " file_id INTEGER," 
                  " attribute_id INTEGER,"
                  " attribute_value TEXT NOT NULL,"
                  " FOREIGN KEY(file_id) REFERENCES files(file_id),"
                  " FOREIGN KEY(attribute_id) REFERENCES attributes(attribute_id),"
                  " PRIMARY KEY(file_id, attribute_id)"
                  ");";
    
    db_query(query);
}

void db_store_tag(char *file, char *tag) {
    char *query = malloc(QUERY_SIZE * sizeof(char));
        
    sprintf(query, 
            "INSERT INTO files (file_path)\n"
            "VALUES ('%s')",
            file);
    
    db_query(query);
     
    sprintf(query, 
            "INSERT INTO tags (tag_name)\n"
            "VALUES ('%s')",
            tag);
    
    db_query(query);
    
    sprintf(query, "INSERT OR REPLACE INTO file_tag_map (file_id, tag_id)\n"
                   "SELECT files.file_id, tags.tag_id\n"
                   "FROM files, tags\n"
                   "WHERE (files.file_path = '%s' AND tags.tag_name = '%s');",
                   file, tag);
    
    db_query(query); 
}

void db_remove_tag(char *file, char *tag) {
    char *query = malloc(QUERY_SIZE * sizeof(char));
    
    sprintf(query, 
            "DELETE FROM file_tag_map\n"
            "WHERE file_id IN\n"
            "  (SELECT file_id FROM files WHERE file_path = '%s')\n"
            "AND tag_id in\n"
            "  (SELECT tag_id FROM tags WHERE tag_name = '%s');\n"
            // delete orphaned files/tags
            "DELETE FROM files\n"
            "WHERE files.file_path = '%s' AND NOT EXISTS\n"
            "  (SELECT * FROM file_tag_map\n"
            "   WHERE files.file_id = file_tag_map.file_id"
            "   AND files.file_path = '%s');\n"
            "DELETE FROM tags\n"
            "WHERE tags.tag_name = '%s' AND NOT EXISTS\n"
            "  (SELECT * FROM file_tag_map\n"
            "   WHERE tags.tag_id = file_tag_map.tag_id"
            "   AND tags.tag_name = '%s');\n",
            file, tag, file, file, tag, tag);
    
    db_query(query);     
}

list db_lookup_file_tags(char *file) {
    char *query = malloc(QUERY_SIZE * sizeof(char));

    sprintf(query, 
            "SELECT tags.tag_name\n"
            "FROM files, tags, file_tag_map\n"
            "WHERE files.file_id = file_tag_map.file_id\n"
            "AND tags.tag_id = file_tag_map.tag_id\n"
            "AND files.file_path = '%s';",
            file);
    
    return db_query(query);
}

list db_tag_query(char *tag) {
    char *query = malloc(QUERY_SIZE * sizeof(char));

    sprintf(query, 
            "SELECT files.file_path\n"
            "FROM files, tags, file_tag_map\n"
            "WHERE files.file_id = file_tag_map.file_id\n"
            "AND tags.tag_id = file_tag_map.tag_id\n"
            "AND tags.tag_name = '%s';",
            tag);
    
    return db_query(query); 
}

list db_tag_list_query(char *tag_list) {
    char *query = malloc(QUERY_SIZE * sizeof(char));

    sprintf(query, 
            "SELECT DISTINCT files.file_path\n"
            "FROM files, tags, file_tag_map\n"
            "WHERE files.file_id = file_tag_map.file_id\n"
            "AND tags.tag_id = file_tag_map.tag_id\n"
            "AND tags.tag_name IN (%s);",
            tag_list);
    
    return db_query(query); 
}

void db_close() {
    sqlite3_close(db);
}
