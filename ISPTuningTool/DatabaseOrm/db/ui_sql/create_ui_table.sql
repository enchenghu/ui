-- After create data table, you can use this sql to create ui table.

-- Delete tables
DROP TABLE IF EXISTS ui_info;
DROP TABLE IF EXISTS ui_module;
DROP TABLE IF EXISTS ui_info_regmodule;

-- Create tables
CREATE TABLE ui_module (
    module_name TEXT PRIMARY KEY,
    permission_name TEXT NOT NULL,
    FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
);
CREATE TABLE ui_info (
    module_name TEXT,
    control_name TEXT,
    control_index INTEGER,
    combined_name TEXT,
    register_address INTEGER,
    permission_name TEXT NOT NULL,
    PRIMARY KEY (module_name, control_name, control_index),
    FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
);
CREATE TABLE ui_info_regmodule (
ui_module_name TEXT NOT NULL,
ui_control_name TEXT NOT NULL,
reg_module_name TEXT NOT NULL,
has_camera_id INTEGER NOT NULL,
FOREIGN key (reg_module_name) REFERENCES module (module_name)
);
