SimpleBlock::SimpleBlock(
    Cluster* pCluster,
    long idx,
    long long start,
    long long size) :
    BlockEntry(pCluster, idx),
    m_block(start, size, 0)
{
}
