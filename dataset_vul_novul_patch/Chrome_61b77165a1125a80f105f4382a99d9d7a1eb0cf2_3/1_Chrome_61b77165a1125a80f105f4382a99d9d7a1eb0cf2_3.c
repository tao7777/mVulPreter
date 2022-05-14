void HTMLLinkElement::processCallback(Node* node)
{
    ASSERT_ARG(node, node && node->hasTagName(linkTag));
    static_cast<HTMLLinkElement*>(node)->process();
}
