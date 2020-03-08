drop table if exists set_lists;
drop table if exists set_lists_songs;
drop table if exists controller_mappings;
drop table if exists connections;
drop table if exists patches;
drop table if exists songs;
drop table if exists triggers;
drop table if exists messages;
drop table if exists instruments;

create table instruments (
  id int primary key,
  type int not null default 0,  -- 0 == input, 1 == output
  name text,
  port_name text
);

create table messages (
  id int primary key,
  name name not null,
  bytes text not null
);

create table triggers (
  id int primary key,
  input_id int not null references instruments(id),
  trigger_message_bytes text not null,
  action text,
  message_id int references messages(id)
);

create table songs (
  id int primary key,
  name text,
  notes text
);

create table patches (
  id int primary key,
  song_id int not null references songs(id),
  position int not null default 0,
  name text,
  start_message_id int references messages(id),
  stop_message_id int references messages(id)
);

create table connections (
  id int primary key,
  patch_id int not null references patches(id),
  position int not null default 0,
  input_id int not null,
  input_chan,
  output_id int not null,
  output_chan,
  bank_msb int,
  bank_lsb int,
  prog int,
  zone_low int not null,
  zone_high int not null,
  xpose int not null,
  pass_through_sysex int not null default 0 -- boolean
);

create table controller_mappings (
  id int primary key,
  connection_id int not null references connections(id),
  cc_num int not null,
  translated_cc_num int not null,
  min int not null,
  max int not null,
  filtered int not null default 0 -- boolean
);

create table set_lists (
  id int primary key,
  name text
);

create table set_lists_songs (
  set_list_id int not null references set_lists(id),
  song_id int not null null references songs(id),
  position int not null default 0
);
