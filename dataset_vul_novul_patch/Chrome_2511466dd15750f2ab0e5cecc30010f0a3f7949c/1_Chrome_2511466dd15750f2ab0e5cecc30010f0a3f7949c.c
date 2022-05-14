   void FocusFirstNameField() {
     LOG(WARNING) << "Clicking on the tab.";
    ASSERT_NO_FATAL_FAILURE(ui_test_utils::ClickOnView(browser(),
                                                       VIEW_ID_TAB_CONTAINER));
    ASSERT_TRUE(ui_test_utils::IsViewFocused(browser(),
                                             VIEW_ID_TAB_CONTAINER));
 
     LOG(WARNING) << "Focusing the first name field.";
     bool result = false;
    ASSERT_TRUE(ui_test_utils::ExecuteJavaScriptAndExtractBool(
        render_view_host(), L"",
        L"if (document.readyState === 'complete')"
        L"  document.getElementById('firstname').focus();"
        L"else"
        L"  domAutomationController.send(false);",
        &result));
    ASSERT_TRUE(result);
  }
