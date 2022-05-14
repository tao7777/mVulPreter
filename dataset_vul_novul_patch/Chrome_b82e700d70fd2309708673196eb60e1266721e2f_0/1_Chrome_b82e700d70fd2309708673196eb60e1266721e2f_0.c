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
 
    if (m_token.type() != HTMLTokenTypes::StartTag)
         return;
 
     PreloadTask task(m_token);
     m_tokenizer->updateStateFor(task.tagName());
 
     if (task.tagName() == styleTag)
         m_inStyle = true;
 
     if (task.tagName() == baseTag)
         updatePredictedBaseElementURL(KURL(m_document->url(), task.baseElementHref()));
 
    task.preload(m_document, m_predictedBaseElementURL.isEmpty() ? m_document->baseURL() : m_predictedBaseElementURL);
 }
