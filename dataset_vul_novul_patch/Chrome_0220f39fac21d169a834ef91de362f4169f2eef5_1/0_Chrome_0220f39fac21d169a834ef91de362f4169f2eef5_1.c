xsltStylesheetPtr XSLStyleSheet::compileStyleSheet()
{
     if (m_embedded)
         return xsltLoadStylesheetPI(document());
 
    // Certain libxslt versions are corrupting the xmlDoc on compilation failures -
    // hence attempting to recompile after a failure is unsafe.
    if (m_compilationFailed)
        return 0;

     ASSERT(!m_stylesheetDocTaken);
     xsltStylesheetPtr result = xsltParseStylesheetDoc(m_stylesheetDoc);
     if (result)
         m_stylesheetDocTaken = true;
    else
        m_compilationFailed = true;
     return result;
 }
