long Chapters::ParseEdition(
  Atom& a = m_atoms[m_atoms_count++];
  a.Init();
 
  return a.Parse(pReader, pos, size);
 }
