PassRefPtr<ChildType> HTMLConstructionSite::attach(Node* parent, PassRefPtr<ChildType> prpChild)
{
    RefPtr<ChildType> child = prpChild;

    if (shouldFosterParent()) {
        fosterParent(child.get());
        ASSERT(child->attached() || !child->parent()->attached());
        return child.release();
     }
 
     parent->parserAddChild(child);

    // An event handler (DOM Mutation, beforeload, et al.) could have removed
    // the child, in which case we shouldn't try attaching it.
    if (!child->parentNode())
        return child.release();

    if (parent->attached() && !child->attached())
        child->attach();
    return child.release();
}
