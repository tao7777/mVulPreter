void AXTree::PopulateOrderedSetItems(const AXNode* ordered_set,
                                     const AXNode* local_parent,
                                     std::vector<const AXNode*>& items,
                                     bool node_is_radio_button) const {
  if (!(ordered_set == local_parent)) {
    if (local_parent->data().role == ordered_set->data().role)
      return;
  }

   for (int i = 0; i < local_parent->child_count(); ++i) {
     const AXNode* child = local_parent->GetUnignoredChildAtIndex(i);
 
     if (node_is_radio_button &&
         child->data().role == ax::mojom::Role::kRadioButton)
      items.push_back(child);

    if (!node_is_radio_button && child->SetRoleMatchesItemRole(ordered_set))
      items.push_back(child);

    if (child->data().role == ax::mojom::Role::kGenericContainer ||
        child->data().role == ax::mojom::Role::kIgnored) {
      PopulateOrderedSetItems(ordered_set, child, items, node_is_radio_button);
    }
  }
}
