#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "init.h"
#include "dbase.h"
#include "tag.h"
#include "passthrough.h"
#include "fs_update.h"
#include "log.h"
#include "encrypt.h"
#include "bloom_filter.h"
#include "hash.h"

#define TIME_STR_LEN 64

char *CONFIG_FILE = "fcfs.config";

char root_dir[PATH_MAX];

static char *full_path(const char *path) {
    static char full[PATH_MAX];
    strncpy(full, root_dir, strlen(root_dir) + 1);
    strncat(full, path, PATH_MAX - strlen(root_dir));
    return full;
}

static int is_action_set(int flags, int action) {
    return (flags >> action) & 1;
}


static void time_str(char *out) {
    time_t now;
    time(&now);
    strftime(out, TIME_STR_LEN, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

static void log_msg(const char *msg, const char *path) {
    char time[TIME_STR_LEN];
    time_str(time);
    fprintf(config.log_f, "%s %s %s\n", time, msg, path);
    fflush(config.log_f);
}

static int fcfs_getattr(const char *path, struct stat *stbuf)
{
    int res = pass_getattr(full_path(path), stbuf);
    return res;
}

static int fcfs_access(const char *path, int mask)
{
    int res = pass_access(full_path(path), mask);
    return res;
}

static int fcfs_readlink(const char *path, char *buf, size_t size)
{
    int res = pass_readlink(full_path(path), buf, size);
    return res;
}

static int fcfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
               off_t offset, struct fuse_file_info *fi)
{
    int res = pass_readdir(full_path(path), buf, filler, offset, fi);
    return res;
}

static int fcfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res = pass_mknod(full_path(path), mode, rdev);
    return res;
}

static int fcfs_mkdir(const char *path, mode_t mode)
{
    int res = pass_mkdir(full_path(path), mode);
    return res;
}

static int fcfs_unlink(const char *path)
{
    int res = pass_unlink(full_path(path));
    return res;
}

static int fcfs_rmdir(const char *path)
{
    int res = pass_rmdir(full_path(path));
    return res;
}

static int fcfs_symlink(const char *from, const char *to)
{
    int res = pass_symlink(from, full_path(to));
    return res;
}

static int fcfs_rename(const char *from, const char *to)
{
    int res = pass_rename(full_path(from), full_path(to));
    return res;
}

static int fcfs_link(const char *from, const char *to)
{
    int res = pass_link(full_path(from), full_path(to));
    return res;
}

static int fcfs_chmod(const char *path, mode_t mode)
{
    int res = pass_chmod(full_path(path), mode);
    return res;
}

static int fcfs_chown(const char *path, uid_t uid, gid_t gid)
{
    int res = pass_chown(full_path(path), uid, gid);
    return res;
}

static int fcfs_truncate(const char *path, off_t size)
{
    int res = pass_truncate(full_path(path), size);
    return res;
}

#ifdef HAVE_UTIMENSAT
static int fcfs_utimens(const char *path, const struct timespec ts[2])
{
    int res = pass_utimens(full_path(path), size);
    return res;
}
#endif

static int fcfs_open(const char *path, struct fuse_file_info *fi)
{
    int res = pass_release(full_path(path), fi);
    return res;
}

static int fcfs_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi)
{   
    int res = pass_read(full_path(path), buf, size, offset, fi);
    return res;
}

static int fcfs_write(const char *path, const char *buf, size_t size,
             off_t offset, struct fuse_file_info *fi)
{
    int flags = get_file_flags(path);
    if (is_action_set(flags, ACTION_LOG)) {
        log_msg("Modified", path);
    }
    int res = pass_write(full_path(path), buf, size, offset, fi);
    return res;
}

static int fcfs_statfs(const char *path, struct statvfs *stbuf)
{
    int res = pass_statfs(full_path(path), stbuf);
    return res;
}

static int fcfs_release(const char *path, struct fuse_file_info *fi)
{
    int res = pass_release(full_path(path), fi);
    return res;
}

static int fcfs_fsync(const char *path, int isdatasync,
             struct fuse_file_info *fi)
{
    int res = pass_fsync(full_path(path), isdatasync, fi);
    return res;
}

#ifdef HAVE_POSIX_FALLOCATE
static int fcfs_fallocate(const char *path, int mode,
            off_t offset, off_t length, struct fuse_file_info *fi)
{
    int res = pass_fallocate(full_path(path), mode, offset, length, fi);
    return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int fcfs_setxattr(const char *path, const char *name, const char *value,
            size_t size, int flags)
{
    int res = pass_setxattr(full_path(path), name, value, size, flags);
    return res;
}

static int fcfs_getxattr(const char *path, const char *name, char *value,
            size_t size)
{
    int res = pass_setxattr(full_path(path), name, value, size);
    return res;
}

static int fcfs_listxattr(const char *path, char *list, size_t size)
{
    int res = pass_setxattr(full_path(path), list, size);
    return res;
}

static int fcfs_removexattr(const char *path, const char *name)
{
    int res = pass_removexattr(full_path(path), name);
    return res;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations fcfs_oper = {
    .getattr    = fcfs_getattr,
    .access     = fcfs_access,
    .readlink   = fcfs_readlink,
    .readdir    = fcfs_readdir,
    .mknod      = fcfs_mknod,
    .mkdir      = fcfs_mkdir,
    .symlink    = fcfs_symlink,
    .unlink     = fcfs_unlink,
    .rmdir      = fcfs_rmdir,
    .rename     = fcfs_rename,
    .link       = fcfs_link,
    .chmod      = fcfs_chmod,
    .chown      = fcfs_chown,
    .truncate   = fcfs_truncate,
#ifdef HAVE_UTIMENSAT
    .utimens    = fcfs_utimens,
#endif
    .open       = fcfs_open,
    .read       = fcfs_read,
    .write      = fcfs_write,
    .statfs     = fcfs_statfs,
    .release    = fcfs_release,
    .fsync      = fcfs_fsync,
#ifdef HAVE_POSIX_FALLOCATE
    .fallocate  = fcfs_fallocate,
#endif
#ifdef HAVE_SETXATTR
    .setxattr   = fcfs_setxattr,
    .getxattr   = fcfs_getxattr,
    .listxattr  = fcfs_listxattr,
    .removexattr    = fcfs_removexattr,
#endif
};

int main(int argc, char *argv[]) {   
    parse_config(CONFIG_FILE);

    if (strcmp(argv[1], "mount") == 0) {
        parse_config_actions(CONFIG_FILE);
    
        // fcfs mount <mnt_path> <root> -> fcfs <mnt_path>
        char *fuse_argv[2];
        fuse_argv[0] = malloc(strlen(argv[0]) + 1);
        fuse_argv[1] = malloc(strlen(argv[2]) + 1);
        strncpy(fuse_argv[0], argv[0], strlen(argv[0]) + 1);
        strncpy(fuse_argv[1], argv[2], strlen(argv[2]) + 1);
        strncpy(root_dir, argv[3], strlen(argv[3]) + 1);
        
        umask(0);
        return fuse_main(2, fuse_argv, &fcfs_oper, NULL);
    }
    else if (strcmp(argv[1], "tag") == 0) {
        // TODO: recursive tag option
        if (argc == 4)
            tag(argv[2], argv[3]);
        else
            list_tags(argv[2]);
    }
    else if (strcmp(argv[1], "untag") == 0) {
        untag(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "tagged") == 0) {
        list_files(argv[2]);
    }
    
    db_close();
    fclose(config.log_f);
}
