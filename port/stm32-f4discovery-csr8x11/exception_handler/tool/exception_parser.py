#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
import re
import binascii
import os

reg_dump_file_name = "temp/dump_reg_cmds_cm4.txt"
mem_dump_file_name = "temp/dump_mem_cmds_cm4.txt"
reg_resotore_cmm_file_name = "temp/restoreRegContext.cmm"
load_binary_file_name = "temp/restoreMemContext.cmm"

register_list = []
regist_spec_name_tuple = ('lr','xpsr','MSP','PSP','sp','pc','basepri','primask','faultmask','CONTROL')

mem_dump_region_list = []
mem_dump_pre = "temp/mem_cm4_"
mem_dump_post = ".bin"

def addOneRegisterValue(name , value):
    global regist_spec_name_tuple
    global register_list
    global reg_restore_cmm_file

    reg_name_matched = re.match('(r\d+)',name)
    if reg_name_matched:
        register_list.append("set $" + name + '=' + value)
        content_line = "r.s" + " " + name + " " + value + "\n"
        reg_restore_cmm_file.write(content_line)
        return

    for spec_name in regist_spec_name_tuple:
        if spec_name == name:
            register_list.append("set $" + name + '=' + value)
            content_line = "r.s" + " " + name + " " + value + "\n"
            reg_restore_cmm_file.write(content_line)
            return

#set $ar0=0x6000d4f0
def genRegDumpCmdTxt(reg_list):
    global reg_dump_file_name

    DUMP_FILE = open(reg_dump_file_name, "w")
    for reg in reg_list:
        DUMP_FILE.write(reg)
        DUMP_FILE.write("\n")
    DUMP_FILE.close()

#restore memdump.bin  binary 0x63fffc00
def genMEMDumpCmdTxt(addr_list):

    global mem_dump_pre
    global mem_dump_post
    global mem_dump_file_name

    #print addr_list
    DUMP_FILE = open(mem_dump_file_name, "w")
    for addr in addr_list:
        mem_file_n = mem_dump_pre + str(addr_list.index(addr)) + mem_dump_post
        content_line = "restore " + mem_file_n + " binary begin with " + addr + "\n"
        DUMP_FILE.write(content_line)

    DUMP_FILE.close()

#python script start

if len(sys.argv) != 2:
    print("Usage: python {} <input_file>".format(sys.argv[0]))
    sys.exit(1)

input_file = sys.argv[1]
print "Parsing crash log：" + input_file
EXCEP_FILE = open(input_file,"r")
excep_data = EXCEP_FILE.read()
excep_array = excep_data.split("\n",-1)
#parse register data
#[16:23:03]reg r1 = 0x10149297
is_begin_for_register = 0
last_mem_addre = 0
dumpdata_file = None
reg_restore_cmm_file = open(reg_resotore_cmm_file_name, "w")
load_binary_file = open(load_binary_file_name, "w")

for line in excep_array:
    if is_begin_for_register == 1:
        is_end = re.match(r'.*Register dump end\:',line)
        if is_end:
            is_begin_for_register = 0
            genRegDumpCmdTxt(register_list)
            #print register_list
            continue

        matched = re.match(r'.*?([a-zA-Z]\w+) += +(0x[a-zA-Z0-9]+)', line)
        if matched:
            #print "matched : " + matched.group()
            #print "matched 1 : " + matched.group(1)
            #print "matched 2 : " + matched.group(2)
            addOneRegisterValue(matched.group(1), matched.group(2))
        continue

    if is_begin_for_register==0:
        is_begin = re.match(r'.*Register dump begin',line)
        if is_begin:
            is_begin_for_register = 1
            continue

    data_matched = re.match(r'.*?(0x[a-zA-Z0-9]+): +([a-zA-Z0-9]+) +([a-zA-Z0-9]+) +([a-zA-Z0-9]+) +([a-zA-Z0-9]+)',line)

    if data_matched:
        #print "matched Data : " + data_matched.group(1) + " " + data_matched.group(2) + " " + data_matched.group(3) \
        #      + " " + data_matched.group(4) + " " + data_matched.group(5)

        cur_addr = int(data_matched.group(1)[2:10], 16)
        #print "cur_addr = 0x%x , last_mem_addre = 0x%x, str = %s" % (cur_addr,last_mem_addre, data_matched.group(1)[2:10])
        #生成.bin文件
        if last_mem_addre == 0:
            dumpfile_name = "{}{:08x}{}".format(mem_dump_pre, cur_addr, mem_dump_post)
            #dumpfile_name = mem_dump_pre + '0' + mem_dump_post
            #dumpfile_name = mem_dump_pre + hex(cur_addr) + mem_dump_post
            if (os.path.exists(dumpfile_name)):
                os.remove(dumpfile_name)
            dumpdata_file = open(dumpfile_name, "wb")
            mem_dump_region_list.append(data_matched.group(1))
            # Data.LOAD.BINARY &binary_path/temp/dump_mem_cm4_080424d0.bin 0x080424d0 /noclear
            content_line = "Data.Load.Binary" + " " + "&binary_path/" + dumpfile_name.replace("temp/", "") + " " + hex(cur_addr) + " /noclear" + "\n"
            load_binary_file.write(content_line)
        else:
            #第n段memory region,生成新的.bin文件
            if (cur_addr - last_mem_addre != 16):
                dumpdata_file.close()
                dumpfile_name = "{}{:08x}{}".format(mem_dump_pre, cur_addr, mem_dump_post)
                #dumpfile_name = mem_dump_pre + str(mem_dump_region_list.__len__()) + mem_dump_post
                #dumpfile_name = mem_dump_pre + hex(cur_addr) + mem_dump_post
                if (os.path.exists(dumpfile_name)):
                    os.remove(dumpfile_name)
                dumpdata_file = open(dumpfile_name, "wb")
                mem_dump_region_list.append(data_matched.group(1))
                # Data.LOAD.BINARY &binary_path/temp/dump_mem_cm4_080424d0.bin 0x080424d0 /noclear
                content_line = "Data.Load.Binary" + " " + "&binary_path/" + dumpfile_name.replace("temp/", "") + " " + hex(cur_addr) + " /noclear" + "\n"
                load_binary_file.write(content_line)
            #elif (cur_addr - last_mem_addre < 4):
            #    raise Exception("MEM DATA in log file has a fetal error, err id : -10001")

        last_mem_addre = cur_addr

        for i in range(2,data_matched.groups().__len__() + 1):
            #print "matched : " + data_matched.group(i)
            count = 7
            right_value =""
            while (count>0):
                right_value += data_matched.group(i)[count-1] + data_matched.group(i)[count]
                count -= 2
            dumpdata_file.write(binascii.a2b_hex(right_value))
    else:
        pass

if dumpdata_file:
    dumpdata_file.close()

if reg_restore_cmm_file:
    reg_restore_cmm_file.close()

if load_binary_file:
    load_binary_file.close()

print mem_dump_region_list

genMEMDumpCmdTxt(mem_dump_region_list)

EXCEP_FILE.close()
