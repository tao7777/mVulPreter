   void FocusFirstNameField() {
     LOG(WARNING) << "Clicking on the tab.";
    ui_test_utils::SimulateMouseClick(browser()->GetSelectedWebContents());
 
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
