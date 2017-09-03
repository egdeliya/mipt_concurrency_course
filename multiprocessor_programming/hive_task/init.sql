add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

drop database if exists s49607 cascade;
create database s49607 location '/user/s49607/metastore';
