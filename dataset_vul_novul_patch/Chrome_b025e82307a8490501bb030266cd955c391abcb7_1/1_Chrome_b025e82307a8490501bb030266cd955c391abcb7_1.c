bool SectionHasAutofilledField(const FormStructure& form_structure,
                               const FormData& form,
                               const std::string& section) {
  DCHECK_EQ(form_structure.field_count(), form.fields.size());
  for (size_t i = 0; i < form_structure.field_count(); ++i) {
    if (form_structure.field(i)->section == section &&
        form.fields[i].is_autofilled) {
      return true;
    }
  }
  return false;
}
