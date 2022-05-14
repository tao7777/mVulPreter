 static void handleMetaReferrer(const String& attributeValue, CachedDocumentParameters* documentParameters, CSSPreloadScanner* cssScanner)
 {
    if (attributeValue.isEmpty() || attributeValue.isNull() || !SecurityPolicy::referrerPolicyFromString(attributeValue, &documentParameters->referrerPolicy)) {
        documentParameters->referrerPolicy = ReferrerPolicyDefault;
     }
     cssScanner->setReferrerPolicy(documentParameters->referrerPolicy);
 }
