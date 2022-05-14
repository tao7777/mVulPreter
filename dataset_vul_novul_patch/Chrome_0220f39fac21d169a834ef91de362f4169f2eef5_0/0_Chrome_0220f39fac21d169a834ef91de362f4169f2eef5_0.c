XSLStyleSheet::XSLStyleSheet(Node* parentNode, const String& originalURL, const KURL& finalURL,  bool embedded)
    : m_ownerNode(parentNode)
    , m_originalURL(originalURL)
    , m_finalURL(finalURL)
    , m_isDisabled(false)
    , m_embedded(embedded)
     , m_processed(true) // The root sheet starts off processed.
     , m_stylesheetDoc(0)
     , m_stylesheetDocTaken(false)
    , m_compilationFailed(false)
     , m_parentStyleSheet(0)
 {
 }
