PassRefPtr<DocumentFragment> XSLTProcessor::transformToFragment(Node* sourceNode, Document* outputDoc)
{
    String resultMIMEType;
    String resultString;
    String resultEncoding;

    if (outputDoc->isHTMLDocument())
        resultMIMEType = "text/html";
 
     if (!transformToString(sourceNode, resultMIMEType, resultString, resultEncoding))
         return 0;
    return createFragmentForTransformToFragment(resultString, resultMIMEType, outputDoc);
 }
