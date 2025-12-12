#!/usr/bin/python3
import os
import sys
import json
import subprocess
import re
import shutil
import time
import os
import logging
import argparse
from pathlib import Path
from datetime import datetime
from collections import OrderedDict

def system_call(command, get_stdout=True):
    if get_stdout:
        process = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
        stdout = process.communicate()[0].decode(
            "ascii", errors="ignore").splitlines()
        er = process.returncode
    else:
        er = os.system(command)
        stdout = ""
    return er, stdout


def parse_args(argv):
    parser = argparse.ArgumentParser(description='parse build')
    parser.add_argument('-j', '--json', metavar='[.json]', required=True,
                        dest='json', action='store',
                        help='load json configure file')
    args = parser.parse_args()
    return args


def initiate(argv):
    tm = datetime.now()
    # parse options
    args = parse_args(argv)
    # load json cfg
    json_cfg = None
    with open(args.json, encoding="utf-8", errors='ignore') as file:
        cfg = json.load(file, object_pairs_hook=OrderedDict)
        if cfg["jsontype"] != "paerse_build":
            logging.error("this json file is not for this application.")
            return None
    return cfg


def modify_targetmk(model, cfg):
    path_target_mk = "./Config/Target.mk"
   
    # modify Target.mk
    text = ""
    try:
        file = open(path_target_mk, encoding="utf-8")
    except FileNotFoundError:
        logging.error("unable to open {}".format(path_target_mk))
        return -1
    for line in file:
        for key, value in model["target"].items():
            line = re.sub('{}\\s*=\\s*[^\\s]*'.format(key),
                          '{} = {}'.format(key, value), line)
        text += line
    if len(text):
        with open(path_target_mk, 'w') as fout:
            fout.write(text.encode("ascii", errors="ignore").decode(
                "ascii", errors="ignore"))  # fix cp950 error
    return 0


def modify_lv_conf(model, cfg):
    path_lv_conf_h = r"./Config/lv_conf.h"

    # modify bl_func.h
    text = ""
    try:
        file = open(path_lv_conf_h, encoding="utf-8")
    except FileNotFoundError:
        logging.error("unable to open {}".format(path_lv_conf_h))
        return -1
    for line in file:
        for key, value in model["lv_conf"].items():
            line = re.sub(
                '#define\\s*{}\\s*[^\\s]*'.format(key), '#define {} {}'.format(key, value), line)
        text += line
    if len(text):
        with open(path_lv_conf_h, 'w') as fout:
            fout.write(text.encode("ascii", errors="ignore").decode(
                "ascii", errors="ignore"))  # fix cp950 error
    return 0

def modify_autoconf(model, cfg):
    path_auto_conf_h = r"./Config/autoconf.h"

    # modify bl_func.h
    text = ""
    try:
        file = open(path_auto_conf_h, encoding="utf-8")
    except FileNotFoundError:
        logging.error("unable to open {}".format(path_auto_conf_h))
        return -1
    for line in file:
        for key, value in model["autoconf"].items():
            line = re.sub(
                '#define\\s*{}\\s*[^\\s]*'.format(key), '#define {} {}'.format(key, value), line)
        text += line
    if len(text):
        with open(path_auto_conf_h, 'w') as fout:
            fout.write(text.encode("ascii", errors="ignore").decode(
                "ascii", errors="ignore"))  # fix cp950 error
    return 0

def make_clean(model, cfg):
    cmd = ' make clean '
    er, _ = system_call(cmd, False)
    return er

def make_all(model, cfg):
    cmd = ' make Media '
    er, _ = system_call(cmd, False)
    return er

def copy_result(model, output_dir):
    output_dir = os.path.join(output_dir, model["name"])
    cmd = \
        "mkdir -p {} && ".format(output_dir) + \
        "mv output/* {} -f;".format(output_dir)
    er, _ = system_call(cmd, False)
    return er

def main(argv):
    cfg = initiate(argv)
    if cfg is None:
        return -1
  
    # create autobuild output folder
    output_dir = cfg["output_dir"]
    er, _ = system_call('rm -rf ' + output_dir, False)
    if er != 0:
            return -1
    
    er, _ = system_call('mkdir -p ' + output_dir, False)
    if er != 0:
            return -1
   
    for model in cfg["models"]:
        system_call('echo -e "============================================================================================"', False)
        system_call('echo -e "Current Model: {}"'.format(model["name"]), False)
        system_call('echo -e "============================================================================================"', False)
        # to guarantee code is clean for built-NG before
        if make_clean(model, cfg) != 0:
            return -1
        if modify_targetmk(model, cfg) != 0:
            return -1
        if modify_lv_conf(model, cfg) != 0:
            return -1
        if make_all(model, cfg) != 0:
            return -1
        if copy_result(model, output_dir) != 0:
            return -1
        if make_clean(model, cfg) != 0:
            return -1

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
