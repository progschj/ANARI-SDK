# Copyright 2021 The Khronos Group
# SPDX-License-Identifier: Apache-2.0

import sys
import os
import json
import hash_gen
import merge_anari
import argparse
import pathlib

class ReferenceGenerator:
    def __init__(self, anari):
        self.anari = anari
        self.indent = "   "

    def format(self, x, name = ""):
        stringified = ""
        if isinstance(x, list):
            stringified = ", ".join(x)
        else:
            stringified = str(x)

        if name in ["name", "types", "elementType", "sourceFeature"]:
            return "`%s`"%stringified
        else:
            return stringified

    def generate_table(self, t, fields):
        code = "[cols=\"" + ",".join("1" for x in fields) + "\"]\n"
        code += "|===\n"
        code += " ".join("|%s"%x.title() for x in fields) + "\n\n"
        for row in t:
            code += " ".join(("|%s"%self.format(row[x], x) if x in row else "| ") for x in fields) + "\n"
        code += "|===\n"
        return code

    def generate_type_table(self, t):
        fields = ["name", "type"]
        code = "[cols=\"" + ",".join("1" for x in fields) + "\"]\n"
        code += "|===\n"
        code += " ".join("|%s"%x.title() for x in fields) + "\n\n"
        for row in t:
            code += "|`%s`\n"%row["name"]
            if row["elements"]>1:
                code += "|`%s[%d]`\n"%(row["baseType"], row["elements"])
            else:
                code += "|`%s`\n"%row["baseType"]
        code += "|===\n"
        return code

    def generate_param_table(self, t):
        fields = ["name", "types", "misc", "description", "feature"]
        code = "[cols=\"" + ",".join("1" for x in fields) + "\"]\n"
        code += "|===\n"
        code += " ".join("|%s"%x.title() for x in fields) + "\n\n"
        for row in t:
            code += "|`%s`\n"%row["name"]
            code += "|`%s`\n"%(", ".join(row["types"]))
            code += "|\n"
            if "required" in row["tags"]:
                code += "required\n"
            if "default" in row:
                code += "default = %s\n"%str(row["default"])
            code += "\n"
            code += "|%s\n"%row["description"]
            code += "|`%s`\n"%row["sourceFeature"]
            #code += " ".join(("|%s"%self.format(row[x], x) if x in row else "| ") for x in fields) + "\n"
        code += "|===\n"
        return code

    def generate_type_reference(self):
        code = "== Types\n"
        for v in self.anari["enums"]:
            code += "=== `%s`\n"%v["name"]
            if v["name"] == "ANARIDataType":
                code += self.generate_type_table(v["values"])
            else:
                code += self.generate_table(v["values"], ["name"])
        for v in self.anari["opaqueTypes"]:
            code += "=== `%s`\n"%v["name"]
            code += "Opaque %s handle\n\n"%v["name"][5:]
        for v in self.anari["structs"]:
            code += "=== `%s`\n"%v["name"]
            code += "[source,cpp]\n----\n"
            code += "struct " + v["name"] + " {\n"
            code += "".join([self.indent + "%s %s;\n"%(a["type"], a["name"]) for a in v["members"]])
            code += "};\n"
            code += "----\n"
        for v in self.anari["functionTypedefs"]:
            code += "=== `%s`\n"%v["name"]
            code += "[source,cpp]\n----\n"
            code += "typedef " + v["returnType"] + " (*" + v["name"] + ") (\n"
            code += ",\n".join([self.indent + "%s %s"%(a["type"], a["name"]) for a in v["arguments"]])
            code += ");\n"
            code += "----\n"
        return code

    def generate_function_reference(self):
        code = "== Functions\n"
        for v in self.anari["functions"]:
            code += "=== `%s`\n"%v["name"]
            code += "[source,cpp]\n----\n"
            code += v["returnType"] + " " + v["name"] + "(\n"
            code += ",\n".join([self.indent + "%s %s"%(a["type"], a["name"]) for a in v["arguments"]])
            code += ");\n"
            code += "----\n"
        return code

    def generate_object_reference(self):
        code = "== Objects\n"
        types = sorted(list({v["type"] for v in self.anari["objects"]}))
        for t in types:
            code += "=== `%s`\n"%t
            for v in self.anari["objects"]:
                if v["type"] == t:
                    if "name" in v:
                        code += "==== %s\n"%v["name"]
                    if "sourceFeature" in v:
                        code += "Feature: `%s`\n\n"%v["sourceFeature"]
                    code += "Description: " + v["description"] + "\n"
                    code += self.generate_param_table(v["parameters"])
                    code += "\n"

        return code

parser = argparse.ArgumentParser(description="Generate query functions for an ANARI device.")
parser.add_argument("-d", "--device", dest="devicespec", type=open, help="The device json file.")
parser.add_argument("-j", "--json", dest="json", type=pathlib.Path, action="append", help="Path to the core and extension json root.")
parser.add_argument("-o", "--output", dest="outdir", type=pathlib.Path, default=pathlib.Path("."), help="Output directory")
parser.add_argument("--includefile", dest="include", action="store_true", help="Generate a header file")
args = parser.parse_args()


#flattened list of all input jsons in supplied directories
jsons = [entry for j in args.json for entry in j.glob("**/*.json")]

#load the device root
device = json.load(args.devicespec)
merge_anari.tag_feature(device)
print("opened " + device["info"]["type"] + " " + device["info"]["name"])

dependencies = merge_anari.crawl_dependencies(device, jsons)
#merge all dependencies
for x in dependencies:
    matches = [p for p in jsons if p.stem == x]
    for m in matches:
        feature = json.load(open(m))
        merge_anari.tag_feature(feature)
        merge_anari.merge(device, feature)

#generate files
gen = ReferenceGenerator(device)

with open(args.outdir/"anariReference.txt", mode='w') as f:
    
    f.write("= ANARI Reference\n")
    f.write(":toc: left\n")
    f.write(":toclevels: 3\n\n")
    f.write(gen.generate_type_reference())
    f.write(gen.generate_function_reference())
    f.write(gen.generate_object_reference())
