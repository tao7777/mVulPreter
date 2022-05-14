int main(int argc, char *argv[])
{
    char buff[1024];
    int fd, nr, nw;
    if (argc < 2) {
        fprintf(stderr,
                "usage: %s output-filename\n"
                "       %s |output-command\n"
                "       %s :host:port\n", argv[0], argv[0], argv[0]);
        return 1;
    }
    fd = open_gen_fd(argv[1]);
    if (fd < 0) {
        perror("open_gen_fd");
        exit(EXIT_FAILURE);
    }
    while ((nr = read(0, buff, sizeof (buff))) != 0) {
        if (nr < 0) {
            if (errno == EINTR)
                continue;
            perror("read");
            exit(EXIT_FAILURE);
        }
        nw = write(fd, buff, nr);
        if (nw < 0) {
            perror("write");
             exit(EXIT_FAILURE);
         }
     }
	close(fd);
     return 0;
 }
