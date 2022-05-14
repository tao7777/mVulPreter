FileReaderLoader::~FileReaderLoader()
 {
     terminate();
     if (!m_urlForReading.isEmpty())
        ThreadableBlobRegistry::unregisterBlobURL(m_urlForReading);
 }
