 inline HTMLIFrameElement::HTMLIFrameElement(Document& document)
     : HTMLFrameElementBase(iframeTag, document),
      did_load_non_empty_document_(false),
       collapsed_by_client_(false),
       sandbox_(HTMLIFrameElementSandbox::Create(this)),
       referrer_policy_(kReferrerPolicyDefault) {}
