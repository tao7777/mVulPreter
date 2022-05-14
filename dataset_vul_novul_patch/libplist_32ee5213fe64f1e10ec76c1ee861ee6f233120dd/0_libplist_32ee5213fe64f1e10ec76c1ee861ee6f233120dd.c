static plist_t parse_bin_node(struct bplist_data *bplist, const char** object)
{
    uint16_t type = 0;
    uint64_t size = 0;

    if (!object)
        return NULL;

    type = (**object) & BPLIST_MASK;
    size = (**object) & BPLIST_FILL;
    (*object)++;

    if (size == BPLIST_FILL) {
        switch (type) {
        case BPLIST_DATA:
        case BPLIST_STRING:
        case BPLIST_UNICODE:
        case BPLIST_ARRAY:
        case BPLIST_SET:
        case BPLIST_DICT:
        {
            uint16_t next_size = **object & BPLIST_FILL;
            if ((**object & BPLIST_MASK) != BPLIST_UINT) {
                PLIST_BIN_ERR("%s: invalid size node type for node type 0x%02x: found 0x%02x, expected 0x%02x\n", __func__, type, **object & BPLIST_MASK, BPLIST_UINT);
                return NULL;
            }
            (*object)++;
            next_size = 1 << next_size;
            if (*object + next_size > bplist->offset_table) {
                PLIST_BIN_ERR("%s: size node data bytes for node type 0x%02x point outside of valid range\n", __func__, type);
                return NULL;
            }
            size = UINT_TO_HOST(*object, next_size);
            (*object) += next_size;
            break;
        }
        default:
            break;
        }
    }

    switch (type)
    {

    case BPLIST_NULL:
        switch (size)
        {

        case BPLIST_TRUE:
        {
            plist_data_t data = plist_new_plist_data();
            data->type = PLIST_BOOLEAN;
            data->boolval = TRUE;
            data->length = 1;
            return node_create(NULL, data);
        }

        case BPLIST_FALSE:
        {
            plist_data_t data = plist_new_plist_data();
            data->type = PLIST_BOOLEAN;
            data->boolval = FALSE;
            data->length = 1;
            return node_create(NULL, data);
        }

        case BPLIST_NULL:
        default:
            return NULL;
        }

    case BPLIST_UINT:
        if (*object + (uint64_t)(1 << size) > bplist->offset_table) {
            PLIST_BIN_ERR("%s: BPLIST_UINT data bytes point outside of valid range\n", __func__);
            return NULL;
        }
        return parse_uint_node(object, size);

    case BPLIST_REAL:
        if (*object + (uint64_t)(1 << size) > bplist->offset_table) {
            PLIST_BIN_ERR("%s: BPLIST_REAL data bytes point outside of valid range\n", __func__);
            return NULL;
        }
        return parse_real_node(object, size);

    case BPLIST_DATE:
        if (3 != size) {
            PLIST_BIN_ERR("%s: invalid data size for BPLIST_DATE node\n", __func__);
            return NULL;
        }
        if (*object + (uint64_t)(1 << size) > bplist->offset_table) {
            PLIST_BIN_ERR("%s: BPLIST_DATE data bytes point outside of valid range\n", __func__);
            return NULL;
        }
         return parse_date_node(object, size);
 
     case BPLIST_DATA:
        if (*object + size < *object || *object + size > bplist->offset_table) {
             PLIST_BIN_ERR("%s: BPLIST_DATA data bytes point outside of valid range\n", __func__);
             return NULL;
         }
         return parse_data_node(object, size);
 
     case BPLIST_STRING:
        if (*object + size < *object || *object + size > bplist->offset_table) {
             PLIST_BIN_ERR("%s: BPLIST_STRING data bytes point outside of valid range\n", __func__);
             return NULL;
         }
        return parse_string_node(object, size);

    case BPLIST_UNICODE:
        if (size*2 < size) {
             PLIST_BIN_ERR("%s: Integer overflow when calculating BPLIST_UNICODE data size.\n", __func__);
             return NULL;
         }
        if (*object + size*2 < *object || *object + size*2 > bplist->offset_table) {
             PLIST_BIN_ERR("%s: BPLIST_UNICODE data bytes point outside of valid range\n", __func__);
             return NULL;
         }
         return parse_unicode_node(object, size);
 
     case BPLIST_SET:
     case BPLIST_ARRAY:
        if (*object + size < *object || *object + size > bplist->offset_table) {
             PLIST_BIN_ERR("%s: BPLIST_ARRAY data bytes point outside of valid range\n", __func__);
             return NULL;
         }
        return parse_array_node(bplist, object, size);

    case BPLIST_UID:
        if (*object + size+1 > bplist->offset_table) {
            PLIST_BIN_ERR("%s: BPLIST_UID data bytes point outside of valid range\n", __func__);
            return NULL;
        }
         return parse_uid_node(object, size);
 
     case BPLIST_DICT:
        if (*object + size < *object || *object + size > bplist->offset_table) {
            PLIST_BIN_ERR("%s: BPLIST_DICT data bytes point outside of valid range\n", __func__);
             return NULL;
         }
         return parse_dict_node(bplist, object, size);

    default:
        PLIST_BIN_ERR("%s: unexpected node type 0x%02x\n", __func__, type);
        return NULL;
    }
    return NULL;
}
