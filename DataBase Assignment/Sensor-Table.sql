SHOW DATABASES;

USE u551180265_ZellaWaltman;

-- Sensor Register:
CREATE TABLE sensor_register(
    node_name VARCHAR(10) NOT NULL,
    manufacturer VARCHAR(10) NOT NULL,
    longitude DECIMAL(15, 12) NOT NULL CHECK (longitude BETWEEN -180 AND 180), -- 15 digits, 12 after decimal
    latitude DECIMAL(15, 12) NOT NULL CHECK (latitude BETWEEN -90 AND 90), -- 15 digits, 12 after decimal
    PRIMARY KEY (node_name)
    );
 
-- Sensor Data: 
CREATE TABLE sensor_data(
    node_name VARCHAR(10) NOT NULL,
    time_received DATETIME NOT NULL,
    temperature DECIMAL(6, 3) NOT NULL CHECK (temperature BETWEEN -10 AND 100), -- Must be between -10 and 100
    humidity DECIMAL(6,3) NOT NULL CHECK (humidity BETWEEN 0 AND 100), -- Must be between 0 and 100
    FOREIGN KEY (node_name) REFERENCES sensor_register(node_name) -- Linked with Sensor Register Node Name. Cannot insert data for a node that does not exist in sensor_register table.
	);
   
-- Register 5 Different Sensors W/ Manufacturer Names
INSERT INTO sensor_register (node_name, manufacturer, longitude, latitude) VALUES
('node_1', 'TI', -96.796989, 32.776665),  -- Dallas, TX
('node_2', 'Bosch', 9.120415, 48.521932),  -- Kusterdingen, Germany
('node_3', 'Honeywell',   -80.841141, 35.223789),  -- Charlotte, NC
('node_4', 'SICK AG',   -73.989308,  40.741895),  -- Waldkirch, Germany
('node_5', 'Initech',   6.117887, 46.1683113);  -- Plan-les-Ouates, Switzerland

INSERT INTO sensor_data (node_name, time_received, temperature, humidity) VALUES

-- Node 1 
('node_1', '2022-10-01 11:00:00', 78.52, 10.02),
('node_1', '2022-10-01 11:30:00', 51.48, 55.20),
('node_1', '2022-10-01 12:00:00', 23.33, 67.23),
('node_1', '2022-10-01 12:30:00', 21.72, 23.21),

-- Node 2
('node_2', '2022-10-01 11:00:00', -3.52, 45.69),
('node_2', '2022-10-01 11:30:00', 5.08, 76.26),
('node_2', '2022-10-01 12:00:00', -9.24, 52.19),
('node_2', '2022-10-01 12:30:00', 4.93, 46.74),

-- Node 3
('node_3', '2022-10-01 11:00:00', 56.78, 18.29),
('node_3', '2022-10-01 11:30:00', 67.45, 54.04),
('node_3', '2022-10-01 12:00:00', 41.50, 64.37),
('node_3', '2022-10-01 12:30:00', 62.34, 35.93),

-- Node 4
('node_4', '2022-10-01 11:00:00', 22.51, 92.20),
('node_4', '2022-10-01 11:30:00', 34.26, 23.45),
('node_4', '2022-10-01 12:00:00', -2.12, 85.34),
('node_4', '2022-10-01 12:30:00', 99.98, 53.67),

-- Node 5
('node_5', '2022-10-01 11:00:00', 21.50, 67.92),
('node_5', '2022-10-01 11:30:00', 45.67, 43.82),
('node_5', '2022-10-01 12:00:00', 44.35, 38.19),
('node_5', '2022-10-01 12:30:00', 72.68, 55.28);

-- Merge data w/ register:
CREATE OR REPLACE VIEW sensor_combined AS
SELECT
  sensor_data.node_name,
  sensor_data.time_received,
  sensor_data.temperature,
  sensor_data.humidity,
  sensor_register.manufacturer,
  sensor_register.longitude,
  sensor_register.latitude
FROM sensor_data, sensor_register
WHERE
	sensor_register.node_name = sensor_data.node_name; -- Ensures that no data from the sensor_data table is accepted if the transmitting node is not registered in the sensor_register table by only keeping rows where node_name matches.

SELECT * FROM sensor_combined; -- Excecute join where node_names are equal
