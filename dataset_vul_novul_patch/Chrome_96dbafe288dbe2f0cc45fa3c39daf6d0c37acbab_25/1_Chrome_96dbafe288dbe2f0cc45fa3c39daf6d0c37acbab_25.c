xsltFreeTemplateHashes(xsltStylesheetPtr style) {
    if (style->templatesHash != NULL)
	xmlHashFree((xmlHashTablePtr) style->templatesHash,
		    (xmlHashDeallocator) xsltFreeCompMatchList);
    if (style->rootMatch != NULL)
        xsltFreeCompMatchList(style->rootMatch);
    if (style->keyMatch != NULL)
        xsltFreeCompMatchList(style->keyMatch);
    if (style->elemMatch != NULL)
        xsltFreeCompMatchList(style->elemMatch);
    if (style->attrMatch != NULL)
        xsltFreeCompMatchList(style->attrMatch);
    if (style->parentMatch != NULL)
        xsltFreeCompMatchList(style->parentMatch);
    if (style->textMatch != NULL)
        xsltFreeCompMatchList(style->textMatch);
    if (style->piMatch != NULL)
         xsltFreeCompMatchList(style->piMatch);
     if (style->commentMatch != NULL)
         xsltFreeCompMatchList(style->commentMatch);
 }
