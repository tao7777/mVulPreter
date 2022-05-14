bool parseXMLDocumentFragment(const String& chunk, DocumentFragment* fragment, Element* parent)
{
    if (!chunk.length())
        return true;
 
     XMLTokenizer tokenizer(fragment, parent);
     
    CString chunkAsUtf8 = chunk.utf8();
    tokenizer.initializeParserContext(chunkAsUtf8.data());
 
     xmlParseContent(tokenizer.m_context);
 
     tokenizer.endDocument();
 
     long bytesProcessed = xmlByteConsumed(tokenizer.m_context);
    if (bytesProcessed == -1 || ((unsigned long)bytesProcessed) != chunkAsUtf8.length())
         return false;
 
    return tokenizer.m_context->wellFormed || xmlCtxtGetLastError(tokenizer.m_context) == 0;
}
