# SeaMaster

SeaMaster is a C version of [PatchMaster](https://patchmaster.org/).

More docs to come.

# To Do

When switching from one patch to another, need to remember note on messages
and allow corresponding note offs to be sent to the same connections. Maybe
when remove connections from an input, move them to another "still waiting
for note offs" vector in the input. When send to current connections and
note off connections, make sure not to send to same connection twice.
Hmm...really want to make sure not to send to same outputs, but then again
connections can modify data before sending to outputs.

How much do I have to worry about this? Will only switch like that during a
song, and it might be the case that the note offs are just fine being sent
by the next patch.

Context for a note: connection to which it would be sent.

inputs are started when patchmaster starts and stops when the pm stops.
Therefore if we hold connection for each note off and send to that
connection we should be all set.



----

Load should count args (e.g. input and output creation) and make sure not
null.
