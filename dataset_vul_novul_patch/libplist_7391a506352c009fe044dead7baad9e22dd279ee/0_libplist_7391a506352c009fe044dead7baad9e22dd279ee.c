int main(int argc, char *argv[])
{
    FILE *iplist = NULL;
    plist_t root_node = NULL;
    char *plist_out = NULL;
    uint32_t size = 0;
    int read_size = 0;
    char *plist_entire = NULL;
    struct stat filestats;
    options_t *options = parse_arguments(argc, argv);

    if (!options)
    {
        print_usage(argc, argv);
        return 0;
    }

    iplist = fopen(options->in_file, "rb");
    if (!iplist) {
        free(options);
        return 1;
     }
 
     stat(options->in_file, &filestats);

    if (filestats.st_size < 8) {
        printf("ERROR: Input file is too small to contain valid plist data.\n");
        return -1;
    }

     plist_entire = (char *) malloc(sizeof(char) * (filestats.st_size + 1));
     read_size = fread(plist_entire, sizeof(char), filestats.st_size, iplist);
     fclose(iplist);

    if (memcmp(plist_entire, "bplist00", 8) == 0)
    {
        plist_from_bin(plist_entire, read_size, &root_node);
        plist_to_xml(root_node, &plist_out, &size);
    }
    else
    {
        plist_from_xml(plist_entire, read_size, &root_node);
        plist_to_bin(root_node, &plist_out, &size);
    }
    plist_free(root_node);
    free(plist_entire);

    if (plist_out)
    {
        if (options->out_file != NULL)
        {
            FILE *oplist = fopen(options->out_file, "wb");
            if (!oplist) {
                free(options);
                return 1;
            }
            fwrite(plist_out, size, sizeof(char), oplist);
            fclose(oplist);
        }
        else
            fwrite(plist_out, size, sizeof(char), stdout);

        free(plist_out);
    }
    else
        printf("ERROR: Failed to convert input file.\n");

    free(options);
    return 0;
}
