BlockGroup::BlockGroup(
    Cluster* pCluster,
    long idx,
    long long block_start,
    long long block_size,
    long long prev,
    long long next,
    long long duration,
    long long discard_padding) :
    BlockEntry(pCluster, idx),
    m_block(block_start, block_size, discard_padding),
    m_prev(prev),
    m_next(next),
    m_duration(duration)
{
}
