#!/usr/bin/env python3

import json
import xmltodict

inFile = 'cppcheck_result.xml'
outFile = 'cppcheck_result.json'

with open(inFile, "r") as fd:
    data = xmltodict.parse(fd.read(), process_namespaces=True)
    fd.close()

with open(outFile, "w") as fd:
    fd.write(json.dumps(data))
    fd.close()
