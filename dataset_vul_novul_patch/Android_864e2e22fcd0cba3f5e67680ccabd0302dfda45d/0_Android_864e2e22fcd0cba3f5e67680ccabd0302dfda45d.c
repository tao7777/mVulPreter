static ssize_t get_node_path_locked(struct node* node, char* buf, size_t bufsize) {
 const char* name;
 size_t namelen;
 if (node->graft_path) {
        name = node->graft_path;
        namelen = node->graft_pathlen;
 } else if (node->actual_name) {
        name = node->actual_name;
        namelen = node->namelen;
 } else {
        name = node->name;
        namelen = node->namelen;
 }

 if (bufsize < namelen + 1) {
 return -1;
 }

 
     ssize_t pathlen = 0;
     if (node->parent && node->graft_path == NULL) {
        pathlen = get_node_path_locked(node->parent, buf, bufsize - namelen - 1);
         if (pathlen < 0) {
             return -1;
         }
        buf[pathlen++] = '/';
 }

    memcpy(buf + pathlen, name, namelen + 1); /* include trailing \0 */
 return pathlen + namelen;
}
