#!/usr/bin/env python

import glob
import os

CPP_TEMPLATE = "SourceModuleTemplate.cpp.temp"
H_TEMPLATE = "SourceModuleTemplate.h.temp"

class SourceData:
    name = None
    ucname = None
    lcname = None
    ucfname = None
    source = None

    def __dir__(self):
        return ['name', 'ucname', 'lcname', 'ucfname', 'source']

def render(data, templ, out):
    th = open(templ, "r")
    template = th.read()
    th.close()

    for att in dir(data):
        template = template.replace("@@" + att + "@@", getattr(data, att))

    out = open(out, "w")
    out.write(template)
    out.close()

    return template

def generate(data, dest):
    baseName = os.path.join(dest, data.ucfname + "Module")
    render(data, CPP_TEMPLATE, baseName + ".cpp")
    render(data, H_TEMPLATE, baseName + ".h")

files = glob.glob("*.ms")

datas = []

for file in files:
    fh = open(file, "r")
    data = SourceData()
    data.name = os.path.splitext(file)[0]
    data.ucname = data.name.upper()
    data.lcname = data.name.lower()
    data.ucfname = data.name.lower().capitalize()
    data.source = fh.read()
    fh.close()

    datas.append(data)

for data in datas:
    generate(data, "../")

