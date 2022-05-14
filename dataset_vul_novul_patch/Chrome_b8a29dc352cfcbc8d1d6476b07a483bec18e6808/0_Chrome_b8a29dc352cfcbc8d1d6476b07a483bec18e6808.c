 void ImageTokenizer::finish()
 {
    if (!m_parserStopped && m_imageElement) {
         CachedImage* cachedImage = m_imageElement->cachedImage();
         Vector<char>& buffer = cachedImage->bufferData(0, 0, 0);
         cachedImage->data(buffer, true);

#if PLATFORM(MAC)
        finishImageLoad(m_doc, cachedImage, buffer.data(), buffer.size());
#endif
    }

    m_doc->finishedParsing();
}
