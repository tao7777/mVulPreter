int rpc_pack(rpc_pkg *pkg, void **data, uint32_t *size)
 {
     static void *send_buf;
     static size_t send_buf_size;
    uint32_t pkg_size = RPC_PKG_HEAD_SIZE + pkg->ext_size + pkg->body_size;
     if (send_buf_size < pkg_size) {
         if (send_buf)
             free(send_buf);
         send_buf_size = pkg_size * 2;
         send_buf = malloc(send_buf_size);
        assert(send_buf != NULL);
     }
 
     memcpy(send_buf, pkg, RPC_PKG_HEAD_SIZE);
    if (pkg->ext_size)
        memcpy(send_buf + RPC_PKG_HEAD_SIZE, pkg->ext, pkg->ext_size);
    if (pkg->body_size)
        memcpy(send_buf + RPC_PKG_HEAD_SIZE + pkg->ext_size, pkg->body, pkg->body_size);

    pkg = send_buf;
    pkg->magic     = htole32(RPC_PKG_MAGIC);
    pkg->command   = htole32(pkg->command);
    pkg->pkg_type  = htole16(pkg->pkg_type);
    pkg->result    = htole32(pkg->result);
    pkg->sequence  = htole32(pkg->sequence);
    pkg->req_id    = htole64(pkg->req_id);
    pkg->body_size = htole32(pkg->body_size);
    pkg->ext_size  = htole16(pkg->ext_size);

    pkg->crc32 = 0;
    pkg->crc32 = htole32(generate_crc32c(send_buf, pkg_size));

    *data = send_buf;
    *size = pkg_size;

    return 0;
}
