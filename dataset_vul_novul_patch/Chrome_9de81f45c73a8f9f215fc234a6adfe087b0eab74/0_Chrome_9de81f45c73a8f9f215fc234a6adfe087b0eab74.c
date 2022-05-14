PasswordAutofillAgent::PasswordAutofillAgent(content::RenderFrame* render_frame)
     : content::RenderFrameObserver(render_frame),
       logging_state_active_(false),
       was_username_autofilled_(false),
      was_password_autofilled_(false) {
   Send(new AutofillHostMsg_PasswordAutofillAgentConstructed(routing_id()));
 }
