create table instruments (
  id integer primary key,
  name text,
  short_name text,
  port_name text,
  input integer not null,
  output integer not null
);

create table messages (
  id integer primary key,
  name text,
  length integer,
  bytes blob
);

create table triggers (
  id integer primary key,
  action string,
  input_instrument_id integer,
  bytes blob
);

create table songs (
  id integer primary key,
  name text,
  notes text
);

create table patches (
  id integer primary key,
  song_id integer not null,
  position integer not null,
  name text
);

create table connections (
  id integer primary key,
  patch_id integer,
  input_id integer,
  input_chan integer,
  output_id integer,
  output_chan integer,
  bank_msb integer,
  bank_lsb integer,
  prog_chg integer,
  zone_low integer,
  zone_high integer,
  xpose integer
);

create table song_lists (
  id integer primary key,
  name text
);

create table song_lists_songs (
  song_list_id integer not null,
  song_id integer not null,
  position integer not null
);
