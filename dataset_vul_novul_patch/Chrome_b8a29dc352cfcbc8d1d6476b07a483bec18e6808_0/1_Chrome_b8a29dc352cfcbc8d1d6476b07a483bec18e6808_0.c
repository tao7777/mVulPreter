 void ImageTokenizer::stopParsing()
 {
     Tokenizer::stopParsing();
    m_imageElement->cachedImage()->error();
 }
