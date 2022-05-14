 void ImageTokenizer::stopParsing()
 {
     Tokenizer::stopParsing();
    if (m_imageElement)
        m_imageElement->cachedImage()->error();
 }
