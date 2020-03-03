#include "message_list.h"
#include "../patchmaster.h"

MessageList::MessageList(wxWindow *parent, wxWindowID id, wxSize size)
  : wxListBox(parent, id, wxDefaultPosition, size, 0, nullptr, wxLB_SINGLE),
    message(nullptr)
{
}

void MessageList::update() {
  PatchMaster *pm = PatchMaster_instance();

  Clear();
  wxArrayString names;
  for (auto& message : pm->messages)
    names.Add(message->name.c_str());
  if (!names.empty())
    InsertItems(names, 0);
}
