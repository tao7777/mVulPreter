void Chapters::Edition::ShallowCopy(Edition& rhs) const
{
    rhs.m_atoms = m_atoms;
    rhs.m_atoms_size = m_atoms_size;
    rhs.m_atoms_count = m_atoms_count;
 }
