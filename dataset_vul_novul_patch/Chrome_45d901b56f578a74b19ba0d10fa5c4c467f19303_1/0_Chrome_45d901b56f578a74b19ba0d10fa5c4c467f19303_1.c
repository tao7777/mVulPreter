TabGroupHeader::TabGroupHeader(const base::string16& group_title) {
TabGroupHeader::TabGroupHeader(TabController* controller, int group)
    : controller_(controller), group_(group) {
  DCHECK(controller);

   constexpr gfx::Insets kPlaceholderInsets = gfx::Insets(4, 27);
   SetBorder(views::CreateEmptyBorder(kPlaceholderInsets));

  views::FlexLayout* layout =
      SetLayoutManager(std::make_unique<views::FlexLayout>());
  layout->SetOrientation(views::LayoutOrientation::kHorizontal)
      .SetCollapseMargins(true)
       .SetMainAxisAlignment(views::LayoutAlignment::kStart)
       .SetCrossAxisAlignment(views::LayoutAlignment::kCenter);
 
  auto title = std::make_unique<views::Label>(GetGroupData()->title());
   title->SetHorizontalAlignment(gfx::ALIGN_TO_HEAD);
   title->SetElideBehavior(gfx::FADE_TAIL);
  title_label_ = AddChildView(std::move(title));
  layout->SetFlexForView(title_label_,
                          views::FlexSpecification::ForSizeRule(
                              views::MinimumFlexSizeRule::kScaleToZero,
                              views::MaximumFlexSizeRule::kUnbounded));

  auto group_menu_button = views::CreateVectorImageButton(/*listener*/ nullptr);
  views::SetImageFromVectorIcon(group_menu_button.get(), kBrowserToolsIcon);
  AddChildView(std::move(group_menu_button));
}
