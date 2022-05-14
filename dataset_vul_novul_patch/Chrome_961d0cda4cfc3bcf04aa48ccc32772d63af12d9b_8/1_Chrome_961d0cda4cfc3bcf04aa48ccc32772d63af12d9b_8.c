  GenerationElementData(autofill::PasswordForm form,
                        autofill::FormSignature form_signature,
                        autofill::FieldSignature field_signature,
                        uint32_t max_password_length)
      : form(std::move(form)),
        form_signature(form_signature),
        field_signature(field_signature),
        max_password_length(max_password_length) {}
