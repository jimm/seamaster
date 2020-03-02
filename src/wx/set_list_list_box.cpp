#include "set_list_list_box.h"
#include "../patchmaster.h"
#include "../cursor.h"

SetListListBox::SetListListBox(wxWindow *parent, wxWindowID id, wxSize size)
  : wxListBox(parent, id, wxDefaultPosition, size)
{
}

void SetListListBox::update() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;

  Clear();
  wxArrayString names;
  for (auto& set_list : pm->set_lists)
    names.Add(set_list->name.c_str());
  if (!names.empty())
    InsertItems(names, 0);

  int i = 0;
  for (auto& set_list : pm->set_lists) {
    if (set_list == cursor->set_list()) {
      SetSelection(i);
      return;
    }
    ++i;
  }
}

void SetListListBox::jump() {
  int selection = GetSelection();
  if (selection != wxNOT_FOUND) {
    PatchMaster *pm = PatchMaster_instance();
    pm->jump_to_set_list_index(selection);
  }
}
