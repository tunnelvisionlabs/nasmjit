#!/usr/bin/env python

# =============================================================================
# [generate-defs.py]
#
# The purpose of this script is to fetch all instruction names into single
# string which won't cause huge reallocation by the linker. The script is 
# included for pure optimization purposes (decrease binary size and count of
# relocation).
# =============================================================================

# =============================================================================
# [Configuration]
# =============================================================================

# Files to process.
FILES = [
  { "file": "../AsmJit/X86/X86Defs.cpp", "arch": "x86" }
]

# =============================================================================
# [Imports]
# =============================================================================

import os, re, string

# =============================================================================
# [Helpers]
# =============================================================================

def readFile(fileName):
  handle = open(fileName, "rb")
  data = handle.read()
  handle.close()
  return data

def writeFile(fileName, data):
  handle = open(fileName, "wb")
  handle.truncate()
  handle.write(data)
  handle.close()

# =============================================================================
# [Main]
# =============================================================================

def processFile(fileName, arch):
  data = readFile(fileName);
  ARCH = arch.upper();

  din = data
  r = re.compile(arch + r"InstInfo\[\][\s]*=[\s]*{(?P<BLOCK>[^}])*}")
  m = r.search(din)

  if not m:
    print "Couldn't match " + arch + "InstInfo[] in " + fileName
    exit(0)

  din = din[m.start():m.end()]
  dout = ""

  dinst = []
  daddr = []
  hinst = {}

  r = re.compile(r'\"(?P<INST>[A-Za-z0-9_ ]+)\"')
  dpos = 0
  for m in r.finditer(din):
    inst = m.group("INST")
    
    if not inst in hinst:
      dinst.append(inst)
      hinst[inst] = dpos

      daddr.append(dpos)
      dpos += len(inst) + 1

  dout += "const char " + arch + "InstName[] =\n"
  for i in xrange(len(dinst)):
    dout += "  \"" + dinst[i] + "\\0\"\n"
  dout += "  ;\n"

  dout += "\n"

  for i in xrange(len(dinst)):
    dout += "#define INST_" + dinst[i].upper().replace(" ", "_") + "_INDEX" + " " + str(daddr[i]) + "\n"

  mb_string = "// ${" + ARCH + "_INST_DATA:BEGIN}\n"
  me_string = "// ${" + ARCH + "_INST_DATA:END}\n"

  mb = data.index(mb_string)
  me = data.index(me_string)

  data = data[:mb + len(mb_string)] + dout + data[me:]

  writeFile(fileName, data)

for item in FILES:
  processFile(item["file"], item["arch"])
