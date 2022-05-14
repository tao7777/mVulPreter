size_t calculate_camera_metadata_entry_data_size(uint8_t type,
        size_t data_count) {
    if (type >= NUM_TYPES) return 0;
    size_t data_bytes = data_count *
            camera_metadata_type_size[type];
    return data_bytes <= 4 ? 0 : ALIGN_TO(data_bytes, DATA_ALIGNMENT);
}
