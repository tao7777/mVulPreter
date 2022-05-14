 static bool dump_fd_info(const char *dest_filename, char *source_filename, int source_base_ofs, uid_t uid, gid_t gid)
 {
    FILE *fp = fopen(dest_filename, "wx");
     if (!fp)
         return false;
 
    unsigned fd = 0;
    while (fd <= 99999) /* paranoia check */
    {
        sprintf(source_filename + source_base_ofs, "fd/%u", fd);
        char *name = malloc_readlink(source_filename);
        if (!name)
            break;
        fprintf(fp, "%u:%s\n", fd, name);
        free(name);

        sprintf(source_filename + source_base_ofs, "fdinfo/%u", fd);
        fd++;
        FILE *in = fopen(source_filename, "r");
        if (!in)
            continue;
        char buf[128];
        while (fgets(buf, sizeof(buf)-1, in))
        {
            /* in case the line is not terminated, terminate it */
            char *eol = strchrnul(buf, '\n');
            eol[0] = '\n';
            eol[1] = '\0';
            fputs(buf, fp);
        }
        fclose(in);
    }

    const int dest_fd = fileno(fp);
    if (fchown(dest_fd, uid, gid) < 0)
    {
        perror_msg("Can't change '%s' ownership to %lu:%lu", dest_filename, (long)uid, (long)gid);
        fclose(fp);
        unlink(dest_filename);
        return false;
    }

    fclose(fp);
    return true;
}
