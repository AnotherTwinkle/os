import json
from datetime import datetime
import functools
import json
import sys
import os

current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

with open('src/assets.json') as f:
	data = json.load(f)

RENDER_FLAGS = {
	"hover" : 0x1,
	"anim"  : 0x2
}

h_prefix = """
#ifndef LEVELS_H
#define LEVELS_H

#include "kernel/util.h"
#include "graphics/pomelo.h"
#include "sprites.h"
#include "level.h"
"""

h_def = "extern struct Level {name};";

h_suffix = "#endif"

c_prefix = """#include "levels.h"
"""

def empty_array(w, h, d = 0):
	return [d for i in range(w*h)]


def get_level_data(ppath, entry):

	if not os.path.exists(os.path.join(ppath, 'sprites', f'{entry['name']}.png')):
		print(f"Python: Warning. Level spritesheet for  {entry['name']} does not exist. Stop.")
		sys.exit(1)

	with open(os.path.join(ppath, 'levels', f'{entry["name"]}.json')) as f:
		ldata = json.load(f)

	if (entry['tile_size'] != ldata['tileSize'] or entry['width_t'] != ldata['mapWidth'] or entry['height_t'] != ldata['mapHeight']):
		print(f"Python : Warning. Level metadata does not match for {entry['name']}. Stop.")
		sys.exit(1)


	w = entry['width_t']
	h = entry['height_t']

	layer_count = len(ldata['layers'])
	rendering_layers = []
	rendering_flags = []
	colliders = empty_array(w, h)

	ldata['layers'].reverse()

	for layer in ldata['layers']:
		name, *flags  = layer["name"].split('|')
		flags = [RENDER_FLAGS[flag] for flag in flags]
		flags = functools.reduce(lambda x, y : x | y, flags) if flags else 0
		rendering_flags.append(flags)

		layer_data = empty_array(w, h, -1)
		for tile in layer['tiles']:
			x = tile['x']
			y = tile['y']
			sprite = int(tile['id'])

			layer_data[x*h + y] = sprite

			if layer['collider']:
				colliders[x*h + y] = 1

		rendering_layers.append(layer_data)

	name = entry['name']

	LEVELS_H_CHUNK = h_def.format(name= name)
	LEVELS_C_CHUNK = f"""
{'\n\n'.join([f's16 {name}_layer_{i}[] = {{{', '.join(str(j) for j in rendering_layers[i])}}};' for i in range(layer_count)])}

s16 *{name}_layers[] = {{{', '.join([f'{name}_layer_{i}' for i in range(layer_count)])}}};

u16 {name}_flags[] = {{{', '.join([str(i) for i in rendering_flags])}}};

u8 {name}_colliders[] = {{{', '.join([str(i) for i in colliders])}}};

struct Level {name} =  {{
	.width_t = {w},
	.height_t = {h},
	.layer_count = {layer_count},
	.spritesheet = &{name}_sprites,
	.rendering_layers = {name}_layers,
	.rendering_flags = {name}_flags,
	.colliders = {name}_colliders
}};
	"""
	# print(LEVELS_C_CHUNK)

	return LEVELS_H_CHUNK, LEVELS_C_CHUNK

def main():
	for program in data['programs']:
		h_chunks = []
		c_chunks = []
		for level in program['levels']:
			ret = get_level_data(program['path'], level)
			h_chunks.append(ret[0])
			c_chunks.append(ret[1])

		H_CONTENT = f"{h_prefix}\n{'\n'.join([chunk for chunk in h_chunks])}\n\n{h_suffix}"

		with open(os.path.join(program['path'], 'levels.h'), 'w') as f:
			f.write(H_CONTENT)

		C_CONTENT = f"{c_prefix}\n{'\n'.join([chunk for chunk in c_chunks])}"

		with open(os.path.join(program['path'], 'levels.c'), 'w') as f:
			f.write(C_CONTENT)

	get_level_data('src/programs/cats/', data['programs'][0]['levels'][0])

if __name__ == "__main__":
	main()