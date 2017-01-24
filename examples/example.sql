insert into instruments (id, name, short_name, port_name, input, output)
values
(1, "Kronos", "kk", "MIDISPORT 4x4 Anniv A", 1, 0),
(2, "Kronos", "kk", "MIDISPORT 4x4 Anniv A", 0, 1),
(3, "Wavestation", "ws", "MIDISPORT 4x4 Anniv B", 1, 0),
(4, "Wavestation", "ws", "MIDISPORT 4x4 Anniv B", 0, 1);

insert into songs (id, name, notes)
values
(1, "To Each His Own", NULL),
(2, "Kronos -> Wavestation", "Kronos channel 1 sent to Wavestation");

insert into patches (id, song_id, position, name)
values
(1, 1, 0, "Vanilla Through"),
(2, 2, 0, "K to W");

insert into connections (id, patch_id, input_id, input_chan, output_id,
  output_chan, bank_msb, bank_lsb, prog_chg, zone_low, zone_high, xpose)
values
(1, 1, 1, NULL, 2, 1, NULL, NULL, NULL, NULL, NULL, NULL),
(2, 1, 3, NULL, 4, 1, NULL, NULL, NULL, NULL, NULL, NULL),
(3, 2, 1, NULL, 4, 1, NULL, NULL, NULL, NULL, NULL, NULL),
(4, 2, 3, NULL, 2, 1, NULL, NULL, NULL, NULL, NULL, NULL);

insert into song_lists (id, name)
values
(1, 'Tonight''s Song List');

insert into song_lists_songs (song_list_id, song_id, position)
values
(1, 2, 0),
(1, 1, 1);
