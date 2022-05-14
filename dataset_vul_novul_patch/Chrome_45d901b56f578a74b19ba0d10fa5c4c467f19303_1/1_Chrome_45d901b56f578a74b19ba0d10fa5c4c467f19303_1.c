TabGroupHeader::TabGroupHeader(const base::string16& group_title) {
   constexpr gfx::Insets kPlaceholderInsets = gfx::Insets(4, 27);
   SetBorder(views::CreateEmptyBorder(kPlaceholderInsets));

  views::FlexLayout* layout =
      SetLayoutManager(std::make_unique<views::FlexLayout>());
  layout->SetOrientation(views::LayoutOrientation::kHorizontal)
      .SetCollapseMargins(true)
       .SetMainAxisAlignment(views::LayoutAlignment::kStart)
       .SetCrossAxisAlignment(views::LayoutAlignment::kCenter);
 
  auto title = std::make_unique<views::Label>(group_title);
   title->SetHorizontalAlignment(gfx::ALIGN_TO_HEAD);
   title->SetElideBehavior(gfx::FADE_TAIL);
  auto* title_ptr = AddChildView(std::move(title));
  layout->SetFlexForView(title_ptr,
                          views::FlexSpecification::ForSizeRule(
                              views::MinimumFlexSizeRule::kScaleToZero,
                              views::MaximumFlexSizeRule::kUnbounded));

  auto group_menu_button = views::CreateVectorImageButton(/*listener*/ nullptr);
  views::SetImageFromVectorIcon(group_menu_button.get(), kBrowserToolsIcon);
  AddChildView(std::move(group_menu_button));
}
