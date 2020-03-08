#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include "cursor.h"

/*
 * A Cursor knows the current SetList, Song, and Patch, how to move between
 * songs and patches, and how to find them given name regexes.
 */

Cursor::Cursor(PatchMaster *pmaster)
  : pm(pmaster)
{
  clear();
}

Cursor::~Cursor() {
}

void Cursor::clear() {
  set_list_index = song_index = patch_index = UNDEFINED;
}

/*
 * Set @set_list to All Songs, @song to first song, and
 * @patch to song's first patch. Song and patch may be +nil+.
 */
void Cursor::init() {
  set_list_index = 0;

  SetList *sl = set_list();
  if (sl != nullptr && sl->songs.size() > 0) {
    song_index = 0;
    Song *s = song();
    patch_index = (s != 0 && s->patches.size() > 0) ? 0 : UNDEFINED;
  }
  else {
    song_index = UNDEFINED;
    patch_index = UNDEFINED;
  }
}

SetList *Cursor::set_list() {
  if (set_list_index != UNDEFINED)
    return pm->set_lists[set_list_index];
  else
    return nullptr;
}

Song *Cursor::song() {
  SetList *sl = set_list();
  if (sl == nullptr || song_index == UNDEFINED || sl->songs.size() == 0)
    return nullptr;
  return sl->songs[song_index];
}

Patch *Cursor::patch() {
  Song *s = song();
  if (s == nullptr || patch_index == UNDEFINED)
    return nullptr;
  return s->patches[patch_index];
}

void Cursor::next_song() {
  if (set_list_index == UNDEFINED)
    return;
  SetList *sl = set_list();
  if (song_index == sl->songs.size()-1)
    return;

  ++song_index;
  patch_index = 0;
}

void Cursor::prev_song() {
  if (set_list_index == UNDEFINED || song_index == 0)
    return;

  --song_index;
  patch_index = 0;
}

void Cursor::next_patch() {
  Song *s = song();
  if (s == nullptr)
    return;

  if (patch_index == s->patches.size()-1)
    next_song();
  else
    ++patch_index;
}

void Cursor::prev_patch() {
  if (song() == nullptr)
    return;

  if (patch_index == 0)
    prev_song();
  else
    --patch_index;
}

bool Cursor::has_next_song() {
  return set_list_index != UNDEFINED && song_index != set_list()->songs.size() - 1;
}

bool Cursor::has_prev_song() {
  return set_list_index != UNDEFINED && song_index != 0;
}

bool Cursor::has_next_patch() {
  Song *s = song();
  return s != nullptr && (patch_index != s->patches.size() - 1 || has_next_song());
}

bool Cursor::has_prev_patch() {
  Song *s = song();
  return s != nullptr && (patch_index > 0 || has_prev_song());
}

void Cursor::jump_to_set_list_index(int i) {
  set_list_index = i;
  jump_to_song_index(0);
}

void Cursor::jump_to_song_index(int i) {
  song_index = i;
  jump_to_patch_index(0);
}

void Cursor::jump_to_patch_index(int i) {
  patch_index = i;
}

void Cursor::goto_song(string name_regex) {
  SetList *sl = set_list();
  Song *new_song = nullptr;
  Named *named;
  regex_t re;
  regmatch_t match;
  int i;

  if (regcomp(&re, name_regex.c_str(), REG_EXTENDED | REG_ICASE) != 0)
    return;

  if (sl != nullptr) {
    i = 0;
    for (auto &song : sl->songs) {
      if (regexec(&re, song->name.c_str(), 1, &match, 0) == 0) {
        song_index = i;
        patch_index = 0;
        return;
      }
      ++i;
    }
  }

  i = 0;
  for (auto &song : pm->all_songs->songs) {
    if (regexec(&re, song->name.c_str(), 1, &match, 0) == 0) {
      set_list_index = 0;
      song_index = i;
      patch_index = 0;
      return;
    }
    ++i;
  }
}

void Cursor::goto_set_list(string name_regex) {
  regex_t re;
  regmatch_t match;

  if (regcomp(&re, name_regex.c_str(), REG_EXTENDED | REG_ICASE) != 0)
    return;

  int i = 0;
  for (auto &set_list : pm->set_lists) {
    if (regexec(&re, set_list->name.c_str(), 1, &match, 0) == 0) {
      set_list_index = i;
      song_index = 0;
      patch_index = 0;
      return;
    }
    ++i;
  }
}
