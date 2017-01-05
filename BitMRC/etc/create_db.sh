#!/bin/bash

function usage() {
echo "create_db.sh <password for root account on db>"
}
if [ -z "$1" ] ; then
 echo "please enter db admin password"
 usage
 exit 1
fi

echo "create db"
mysql -u root --password=$1 <<END_OF
DROP DATABASE bitmrc_test;
CREATE DATABASE IF NOT EXISTS bitmrc DEFAULT CHARACTER SET latin1;
CREATE DATABASE IF NOT EXISTS bitmrc_test DEFAULT CHARACTER SET latin1;
END_OF
echo "grant"
mysql -u root --password=$1 <<END_OF
CREATE USER  'bitmrc'@'localhost' IDENTIFIED BY 'BitMRC';
END_OF

echo "grant_2"
mysql -u root --password=$1 <<END_OF
grant ALL on  bitmrc.* to 'bitmrc'@'%';
grant ALL on  bitmrc_test.* to 'bitmrc'@'%';
END_OF


for i in bitmrc bitmrc_test; do
echo "DB $i :: stored procedures: save_system_table"
mysql -u root --password=$1 <<END_OF
DELIMITER $$;
use $i ;
DROP PROCEDURE IF EXISTS save_system_table;

CREATE DEFINER='root'@'localhost' PROCEDURE save_system_table (
	in node_id_in int, in working_mode_in int, in networking_flags_in int,
	in stream_id1_in int, in stream_id2_in int,
	in stream_id3_in int, in stream_id4_in int,
	in last_startup_result_in int,
	in database_version_in int,in config_name_in varchar(255)
)
BEGIN
	declare fnd int;
	set fnd = (select count(*) from system where config_name = config_name_in);

	if (fnd>0) then

	update system set node_id = node_id_in, working_mode = working_mode_in, networking_flags=networking_flags_in,
			database_version = database_version_in, stream_id1 = stream_id1_in, last_startup_result = last_startup_result_in,
			stream_id2 = stream_id2_in,stream_id3 = stream_id3_in,stream_id4 = stream_id4_in
			where config_name = config_name_in;

	else

	insert into system ( node_id, working_mode, networking_flags,database_version,stream_id1,stream_id2,stream_id3,stream_id4,config_name)
	values (node_id_in, working_mode_in, networking_flags_in,database_version_in, stream_id1_in, stream_id2_in,stream_id3_in,stream_id4_in,config_name_in);

	end if;
END
END_OF
done

for i in bitmrc bitmrc_test; do
echo "DB $i :: save_address"
mysql -u root --password=$1 <<END_OF
DELIMITER $$;
use $i ;

DROP PROCEDURE IF EXISTS save_address;
CREATE DEFINER='root'@'localhost' PROCEDURE save_address (
	in label_in varchar(255),
	in address_in varchar(255),
	in version_in int,
	in stream_id_in int,
	in last_request_in datetime)
BEGIN
	declare fnd int;
	set fnd = (select count(*) from addressbook where address = address_in);

	if (fnd>0) then

	update addressbook set label = label_in, last_request = last_request_in
	where address = address_in;

	else

		insert into addressbook (label,address,version,stream,last_request)
			values (label_in,address_in,version_in,stream_id_in,last_request_in);
	end if;
END
END_OF
done

for i in bitmrc bitmrc_test; do
echo "DB $i :: save_pubkey"
mysql -u root --password=$1 <<END_OF
DELIMITER $$;
use $i ;
DROP PROCEDURE IF EXISTS save_pubkey;
CREATE DEFINER='root'@'localhost' PROCEDURE save_pubkey (
	in address_in varchar(255),in address_version_in int,
	in sign_key_in varchar(255), in enc_key_in varchar(255),
	in usedpersonally_in int)
BEGIN
	declare fnd int;
	set fnd = (select count(*) from pubkeys where address = address_in);

	if (fnd>0) then

	update pubkeys set signkey = sign_key_in,enckey=enc_key_in,time = now(), usedpersonally = usedpersonally_in
	where address = address_in;

	else

		insert into pubkeys (address,addressversion,signkey,enckey,time,usedpersonally)
			values (address_in, address_version_in, sign_key_in, enc_key_in, now(), usedpersonally_in);
	end if;
END
END_OF
done

for i in bitmrc bitmrc_test; do
echo "DB $i :: save_address_and_pubkey"
mysql -u root --password=$1 <<END_OF
use $i ;
DELIMITER $$;
DROP PROCEDURE IF EXISTS save_address_and_pubkey;
CREATE DEFINER='root'@'localhost' PROCEDURE save_address_and_pubkey (
	in label_in varchar(255),
	in address_in varchar(255),
	in address_version_in int,
	in stream_in int,
	in date_in datetime,
	in sign_key_in varchar(255),
	in enc_key_in varchar(255),
	in usedpersonally_in int)
BEGIN
	call save_address(label_in, address_in, address_version_in, stream_in,now());
	call save_pubkey(address_in, address_version_in, sign_key_in, enc_key_in, usedpersonally_in);
END
END_OF

done
