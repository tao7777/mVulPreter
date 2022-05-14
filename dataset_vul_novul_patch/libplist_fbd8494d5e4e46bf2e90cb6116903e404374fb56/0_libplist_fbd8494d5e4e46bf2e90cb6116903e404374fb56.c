static plist_t parse_string_node(const char **bnode, uint64_t size)
{
    plist_data_t data = plist_new_plist_data();
 
     data->type = PLIST_STRING;
     data->strval = (char *) malloc(sizeof(char) * (size + 1));
    if (!data->strval) {
        plist_free_data(data);
        PLIST_BIN_ERR("%s: Could not allocate %" PRIu64 " bytes\n", __func__, sizeof(char) * (size + 1));
        return NULL;
    }
     memcpy(data->strval, *bnode, size);
     data->strval[size] = '\0';
     data->length = strlen(data->strval);

    return node_create(NULL, data);
}
