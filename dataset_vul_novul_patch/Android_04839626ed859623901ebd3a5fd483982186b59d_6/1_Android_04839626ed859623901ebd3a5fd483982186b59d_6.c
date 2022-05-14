void Chapters::Edition::Clear()
{
    while (m_atoms_count > 0)
    {
        Atom& a = m_atoms[--m_atoms_count];
        a.Clear();
     }
 
    delete[] m_atoms;
    m_atoms = NULL;
 
    m_atoms_size = 0;
 }
