
#ifndef VFS_H
#define VFS_H 1

#include <stdint.h>
#include <tree.h>
#include <list.h>

/*  Path stuff */
#define PATH_SEPARATOR     '/'
#define PATH_SEPARATOR_STR "/"
#define PATH_UP            ".."
#define PATH_DOT           "."
#define VFS_EXT2_MAGIC      0xeeee2222

/*  Permission bits */
#define O_RDONLY     0x0000
#define O_WRONLY     0x0001
#define O_RDRW       0x0002
#define O_APPEND     0x0008
#define O_CREAT      0x0200
#define O_TRUNC      0x0400
#define O_EXCL       0x0800
#define O_NOFOLLOW   0x1000
#define O_PATH       0x2000

/*  File system types */
#define FS_FILE      0x01
#define FS_DIR       0x02
#define FS_CHARDEV   0x04
#define FS_BLOCKDEV  0x08
#define FS_PIPE      0x10
#define FS_SYMLINK   0x20
#define FS_MNTPT     0x40

/*  EXT2 fs types */
#define _IFMT   0170000 /*  Type of file      */
#define _IFDIR  0040000 /*  Directory         */
#define _IFCHR  0020000 /*  Character special */
#define _IFBLK  0060000 /*  Block special     */
#define _IFREG  0100000 /*  Regular           */
#define _IFLNK  0120000 /*  Symbolic link     */
#define _IFSOCK 0140000 /*  Socket            */
#define _IFIFO  0010000 /*  FIFO              */

struct vfs_node;
struct dir_ent;

/*  Callback type definitions */
typedef uint32_t         (*get_file_size_callback)(struct vfs_node*);
typedef uint32_t         (*read_callback)         (struct vfs_node*, uint32_t, uint32_t, char*);
typedef uint32_t         (*write_callback)        (struct vfs_node*, uint32_t, uint32_t, char*);
typedef void             (*open_callback)         (struct vfs_node*, uint32_t);
typedef void             (*close_callback)        (struct vfs_node*);
typedef struct dirent*   (*readdir_callback)      (struct vfs_node*, uint32_t);
typedef struct vfs_node* (*finddir_callback)      (struct vfs_node*, char*);
typedef void             (*create_callback)       (struct vfs_node*, char*, uint16_t);
typedef void             (*unlink_callback)       (struct vfs_node*, char*);
typedef void             (*mkdir_callback)        (struct vfs_node*, char*, uint16_t);
typedef int              (*ioctl_callback)        (struct vfs_node*, int, void*);
typedef int              (*get_size_callback)     (struct vfs_node*);
typedef void             (*chmod_callback)        (struct vfs_node*, uint32_t);
typedef char**           (*listdir_callback)      (struct vfs_node*);

typedef struct vfs_node
{
        // File info
        char     name[256]; // node name
        void*    device; // device on which node resides?
        uint32_t mask;
        uint32_t uid;
        uint32_t gid;
        uint32_t flags;
        uint32_t inode_num;
        uint32_t size;
        uint32_t fs_type;
        uint32_t open_flags;
        // time data
        uint32_t create_time;
        uint32_t access_time;
        uint32_t modified_time;

        uint32_t     offset; // disk geo
        unsigned int nlink; // IDK
        int          refcount;

        // File operations
        read_callback           read;
        write_callback          write;
        open_callback           open;
        close_callback          close;
        readdir_callback        readdir;
        finddir_callback        finddir;
        create_callback         create;
        unlink_callback         unlink;
        mkdir_callback          mkdir;
        ioctl_callback          ioctl;
        get_size_callback       get_size;
        chmod_callback          chmod;
        get_file_size_callback  get_file_size;
        listdir_callback        listdir;

} vfs_node_t;

typedef struct dirent
{
        char     name[256];
        uint32_t inode_num;
} dirent_t;

typedef struct vfs_entry
{
        char*       name;
        vfs_node_t* file;
} vfs_entry_t;

// Function Declarations
uint32_t     vfs_get_file_size(vfs_node_t*);
uint32_t     vfs_read(vfs_node_t*, uint32_t, uint32_t, char*);
uint32_t     vfs_write(vfs_node_t*, uint32_t, uint32_t, char*);
void         vfs_open(vfs_node_t*, uint32_t);
void         vfs_close(vfs_node_t*);
vfs_node_t*  vfs_finddir(vfs_node_t*, char*);
void         vfs_mkdir(char*, uint16_t);
void         vfs_mkfile(char*, uint16_t);
int          vfs_create_file(char*, uint16_t);
vfs_node_t*  file_open(const char*, uint32_t);
char*        expand_path(char*);
int          vfs_ioctl(vfs_node_t*, int, void*);
void         vfs_chmod(vfs_node_t*, uint32_t);
void         vfs_unlink(char*);
int          vfs_symlink(char*, char*);
int          vfs_readlink(vfs_node_t*, char*, uint32_t);
void         vfs_init();
void         vfs_mount(char*, vfs_node_t*);
typedef vfs_node_t* (*vfs_mount_callback)(char*, char*);
void         vfs_register(char*, vfs_mount_callback);
void         vfs_mount_dev(char*, vfs_node_t*);
void         print_vfstree();
void         vfs_db_listdir(char*);

#endif // VFS_H
