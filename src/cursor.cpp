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
  // Do not erase names saved by mark();
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

  if (sl != nullptr) {
    named = find_in_list(reinterpret_cast<vector<Named *> *>(&sl->songs), name_regex);
    new_song = reinterpret_cast<Song *>(named);
  }
  if (new_song == nullptr) {
    named = find_in_list(reinterpret_cast<vector<Named *> *>(&pm->all_songs->songs),
                         name_regex);
    new_song = reinterpret_cast<Song *>(named);
  }

  if (new_song == nullptr)
    return;

  Patch *new_patch = new_song == nullptr ? nullptr : new_song->patches[0];

  SetList *new_set_list = pm->all_songs;
  Song *curr_song = song();
  if (((new_song != nullptr && new_song != curr_song) || (new_song == curr_song && patch() != new_patch))
      && sl != nullptr
      && find(sl->songs.begin(), sl->songs.end(), new_song) != sl->songs.end())
  {
      new_set_list = sl;
  }

  set_list_index = find(pm->set_lists.begin(), pm->set_lists.end(), new_set_list) - pm->set_lists.begin();
  song_index = find(new_set_list->songs.begin(), new_set_list->songs.end(), new_song) - new_set_list->songs.begin();
  patch_index = find(new_song->patches.begin(), new_song->patches.end(), new_patch) - new_song->patches.begin();
}

void Cursor::goto_set_list(string name_regex) {
  Named *named = find_in_list(reinterpret_cast<vector<Named *> *>(&pm->set_lists), name_regex);
  if (named == nullptr)
    return;

  SetList *new_set_list = reinterpret_cast<SetList *>(named);
  if (new_set_list != set_list()) {
    set_list_index = find(pm->set_lists.begin(), pm->set_lists.end(), new_set_list) - pm->set_lists.begin();
    song_index = 0;
    patch_index = 0;
  }
}

// Attempt to go to the same set list, song, and patch that the other
// cursor `c` points to. Called when (re)loading a file.
void Cursor::attempt_goto(Cursor *c) {
  init();

  if (c->set_list() != nullptr)
    set_list_index =
      find_nearest_match_index(reinterpret_cast<vector<Named *> *>(&pm->set_lists),
                               c->set_list()->name);

  if (c->song() == nullptr)
    return;

  if (set_list_index != 0) { // look first in set list (unless it's all_songs)
    song_index =
      find_nearest_match_index(reinterpret_cast<vector<Named *> *>(&set_list()->songs),
                               c->song()->name);
  }
  if (song_index == -1)         // not found there, must be in all_songs
    song_index =
      find_nearest_match_index(reinterpret_cast<vector<Named *> *>(&pm->all_songs->songs),
                               c->song()->name);

  if (c->patch() != nullptr)
    patch_index =
      find_nearest_match_index(reinterpret_cast<vector<Named *> *>(&song()->patches),
                               c->patch()->name);
  else
    patch_index = 0;
}


Named *Cursor::find_in_list(vector<Named *> *list, string regex) {
  regex_t re;
  regmatch_t pm;

  if (regcomp(&re, regex.c_str(), REG_EXTENDED | REG_ICASE) != 0)
    return nullptr;

  for (auto& named : *list) {
    if (regexec(&re, named->name.c_str(), 1, &pm, 0) == 0)
      return named;
  }
  return nullptr;
}

int Cursor::find_nearest_match_index(vector<Named *> *list, string str) {
  string target = downcased_copy(str);
  vector<int> distances;
  for (auto& named : *list)
    distances.push_back(damerau_levenshtein(target, downcased_copy(named->name)));

  int min_dist = 9999, min_index = -1;
  
  for (vector<Named *>::iterator i = list->begin(); i != list->end(); ++i) {
    int di = i - list->begin();
    if (distances[di] < min_dist) {
      min_dist = distances[di];
      min_index = di;
    }
  }

  return min_index;
}

// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
unsigned int Cursor::damerau_levenshtein(const string& s1, const string& s2) {
  const size_t len1 = s1.size(), len2 = s2.size();
  vector<vector<unsigned int> > d(len1 + 1, vector<unsigned int>(len2 + 1));

  d[0][0] = 0;
  for (unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
  for (unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

  for (unsigned int i = 1; i <= len1; ++i)
    for (unsigned int j = 1; j <= len2; ++j)
      d[i][j] = min(min(d[i - 1][j] + 1, d[i][j - 1] + 1),
                    d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
  return d[len1][len2];
}

string Cursor::downcased_copy(string str) {
  string downcased = string(str);
  for (string::iterator i = downcased.begin(); i != downcased.end(); ++i)
    *i = tolower(*i);
  return downcased;
}
