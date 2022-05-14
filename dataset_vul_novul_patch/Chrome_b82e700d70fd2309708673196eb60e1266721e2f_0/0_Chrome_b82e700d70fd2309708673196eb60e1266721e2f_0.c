void HTMLPreloadScanner::processToken()
{
    if (m_inStyle) {
        if (m_token.type() == HTMLTokenTypes::Character)
            m_cssScanner.scan(m_token);
        else if (m_token.type() == HTMLTokenTypes::EndTag) {
            m_inStyle = false;
            m_cssScanner.reset();
         }
     }
 
    if (m_token.type() != HTMLTokenTypes::StartTag) {
#if ENABLE(TEMPLATE_ELEMENT)
        if (m_templateCount && m_token.type() == HTMLTokenTypes::EndTag && AtomicString(m_token.name().data()) == templateTag)
            m_templateCount--;
#endif
         return;
    }
 
     PreloadTask task(m_token);
     m_tokenizer->updateStateFor(task.tagName());
 
#if ENABLE(TEMPLATE_ELEMENT)
    if (task.tagName() == templateTag)
        m_templateCount++;
#endif

     if (task.tagName() == styleTag)
         m_inStyle = true;
 
     if (task.tagName() == baseTag)
         updatePredictedBaseElementURL(KURL(m_document->url(), task.baseElementHref()));
 
    bool preload = true;

#if ENABLE(TEMPLATE_ELEMENT)
    if (m_templateCount)
        preload = false;
#endif

    if (preload)
        task.preload(m_document, m_predictedBaseElementURL.isEmpty() ? m_document->baseURL() : m_predictedBaseElementURL);
 }
