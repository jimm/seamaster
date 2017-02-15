# SeaMaster

SeaMaster is a MIDI processing and patching system. It allows a musician
to reconfigure a MIDI setup instantaneously and modify the MIDI data in real
time.

With SeaMaster a performer can split controlling keyboards, layer MIDI
channels, transpose them, send program changes and System Exclusive
messages, limit controller and velocity values, and much more. At the stomp
of a foot switch (or any other MIDI event), an entire MIDI system can be
totally reconfigured.

SeaMaster is a C version of [PatchMaster](https://patchmaster.org/).

# Notes

Default patch.

# To Do

- Start and stop messages for patches.

- Sysex in messages.

- Fix window resizing.

- Load should be more robust. Count args (e.g. input and output creation)
  and make sure not null.

- Info window should bold "\*text\*" and italicize "\_text\_".

- It wouldn't be difficult to have the loader support Markdown as well as
  Org mode. Besides heading character changes, comments couldn't start with
  the '#' character.

- Documentation.

- Names for connections.
