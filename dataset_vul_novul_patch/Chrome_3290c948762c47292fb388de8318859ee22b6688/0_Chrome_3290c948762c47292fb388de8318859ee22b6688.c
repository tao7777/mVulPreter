void MenuGtk::BuildMenuIn(GtkWidget* menu,
                          const MenuCreateMaterial* menu_data,
                          GtkAccelGroup* accel_group) {
  GtkWidget* last_menu_item = NULL;
  for (; menu_data->type != MENU_END; ++menu_data) {
    GtkWidget* menu_item = NULL;

    std::string label;
    if (menu_data->label_argument) {
      label = l10n_util::GetStringFUTF8(
          menu_data->label_id,
          l10n_util::GetStringUTF16(menu_data->label_argument));
    } else if (menu_data->label_id) {
      label = l10n_util::GetStringUTF8(menu_data->label_id);
    } else if (menu_data->type != MENU_SEPARATOR) {
      label = delegate_->GetLabel(menu_data->id);
      DCHECK(!label.empty());
    }

    label = ConvertAcceleratorsFromWindowsStyle(label);

    switch (menu_data->type) {
      case MENU_RADIO:
        if (GTK_IS_RADIO_MENU_ITEM(last_menu_item)) {
          menu_item = gtk_radio_menu_item_new_with_mnemonic_from_widget(
              GTK_RADIO_MENU_ITEM(last_menu_item), label.c_str());
        } else {
          menu_item = gtk_radio_menu_item_new_with_mnemonic(
              NULL, label.c_str());
        }
        break;
      case MENU_CHECKBOX:
        menu_item = gtk_check_menu_item_new_with_mnemonic(label.c_str());
        break;
      case MENU_SEPARATOR:
        menu_item = gtk_separator_menu_item_new();
        break;
      case MENU_NORMAL:
      default:
        menu_item = gtk_menu_item_new_with_mnemonic(label.c_str());
        break;
    }

    if (menu_data->submenu) {
      GtkWidget* submenu = gtk_menu_new();
      BuildMenuIn(submenu, menu_data->submenu, accel_group);
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), submenu);
    } else if (menu_data->custom_submenu) {
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item),
                                menu_data->custom_submenu->menu_.get());
       submenus_we_own_.push_back(menu_data->custom_submenu);
     }
 
    if (menu_data->accel_key) {
       gtk_widget_add_accelerator(menu_item,
                                  "activate",
                                 menu_data->only_show || !accel_group ?
                                     dummy_accel_group_ : accel_group,
                                  menu_data->accel_key,
                                  GdkModifierType(menu_data->accel_modifiers),
                                  GTK_ACCEL_VISIBLE);
    }

    g_object_set_data(G_OBJECT(menu_item), "menu-data",
                      const_cast<MenuCreateMaterial*>(menu_data));

    g_signal_connect(G_OBJECT(menu_item), "activate",
                     G_CALLBACK(OnMenuItemActivated), this);

    gtk_widget_show(menu_item);
    gtk_menu_append(menu, menu_item);
    last_menu_item = menu_item;
  }
}
