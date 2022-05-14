 void BrowserEventRouter::DispatchTabUpdatedEvent(
    WebContents* contents, DictionaryValue* changed_properties) {
   DCHECK(changed_properties);
   DCHECK(contents);
 
  scoped_ptr<ListValue> args_base(new ListValue());

   args_base->AppendInteger(ExtensionTabUtil::GetTabId(contents));
 
  args_base->Append(changed_properties);
 
  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());

  scoped_ptr<Event> event(new Event(events::kOnTabUpdated, args_base.Pass()));
   event->restrict_to_profile = profile;
   event->user_gesture = EventRouter::USER_GESTURE_NOT_ENABLED;
   event->will_dispatch_callback =
      base::Bind(&WillDispatchTabUpdatedEvent, contents);
   ExtensionSystem::Get(profile)->event_router()->BroadcastEvent(event.Pass());
 }
