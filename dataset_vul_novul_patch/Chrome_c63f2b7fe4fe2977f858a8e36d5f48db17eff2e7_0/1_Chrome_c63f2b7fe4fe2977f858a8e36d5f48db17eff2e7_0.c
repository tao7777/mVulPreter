void FactoryRegistry::ResetFunctions() {

  RegisterFunction<GetWindowFunction>();
  RegisterFunction<GetCurrentWindowFunction>();
  RegisterFunction<GetLastFocusedWindowFunction>();
  RegisterFunction<GetAllWindowsFunction>();
  RegisterFunction<CreateWindowFunction>();
  RegisterFunction<UpdateWindowFunction>();
  RegisterFunction<RemoveWindowFunction>();

  RegisterFunction<GetTabFunction>();
  RegisterFunction<GetCurrentTabFunction>();
  RegisterFunction<GetSelectedTabFunction>();
  RegisterFunction<GetAllTabsInWindowFunction>();
  RegisterFunction<CreateTabFunction>();
  RegisterFunction<UpdateTabFunction>();
  RegisterFunction<MoveTabFunction>();
  RegisterFunction<RemoveTabFunction>();
  RegisterFunction<DetectTabLanguageFunction>();
  RegisterFunction<CaptureVisibleTabFunction>();
  RegisterFunction<TabsExecuteScriptFunction>();
  RegisterFunction<TabsInsertCSSFunction>();

  RegisterFunction<EnablePageActionFunction>();
  RegisterFunction<DisablePageActionFunction>();
  RegisterFunction<PageActionShowFunction>();
  RegisterFunction<PageActionHideFunction>();
  RegisterFunction<PageActionSetIconFunction>();
  RegisterFunction<PageActionSetTitleFunction>();
  RegisterFunction<PageActionSetPopupFunction>();

  RegisterFunction<BrowserActionSetIconFunction>();
  RegisterFunction<BrowserActionSetTitleFunction>();
  RegisterFunction<BrowserActionSetBadgeTextFunction>();
  RegisterFunction<BrowserActionSetBadgeBackgroundColorFunction>();
  RegisterFunction<BrowserActionSetPopupFunction>();

  RegisterFunction<GetBookmarksFunction>();
  RegisterFunction<GetBookmarkChildrenFunction>();
  RegisterFunction<GetBookmarkRecentFunction>();
  RegisterFunction<GetBookmarkTreeFunction>();
  RegisterFunction<GetBookmarkSubTreeFunction>();
  RegisterFunction<SearchBookmarksFunction>();
  RegisterFunction<RemoveBookmarkFunction>();
  RegisterFunction<RemoveTreeBookmarkFunction>();
  RegisterFunction<CreateBookmarkFunction>();
  RegisterFunction<MoveBookmarkFunction>();
  RegisterFunction<UpdateBookmarkFunction>();

  RegisterFunction<ShowInfoBarFunction>();

  RegisterFunction<CopyBookmarkManagerFunction>();
  RegisterFunction<CutBookmarkManagerFunction>();
  RegisterFunction<PasteBookmarkManagerFunction>();
  RegisterFunction<CanPasteBookmarkManagerFunction>();
  RegisterFunction<ImportBookmarksFunction>();
  RegisterFunction<ExportBookmarksFunction>();
  RegisterFunction<SortChildrenBookmarkManagerFunction>();
  RegisterFunction<BookmarkManagerGetStringsFunction>();
  RegisterFunction<StartDragBookmarkManagerFunction>();
  RegisterFunction<DropBookmarkManagerFunction>();
  RegisterFunction<GetSubtreeBookmarkManagerFunction>();
  RegisterFunction<CanEditBookmarkManagerFunction>();

  RegisterFunction<AddUrlHistoryFunction>();
  RegisterFunction<DeleteAllHistoryFunction>();
  RegisterFunction<DeleteRangeHistoryFunction>();
  RegisterFunction<DeleteUrlHistoryFunction>();
  RegisterFunction<GetVisitsHistoryFunction>();
  RegisterFunction<SearchHistoryFunction>();

  RegisterFunction<ExtensionIdleQueryStateFunction>();

  RegisterFunction<GetAcceptLanguagesFunction>();

  RegisterFunction<GetProcessIdForTabFunction>();

  RegisterFunction<MetricsGetEnabledFunction>();
  RegisterFunction<MetricsSetEnabledFunction>();
  RegisterFunction<MetricsRecordUserActionFunction>();
  RegisterFunction<MetricsRecordValueFunction>();
  RegisterFunction<MetricsRecordPercentageFunction>();
  RegisterFunction<MetricsRecordCountFunction>();
  RegisterFunction<MetricsRecordSmallCountFunction>();
  RegisterFunction<MetricsRecordMediumCountFunction>();
  RegisterFunction<MetricsRecordTimeFunction>();
  RegisterFunction<MetricsRecordMediumTimeFunction>();
  RegisterFunction<MetricsRecordLongTimeFunction>();

#if defined(OS_WIN)
  RegisterFunction<RlzRecordProductEventFunction>();
  RegisterFunction<RlzGetAccessPointRlzFunction>();
  RegisterFunction<RlzSendFinancialPingFunction>();
  RegisterFunction<RlzClearProductStateFunction>();
#endif

  RegisterFunction<GetCookieFunction>();
  RegisterFunction<GetAllCookiesFunction>();
  RegisterFunction<SetCookieFunction>();
  RegisterFunction<RemoveCookieFunction>();
  RegisterFunction<GetAllCookieStoresFunction>();

  RegisterFunction<ExtensionTestPassFunction>();
  RegisterFunction<ExtensionTestFailFunction>();
  RegisterFunction<ExtensionTestLogFunction>();
  RegisterFunction<ExtensionTestQuotaResetFunction>();
  RegisterFunction<ExtensionTestCreateIncognitoTabFunction>();
  RegisterFunction<ExtensionTestSendMessageFunction>();
  RegisterFunction<ExtensionTestGetConfigFunction>();

  RegisterFunction<GetFocusedControlFunction>();
   RegisterFunction<SetAccessibilityEnabledFunction>();
 
   RegisterFunction<ExtensionTtsSpeakFunction>();
   RegisterFunction<ExtensionTtsStopSpeakingFunction>();
  RegisterFunction<ExtensionTtsIsSpeakingFunction>();
  RegisterFunction<ExtensionTtsSpeakCompletedFunction>();
 
   RegisterFunction<CreateContextMenuFunction>();
  RegisterFunction<UpdateContextMenuFunction>();
  RegisterFunction<RemoveContextMenuFunction>();
  RegisterFunction<RemoveAllContextMenusFunction>();

  RegisterFunction<OmniboxSendSuggestionsFunction>();
  RegisterFunction<OmniboxSetDefaultSuggestionFunction>();

  RegisterFunction<CollapseSidebarFunction>();
  RegisterFunction<ExpandSidebarFunction>();
  RegisterFunction<GetStateSidebarFunction>();
  RegisterFunction<HideSidebarFunction>();
  RegisterFunction<NavigateSidebarFunction>();
  RegisterFunction<SetBadgeTextSidebarFunction>();
  RegisterFunction<SetIconSidebarFunction>();
  RegisterFunction<SetTitleSidebarFunction>();
  RegisterFunction<ShowSidebarFunction>();

#if defined(TOOLKIT_VIEWS)
  RegisterFunction<SendKeyboardEventInputFunction>();
#endif

#if defined(TOUCH_UI)
  RegisterFunction<HideKeyboardFunction>();
  RegisterFunction<SetKeyboardHeightFunction>();
#endif

#if defined(OS_CHROMEOS) && defined(TOUCH_UI)
  RegisterFunction<CandidateClickedInputUiFunction>();
  RegisterFunction<CursorUpInputUiFunction>();
  RegisterFunction<CursorDownInputUiFunction>();
  RegisterFunction<PageUpInputUiFunction>();
  RegisterFunction<PageDownInputUiFunction>();
  RegisterFunction<RegisterInputUiFunction>();
  RegisterFunction<PageUpInputUiFunction>();
  RegisterFunction<PageDownInputUiFunction>();
#endif

  RegisterFunction<GetAllExtensionsFunction>();
  RegisterFunction<GetExtensionByIdFunction>();
  RegisterFunction<LaunchAppFunction>();
  RegisterFunction<SetEnabledFunction>();
  RegisterFunction<UninstallFunction>();

  RegisterFunction<SetUpdateUrlDataFunction>();
  RegisterFunction<IsAllowedIncognitoAccessFunction>();
  RegisterFunction<IsAllowedFileSchemeAccessFunction>();

  RegisterFunction<GetBrowserLoginFunction>();
  RegisterFunction<GetStoreLoginFunction>();
  RegisterFunction<SetStoreLoginFunction>();
  RegisterFunction<PromptBrowserLoginFunction>();
  RegisterFunction<BeginInstallFunction>();
  RegisterFunction<BeginInstallWithManifestFunction>();
  RegisterFunction<CompleteInstallFunction>();

  RegisterFunction<WebRequestAddEventListener>();
  RegisterFunction<WebRequestEventHandled>();

  RegisterFunction<GetPreferenceFunction>();
  RegisterFunction<SetPreferenceFunction>();
  RegisterFunction<ClearPreferenceFunction>();

#if defined(OS_CHROMEOS)
  RegisterFunction<GetChromeosInfoFunction>();

  RegisterFunction<CancelFileDialogFunction>();
  RegisterFunction<ExecuteTasksFileBrowserFunction>();
  RegisterFunction<FileDialogStringsFunction>();
  RegisterFunction<GetFileTasksFileBrowserFunction>();
  RegisterFunction<GetVolumeMetadataFunction>();
  RegisterFunction<RequestLocalFileSystemFunction>();
  RegisterFunction<AddFileWatchBrowserFunction>();
  RegisterFunction<RemoveFileWatchBrowserFunction>();
  RegisterFunction<SelectFileFunction>();
  RegisterFunction<SelectFilesFunction>();
  RegisterFunction<UnmountVolumeFunction>();
  RegisterFunction<ViewFilesFunction>();

  RegisterFunction<PlayAtMediaplayerFunction>();
  RegisterFunction<SetPlaybackErrorMediaplayerFunction>();
  RegisterFunction<GetPlaylistMediaplayerFunction>();
  RegisterFunction<TogglePlaylistPanelMediaplayerFunction>();
  RegisterFunction<ToggleFullscreenMediaplayerFunction>();
#if defined(TOUCH_UI)
  RegisterFunction<SendHandwritingStrokeFunction>();
  RegisterFunction<CancelHandwritingStrokesFunction>();
#endif
#endif

  RegisterFunction<WebSocketProxyPrivateGetPassportForTCPFunction>();

  RegisterFunction<AttachDebuggerFunction>();
  RegisterFunction<DetachDebuggerFunction>();
  RegisterFunction<SendRequestDebuggerFunction>();

  RegisterFunction<GetResourceIdentifiersFunction>();
  RegisterFunction<ClearContentSettingsFunction>();
  RegisterFunction<GetContentSettingFunction>();
  RegisterFunction<SetContentSettingFunction>();

  RegisterFunction<AppNotifyFunction>();
  RegisterFunction<AppClearAllNotificationsFunction>();
}
