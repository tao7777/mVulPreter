bool RenderMenuList::multiple()
bool RenderMenuList::multiple() const
 {
     return toHTMLSelectElement(node())->multiple();
 }
