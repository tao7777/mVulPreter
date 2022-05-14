int append_camera_metadata(camera_metadata_t *dst,

         const camera_metadata_t *src) {
     if (dst == NULL || src == NULL ) return ERROR;
 
    // Check for overflow
    if (src->entry_count + dst->entry_count < src->entry_count) return ERROR;
    if (src->data_count + dst->data_count < src->data_count) return ERROR;
    // Check for space
     if (dst->entry_capacity < src->entry_count + dst->entry_count) return ERROR;
     if (dst->data_capacity < src->data_count + dst->data_count) return ERROR;
 
    memcpy(get_entries(dst) + dst->entry_count, get_entries(src),
 sizeof(camera_metadata_buffer_entry_t[src->entry_count]));
    memcpy(get_data(dst) + dst->data_count, get_data(src),
 sizeof(uint8_t[src->data_count]));
 if (dst->data_count != 0) {
 camera_metadata_buffer_entry_t *entry = get_entries(dst) + dst->entry_count;
 for (size_t i = 0; i < src->entry_count; i++, entry++) {
 if ( calculate_camera_metadata_entry_data_size(entry->type,
                            entry->count) > 0 ) {
                entry->data.offset += dst->data_count;
 }
 }
 }
 if (dst->entry_count == 0) {
        dst->flags |= src->flags & FLAG_SORTED;
 } else if (src->entry_count != 0) {
        dst->flags &= ~FLAG_SORTED;
 } else {
 }
    dst->entry_count += src->entry_count;
    dst->data_count += src->data_count;

    assert(validate_camera_metadata_structure(dst, NULL) == OK);
 return OK;
}
