-- After create data table, you can use this sql to create ui table.

-- Delete tables
DROP TABLE IF EXISTS awb_calibration_register;

-- Create tables
CREATE TABLE awb_calibration_register (
    awb_register_code INTEGER,
    module_name TEXT,
    combined_name TEXT,
    PRIMARY KEY (awb_register_code)
);
