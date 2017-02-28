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

For more information see the [wiki](https://github.com/jimm/seamaster/wiki).

# To Do

See the [To Do](https://github.com/jimm/seamaster/wiki/To-Do) list on the Wiki.

Ideas for how start/stop messages should appear:

## Song
### Patch

Use a definition list. Not sure how that works in Markdown.

- start :: b1, b2, b3, ...
- stop :: b1, b2, b3, ...
