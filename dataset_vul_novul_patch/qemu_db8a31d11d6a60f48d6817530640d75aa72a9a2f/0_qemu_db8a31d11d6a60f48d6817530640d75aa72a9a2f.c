 static int get_refcount(BlockDriverState *bs, int64_t cluster_index)
 {
     BDRVQcowState *s = bs->opaque;
    uint64_t refcount_table_index, block_index;
     int64_t refcount_block_offset;
     int ret;
     uint16_t *refcount_block;
    uint16_t refcount;

    refcount_table_index = cluster_index >> (s->cluster_bits - REFCOUNT_SHIFT);
    if (refcount_table_index >= s->refcount_table_size)
        return 0;
    refcount_block_offset =
        s->refcount_table[refcount_table_index] & REFT_OFFSET_MASK;
    if (!refcount_block_offset)
        return 0;

    ret = qcow2_cache_get(bs, s->refcount_block_cache, refcount_block_offset,
        (void**) &refcount_block);
    if (ret < 0) {
        return ret;
    }

    block_index = cluster_index &
        ((1 << (s->cluster_bits - REFCOUNT_SHIFT)) - 1);
    refcount = be16_to_cpu(refcount_block[block_index]);

    ret = qcow2_cache_put(bs, s->refcount_block_cache,
        (void**) &refcount_block);
    if (ret < 0) {
        return ret;
    }

    return refcount;
}
