
#include <vfs.h>
#include <malloc.h>
#include <string.h>
#include <tree.h>

/*  Base info about vfs */
gtree_t*    vfs_tree;
vfs_node_t* vfs_root;

uint32_t
vfs_get_file_size(vfs_node_t* node)
{
        if (node && node->get_file_size)
                return node->get_file_size(node);

        return 0;
}

void
vfs_db_listdir(char* name)
{
        vfs_node_t* node = file_open(name, 0);
        if (!node){
                printf("Could not open file\n");
                return 0;
        }

        if (!node->listdir) return;
        char** files = node->listdir(node);
        char** save  = files;
        while (*files){
                printf("%s ", *files);
                free(*files);
                files++;
        }
        free(save);
        printf("\n");
}

static void
__print_vfs_tree_recur(gtreenode_t* node, int parent_offset)
{
        if (!node) return;

        char* tmp = calloc(512, sizeof(char));
        int len = 0;

        for (unsigned int i = 0; i < parent_offset; ++i)
                strcat(tmp, " ");

        char* curr = tmp + strlen(tmp);
        vfs_entry_t* fnode = (vfs_entry_t*) node->value;
        if(fnode->file)
                printf(curr, " %s(0x%x, %s)", fnode->name, (unsigned int)fnode->file, fnode->file->name);
        else
                printf(curr, " %s(empty)", fnode->name);

        printf("%s\n", tmp);
        len = strlen(fnode->name);
        free(tmp);
        foreach(child, node->children){
                __print_vfs_tree_recur(child->val, parent_offset + len + 1);
        }

}

void
print_vfstree()
{
        __print_vfs_tree_recur(vfs_tree->root, 0);
}

/*  Physical fs wrappers */
uint32_t
vfs_read(vfs_node_t* node, uint32_t offset, uint32_t size, char* buffer)
{
        if(node && node->read){
                uint32_t ret = node->read(node, offset, size, buffer);
                return ret;
        }
        return -1;
}

uint32_t
vfs_write(vfs_node_t* node, uint32_t offset, uint32_t size, char* buffer)
{
        if (node && node->write){
                uint32_t ret = node->write(node, offset, size, buffer);
                return ret;
        }
        return -1;
}

void
vfs_open(vfs_node_t* node, uint32_t flags)
{
        if (!node) return;
        if (node->refcount >= 0) node->refcount ++;
        node->open(node, flags);
}

void
vfs_close(vfs_node_t* node)
{
        if(!node || node == vfs_root || node->refcount == -1) return; // cannot close device
        node->refcount--;
        if(node->refcount == 0)
                node->close(node);
}

void
vfs_chmod(vfs_node_t* node, uint32_t mode)
{
        if(node->chmod) return node->chmod(node, mode);
}

dirent_t*
vfs_readdir(vfs_node_t* node, unsigned int index)
{
        if(node && (node->flags & FS_DIR) && node->finddir)
                return node->finddir(node, index);
        return NULL;
}

vfs_node_t*
vfs_finddir(vfs_node_t* node, char* name)
{
        if(node && (node->flags & FS_DIR) && node->finddir)
                return node->finddir(node, name);
        return NULL;
}

int
vfs_ioctl(vfs_node_t* node, int request, void* arg)
{
        if (!node) return -1;
        if(node->ioctl) return node->ioctl(node, request, arg);
        return 25;
}

void
vfs_mkdir(char* name, uint16_t permission)
{
        // extract parent directory and dir to be made
        int i = strlen(name);
        char* dirname = strdup(name);
        char* save_dirname = dirname;
        char* parent_path = "/";
        while (i >= 0){
                if(dirname[i] == '/'){
                        if(i != 0){
                                dirname[i] = '\0';
                                parent_path = dirname;
                        }
                        dirname = &dirname[i++];
                        break;
                }
                i--;
        }

        // open file
        vfs_node_t* parent = file_open(parent_path, 0);
        if(!parent)
                free(save_dirname);

        // call mkdir
        if(parent->mkdir)
                parent->mkdir(parent, dirname, permission);
        free(save_dirname);

        vfs_close(parent);
}

// might throw error
int
vfs_create_file(char* name, uint16_t permission)
{
        // extract parent directory and dir to be made
        int i = strlen(name);
        char* dirname = strdup(name);
        char* save_dirname = dirname;
        char* parent_path = "/";
        while (i >= 0){
                if(dirname[i] == '/'){
                        if(i != 0){
                                dirname[i] = '\0';
                                parent_path = dirname;
                        }
                        dirname = &dirname[i++];
                        break;
                }
                i--;
        }

        // open parent
        vfs_node_t* parent = file_open(parent_path, 0);
        if(!parent){
                free(save_dirname);
                return -1;
        }

        if (parent->create)
                parent->create(parent, dirname, permission);
        free(save_dirname);
        vfs_close(parent);
        return 0;}

void
vfs_unlink(char* name)
{
        // extract parent directory and dir to be made
        int i = strlen(name);
        char* dirname = strdup(name);
        char* save_dirname = dirname;
        char* parent_path = "/";
        while (i >= 0){
                if(dirname[i] == '/'){
                        if(i != 0){
                                dirname[i] = '\0';
                                parent_path = dirname;
                        }
                        dirname = &dirname[i++];
                        break;
                }
                i--;
        }

        // open parent
        vfs_node_t* parent = file_open(parent_path, 0);
        if(!parent){
                free(save_dirname);
                return -1;
        }

        // call sys call
        if(parent->unlink)
                parent->unlink(parent, dirname);

        free(save_dirname);
        vfs_close(parent);
}

char*
expand_path(char* input)
{
        list_t* input_list = str_split(input, "/", NULL);
        char* ret = list2str(input_list, "/");
        return ret;
}

static vfs_node_t*
__get_mountpoint_recur(char** path, gtreenode_t* subroot)
{
        int found = 0;
        char* curr_token = strsep(path, "/");

        if(curr_token == NULL || !strcmp(curr_token, "")){
                vfs_entry_t* ent = (vfs_entry_t*) subroot->value;
                return ent->file;
        }

        foreach(child, subroot->children){
                gtreenode_t* tchild = (gtreenode_t*) child->val;
                vfs_entry_t* ent = (vfs_entry_t*)(tchild->value);
                if(strcmp(ent->name, curr_token) == 0){
                        found = 1;
                        subroot = tchild;
                        break;
                }
        }

        if (!found){
                *path = curr_token;
                return ((vfs_entry_t*)(subroot->value))->file;
        }

        return __get_mountpoint_recur(path, subroot);
}

vfs_node_t*
get_mountpoint(char** path)
{
        if(strlen(*path) > 1 && (*path)[strlen(*path)] == '/')
                *(path)[strlen(*path) - 1] = '\0';
        if(!*path || *(path)[0] != '/') return NULL;
        if (strlen(*path) == 1){
                *path = '\0';
                vfs_entry_t* ent = (vfs_entry_t*) vfs_tree->root->value;
                return ent->file;
        }
        (*path)++;
        return __get_mountpoint_recur(path, vfs_tree->root);
}

vfs_node_t*
file_open(const char* _filename, uint32_t flags)
{
        char* curr_token = NULL;
        char* filename = strdup(_filename);
        char* free_filename = filename;
        char* save = strdup(filename);
        char* original_filename = filename;
        char* new_start = NULL;
        vfs_node_t* nextnode = NULL;
        vfs_node_t* startpoint = get_mountpoint(&filename);
        if(!startpoint) return NULL;
        if (filename)
                new_start = strstr(save + (filename - original_filename), filename);
        while (filename != NULL && ((curr_token = strsep(&new_start, "/")) != NULL)) {
                nextnode = vfs_finddir(startpoint, curr_token);
                if (!nextnode) return NULL;
                startpoint = nextnode;
        }

        if (!nextnode)
                nextnode = startpoint;

        vfs_open(nextnode, flags);
        free(save);
        free(free_filename);

        return nextnode;
}

void
vfs_init()
{
        vfs_tree = tree_create();
        vfs_entry_t* root = malloc(sizeof(vfs_entry_t));
        root->name = strdup("root");
        root->file = NULL;
        tree_insert(vfs_tree, NULL, root);
}

/*  Mounting a device */
void
vfs_mount_dev(char* mountpoint, vfs_node_t* node)
{
        vfs_mount(mountpoint, node);
}

static void
__vfs_mount_recur(char* path, gtreenode_t* subroot, vfs_node_t* fs_obj)
{
        int found = 0;
        char* curr_token = strsep(&path, "/");

        if (curr_token == NULL || !strcmp(curr_token, "")) {
                vfs_entry_t* ent = (vfs_entry_t*) subroot->value;
                if (ent->file){
                        printf("This file is already mounted. Please select a new one\n");
                        return;
                }

                if (!strcmp(ent->name, "/")) vfs_root = fs_obj;
                ent->file = fs_obj;
                return;
        }

        foreach(child, subroot->children){
                gtreenode_t* tchild = (gtreenode_t*) child->val;
                vfs_entry_t* ent = (vfs_entry_t*)(tchild->value);
                if(strcmp(ent->name, curr_token) == 0){
                        found = 1;
                        subroot = tchild;
                }
        }

        if (!found){
                vfs_entry_t* ent = calloc(1, sizeof(vfs_entry_t));
                ent->name = strdup(curr_token);
                subroot = tree_insert(vfs_tree, subroot, ent);
        }

        __vfs_mount_recur(path, subroot, fs_obj);
}

void
vfs_mount(char* path, vfs_node_t* fs_obj)
{
        fs_obj->refcount = -1;
        fs_obj->fs_type  = 0;
        if(path[0] == '/' && strlen(path) == 1){
                vfs_entry_t* ent = (vfs_entry_t*) vfs_tree->root->value;
                if(ent->file){
                        printf("Path is already mounted\n");
                        return;
                }

                vfs_root = fs_obj;
                ent->file = fs_obj;
                return;
        }

        __vfs_mount_recur(path + 1, vfs_tree->root, fs_obj);
}
