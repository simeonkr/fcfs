#include <fuse.h>

int pass_getattr(const char *path, struct stat *stbuf);

int pass_access(const char *path, int mask);

int pass_readlink(const char *path, char *buf, size_t size);

int pass_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
               off_t offset, struct fuse_file_info *fi);

int pass_mknod(const char *path, mode_t mode, dev_t rdev);

int pass_mkdir(const char *path, mode_t mode);

int pass_unlink(const char *path);

int pass_rmdir(const char *path);

int pass_symlink(const char *from, const char *to);

int pass_rename(const char *from, const char *to);

int pass_link(const char *from, const char *to);

int pass_chmod(const char *path, mode_t mode);

int pass_chown(const char *path, uid_t uid, gid_t gid);

int pass_truncate(const char *path, off_t size);

#ifdef HAVE_UTIMENSAT
int pass_utimens(const char *path, const struct timespec ts[2]);
#endif

int pass_open(const char *path, struct fuse_file_info *fi);

int pass_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi);

int pass_write(const char *path, const char *buf, size_t size,
             off_t offset, struct fuse_file_info *fi);

int pass_statfs(const char *path, struct statvfs *stbuf);

int pass_release(const char *path, struct fuse_file_info *fi);

int pass_fsync(const char *path, int isdatasync,
             struct fuse_file_info *fi);
#ifdef HAVE_POSIX_FALLOCATE
int pass_fallocate(const char *path, int mode,
            off_t offset, off_t length, struct fuse_file_info *fi);
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
int pass_setxattr(const char *path, const char *name, const char *value,
            size_t size, int flags);

int pass_getxattr(const char *path, const char *name, char *value,
            size_t size);

int pass_listxattr(const char *path, char *list, size_t size);

int pass_removexattr(const char *path, const char *name);
#endif /* HAVE_SETXATTR */
