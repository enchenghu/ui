import os
import csv

HW_MODULE_SPECIAL = ['RAWL', 'RAWM', 'RAWS', 'RAWV']


def read_csv_files(directory: str, delimiter: str = ',') -> dict[str, list[list[str]]]:
    csv_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(".csv"):
                csv_files.append(os.path.join(root, file))

    result = {}
    for file in csv_files:
        tmp = []
        with open(file, 'r') as csv_file:
            reader = csv.reader(csv_file, delimiter=delimiter)
            for row in reader:
                tmp.append(row)
        result[file] = tmp
    return result


def extract_bit_mask(bit_mask: str, parameters: dict[str, int]) -> int:
    # [7:4] -> 0xF0 [0] -> 0x01
    bit_mask = bit_mask.replace("[", "").replace("]", "")
    bit_mask = bit_mask.split(":")
    if len(bit_mask) == 1:
        return 1 << calc_int_value(bit_mask[0], parameters)
    else:
        result = 0
        for i in range(calc_int_value(bit_mask[1], parameters), calc_int_value(bit_mask[0], parameters) + 1):
            result += 1 << i
        return result


def count_1(bit_mask: int) -> int:
    # count continuous 1
    count = 0
    while bit_mask != 0:
        bit_mask = bit_mask & (bit_mask << 1)
        count += 1
    return count


def check_hex(value: str) -> bool:
    try:
        int(value, 16)
        return True
    except ValueError:
        return False


def calc_int_value(value: str, parameters: dict[str, int], base: int = 10) -> int:
    # value maybe like "0x1" or "0b1" or "1" or "parameter_name" or "parameter_name-1" or "parameter_name-1-2"
    # parameters is a dict, like {"a": 1, "b": 2}
    # return int value
    if value.isdigit():
        return int(value, base)
    elif value.startswith("0x") or check_hex(value):
        return int(value, 16)
    elif value.startswith("0b"):
        return int(value, 2)
    else:
        tmp = value.split("-")
        if tmp[0] not in parameters:
            print("Parameter {} not found".format(tmp[0]))
            exit(1)
        result = parameters[tmp[0]]
        for i in range(1, len(tmp)):
            if tmp[i].isdigit():
                result -= int(tmp[i])
            else:
                print("Unknown value {}".format(tmp[i]))
        return result


def extract_name(name: str, bit_mask: int, parameters: dict[str, int], ) -> tuple[str, int]:
    # name maybe like "name[7:4]" or "name" or "name[0]"
    # if name is "name[7:4]", and bit_mask has 4 continuous 1, then return ("name", 0xF0), else return ("name", 0)
    # if name is "name[0]", and bit_mask has 1, then return ("name", 0x01), else return ("name", 0)
    # name may contain "[", "]"
    # extract last [ and ] first
    idx = name.rfind("[")
    tmp_name = name
    tmp_bit_mask = 0
    if idx != -1:
        tmp_bit_mask = extract_bit_mask(name[idx:], parameters)
        tmp_name = name[:idx]
    if count_1(bit_mask) == count_1(tmp_bit_mask):
        return tmp_name, tmp_bit_mask
    else:
        return name, 0


def analyze_csv_files(directory: str) -> dict[int, list[dict[str, None]]]:
    csv_list = read_csv_files(directory, delimiter=";")
    result = {}
    for file_name, file in csv_list.items():
        base_addr = 0
        base_sram_addr = dict()
        addr_bits = 0
        parameters = {}
        module_id = ""
        header_finished = False
        now_addr = 0
        sp_name = ""
        for sp in HW_MODULE_SPECIAL:
            if file_name.find(sp) != -1:
                sp_name = sp
                break
        for row in file:
            if header_finished:
                if len(row) >= 6:
                    if row[0] == "end":
                        continue
                    elif row[0] != "":
                        if int(row[0], 16) + base_addr not in result:
                            result[int(row[0], 16) + base_addr] = []
                        else:
                            print("Address {} already exist".format(
                                int(row[0], 16) + base_addr))
                        now_addr = int(row[0], 16) + base_addr
                        result[now_addr].append({})
                        result[now_addr][-1]["addr_bits"] = addr_bits
                        result[now_addr][-1]["base_addr"] = base_addr
                        result[now_addr][-1]["module_name"] = module_id if sp_name == "" else module_id + "_" + sp_name
                        result[now_addr][-1]["RW"] = row[2]
                        result[now_addr][-1]["bits"] = calc_int_value(
                            row[3], parameters)
                        result[now_addr][-1]["default"] = calc_int_value(
                            row[4], parameters, 16)
                        result[now_addr][-1]["description"] = row[5]
                    else:
                        if row[5].startswith("s_"):
                            tmp = result[now_addr].pop()
                            if result[now_addr] == []:
                                del result[now_addr]
                            tmpname = row[5][:row[5].find("[")]
                            if tmpname not in base_sram_addr:
                                now_addr = base_sram_addr[""] + now_addr - base_addr
                            else:
                                now_addr = base_sram_addr[tmpname] + now_addr - base_addr
                            if now_addr not in result:
                                result[now_addr] = []
                            print("sram addr: {}".format(hex(now_addr)))
                            result[now_addr].append(tmp)
                        if "sub_bits" not in result[now_addr][-1]:
                            result[now_addr][-1]["sub_bits"] = []
                        result[now_addr][-1]["sub_bits"].append({
                            "original_name": row[5] if sp_name == "" else sp_name + "_" + row[5],
                            "RW": result[now_addr][-1]["RW"] if row[2] == "" else row[2],
                            "bit_mask": extract_bit_mask(row[3], parameters),
                            "default": result[now_addr][-1]["default"] & extract_bit_mask(row[3], parameters) if row[
                                                                                                                     4] == "" or
                                                                                                                 row[
                                                                                                                     4] == "A" or
                                                                                                                 row[
                                                                                                                     4] == "M" else int(
                                row[4], 16),
                        })
                        result[now_addr][-1]["sub_bits"][-1]["name"], result[now_addr][-1]["sub_bits"][-1][
                            "target_bit_mask"] = extract_name(
                            row[5], result[now_addr][-1]["sub_bits"][-1]["bit_mask"], parameters)
                        if result[now_addr][-1]["sub_bits"][-1]["target_bit_mask"] == 0:
                            bit_mask = (1 << count_1(
                                result[now_addr][-1]["sub_bits"][-1]["bit_mask"])) - 1
                            result[now_addr][-1]["sub_bits"][-1]["target_bit_mask"] = bit_mask

            elif row[0] == "sub_addr":
                header_finished = True
            else:
                for i in range(len(row)):
                    if row[i] == "base_addr":
                        base_addr = int(row[i + 1], 16)
                    elif row[i] == "addr_bits":
                        addr_bits = int(row[i + 1])
                    elif row[i] == "parameter":
                        parameter = row[i + 1].split("=")
                        parameters[parameter[0]] = int(parameter[1])
                    elif row[i] == "module_name":
                        module_id = row[i + 1]
                    elif row[i] == "base_sram_addr":
                        base_sram_addr[""] = int(row[i + 1], 16)
                    elif row[i].startswith("base_sram_addr"):
                        base_sram_addr[row[i].split(
                            ":")[1]] = int(row[i + 1], 16)
    return result


# CREATE TABLE permission (
#     permission_name TEXT PRIMARY KEY
# );
# CREATE TABLE register (
#     address INTEGER PRIMARY KEY,
#     display_name TEXT NOT NULL,
#     rw TEXT NOT NULL,
#     default_value INTEGER NOT NULL DEFAULT 0,
#     register_description TEXT NOT NULL,
#     permission_name TEXT NOT NULL,
#     FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
# );
# CREATE TABLE module (
#     module_name TEXT PRIMARY KEY,
#     module_description TEXT NOT NULL,
#     permission_name TEXT NOT NULL,
#     FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
# );
# CREATE TABLE module_base_addr (
#     module_name TEXT,
#     camera_id INTEGER,
#     base_addr INTEGER NOT NULL,
#     PRIMARY KEY (module_name, camera_id),
#     FOREIGN KEY (module_name) REFERENCES module (module_name),
#     FOREIGN KEY (camera_id) REFERENCES camera (camera_id)
# );
# CREATE TABLE camera (
#     camera_id INTEGER PRIMARY KEY,
#     permission_name TEXT NOT NULL,
#     FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
# );
# CREATE TABLE combined_register (
#     combined_id INTEGER PRIMARY KEY AUTOINCREMENT,
#     module_name TEXT,
#     camera_id INTEGER,
#     combined_name TEXT NOT NULL,
#     display_name TEXT NOT NULL,
#     combined_description TEXT NOT NULL,
#     permission_name TEXT,
#     FOREIGN KEY (module_name) REFERENCES module (module_name),
#     FOREIGN KEY (camera_id) REFERENCES camera (camera_id),
#     FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
# );
# CREATE TABLE register_bits (
#     register_addr INTEGER,
#     bit_mask INTEGER,
#     register_bits_name TEXT NOT NULL,
#     combined_id INTEGER,
#     combined_bit_mask INTEGER NOT NULL DEFAULT 0,
#     rw TEXT NOT NULL,
#     default_value INTEGER NOT NULL DEFAULT 0,
#     register_bits_description TEXT NOT NULL,
#     permission_name TEXT,
#     PRIMARY KEY (register_addr, bit_mask),
#     FOREIGN KEY (register_addr) REFERENCES register (address),
#     FOREIGN KEY (combined_id) REFERENCES combined_register (combined_id),
#     FOREIGN KEY (permission_name) REFERENCES permission (permission_name)
# );


def generate_insert_sql(res: dict[int, list[dict[str, None]]]) -> list[str]:
    register_sql = []
    module_sql = []
    module_base_addr_sql = []
    camera_sql = []
    combined_register_sql = []
    register_bits_sql = []
    permission_sql = ["INSERT INTO permission VALUES ('Administrator');"]
    for addr in res:
        addr_bit_mask = 0
        for x in res[addr][-1]["sub_bits"]:
            addr_bit_mask |= x["bit_mask"]
        register_sql.append(
            f"({addr}, '{res[addr][-1]['sub_bits'][0]['name']}', '{res[addr][-1]['RW']}', {res[addr][-1]['default']}, '{res[addr][-1]['description']}', 'Administrator'),")
        for sub_addr in res[addr][-1]["sub_bits"]:
            mask_len = int((res[addr][-1]["bits"] / 8 + 1) // 2 * 2)
            module_name = res[addr][-1]["module_name"]
            camera_id = -1
            if res[addr][-1]["module_name"].find("cam") == 0:
                module_name = res[addr][-1]["module_name"][5:]
                camera_id = int(res[addr][-1]["module_name"][3])
            module_sql.append(
                f"('{module_name}', '', 'Administrator'),")
            if (camera_id != -1):
                camera_sql.append(
                    f"({camera_id}, 'Administrator'),")
            tmp_camera_id = camera_id if camera_id != -1 else "NULL"
            module_base_addr_sql.append(
                f"('{module_name}', {tmp_camera_id}, {res[addr][-1]['base_addr']}),")
            combined_register_sql.append(
                f"('{module_name}', {tmp_camera_id}, '{sub_addr['name']}', '{sub_addr['name']}', '', 'Administrator'),")
    combined_register_sql = list(set(combined_register_sql))
    combined_register_sql.sort()
    for addr in res:
        module_name = res[addr][-1]["module_name"]
        camera_id = -1
        if res[addr][-1]["module_name"].find("cam") == 0:
            module_name = res[addr][-1]["module_name"][5:]
            camera_id = int(res[addr][-1]["module_name"][3])
        tmp_camera_id = str(camera_id) if camera_id != -1 else "NULL"
        for sub_addr in res[addr][-1]["sub_bits"]:
            combined_id = 0
            for i in range(len(combined_register_sql)):
                c_module_name = combined_register_sql[i].split(",")[0].strip()[2:-1]
                c_camera_id = combined_register_sql[i].split(",")[1].strip()
                c_name = combined_register_sql[i].split(",")[2].strip()[1:-1]
                if c_name == sub_addr["name"] and c_module_name == module_name and c_camera_id == tmp_camera_id:
                    combined_id = i + 1
                    break
            mask_len = int((res[addr][-1]["bits"] / 8 + 1) // 2 * 2)
            register_bits_sql.append(
                f"({addr}, {sub_addr['bit_mask']}, '{sub_addr['unique_name']}', {combined_id}, {sub_addr['target_bit_mask']}, '{sub_addr['RW']}', {sub_addr['default']}, '', 'Administrator'),")

    # make unique
    register_sql = list(set(register_sql))
    register_sql.sort()
    module_sql = list(set(module_sql))
    module_sql.sort()
    module_base_addr_sql = list(set(module_base_addr_sql))
    module_base_addr_sql.sort()
    camera_sql = list(set(camera_sql))
    camera_sql.sort()
    register_bits_sql = list(set(register_bits_sql))
    tmp_reg_bits_sql = []
    reg_bit_set = set()
    for row in register_bits_sql:
        # pick up register_addr and bit_mask
        tmp_row = row[row.find("(") + 1:row.find(")")]
        tmp = tmp_row.split(",")
        tmp = tuple(tmp[0:2])
        if tmp not in reg_bit_set:
            reg_bit_set.add(tmp)
            tmp_reg_bits_sql.append(row)
        else:
            print("Duplicate register_bits: {}".format(row))
    register_bits_sql = tmp_reg_bits_sql
    register_bits_sql.sort()
    permission_sql = list(set(permission_sql))

    register_sql[-1] = register_sql[-1][:-1] + ";"
    module_sql[-1] = module_sql[-1][:-1] + ";"
    module_base_addr_sql[-1] = module_base_addr_sql[-1][:-1] + ";"
    camera_sql[-1] = camera_sql[-1][:-1] + ";"
    combined_register_sql[-1] = combined_register_sql[-1][:-1] + ";"
    register_bits_sql[-1] = register_bits_sql[-1][:-1] + ";"

    register_sql.insert(0, "INSERT INTO register VALUES ")
    module_sql.insert(0, "INSERT INTO module VALUES ")
    module_base_addr_sql.insert(0, "INSERT INTO module_base_addr VALUES ")
    camera_sql.insert(0, "INSERT INTO camera VALUES ")
    combined_register_sql.insert(
        0,
        "INSERT INTO combined_register (module_name, camera_id, combined_name, display_name, combined_description, permission_name) VALUES ")
    register_bits_sql.insert(0, "INSERT INTO register_bit_info VALUES ")

    trun_sql = ["DELETE FROM register_bit_info;", "DELETE FROM combined_register;", "DELETE FROM register;",
                "DELETE FROM module_base_addr;", "DELETE FROM module;", "DELETE FROM camera;",
                "DELETE FROM permission;", "DELETE FROM `sqlite_sequence` WHERE `name` = 'combined_register';"
                ]
    return trun_sql + permission_sql + register_sql + module_sql + camera_sql + module_base_addr_sql + combined_register_sql + register_bits_sql


def load_all_csv(all_csv_path: str) -> dict[int, list[dict[str, None]]]:
    result = {}
    with open(all_csv_path, 'r') as csv_file:
        for raw_row in csv_file:
            row = raw_row.strip().split(",")
            if row[0] == "module_id":
                continue
            addr = int(row[4], 16)
            if addr not in result:
                result[addr] = []
            append_new = True
            append_sub_bits_idx = -1
            for i in range(len(result[addr])):
                if result[addr][i]["module_name"] == row[0] and \
                        (result[addr][i]["RW"] == row[5] or True) and \
                        (result[addr][i]["bits"] == int(row[6])) and \
                        result[addr][i]["default"] == int(row[8], 16) and \
                        result[addr][i]["description"] == row[9]:
                    sub_bits_dup = False
                    for sub_bits in result[addr][i]["sub_bits"]:
                        if sub_bits["bit_mask"] ^ int(row[15], 16) == 0:
                            sub_bits_dup = True
                    if sub_bits_dup:
                        continue
                    append_new = False
                    append_sub_bits_idx = i
                    break
            if append_new:
                result[addr].append({
                    "module_name": row[0],
                    "base_addr": int(row[3], 16),
                    "RW": row[5],
                    "bits": int(row[6]),
                    "default": int(row[8], 16),
                    "description": row[9],
                    "sub_bits": []
                })
            result[addr][append_sub_bits_idx]["sub_bits"].append({
                "original_name": row[11],
                "RW": row[14],
                "bit_mask": int(row[15], 16),
                "default": int(row[16], 16),
                "name": row[12],
                "target_bit_mask": int(row[13], 16),
                "unique_name": row[10]
            })
    return result


if __name__ == '__main__':
    print("1. Generate insert.sql from M1_CSV")
    print("2. Generate insert.sql from M1_all_new.csv")
    print("Please input your choice: ", end="")
    choice = input()
    if choice == "1":
        res = analyze_csv_files("M1_CSV")
        res_file = "M1_all.csv"
        with open(res_file, 'w') as csv_file:
            writer = csv.writer(csv_file, delimiter=',', lineterminator="\n")
            writer.writerow(
                ["module_id", "module_name", "camera", "base_addr", "address", "RW", "bits", "bit_mask", "default_all",
                 "description", "unique_name", "origin_name", "combine_name", "target_bit_mask", "RW",
                 "origin_bit_mask", "default"])
            for addr in res:
                for i in range(len(res[addr])):
                    addr_bit_mask = 0
                    for x in res[addr][i]["sub_bits"]:
                        addr_bit_mask |= x["bit_mask"]
                    for sub_addr in res[addr][i]["sub_bits"]:
                        mask_len = int((res[addr][i]["bits"] / 8 + 1) // 2 * 2)
                        module_name = res[addr][i]["module_name"]
                        camera_id = -1
                        sub_addr["unique_name"] = res[addr][i]["module_name"] + \
                                                  '_' + sub_addr['original_name']
                        if res[addr][i]["module_name"].find("cam") == 0:
                            module_name = res[addr][i]["module_name"][5:]
                            camera_id = int(res[addr][i]["module_name"][3])
                        writer.writerow([
                            res[addr][i]["module_name"],
                            module_name,
                            camera_id if camera_id != -1 else "",
                            f"0x{res[addr][i]['base_addr']:08X}",
                            f"0x{addr:08X}",
                            res[addr][i]["RW"],
                            res[addr][i]["bits"],
                            f"0x{addr_bit_mask:0{mask_len}X}",
                            f"0x{res[addr][i]['default']:0{mask_len}X}",
                            res[addr][i]["description"],
                            sub_addr["unique_name"],
                            sub_addr["original_name"],
                            sub_addr["name"],
                            f"0x{sub_addr['target_bit_mask']:08X}",
                            sub_addr["RW"],
                            f"0x{sub_addr['bit_mask']:0{mask_len}X}",
                            f"0x{sub_addr['default']:0{mask_len}X}"
                        ])
        sql = generate_insert_sql(res)
        with open("insert.sql", 'w') as sql_file:
            for line in sql:
                sql_file.write(line + "\n")
    elif choice == "2":
        res = load_all_csv("M1_all_new.csv")
        sql = generate_insert_sql(res)
        with open("insert.sql", 'w') as sql_file:
            for line in sql:
                sql_file.write(line + "\n")
