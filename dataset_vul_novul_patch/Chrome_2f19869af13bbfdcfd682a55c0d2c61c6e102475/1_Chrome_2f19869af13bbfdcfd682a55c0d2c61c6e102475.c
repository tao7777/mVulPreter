base::string16 GetRelyingPartyIdString(
    AuthenticatorRequestDialogModel* dialog_model) {
  static constexpr char kRpIdUrlPrefix[] = "https://";
  static constexpr int kDialogWidth = 300;
   const auto& rp_id = dialog_model->relying_party_id();
   DCHECK(!rp_id.empty());
   GURL rp_id_url(kRpIdUrlPrefix + rp_id);
  auto max_static_string_length = gfx::GetStringWidthF(
      l10n_util::GetStringUTF16(IDS_WEBAUTHN_GENERIC_TITLE), gfx::FontList(),
      gfx::Typesetter::DEFAULT);
  return url_formatter::ElideHost(rp_id_url, gfx::FontList(),
                                  kDialogWidth - max_static_string_length);
 }
