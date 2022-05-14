FileReaderLoader::~FileReaderLoader()
 {
     terminate();
     if (!m_urlForReading.isEmpty())
        BlobRegistry::unregisterBlobURL(m_urlForReading);
 }
