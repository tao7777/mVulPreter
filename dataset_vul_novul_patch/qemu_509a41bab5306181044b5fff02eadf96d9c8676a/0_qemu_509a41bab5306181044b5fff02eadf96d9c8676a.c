static int cloop_open(BlockDriverState *bs, QDict *options, int flags,
                      Error **errp)
{
    BDRVCloopState *s = bs->opaque;
    uint32_t offsets_size, max_compressed_block_size = 1, i;
    int ret;

    bs->read_only = 1;

    /* read header */
    ret = bdrv_pread(bs->file, 128, &s->block_size, 4);
    if (ret < 0) {
        return ret;
    }
    s->block_size = be32_to_cpu(s->block_size);
    if (s->block_size % 512) {
        error_setg(errp, "block_size %u must be a multiple of 512",
                   s->block_size);
        return -EINVAL;
    }
    if (s->block_size == 0) {
        error_setg(errp, "block_size cannot be zero");
        return -EINVAL;
    }

    /* cloop's create_compressed_fs.c warns about block sizes beyond 256 KB but
     * we can accept more.  Prevent ridiculous values like 4 GB - 1 since we
     * need a buffer this big.
     */
    if (s->block_size > MAX_BLOCK_SIZE) {
        error_setg(errp, "block_size %u must be %u MB or less",
                   s->block_size,
                   MAX_BLOCK_SIZE / (1024 * 1024));
        return -EINVAL;
    }

    ret = bdrv_pread(bs->file, 128 + 4, &s->n_blocks, 4);
    if (ret < 0) {
        return ret;
    }
     s->n_blocks = be32_to_cpu(s->n_blocks);
 
     /* read offsets */
    if (s->n_blocks > UINT32_MAX / sizeof(uint64_t)) {
        /* Prevent integer overflow */
        error_setg(errp, "n_blocks %u must be %zu or less",
                   s->n_blocks,
                   UINT32_MAX / sizeof(uint64_t));
        return -EINVAL;
    }
     offsets_size = s->n_blocks * sizeof(uint64_t);
     s->offsets = g_malloc(offsets_size);
        if (i > 0) {
            uint32_t size = s->offsets[i] - s->offsets[i - 1];
            if (size > max_compressed_block_size) {
                max_compressed_block_size = size;
            }
        }
    }
