XSLStyleSheet::XSLStyleSheet(XSLImportRule* parentRule, const String& originalURL, const KURL& finalURL)
    : m_ownerNode(0)
    , m_originalURL(originalURL)
    , m_finalURL(finalURL)
    , m_isDisabled(false)
    , m_embedded(false)
     , m_processed(false) // Child sheets get marked as processed when the libxslt engine has finally seen them.
     , m_stylesheetDoc(0)
     , m_stylesheetDocTaken(false)
    , m_compilationFailed(false)
     , m_parentStyleSheet(parentRule ? parentRule->parentStyleSheet() : 0)
 {
 }
