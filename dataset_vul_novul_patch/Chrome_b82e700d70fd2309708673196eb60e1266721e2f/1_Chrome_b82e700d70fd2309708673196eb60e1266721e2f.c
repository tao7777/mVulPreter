HTMLPreloadScanner::HTMLPreloadScanner(Document* document, const HTMLParserOptions& options)
    : m_document(document)
     , m_cssScanner(document)
     , m_tokenizer(HTMLTokenizer::create(options))
     , m_inStyle(false)
 {
 }
