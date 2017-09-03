add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE velkerr;

SELECT TRANSFORM(ip, date, url, pagesize, statuscode, browser) 
USING 'ch_script.sh' // your script
AS ... // to be continued
