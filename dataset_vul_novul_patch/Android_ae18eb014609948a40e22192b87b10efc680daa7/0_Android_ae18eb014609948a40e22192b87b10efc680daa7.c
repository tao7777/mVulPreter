 static void print_maps(struct pid_info_t* info)
 {
     FILE *maps;

     size_t offset;
     char device[10];
     long int inode;
    char file[1024];
 
     strlcat(info->path, "maps", sizeof(info->path));
 
    maps = fopen(info->path, "r");

     if (!maps)
         goto out;
 
    while (fscanf(maps, "%*x-%*x %*s %zx %5s %ld %1023s\n",
                  &offset, device, &inode, file) == 4) {
         if (inode == 0 || !strcmp(device, "00:00"))
             continue;

        printf("%-9s %5d %10s %4s %9s %18s %9zd %10ld %s\n",
                info->cmdline, info->pid, info->user, "mem",
 "???", device, offset, inode, file);
 }

    fclose(maps);

out:
    info->path[info->parent_length] = '\0';
}
