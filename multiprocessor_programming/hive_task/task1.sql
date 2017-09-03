add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
--add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49607;

DROP TABLE IF EXISTS user_logs;

CREATE EXTERNAL TABLE user_logs(
    ip STRING,
    date STRING,
    url STRING,
    pagesize STRING,
    statuscode STRING,
    browser STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES(
    "input.regex" = '^(\\S*)\\t{3}(\\d{8})\\S*\\t(\\S*)\\t(\\d{1,})\\t(\\d{1,})\\t(\\S*).*$',
    "output.format.string" = "%1$s %2$s %3$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_logs_S';

SELECT * FROM user_logs;

DROP TABLE IF EXISTS user_data;

CREATE EXTERNAL TABLE user_data(
    ip STRING,
    browser STRING,
    gender STRING,
    age STRING 
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES(
    "input.regex" = '^(\\S*)\\t(\\S*)\\t(\\S*)\\t(\\d{1,}).*$',
    "output.format.string" = "%1$s %2$s %3$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_data';

SELECT * FROM user_data LIMIT 50;

DROP TABLE IF EXISTS ip_data;

CREATE EXTERNAL TABLE ip_data(
    ip STRING,
    area STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES(
    "input.regex" = '^(\\S*)\\t(\\S*).*$',
    "output.format.string" = "%1$s %2$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/ip_data';

SELECT * FROM ip_data LIMIT 50;

