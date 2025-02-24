#!/disk/usb/ssd01/stable-diffusion-test/venv/bin/python

import os
import sys
import re
import json
import argparse

import subprocess
import urllib.parse

######################################################################
# VARS
APPNAME = "sdgp"

""" @import@ """

PNGH = b"\x89\x50\x4e\x47\x0d\x0a\x1a\x0a"
IHDR = b"\x00\x00\x00\x0d\x49\x48\x44\x52"

def read_exif(f) -> dict:

    e = {}
    t = ''
    w = 0
    h = 0
    d = 0
    p = ''

    try:
        fh = open(f ,'rb')
    except Exception as e:
        e['_ERROR_'] = e
        return e
    # PNG Header
    t = fh.read(8)
    if(t != PNGH):
        fh.close()
        e['_ERROR_'] = 'Invalid PNG Signature'
        return e
    # IHDR Chunk
    t = fh.read(8)
    if(t != IHDR):
        fh.close()
        e['_ERROR_'] = 'Invalid IHDR'
        return e

    e['PNG'] = 1

    # Width
    t = fh.read(4)
    e['width'] = int.from_bytes(t)
    # Height
    t = fh.read(4)
    e['height'] = int.from_bytes(t)
    # Pixel Depth
    t = fh.read(1)
    e['depth'] = int.from_bytes(t)
    # Color Type
    t = fh.read(1)
    e['color'] = int.from_bytes(t)
    # Compression Method
    t = fh.read(1)
    e['compression'] = int.from_bytes(t)
    # Filter Type
    t = fh.read(1)
    e['filter'] = int.from_bytes(t)
    # Interlace
    t = fh.read(1)
    e['interlace'] = int.from_bytes(t)
    # CRC
    t = fh.read(4)
    e['CRC'] = int.from_bytes(t)

    e['parameters_Pos'] = fh.tell()

    # getChunkSize()
    t = fh.read(4)
    # Size of parameters
    e['parameters_Size'] = int.from_bytes(t);

    # check tEXt(when iTXt on Meitu)
    t = fh.read(4)
    if t != b'tEXt' and t != b'iTXt':
        fh.close()
        e['_ERROR_'] = 'tEXt: Nothing tEXt chunk!('+t.decode('utf-8')+')'
        return e

    # parameters
    p = fh.read(e['parameters_Size'])

    if re.match('parameters', p.decode('utf-8')):
        t = re.sub('^parameters.', '', p.decode('utf-8'))
        s = t.split('\n', 2);
        e['prompt'] = s.pop(0)
        e['negativePrompt'] = s.pop(0)
        r = s[0].split(', ')
        for i in range(len(r)):
            k, v = r[i].split(': ', 1)
            e[k] = v
    elif re.match('generation_data', p.decode('utf-8')):
        t = re.sub('^generation_data.', '', p.decode('utf-8'))
        t = re.sub('[^\w]$', '', t)
        mg = json.loads(t)
        for k in mg.keys():
            e[k] = mg[k]
    else:
        fh.close()
        e['_ERROR_'] = 'tEXt: Not a Stable Diffusion Parameters!'
        return e

    # CRC
    t = fh.read(4)
    e['tEXt_CRC'] = int.from_bytes(t)

    fh.close()

    if len(e) == 0:
        e['_ERROR_'] = 'LEN: Not have a length!'
        return e

    return e

######################################################################
# ArgParse

parser = argparse.ArgumentParser()
parser.add_argument('-i', type=str, required=True)
args = parser.parse_args()

path = args.i

path = re.sub('file://', '', path)
path = urllib.parse.unquote(path)

if path.endswith(".png") is False:
    cmnd = ['zenity', '--error', '--window-icon="warning"', f'--text={APPNAME}: Not PNG format.\n{path}.']
    subprocess.call(cmnd)
    exit(1)
if os.path.isfile(path) is False:
    cmnd = ['zenity', '--error', '--window-icon="warning"', f'--text={APPNAME}: Not exists file.\n{path}.']
    subprocess.call(cmnd)
    exit(1)

hako = read_exif(path)

if '_ERROR_' in hako:
    cmnd = ['zenity', '--error', '--window-icon="warning"', f'--text={hako["_ERROR_"]}.']
    subprocess.call(cmnd)
    exit(1)

prmp = hako.pop("prompt").replace("<", "&lt;")
prmp = prmp.replace(">", "&gt;")
prmp = prmp.replace("\x22", "&quot;")
prmp = prmp.replace("|", "\\|")

ngtv = hako.pop("negativePrompt").replace("<", "&lt;")
ngtv = ngtv.replace(">", "&gt;")
ngtv = ngtv.replace("\x22", "&quot;")
ngtv = ngtv.replace("|", "\\|")

hako.pop("PNG")

hall = "" #json.dumps(hako, indent=4)

for k in hako.keys():
    hall = hall + f"{k}: {hako[k]}, "

cmnd = ['zenity', '--info', '--window-icon="info"', f'--text=Prompt: {prmp}\n\n{ngtv}\n\n{hall}']
subprocess.call(cmnd)


