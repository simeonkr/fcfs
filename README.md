The goal of this project is to implement a [FUSE](https://en.wikipedia.org/wiki/Filesystem_in_Userspace) file system for Linux that wraps an existing file system to provide additional
features such as tagging and simple per-file versioning, logging and encryption (all selectively-configured via tags).
Its aims are to augment any given file system with functionality centered on the
organization and management of personal data and also to be extensible by
providing a simple interface for adding new features.

Users can tag files and then specify tag-based or pattern-based *actions* on files through a configuration file. For example, if the configuration file has lines like

```
encrypt tag 'secure'  
```

```
log regex */sys/*  
```

then all files tagged `'secure'` will be stored encrypted on the (underlying) file system and all files matching the pattern `*/sys/*` will have their modifications logged.

See the [todo](todo.md) for current progress.