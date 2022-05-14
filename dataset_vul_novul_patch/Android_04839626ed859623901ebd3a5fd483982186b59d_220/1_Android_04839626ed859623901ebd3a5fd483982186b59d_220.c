Cluster::~Cluster()
{
    if (m_entries_count <= 0)
        return;
    BlockEntry** i = m_entries;
    BlockEntry** const j = m_entries + m_entries_count;
    while (i != j)
    {
         BlockEntry* p = *i++;
         assert(p);
         delete p;
     }
 
    delete[] m_entries;
 }
