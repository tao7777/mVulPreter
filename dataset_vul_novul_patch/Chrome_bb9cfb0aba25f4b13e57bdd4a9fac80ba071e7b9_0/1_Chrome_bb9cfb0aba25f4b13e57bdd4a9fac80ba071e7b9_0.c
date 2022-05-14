void HTMLInputElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == nameAttr) {
        removeFromRadioButtonGroup();
        m_name = value;
        addToRadioButtonGroup();
        HTMLTextFormControlElement::parseAttribute(name, value);
    } else if (name == autocompleteAttr) {
        if (equalIgnoringCase(value, "off"))
            m_autocomplete = Off;
        else {
            if (value.isEmpty())
                m_autocomplete = Uninitialized;
            else
                m_autocomplete = On;
        }
    } else if (name == typeAttr)
        updateType();
    else if (name == valueAttr) {
        if (!hasDirtyValue()) {
            updatePlaceholderVisibility(false);
            setNeedsStyleRecalc();
        }
        setFormControlValueMatchesRenderer(false);
        setNeedsValidityCheck();
        m_valueAttributeWasUpdatedAfterParsing = !m_parsingInProgress;
        m_inputType->valueAttributeChanged();
    } else if (name == checkedAttr) {
        if (!m_parsingInProgress && m_reflectsCheckedAttribute) {
            setChecked(!value.isNull());
            m_reflectsCheckedAttribute = true;
        }
    } else if (name == maxlengthAttr)
        parseMaxLengthAttribute(value);
    else if (name == sizeAttr) {
        int oldSize = m_size;
        int valueAsInteger = value.toInt();
        m_size = valueAsInteger > 0 ? valueAsInteger : defaultSize;
        if (m_size != oldSize && renderer())
            renderer()->setNeedsLayoutAndPrefWidthsRecalc();
    } else if (name == altAttr)
        m_inputType->altAttributeChanged();
    else if (name == srcAttr)
        m_inputType->srcAttributeChanged();
    else if (name == usemapAttr || name == accesskeyAttr) {
    } else if (name == onsearchAttr) {
        setAttributeEventListener(eventNames().searchEvent, createAttributeEventListener(this, name, value));
    } else if (name == resultsAttr) {
        int oldResults = m_maxResults;
        m_maxResults = !value.isNull() ? std::min(value.toInt(), maxSavedResults) : -1;
        if (m_maxResults != oldResults && (m_maxResults <= 0 || oldResults <= 0))
            lazyReattachIfAttached();
        setNeedsStyleRecalc();
        UseCounter::count(document(), UseCounter::ResultsAttribute);
    } else if (name == incrementalAttr) {
        setNeedsStyleRecalc();
        UseCounter::count(document(), UseCounter::IncrementalAttribute);
    } else if (name == minAttr) {
        m_inputType->minOrMaxAttributeChanged();
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::MinAttribute);
    } else if (name == maxAttr) {
        m_inputType->minOrMaxAttributeChanged();
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::MaxAttribute);
    } else if (name == multipleAttr) {
        m_inputType->multipleAttributeChanged();
        setNeedsValidityCheck();
    } else if (name == stepAttr) {
        m_inputType->stepAttributeChanged();
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::StepAttribute);
    } else if (name == patternAttr) {
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::PatternAttribute);
    } else if (name == precisionAttr) {
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::PrecisionAttribute);
    } else if (name == disabledAttr) {
        HTMLTextFormControlElement::parseAttribute(name, value);
        m_inputType->disabledAttributeChanged();
    } else if (name == readonlyAttr) {
        HTMLTextFormControlElement::parseAttribute(name, value);
        m_inputType->readonlyAttributeChanged();
    } else if (name == listAttr) {
        m_hasNonEmptyList = !value.isEmpty();
        if (m_hasNonEmptyList) {
            resetListAttributeTargetObserver();
            listAttributeTargetChanged();
        }
        UseCounter::count(document(), UseCounter::ListAttribute);
    }
#if ENABLE(INPUT_SPEECH)
    else if (name == webkitspeechAttr) {
        if (renderer()) {
            detach();
             m_inputType->destroyShadowSubtree();
             m_inputType->createShadowSubtree();
             if (!attached())
                 attach();
        } else {
            m_inputType->destroyShadowSubtree();
            m_inputType->createShadowSubtree();
        }
        setFormControlValueMatchesRenderer(false);
        setNeedsStyleRecalc();
        UseCounter::count(document(), UseCounter::PrefixedSpeechAttribute);
    } else if (name == onwebkitspeechchangeAttr)
        setAttributeEventListener(eventNames().webkitspeechchangeEvent, createAttributeEventListener(this, name, value));
#endif
    else if (name == webkitdirectoryAttr) {
        HTMLTextFormControlElement::parseAttribute(name, value);
        UseCounter::count(document(), UseCounter::PrefixedDirectoryAttribute);
    }
    else
        HTMLTextFormControlElement::parseAttribute(name, value);
    m_inputType->attributeChanged();
}
