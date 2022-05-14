views::View* CardUnmaskPromptViews::CreateFootnoteView() {
  if (!controller_->CanStoreLocally())
    return nullptr;

  storage_row_ = new FadeOutView();
  views::BoxLayout* storage_row_layout = new views::BoxLayout(
      views::BoxLayout::kHorizontal, kEdgePadding, kEdgePadding, 0);
  storage_row_->SetLayoutManager(storage_row_layout);
   storage_row_->SetBorder(
       views::Border::CreateSolidSidedBorder(1, 0, 0, 0, kSubtleBorderColor));
   storage_row_->set_background(
      views::Background::CreateSolidBackground(kLightShadingColor));
 
   storage_checkbox_ = new views::Checkbox(l10n_util::GetStringUTF16(
       IDS_AUTOFILL_CARD_UNMASK_PROMPT_STORAGE_CHECKBOX));
  storage_checkbox_->SetChecked(controller_->GetStoreLocallyStartState());
  storage_row_->AddChildView(storage_checkbox_);
  storage_row_layout->SetFlexForView(storage_checkbox_, 1);

  storage_row_->AddChildView(new TooltipIcon(l10n_util::GetStringUTF16(
      IDS_AUTOFILL_CARD_UNMASK_PROMPT_STORAGE_TOOLTIP)));

  return storage_row_;
 }
