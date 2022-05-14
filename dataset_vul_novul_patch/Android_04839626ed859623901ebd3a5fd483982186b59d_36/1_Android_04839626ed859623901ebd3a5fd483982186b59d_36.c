bool Chapters::Edition::ExpandAtomsArray()
{
    if (m_atoms_size > m_atoms_count)
        return true;  // nothing else to do
 
    const int size = (m_atoms_size == 0) ? 1 : 2 * m_atoms_size;
 
    Atom* const atoms = new (std::nothrow) Atom[size];
 
    if (atoms == NULL)
        return false;
 
    for (int idx = 0; idx < m_atoms_count; ++idx)
    {
        m_atoms[idx].ShallowCopy(atoms[idx]);
     }
 
    delete[] m_atoms;
    m_atoms = atoms;
 
    m_atoms_size = size;
    return true;
 }
