# Introduction

This is an example [SeaMaster](https://github.com/jimm/seamaster) file. It
is a [Markdown](http://daringfireball.net/projects/markdown/syntax) file,
which is a plain text file format. This file describes your MIDI setup,
songs, and patches. Only a few top-level sections are read by SeaMaster, any
others such as this one are ignored. They must be in the following order,
though ignored sections may appear anywhere, including between them.

- Instruments
- Messages (/optional/)
- Triggers (/optional/)
- Songs
    - Patches
        - Connections
- Set Lists (/optional/)

This sections, since it isn't one of those, is ignored.

In general, indentation doesn't matter. Beginning whitespace is ignored,
except within song notes.

# Instruments

This section describes the instruments in your MIDI setup by listing input
and output MIDI ports. Each one specifies the port name, a short name you
use below to refer to the port, and a longer display name.

The short names for inputs and outputs need to be unique within the type
(input or output), but an input and an output can share the same short and
long names.

The header row and separator rows are optional. The first few letters of the
first column determines if the row describes an input or output. If they
match "in" or "out" then the row describes a MIDI port.

All text except the table itself is ignored.

| I/O | port                       | short | name        |
|-----+----------------------------+-------+-------------|
| in  | MIDISPORT 4x4 Anniv Port A | kk    | Kronos      |
| in  | MIDISPORT 4x4 Anniv Port B | ws    | Wavestation |
|-----+----------------------------+-------+-------------|
| out | MIDISPORT 4x4 Anniv Port A | kk    | Kronos      |
| out | MIDISPORT 4x4 Anniv Port B | ws    | Wavestation |

# Messages

A message is a named list of MIDI one- to three-byte messages. Bytes can be
hex or decimal. There can be multiple messages but each one must appear on a
separate line. Sysex is not yet supported. Commas are ignored. Sorry, there
is not yet support for symbolic names such as "CONTROLLER" or "TUNE_REQUEST"
for bytes yet.

Within each message only Markdown block delimiters ("```" and "```" lines)
are ignored; all other lines are assumed to be MIDI messages.

## Tune Request

```
0xf6
```

## Multiple Note-Offs

0x80, 64, 0
0x81, 64, 0
0x82, 42, 0

# Triggers

Triggers listen for a particular message from an input instrument and, when
that message is seen, perform some action.

The title and separator lines are ignored. Actually, any line where the
first column isn't a legal input instrument short name is ignored.

The first column contains the input instrument short name. The second is a
one- to three-byte MIDI message that triggers the action or message send.

The third column is one of "next song", "next patch", "prev (or 'previous')
song", "prev patch", or "message". "Message" requires a message name.


  | input | bytes         | action     | message      |
  |-------+---------------+------------+--------------|
  | kk    | 0xb0, 50, 127 | next song  |              |
  | kk    | 0xb0, 51, 127 | prev song  |              |
  | kk    | 0xb0, 52, 127 | next patch |              |
  | kk    | 0xb0, 53, 127 | prev patch |              |
  | ws    | 0xb0, 50, 127 | message    | Tune Request |

# Songs

Indentation doesn't matter. Again, this text is ignored. Song notes appear
after the song heading and before the first patch heading.

## To Each His Own

These are optional song notes.
They will be displayed on-screen.

### Vanilla Through
#### kk, all, kk, all
#### ws, all, ws, all

## Kronos -> Wavestation

```
Kronos channel 1 sent to Wavestation.

The Org mode block begin/end commands are ignored.
```

### patch K to W
#### kk, all, ws, all
#### ws, all, kk, all

# Set Lists

List items can start with either "-" or "*".

## Tonight's Song List

* To Each His Own
* Kronos -> Wavestation
