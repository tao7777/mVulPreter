bool ShouldUseNativeViews() {
#if defined(OS_WIN) || defined(OS_MACOSX) || defined(OS_LINUX)
  return base::FeatureList::IsEnabled(kAutofillExpandedPopupViews) ||
         base::FeatureList::IsEnabled(::features::kExperimentalUi);
#else
  return false;
#endif
}
