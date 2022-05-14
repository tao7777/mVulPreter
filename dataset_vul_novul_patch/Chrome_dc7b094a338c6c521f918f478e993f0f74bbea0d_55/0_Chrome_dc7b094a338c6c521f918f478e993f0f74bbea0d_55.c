bool ConvertProperty(IBusProperty* ibus_prop,
                     int selection_item_id,
                     ImePropertyList* out_prop_list) {
  DCHECK(ibus_prop);
  DCHECK(ibus_prop->key);
  DCHECK(out_prop_list);

  const bool has_sub_props = PropertyHasChildren(ibus_prop);
  if (has_sub_props && (ibus_prop->type != PROP_TYPE_MENU)) {
    LOG(ERROR) << "The property has sub properties, "
               << "but the type of the property is not PROP_TYPE_MENU";
    return false;
   }
   if ((!has_sub_props) && (ibus_prop->type == PROP_TYPE_MENU)) {
    VLOG(1) << "Property list is empty";
     return false;
   }
   if (ibus_prop->type == PROP_TYPE_SEPARATOR ||
      ibus_prop->type == PROP_TYPE_MENU) {
    return true;
  }

  const bool is_selection_item = (ibus_prop->type == PROP_TYPE_RADIO);
  selection_item_id = is_selection_item ?
      selection_item_id : ImeProperty::kInvalidSelectionItemId;

  bool is_selection_item_checked = false;
  if (ibus_prop->state == PROP_STATE_INCONSISTENT) {
    LOG(WARNING) << "The property is in PROP_STATE_INCONSISTENT, "
                 << "which is not supported.";
  } else if ((!is_selection_item) && (ibus_prop->state == PROP_STATE_CHECKED)) {
    LOG(WARNING) << "PROP_STATE_CHECKED is meaningful only if the type is "
                 << "PROP_TYPE_RADIO.";
  } else {
    is_selection_item_checked = (ibus_prop->state == PROP_STATE_CHECKED);
  }

  if (!ibus_prop->key) {
    LOG(ERROR) << "key is NULL";
  }
  if (ibus_prop->tooltip && (!ibus_prop->tooltip->text)) {
    LOG(ERROR) << "tooltip is NOT NULL, but tooltip->text IS NULL: key="
               << Or(ibus_prop->key, "");
  }
  if (ibus_prop->label && (!ibus_prop->label->text)) {
    LOG(ERROR) << "label is NOT NULL, but label->text IS NULL: key="
               << Or(ibus_prop->key, "");
  }

  std::string label =
      ((ibus_prop->tooltip &&
        ibus_prop->tooltip->text) ? ibus_prop->tooltip->text : "");
  if (label.empty()) {
    label = (ibus_prop->label && ibus_prop->label->text)
        ? ibus_prop->label->text : "";
  }
  if (label.empty()) {
    label = Or(ibus_prop->key, "");
  }

  out_prop_list->push_back(ImeProperty(ibus_prop->key,
                                       label,
                                       is_selection_item,
                                       is_selection_item_checked,
                                       selection_item_id));
  return true;
}
