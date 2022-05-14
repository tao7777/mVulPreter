long BlockGroup::Parse()
{
    const long status = m_block.Parse(m_pCluster);
    if (status)
        return status;
    m_block.SetKey((m_prev > 0) && (m_next <= 0));
    return 0;
}
