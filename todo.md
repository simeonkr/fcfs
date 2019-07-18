* update on mount (config change may lead to certain files having their encrypted/compressed status changed)
* update on dbase change (when user tags during mount)
* unimplemented functions in `init.c`
* config file: remove need for quotes in tags
* canonical file paths

---

* refactor: create a void ptr / scalar type
* refactor: use consts

---

* more advanced logging?
* shrinking hash table (not really necessary)
* testing on large directories

---

* generate symlinks for tags
* encryption (use OpenSSL's `EVP_aes_128_ctr`)
* compression
* versioning/backup
	* write protection for backups
	* commands to restore files to an earlier time
	* command to perform integrity checks for backup
	* restore to previous point in time
	* user-defined checkpoints
* null dirs
* trash can (like backup but only activated on deletion)
* usage statistics