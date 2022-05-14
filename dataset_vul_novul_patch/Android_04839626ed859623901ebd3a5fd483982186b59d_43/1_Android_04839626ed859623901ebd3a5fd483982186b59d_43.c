const Chapters::Atom* Chapters::Edition::GetAtom(int index) const
{
    if (index < 0)
        return NULL;
 
    if (index >= m_atoms_count)
        return NULL;
 
    return m_atoms + index;
 }
