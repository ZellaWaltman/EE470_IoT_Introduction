-- Sensor Register:
DROP TABLE IF EXISTS
    SensorRegister; -- Overwrite old table if it exists
CREATE TABLE SensorRegister(
    node_name VARCHAR(10) NOT NULL UNIQUE,
    manufacturer VARCHAR(10) NOT NULL,
    longitude DECIMAL(10, 7) NOT NULL CHECK
        (longitude BETWEEN -180 AND 180),
        latitude DECIMAL(10, 7) NOT NULL
    CHECK
        (latitude BETWEEN -90 AND 90),
        PRIMARY KEY(node_name)
) ENGINE = InnoDB;

-- Sensor Data:
DROP TABLE IF EXISTS
    SensorData; -- Overwrite old table if it exists
CREATE TABLE SensorData(
    node_name VARCHAR(10) NOT NULL,
    time_received DATETIME NOT NULL,
    temperature DECIMAL(6, 3) NOT NULL CHECK
        (
            temperature BETWEEN -10 AND 100
        ),
        humidity DECIMAL(6, 3) NOT NULL
    CHECK
        (humidity BETWEEN 0 AND 100),
        -- Linked with Sensor Register Node Name. Cannot insert data for a node that does not exist in SensorRegister table.
        FOREIGN KEY(node_name) REFERENCES SensorRegister(node_name)
        -- If node name changes, update everywhere. If there is data associated with a node, the node cannot be deleted in the Sensor Register
        ON UPDATE CASCADE ON DELETE RESTRICT,
        -- No duplicate rows allowed:
        UNIQUE KEY uq_node_time_vals(
            node_name,
            time_received,
            temperature,
            humidity
        ),
        -- Use index to help sort through data seperated by nodes more easily. This will allow the website to run faster when we use a dropdown to sort through node names.
        INDEX ix_node_time(node_name, time_received)
) ENGINE = InnoDB;

-- Sensor Activity:
DROP TABLE IF EXISTS
    SensorActivity; -- Overwrite old table if it exists
CREATE TABLE SensorActivity(
    node_name VARCHAR(10) NOT NULL,
    data_count INT UNSIGNED NOT NULL DEFAULT 0,
    -- Same Rules for node_name as Sensor Data
    PRIMARY KEY(node_name),
    -- used for "ON DUPLICATE KEY UPDATE" 
    FOREIGN KEY(node_name) REFERENCES SensorRegister(node_name) ON UPDATE CASCADE ON DELETE RESTRICT
) ENGINE = InnoDB; DELIMITER
    $$ -- Don't stop reading at first ;, wait until delimeter end

  -- When a new row is inserted into SensorData, execute trigger
CREATE TRIGGER trg_increment_activity AFTER INSERT ON
    SensorData FOR EACH ROW
BEGIN
        -- Increment Sensor Activity count for whichever node has been updated
    INSERT INTO SensorActivity(node_name, data_count)
VALUES(NEW.node_name, 1) -- look for node name of newly inserted data. Initial count is 1 for new nodes.
ON DUPLICATE KEY
UPDATE
    data_count = data_count + 1 ; -- If node doesn't yet exist in the Sensor Activity table, insert a new row with count = 1. Important, because Sensor Activity count begins empty.
    END $$
DELIMITER
    ;

-- Insert Node Data into SensorRegister
INSERT INTO SensorRegister(
    node_name,
    manufacturer,
    longitude,
    latitude
)
VALUES(
    'node_1',
    'man_1',
    -122.6769136,
    38.3367350
),(
    'node_2',
    'man_2',
    -122.6769136,
    38.3367350
);
