views::View* AutofillDialogViews::CreateFootnoteView() {
  footnote_view_ = new LayoutPropagationView();
  footnote_view_->SetLayoutManager(
      new views::BoxLayout(views::BoxLayout::kVertical,
                           kDialogEdgePadding,
                           kDialogEdgePadding,
                           0));
   footnote_view_->SetBorder(
       views::Border::CreateSolidSidedBorder(1, 0, 0, 0, kSubtleBorderColor));
   footnote_view_->set_background(
      views::Background::CreateSolidBackground(kLightShadingColor));
 
   legal_document_view_ = new views::StyledLabel(base::string16(), this);
 
  footnote_view_->AddChildView(legal_document_view_);
  footnote_view_->SetVisible(false);

  return footnote_view_;
}
