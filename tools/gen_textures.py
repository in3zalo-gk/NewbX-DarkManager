#!/usr/bin/env python3
"""Generates the original torch / redstone textures (16x16). Alpha 252 = full glow, see functions/glow.h."""
import os
from PIL import Image

OUT = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "assets", "textures", "blocks")
G = 252  # glow alpha

def img():
    return Image.new("RGBA", (16, 16), (0, 0, 0, 0))

def stick(im, rows=range(8, 16)):
    for y in rows:
        k = (y - 8) % 3
        im.putpixel((7, y), (112 - 6*k, 78 - 5*k, 44 - 3*k, 255))
        im.putpixel((8, y), (88 - 6*k, 60 - 4*k, 34 - 3*k, 255))
    im.putpixel((7, 8), (60, 42, 28, 255)); im.putpixel((8, 8), (48, 34, 24, 255))  # charred top

def torch(name, core, mid, edge, tip):
    im = img()
    stick(im)
    im.putpixel((7, 6), core + (G,)); im.putpixel((8, 6), mid + (G,))
    im.putpixel((7, 7), mid + (G,));  im.putpixel((8, 7), edge + (G,))
    im.putpixel((7, 5), tip + (G,)); im.putpixel((8, 5), core + (G,))   # spare pixels (icon), model uses rows 6-7
    im.save(os.path.join(OUT, name))

def redstone_torch(name):
    im = img()
    stick(im, range(9, 16))
    cells = {(7,5):(255,130,110),(8,5):(240,60,55),
             (6,6):(200,25,30),(7,6):(255,110,95),(8,6):(250,70,60),(9,6):(190,20,28),
             (6,7):(170,15,25),(7,7):(240,50,50),(8,7):(230,40,45),(9,7):(160,12,22),
             (7,8):(200,25,30),(8,8):(150,12,20)}
    for (x, y), c in cells.items():
        im.putpixel((x, y), c + (G,))
    im.save(os.path.join(OUT, name))

def redstone_lamp_on(name):
    im = Image.new("RGBA", (16, 16), (0, 0, 0, 255))
    frame = [(122, 64, 34), (98, 50, 28), (80, 40, 24)]
    for y in range(16):
        for x in range(16):
            border = x in (0, 15) or y in (0, 15)
            cross = x in (7, 8) or y in (7, 8)
            if border or cross:
                im.putpixel((x, y), frame[(x*3 + y*5) % 3] + (255,))
            else:
                # each 6x6 cell: hot center, amber edge
                cx = (x - 1) % 8 if x < 7 else (x - 9) % 8
                cy = (y - 1) % 8 if y < 7 else (y - 9) % 8
                d = max(abs(cx - 2.5), abs(cy - 2.5)) / 2.5
                r = int(255 - 25*d); g = int(214 - 70*d); b = int(120 - 70*d)
                im.putpixel((x, y), (r, g, max(b, 30), G))
    im.save(os.path.join(OUT, name))

os.makedirs(OUT, exist_ok=True)
torch("torch_on.png",     (255, 236, 150), (255, 190, 70), (240, 120, 30), (255, 246, 200))
torch("soul_torch.png",   (215, 252, 255), (110, 225, 255), (55, 160, 235), (235, 255, 255))
redstone_torch("redstone_torch_on.png")
redstone_lamp_on("redstone_lamp_on.png")
print("textures written to", OUT)
