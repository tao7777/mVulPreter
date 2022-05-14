HTMLFrameOwnerElement::HTMLFrameOwnerElement(const QualifiedName& tag_name,
                                             Document& document)
     : HTMLElement(tag_name, document),
       content_frame_(nullptr),
       embedded_content_view_(nullptr),
