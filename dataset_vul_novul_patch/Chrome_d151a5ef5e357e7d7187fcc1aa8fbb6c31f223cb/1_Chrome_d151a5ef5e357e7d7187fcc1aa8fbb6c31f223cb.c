GtkWidget* PageInfoWindowGtk::CreateSection(
    const PageInfoModel::SectionInfo& section) {
  GtkWidget* vbox = gtk_vbox_new(FALSE, gtk_util::kControlSpacing);
  GtkWidget* label = gtk_label_new(UTF16ToUTF8(section.title).c_str());

  PangoAttrList* attributes = pango_attr_list_new();
  pango_attr_list_insert(attributes,
                         pango_attr_weight_new(PANGO_WEIGHT_BOLD));
  gtk_label_set_attributes(GTK_LABEL(label), attributes);
  pango_attr_list_unref(attributes);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

  GtkWidget* section_box = gtk_hbox_new(FALSE, 0);
  ResourceBundle& rb = ResourceBundle::GetSharedInstance();
  GtkWidget* image = gtk_image_new_from_pixbuf(section.state ?
      rb.GetPixbufNamed(IDR_PAGEINFO_GOOD) :
      rb.GetPixbufNamed(IDR_PAGEINFO_BAD));
  gtk_box_pack_start(GTK_BOX(section_box), image, FALSE, FALSE,
                     gtk_util::kControlSpacing);
  gtk_misc_set_alignment(GTK_MISC(image), 0, 0);

  GtkWidget* text_box = gtk_vbox_new(FALSE, gtk_util::kControlSpacing);
  if (!section.head_line.empty()) {
    label = gtk_label_new(UTF16ToUTF8(section.head_line).c_str());
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_box_pack_start(GTK_BOX(text_box), label, FALSE, FALSE, 0);
  }
   label = gtk_label_new(UTF16ToUTF8(section.description).c_str());
   gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
   gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
   gtk_box_pack_start(GTK_BOX(text_box), label, FALSE, FALSE, 0);
   gtk_widget_set_size_request(label, 400, -1);
 
  gtk_box_pack_start(GTK_BOX(section_box), text_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), section_box, TRUE, TRUE, 0);

  return vbox;
}
