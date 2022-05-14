BlockEntry::BlockEntry(Cluster* p, long idx) :
BlockEntry::~BlockEntry() {}

bool BlockEntry::EOS() const { return (GetKind() == kBlockEOS); }

const Cluster* BlockEntry::GetCluster() const { return m_pCluster; }

long BlockEntry::GetIndex() const { return m_index; }

SimpleBlock::SimpleBlock(Cluster* pCluster, long idx, long long start,
                         long long size)
    : BlockEntry(pCluster, idx), m_block(start, size, 0) {}

long SimpleBlock::Parse() { return m_block.Parse(m_pCluster); }

BlockEntry::Kind SimpleBlock::GetKind() const { return kBlockSimple; }

const Block* SimpleBlock::GetBlock() const { return &m_block; }

BlockGroup::BlockGroup(Cluster* pCluster, long idx, long long block_start,
                       long long block_size, long long prev, long long next,
                       long long duration, long long discard_padding)
    : BlockEntry(pCluster, idx),
      m_block(block_start, block_size, discard_padding),
      m_prev(prev),
      m_next(next),
      m_duration(duration) {}

long BlockGroup::Parse() {
  const long status = m_block.Parse(m_pCluster);

  if (status)
    return status;

  m_block.SetKey((m_prev > 0) && (m_next <= 0));

  return 0;
 }
