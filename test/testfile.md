# Instruments

Short names for inputs and outputs don't have to be same or different. This
text is ignored.

| I/O | port            | short | name          |
| --- | --------------- | ----- | --------------|
| in  | input port one  | one   | first input   |
| in  | input port two  | two   | second input  |
|:---:| --------------- | ----- | --------------|
| out | output port one | one   | first output  |
| out | output port two | two   | second output |

# Messages

Bytes can be hex or decimal and can span multiple lines. One message (one-
to three-byte MIDI message) per line. Sysex is not yet supported. Commas are
ignored. Sorry, no symbolic names yet.

## Tune Request

```
0xf6
```

## Multiple Note-Offs

0x80, 64, 0
0x81, 64, 0
0x82, 42, 127

# Triggers

Triggers...

  | input | bytes         | action     | message      |
  |-------+---------------+------------+--------------|
  | one   | 0xb0, 50, 127 | next song  |              |
  | one   | 0xb0, 51, 127 | prev song  |              |
  | one   | 0xb0, 52, 127 | next patch |              |
  | one   | 0xb0, 53, 127 | prev patch |              |
  | one   | 0xb0, 54, 127 | message    | Tune Request |

# Songs

Indentation doesn't matter. Again, this text is ignored. Song notes appear
after the song heading and before the first patch heading.

## To Each His Own

### Vanilla Through, Filter Two's Sustain
#### one, all, one, all
#### two, all, two, all

     - bank 3, 2
     - pc 12
     - cc 64 filter

### One Up One Octave and CC Vol -> Pan, Two Down One Octave
#### one, all, one, all
     - xpose 12
     - cc 7 map 10, limit 1 120
#### two, all, two, all
     - xpose -12

## Another Song
   
```
the line before begin_example contains only whitespace
this song has note text
that spans multiple lines
```

### Two Inputs Merging
#### one, 3, two, 4
     - xpose 12
     - zone 0, 63
#### two, 1, two, 5
     - zone e4, G9
     - xpose -12

### Split Into Two Outputs
#### one, 3, one, 4
     - xpose 12
     - zone 0, 63
#### one, 1, two, 5
     - zone e4, g9
     - xpose -12

## Song Without Explicit Patch

# Ignore This Section

Any top-level section that isn't one of
- Instruments
- Messages
- Triggers
- Songs
- Set Lists
should be ignored. Let's see:

## This is Not a Song

### This is Not a Patch
#### bad, 2, badder, 99

# Set Lists

Two other bullet list characters are accepted: "*" and "+".

## Song List One

* To Each His Own
* Another Song

## Song List Two

+ Another Song
+ To Each His Own
