# SeaMaster

SeaMaster is a C version of [PatchMaster](https://patchmaster.org/).

More docs to come.

# To Do

- Default pass-through patch if none defined for a song. Maps all channels
  of all inputs to output with matching short name. If there is not at least
  one such pair, that is an error that causes loading to fail.

- Start and stop messages for patches.

- Messages.

- Triggers.

- Fix window resizing.

- Load should be more robust. Count args (e.g. input and output creation)
  and make sure not null.

- Info window should bold "\*text\*" and italicize "\_text\_".

- It wouldn't be difficult to have the loader support Markdown as well as
  Org mode. Besides heading character changes, comments couldn't start with
  the '#' character.

- Documentation.

- Names for connections.
