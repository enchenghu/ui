-- Delete all tables
DROP TABLE IF EXISTS register_bit_info;
DROP TABLE IF EXISTS combined_register;
DROP TABLE IF EXISTS module_base_addr;
DROP TABLE IF EXISTS camera;
DROP TABLE IF EXISTS module;
DROP TABLE IF EXISTS register;
DROP TABLE IF EXISTS permission;

-- Create tables
CREATE TABLE permission (
    permission_name TEXT PRIMARY KEY
);
CREATE TABLE register (
    address INTEGER PRIMARY KEY,
    display_name TEXT NOT NULL,
    rw TEXT NOT NULL,
    default_value INTEGER NOT NULL DEFAULT 0,
    register_description TEXT NOT NULL,
    permission_name TEXT NOT NULL,
    FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
);
CREATE TABLE module (
    module_name TEXT PRIMARY KEY,
    module_description TEXT NOT NULL,
    permission_name TEXT NOT NULL,
    FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
);
CREATE TABLE camera (
    camera_id INTEGER PRIMARY KEY,
    permission_name TEXT NOT NULL,
    FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
);
CREATE TABLE module_base_addr (
    module_name TEXT,
    camera_id INTEGER,
    base_addr INTEGER NOT NULL,
    PRIMARY KEY (module_name, camera_id),
    FOREIGN KEY (module_name) REFERENCES module (module_name),
    FOREIGN KEY (camera_id) REFERENCES camera (camera_id)
);
CREATE TABLE combined_register (
    combined_id INTEGER PRIMARY KEY AUTOINCREMENT,
    module_name TEXT,
    camera_id INTEGER,
    combined_name TEXT NOT NULL,
    display_name TEXT NOT NULL,
    combined_description TEXT NOT NULL,
    permission_name TEXT,
    FOREIGN KEY (module_name) REFERENCES module (module_name),
    FOREIGN KEY (camera_id) REFERENCES camera (camera_id),
    FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
);
CREATE TABLE register_bit_info (
    register_addr INTEGER,
    bit_mask INTEGER,
    register_bit_name TEXT NOT NULL,
    combined_id INTEGER,
    combined_bit_mask INTEGER NOT NULL DEFAULT 0,
    rw TEXT NOT NULL,
    default_value INTEGER NOT NULL DEFAULT 0,
    register_bit_description TEXT NOT NULL,
    permission_name TEXT,
    PRIMARY KEY (register_addr, bit_mask),
    FOREIGN KEY (register_addr) REFERENCES register (address),
    FOREIGN KEY (combined_id) REFERENCES combined_register (combined_id),
    FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
);