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

# To Do / Bugs

See the [To Do](https://github.com/jimm/seamaster/wiki/To-Do) list on the Wiki.

Should the rememberance of program changes messages be moved to outputs?
After all, they will recieve the actual (possibly modified) program changes.
