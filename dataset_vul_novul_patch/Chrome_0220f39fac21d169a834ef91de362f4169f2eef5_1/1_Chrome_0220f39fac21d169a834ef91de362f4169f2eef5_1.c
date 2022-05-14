xsltStylesheetPtr XSLStyleSheet::compileStyleSheet()
{
     if (m_embedded)
         return xsltLoadStylesheetPI(document());
 
     ASSERT(!m_stylesheetDocTaken);
     xsltStylesheetPtr result = xsltParseStylesheetDoc(m_stylesheetDoc);
     if (result)
         m_stylesheetDocTaken = true;
     return result;
 }
