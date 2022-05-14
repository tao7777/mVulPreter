void HTMLLinkElement::attach()
{
    if (m_shouldProcessAfterAttach) {
        m_shouldProcessAfterAttach = false;
        queuePostAttachCallback(&HTMLLinkElement::processCallback, this);
    }
    HTMLElement::attach();
}
