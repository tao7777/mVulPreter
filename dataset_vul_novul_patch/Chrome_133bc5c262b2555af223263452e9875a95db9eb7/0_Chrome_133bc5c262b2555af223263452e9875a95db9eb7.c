HTMLTreeBuilderSimulator::SimulatedToken HTMLTreeBuilderSimulator::Simulate(
    const CompactHTMLToken& token,
    HTMLTokenizer* tokenizer) {
  SimulatedToken simulated_token = kOtherToken;

  if (token.GetType() == HTMLToken::kStartTag) {
    const String& tag_name = token.Data();
    if (ThreadSafeMatch(tag_name, SVGNames::svgTag))
      namespace_stack_.push_back(SVG);
    if (ThreadSafeMatch(tag_name, MathMLNames::mathTag))
       namespace_stack_.push_back(kMathML);
     if (InForeignContent() && TokenExitsForeignContent(token))
       namespace_stack_.pop_back();
    if (IsHTMLIntegrationPointForStartTag(token) ||
        (namespace_stack_.back() == kMathML && TokenExitsMath(token))) {
       namespace_stack_.push_back(HTML);
    } else if (!InForeignContent()) {
       if (ThreadSafeMatch(tag_name, textareaTag) ||
          ThreadSafeMatch(tag_name, titleTag)) {
        tokenizer->SetState(HTMLTokenizer::kRCDATAState);
      } else if (ThreadSafeMatch(tag_name, scriptTag)) {
        tokenizer->SetState(HTMLTokenizer::kScriptDataState);
        simulated_token = kScriptStart;
      } else if (ThreadSafeMatch(tag_name, linkTag)) {
        simulated_token = kLink;
      } else if (!in_select_insertion_mode_) {
        if (ThreadSafeMatch(tag_name, plaintextTag) &&
            !in_select_insertion_mode_) {
          tokenizer->SetState(HTMLTokenizer::kPLAINTEXTState);
        } else if (ThreadSafeMatch(tag_name, styleTag) ||
                   ThreadSafeMatch(tag_name, iframeTag) ||
                   ThreadSafeMatch(tag_name, xmpTag) ||
                   (ThreadSafeMatch(tag_name, noembedTag) &&
                    options_.plugins_enabled) ||
                   ThreadSafeMatch(tag_name, noframesTag) ||
                   (ThreadSafeMatch(tag_name, noscriptTag) &&
                    options_.script_enabled)) {
          tokenizer->SetState(HTMLTokenizer::kRAWTEXTState);
        }
      }

      if (ThreadSafeMatch(tag_name, selectTag)) {
        in_select_insertion_mode_ = true;
      } else if (in_select_insertion_mode_ && TokenExitsInSelect(token)) {
        in_select_insertion_mode_ = false;
      }
    }
  }

  if (token.GetType() == HTMLToken::kEndTag ||
      (token.GetType() == HTMLToken::kStartTag && token.SelfClosing() &&
       InForeignContent())) {
    const String& tag_name = token.Data();
    if ((namespace_stack_.back() == SVG &&
          ThreadSafeMatch(tag_name, SVGNames::svgTag)) ||
         (namespace_stack_.back() == kMathML &&
          ThreadSafeMatch(tag_name, MathMLNames::mathTag)) ||
        IsHTMLIntegrationPointForEndTag(token) ||
         (namespace_stack_.Contains(kMathML) &&
          namespace_stack_.back() == HTML && TokenExitsMath(token))) {
       namespace_stack_.pop_back();
    }
    if (ThreadSafeMatch(tag_name, scriptTag)) {
      if (!InForeignContent())
        tokenizer->SetState(HTMLTokenizer::kDataState);
      return kScriptEnd;
    } else if (ThreadSafeMatch(tag_name, selectTag)) {
      in_select_insertion_mode_ = false;
    }
    if (ThreadSafeMatch(tag_name, styleTag))
      simulated_token = kStyleEnd;
  }

  tokenizer->SetForceNullCharacterReplacement(InForeignContent());
  tokenizer->SetShouldAllowCDATA(InForeignContent());
   return simulated_token;
 }
