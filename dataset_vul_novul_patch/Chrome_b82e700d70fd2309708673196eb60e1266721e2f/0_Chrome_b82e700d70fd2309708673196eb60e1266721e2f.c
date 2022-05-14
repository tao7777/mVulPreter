HTMLPreloadScanner::HTMLPreloadScanner(Document* document, const HTMLParserOptions& options)
    : m_document(document)
     , m_cssScanner(document)
     , m_tokenizer(HTMLTokenizer::create(options))
     , m_inStyle(false)
#if ENABLE(TEMPLATE_ELEMENT)
    , m_templateCount(0)
#endif
 {
 }
