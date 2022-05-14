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
 
    // Invisible children should not be counted.
    // However, in the collapsed container case (e.g. a combobox), items can
    // still be chosen/navigated. However, the options in these collapsed
    // containers are historically marked invisible. Therefore, in that case,
    // count the invisible items. Only check 2 levels up, as combobox containers
    // are never higher.
    if (child->data().HasState(ax::mojom::State::kInvisible) &&
        !IsCollapsed(local_parent) && !IsCollapsed(local_parent->parent())) {
      continue;
    }

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
