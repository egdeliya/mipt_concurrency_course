add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49607;

SELECT date, COUNT(url) AS number
FROM user_logs
GROUP BY (date) 
ORDER BY (number)
