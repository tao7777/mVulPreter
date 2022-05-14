base::string16 GetRelyingPartyIdString(
    AuthenticatorRequestDialogModel* dialog_model) {
  static constexpr char kRpIdUrlPrefix[] = "https://";
  static constexpr int kDialogWidth = 300;
   const auto& rp_id = dialog_model->relying_party_id();
   DCHECK(!rp_id.empty());
   GURL rp_id_url(kRpIdUrlPrefix + rp_id);
  return url_formatter::ElideHost(rp_id_url, gfx::FontList(), kDialogWidth);
 }
