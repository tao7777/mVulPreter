 inline HTMLIFrameElement::HTMLIFrameElement(Document& document)
     : HTMLFrameElementBase(iframeTag, document),
       collapsed_by_client_(false),
       sandbox_(HTMLIFrameElementSandbox::Create(this)),
       referrer_policy_(kReferrerPolicyDefault) {}
