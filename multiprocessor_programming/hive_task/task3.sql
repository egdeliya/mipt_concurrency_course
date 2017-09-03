add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49607;

SELECT areas.area
FROM    (
		SELECT area, COUNT(ip) as num 
		FROM ip_data
		GROUP BY area
	) as areas,
	(
		SELECT COUNT(*)/COUNT(DISTINCT area) as av
		FROM ip_data
	) as avg_num
WHERE areas.num > avg_num.av
