eval_js(WebKitWebView * web_view, gchar *script, GString *result) {
     WebKitWebFrame *frame;
     JSGlobalContextRef context;
     JSObjectRef globalobject;
    JSStringRef var_name;
 
     JSStringRef js_script;
     JSValueRef js_result;
    JSStringRef js_result_string;
    size_t js_result_size;

    js_init();

    frame = webkit_web_view_get_main_frame(WEBKIT_WEB_VIEW(web_view));
     context = webkit_web_frame_get_global_context(frame);
     globalobject = JSContextGetGlobalObject(context);
 
    /* uzbl javascript namespace */
    var_name = JSStringCreateWithUTF8CString("Uzbl");
    JSObjectSetProperty(context, globalobject, var_name,
                        JSObjectMake(context, uzbl.js.classref, NULL),
                        kJSClassAttributeNone, NULL);
     /* evaluate the script and get return value*/
     js_script = JSStringCreateWithUTF8CString(script);
     js_result = JSEvaluateScript(context, js_script, globalobject, NULL, 0, NULL);
    if (js_result && !JSValueIsUndefined(context, js_result)) {
        js_result_string = JSValueToStringCopy(context, js_result, NULL);
        js_result_size = JSStringGetMaximumUTF8CStringSize(js_result_string);

        if (js_result_size) {
            char js_result_utf8[js_result_size];
            JSStringGetUTF8CString(js_result_string, js_result_utf8, js_result_size);
            g_string_assign(result, js_result_utf8);
        }

        JSStringRelease(js_result_string);
     }
 
     /* cleanup */
    JSObjectDeleteProperty(context, globalobject, var_name, NULL);
    JSStringRelease(var_name);
     JSStringRelease(js_script);
 }
