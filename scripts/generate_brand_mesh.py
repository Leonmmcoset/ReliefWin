#!/usr/bin/env python3
"""Generate the diagnostic tool's colour mesh from the supplied ReliefWin mark."""

from collections import Counter, defaultdict
from pathlib import Path
import re

from PIL import Image, ImageFilter

ROOT = Path(__file__).resolve().parents[1]


def cross(a, b, c):
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def area(points):
    return sum(a[0] * b[1] - a[1] * b[0]
               for a, b in zip(points, points[1:] + points[:1])) / 2


def simplify(points, tolerance):
    if len(points) < 3:
        return points
    a, b = points[0], points[-1]
    length2 = (b[0] - a[0]) ** 2 + (b[1] - a[1]) ** 2
    distance = []
    for p in points[1:-1]:
        t = max(0, min(1, ((p[0] - a[0]) * (b[0] - a[0]) +
                          (p[1] - a[1]) * (b[1] - a[1])) / length2)) if length2 else 0
        distance.append((p[0] - a[0] - t * (b[0] - a[0])) ** 2 +
                        (p[1] - a[1] - t * (b[1] - a[1])) ** 2)
    index = max(range(len(distance)), key=distance.__getitem__)
    if distance[index] <= tolerance ** 2:
        return [a, b]
    index += 1
    return simplify(points[:index + 1], tolerance)[:-1] + simplify(points[index:], tolerance)


def contours(mask, width, height):
    edges = defaultdict(list)
    for y in range(height):
        for x in range(width):
            if not mask[y * width + x]:
                continue
            if y == 0 or not mask[(y - 1) * width + x]:
                edges[x, y].append((x + 1, y))
            if x == width - 1 or not mask[y * width + x + 1]:
                edges[x + 1, y].append((x + 1, y + 1))
            if y == height - 1 or not mask[(y + 1) * width + x]:
                edges[x + 1, y + 1].append((x, y + 1))
            if x == 0 or not mask[y * width + x - 1]:
                edges[x, y + 1].append((x, y))
    loops = []
    while edges:
        start = next(iter(edges))
        point = start
        loop = []
        while True:
            loop.append(point)
            choices = edges[point]
            next_point = choices.pop()
            if not choices:
                del edges[point]
            point = next_point
            if point == start:
                break
        loops.append(loop)
    return loops


def triangulate(points):
    points = points[:]
    if area(points) < 0:
        points.reverse()
    triangles = []
    while len(points) > 3:
        for index, b in enumerate(points):
            a, c = points[index - 1], points[(index + 1) % len(points)]
            if cross(a, b, c) <= 0:
                continue
            if any(p not in (a, b, c) and cross(a, b, p) >= 0 and
                   cross(b, c, p) >= 0 and cross(c, a, p) >= 0 for p in points):
                continue
            triangles.append((a, b, c))
            del points[index]
            break
        else:
            raise ValueError("The mark has a contour that cannot be triangulated")
    triangles.append(tuple(points))
    return triangles


def main():
    image = Image.open(ROOT / 'media/graphics/branding/reliefwin-mark.png').convert('RGBA')
    width, height = image.size
    pixels = list(image.getdata())
    colors = Counter(p[:3] for p in pixels if p[3] >= 128)
    first = colors.most_common(1)[0][0]
    second = next(color for color, _ in colors.most_common()
                  if sum((a - b) ** 2 for a, b in zip(color, first)) > 1500)
    palette = [first, second]
    labels = [min(range(2), key=lambda i: sum((a - b) ** 2 for a, b in zip(p[:3], palette[i])))
              if p[3] >= 128 else -1 for p in pixels]
    mesh = []
    for index, color in enumerate(palette):
        mask_image = Image.new('L', image.size)
        mask_image.putdata([255 if label == index else 0 for label in labels])
        mask = [value >= 128 for value in mask_image.filter(ImageFilter.MedianFilter(3)).getdata()]
        loops = sorted(contours(mask, width, height), key=lambda points: abs(area(points)), reverse=True)
        for loop in loops[:1]:
            if area(loop) < 0:
                raise ValueError('The mark has a hole that needs a contour mesh with holes')
            pivot = max(range(len(loop)), key=lambda i: (loop[i][0] - loop[0][0]) ** 2 +
                        (loop[i][1] - loop[0][1]) ** 2)
            polygon = (simplify(loop[:pivot + 1], 1.5)[:-1] +
                       simplify(loop[pivot:] + loop[:1], 1.5)[:-1])
            for triangle in triangulate(polygon):
                mesh.append((triangle, color))
    header = '''/*
 * PROJECT:     ReliefWin DirectX Diagnostic Tool
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Mesh generated from the supplied ReliefWin logo
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

'''
    lines = [header, 'static const MARK_TRIANGLE MarkMesh[] =\n{\n']
    for triangle, color in mesh:
        coordinates = ', '.join('{%.6ff, %.6ff}' %
                                ((x - width / 2) * 2.2 / width,
                                 (height / 2 - y) * 2.2 / width) for x, y in triangle)
        lines.append('    {{%s}, 0xff%02x%02x%02x},\n' % (coordinates, *color))
    lines.append('};\n')
    (ROOT / 'base/applications/dxdiag/d3dmark_mesh.h').write_text(''.join(lines))
    path = ROOT / 'base/applications/dxdiag/d3dmark.h'
    source = path.read_text()
    source, count = re.subn(r'(#define D3DTEST_MARK_VERTEX_COUNT )\d+',
                           lambda match: match[1] + str(len(mesh) * 3), source)
    if count != 1:
        raise ValueError('The diagnostic mesh capacity declaration was not found')
    path.write_text(source)
    print('ReliefWin diagnostic mark: %d triangles, %d vertices, colours %r' %
          (len(mesh), len(mesh) * 3, palette))


if __name__ == '__main__':
    main()
