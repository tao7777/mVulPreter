long Chapters::Edition::ParseAtom(
    IMkvReader* pReader,
    long long pos,
    long long size)
{
    if (!ExpandAtomsArray())
        return -1;
 
    Atom& a = m_atoms[m_atoms_count++];
    a.Init();
 
    return a.Parse(pReader, pos, size);
 }
