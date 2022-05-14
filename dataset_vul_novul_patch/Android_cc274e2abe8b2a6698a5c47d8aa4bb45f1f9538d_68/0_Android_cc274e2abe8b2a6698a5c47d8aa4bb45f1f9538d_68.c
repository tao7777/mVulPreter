Segment::~Segment() {
 const long count = m_clusterCount + m_clusterPreloadCount;

 Cluster** i = m_clusters;
 Cluster** j = m_clusters + count;

 
   while (i != j) {
     Cluster* const p = *i++;
     delete p;
   }
 
 delete[] m_clusters;

 delete m_pTracks;

   delete m_pInfo;
   delete m_pCues;
   delete m_pChapters;
  delete m_pTags;
   delete m_pSeekHead;
 }
