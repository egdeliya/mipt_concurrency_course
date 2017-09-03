add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49607;

SELECT area, gender, COUNT(gender) as hits
FROM user_data INNER JOIN ip_data ON user_data.ip = ip_data.ip 
GROUP BY area, gender 
LIMIT 100 
