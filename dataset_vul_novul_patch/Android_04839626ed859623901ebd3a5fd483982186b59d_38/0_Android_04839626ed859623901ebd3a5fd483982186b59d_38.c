bool Chapters::ExpandEditionsArray()
  Edition& e = m_editions[m_editions_count++];
  e.Init();
 
  return e.Parse(m_pSegment->m_pReader, pos, size);
}
 
Chapters::Edition::Edition() {}
 
Chapters::Edition::~Edition() {}
 
int Chapters::Edition::GetAtomCount() const { return m_atoms_count; }

const Chapters::Atom* Chapters::Edition::GetAtom(int index) const {
  if (index < 0)
    return NULL;

  if (index >= m_atoms_count)
    return NULL;

  return m_atoms + index;
}

void Chapters::Edition::Init() {
  m_atoms = NULL;
  m_atoms_size = 0;
  m_atoms_count = 0;
}

void Chapters::Edition::ShallowCopy(Edition& rhs) const {
  rhs.m_atoms = m_atoms;
  rhs.m_atoms_size = m_atoms_size;
  rhs.m_atoms_count = m_atoms_count;
}

void Chapters::Edition::Clear() {
  while (m_atoms_count > 0) {
    Atom& a = m_atoms[--m_atoms_count];
    a.Clear();
  }

  delete[] m_atoms;
  m_atoms = NULL;

  m_atoms_size = 0;
}

long Chapters::Edition::Parse(IMkvReader* pReader, long long pos,
                              long long size) {
  const long long stop = pos + size;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)  // weird
      continue;

    if (id == 0x36) {  // Atom ID
      status = ParseAtom(pReader, pos, size);

      if (status < 0)  // error
        return status;
     }
 
    pos += size;
    assert(pos <= stop);
  }
 
  assert(pos == stop);
  return 0;
 }
