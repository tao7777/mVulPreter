Cluster* Cluster::Create(
    Segment* pSegment,
    long idx,
    long long off)
{
    assert(pSegment);
    assert(off >= 0);
    const long long element_start = pSegment->m_start + off;
    Cluster* const pCluster = new Cluster(pSegment,
                                          idx,
                                          element_start);
    assert(pCluster);
    return pCluster;
}
