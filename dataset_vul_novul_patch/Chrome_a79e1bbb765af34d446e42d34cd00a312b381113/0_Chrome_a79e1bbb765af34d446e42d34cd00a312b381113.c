void OnSuggestionModelAdded(UiScene* scene,
                            UiBrowserInterface* browser,
                            Model* model,
                            SuggestionBinding* element_binding) {
  auto icon = base::MakeUnique<VectorIcon>(100);
  icon->SetDrawPhase(kPhaseForeground);
  icon->SetType(kTypeOmniboxSuggestionIcon);
  icon->set_hit_testable(false);
  icon->SetSize(kSuggestionIconSizeDMM, kSuggestionIconSizeDMM);
  BindColor(model, icon.get(), &ColorScheme::omnibox_icon,
            &VectorIcon::SetColor);
  VectorIcon* p_icon = icon.get();

  auto icon_box = base::MakeUnique<UiElement>();
  icon_box->SetDrawPhase(kPhaseNone);
  icon_box->SetType(kTypeOmniboxSuggestionIconField);
  icon_box->SetSize(kSuggestionIconFieldWidthDMM, kSuggestionHeightDMM);
  icon_box->AddChild(std::move(icon));

  auto content_text = base::MakeUnique<Text>(kSuggestionContentTextHeightDMM);
  content_text->SetDrawPhase(kPhaseForeground);
  content_text->SetType(kTypeOmniboxSuggestionContentText);
  content_text->set_hit_testable(false);
  content_text->SetTextLayoutMode(TextLayoutMode::kSingleLineFixedWidth);
  content_text->SetSize(kSuggestionTextFieldWidthDMM, 0);
  content_text->SetTextAlignment(UiTexture::kTextAlignmentLeft);
  BindColor(model, content_text.get(), &ColorScheme::omnibox_suggestion_content,
            &Text::SetColor);
  Text* p_content_text = content_text.get();

  auto description_text =
      base::MakeUnique<Text>(kSuggestionDescriptionTextHeightDMM);
   description_text->SetDrawPhase(kPhaseForeground);
   description_text->SetType(kTypeOmniboxSuggestionDescriptionText);
   description_text->set_hit_testable(false);
  description_text->SetTextLayoutMode(TextLayoutMode::kSingleLineFixedWidth);
   description_text->SetSize(kSuggestionTextFieldWidthDMM, 0);
   description_text->SetTextAlignment(UiTexture::kTextAlignmentLeft);
   BindColor(model, description_text.get(),
            &ColorScheme::omnibox_suggestion_description, &Text::SetColor);
  Text* p_description_text = description_text.get();

  auto text_layout = base::MakeUnique<LinearLayout>(LinearLayout::kDown);
  text_layout->SetType(kTypeOmniboxSuggestionTextLayout);
  text_layout->set_hit_testable(false);
  text_layout->set_margin(kSuggestionLineGapDMM);
  text_layout->AddChild(std::move(content_text));
  text_layout->AddChild(std::move(description_text));

  auto right_margin = base::MakeUnique<UiElement>();
  right_margin->SetDrawPhase(kPhaseNone);
  right_margin->SetSize(kSuggestionRightMarginDMM, kSuggestionHeightDMM);

  auto suggestion_layout = base::MakeUnique<LinearLayout>(LinearLayout::kRight);
  suggestion_layout->SetType(kTypeOmniboxSuggestionLayout);
  suggestion_layout->set_hit_testable(false);
  suggestion_layout->AddChild(std::move(icon_box));
  suggestion_layout->AddChild(std::move(text_layout));
  suggestion_layout->AddChild(std::move(right_margin));

  auto background = Create<Button>(
      kNone, kPhaseForeground,
      base::BindRepeating(
          [](UiBrowserInterface* b, Model* m, SuggestionBinding* e) {
            b->Navigate(e->model()->destination);
            m->omnibox_input_active = false;
          },
          base::Unretained(browser), base::Unretained(model),
          base::Unretained(element_binding)));

  background->SetType(kTypeOmniboxSuggestionBackground);
  background->set_hit_testable(true);
  background->set_bubble_events(true);
  background->set_bounds_contain_children(true);
  background->set_hover_offset(0.0);
  BindButtonColors(model, background.get(),
                   &ColorScheme::suggestion_button_colors,
                   &Button::SetButtonColors);
  background->AddChild(std::move(suggestion_layout));

  element_binding->bindings().push_back(
      VR_BIND_FUNC(base::string16, SuggestionBinding, element_binding,
                   model()->content, Text, p_content_text, SetText));
  element_binding->bindings().push_back(
      base::MakeUnique<Binding<base::string16>>(
          base::BindRepeating(
              [](SuggestionBinding* m) { return m->model()->description; },
              base::Unretained(element_binding)),
          base::BindRepeating(
              [](Text* v, const base::string16& text) {
                v->SetVisibleImmediately(!text.empty());
                v->set_requires_layout(!text.empty());
                if (!text.empty()) {
                  v->SetText(text);
                }
              },
              base::Unretained(p_description_text))));
  element_binding->bindings().push_back(
      VR_BIND(AutocompleteMatch::Type, SuggestionBinding, element_binding,
              model()->type, VectorIcon, p_icon,
              SetIcon(AutocompleteMatch::TypeToVectorIcon(value))));
  element_binding->set_view(background.get());
  scene->AddUiElement(kOmniboxSuggestions, std::move(background));
}
