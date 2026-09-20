#!/usr/bin/env python3
"""Generates assets/pack_icon.png (original artwork: moonlit foggy forest)."""
import math, os, random
from PIL import Image, ImageDraw, ImageFilter

S = 256
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
random.seed(7)

# sky gradient (navy -> teal-purple haze at the horizon)
img = Image.new("RGB", (S, S))
px = img.load()
for y in range(S):
    t = y / (S - 1)
    top = (8, 12, 32); mid = (36, 40, 84); low = (88, 84, 128)
    if t < 0.6:
        k = t / 0.6; c = tuple(int(top[i] + (mid[i]-top[i])*k) for i in range(3))
    else:
        k = (t - 0.6) / 0.4; c = tuple(int(mid[i] + (low[i]-mid[i])*k) for i in range(3))
    for x in range(S):
        px[x, y] = c

# stars
d = ImageDraw.Draw(img)
for _ in range(70):
    x, y = random.randrange(S), random.randrange(int(S*0.55))
    b = random.randrange(120, 235)
    d.point((x, y), fill=(b, b, min(255, b+20)))

# moon with soft glow
glow = Image.new("RGB", (S, S), (0, 0, 0))
gd = ImageDraw.Draw(glow)
mx, my, mr = 170, 78, 30
gd.ellipse((mx-mr*2.2, my-mr*2.2, mx+mr*2.2, my+mr*2.2), fill=(70, 80, 130))
glow = glow.filter(ImageFilter.GaussianBlur(24))
img = Image.blend(img, Image.composite(glow, img, glow.convert("L")), 0.0)
img = Image.fromarray(__import__("numpy").clip(__import__("numpy").asarray(img, dtype=int) + __import__("numpy").asarray(glow, dtype=int), 0, 255).astype("uint8"))
d = ImageDraw.Draw(img)
d.ellipse((mx-mr, my-mr, mx+mr, my+mr), fill=(214, 224, 244))
for cx, cy, cr in ((mx-9, my-6, 7), (mx+10, my+8, 5), (mx+2, my-14, 4)):
    d.ellipse((cx-cr, cy-cr, cx+cr, cy+cr), fill=(190, 202, 228))

def fog(img, y0, alpha, col=(120, 120, 165)):
    layer = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    ld = ImageDraw.Draw(layer)
    ld.rectangle((0, y0, S, S), fill=col + (alpha,))
    layer = layer.filter(ImageFilter.GaussianBlur(10))
    img.paste(layer, (0, 0), layer)

def hills(color, base, amp, seed):
    random.seed(seed)
    ph = [random.random()*6.28 for _ in range(3)]
    pts = [(0, S)]
    for x in range(0, S+1, 4):
        y = base + amp*(math.sin(x/38+ph[0]) + 0.5*math.sin(x/17+ph[1]) + 0.25*math.sin(x/7+ph[2]))
        pts.append((x, y))
    pts.append((S, S))
    ImageDraw.Draw(img).polygon(pts, fill=color)

def pines(color, y_base, n, h, seed):
    random.seed(seed)
    dd = ImageDraw.Draw(img)
    for _ in range(n):
        x = random.randrange(-6, S+6); hh = h*random.uniform(0.7, 1.3); w = hh*0.33
        for i in range(4):
            k = i/4
            yt = y_base - hh + hh*0.75*k
            dd.polygon([(x, yt), (x - w*(0.45+0.55*(k+0.25)), yt + hh*0.32), (x + w*(0.45+0.55*(k+0.25)), yt + hh*0.32)], fill=color)
        dd.rectangle((x-1, y_base - hh*0.1, x+1, y_base+4), fill=color)

hills((44, 46, 88), 168, 10, 1)
fog(img, 150, 90)
hills((28, 32, 66), 190, 8, 2); pines((26, 30, 62), 200, 16, 44, 3)
fog(img, 178, 110, (96, 100, 150))
hills((14, 18, 40), 214, 6, 4); pines((12, 16, 36), 226, 12, 66, 5)
fog(img, 206, 120, (70, 72, 118))
d = ImageDraw.Draw(img)
d.rectangle((0, 238, S, S), fill=(8, 10, 24))

# warm torch light near the bottom (dark fantasy accent)
warm = Image.new("RGB", (S, S), (0, 0, 0))
wd = ImageDraw.Draw(warm)
wd.ellipse((30, 212, 74, 256), fill=(150, 78, 20))
warm = warm.filter(ImageFilter.GaussianBlur(14))
import numpy as np
img = Image.fromarray(np.clip(np.asarray(img, dtype=int) + np.asarray(warm, dtype=int), 0, 255).astype("uint8"))
d = ImageDraw.Draw(img)
d.rectangle((51, 226, 53, 244), fill=(74, 50, 30))
d.ellipse((49, 219, 55, 228), fill=(255, 190, 70)); d.ellipse((51, 221, 53, 226), fill=(255, 240, 170))

img.save(os.path.join(ROOT, "assets", "pack_icon.png"))
print("icon written", img.size)
