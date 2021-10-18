#!/usr/bin/env python3

""" Compiles the ipsys source code.

Usage:
  {this_script} [options]

Options:
  -h  --help        Prints this docstring.
  -l  --launch      Executes the bin if compiled, with what follows as args.
  -d  --debug       Standard debuging build, defines DEBUG, launches with -d.
  --use-glew        Use the GLEW OpenGL extention loader.
  --opengl-notifications     Enables OpenGL notifications.

Example usage for debug:
  {this_script} -d -l
"""

import sys
import os
import re

def print_blue(*args, **kwargs):
	print("\x1b[36m", end = "")
	print(*args, **kwargs)
	print("\x1b[39m", end = "", flush = True)

# Launch option -l
if "-l" in sys.argv[1:]:
	option_launch = True
	i = sys.argv[1:].index("-l")
elif "--launch" in sys.argv[1:]:
	option_launch = True
	i = sys.argv[1:].index("--launch")
else:
	option_launch = False
if option_launch:
	options = sys.argv[1:i+1]
	launch_args = sys.argv[i+2:]
else:
	options = sys.argv[1:]

# Options
def cmdline_has_option(*option_names):
	for option_name in option_names:
		if option_name in options:
			return True
	return False
option_help = cmdline_has_option("-h", "--help")
option_debug = cmdline_has_option("-d", "--debug")
option_use_glew = cmdline_has_option("--use-glew")
option_opengl_notifications = cmdline_has_option("--opengl-notifications")
option_mac = cmdline_has_option("--mac")
release_build = not option_debug
src_dir_name = "src"
bin_dir_name = "bin"
bin_name = "ipsys"

# Help message if -h
if option_help:
	this_script = sys.argv[0]
	python = "" if this_script.startswith("./") else "python3 "
	print(__doc__.strip().format(this_script = python + sys.argv[0]))
	sys.exit(0)

# Embed content
embedded_header_file_name = "embedded.h" # See this file for some explanations
embedded_source_file_name = "embedded.c" # This one will be overwritten
embed_re = r"EMBEDDED\s*\(\s*\"([^\"]+)\"\s*\)\s*([^\s][^;]+[^\s])\s*;"
def escaped_file_content(filepath):
	if option_debug:
		print(f"Embed file \"{filepath}\"")
	try:
		with open(filepath, "r") as file:
			return file.read().translate({
				ord("\""): "\\\"", ord("\\"): "\\\\",
				ord("\n"): "\\n", ord("\t"): "\\t"})
	except FileNotFoundError as error:
		print("\x1b[31mEmbedded file error:\x1b[39m " +
			"The embedded content generator couldn't find the file " +
			f"\"{filepath}\" used in an EMBEDDED macro in the " +
			f"\"{embedded_header_file_name}\" header file.")
		raise error
generated_c = []
generated_c.append("")
generated_c.append("/* This file is overwritten at each compilation.")
generated_c.append(f" * Do not modify, see \"{embedded_header_file_name}\" " +
	"or \"_comp.py\" instead. */")
embedded_header_path = os.path.join(src_dir_name, embedded_header_file_name)
with open(embedded_header_path, "r") as embedded_header_file:
	for match in re.finditer(embed_re, embedded_header_file.read()):
		partial_file_path = match.group(1)
		file_path = os.path.join(src_dir_name, partial_file_path)
		escaped_content = escaped_file_content(file_path)
		variable_declaration = match.group(2)
		generated_c.append("")
		generated_c.append(f"/* Content of \"{file_path}\". */")
		generated_c.append(f"{variable_declaration} = \"{escaped_content}\";")
embedded_source_path = os.path.join(src_dir_name, embedded_source_file_name)
with open(embedded_source_path, "w") as embedded_source_file:
	embedded_source_file.write("\n".join(generated_c) + "\n")

# List src files
src_file_names = []
for dir_name, _, file_names in os.walk(src_dir_name):
	for file_name in file_names:
		if file_name.split(".")[-1] == "c":
			src_file_names.append(os.path.join(dir_name, file_name))

# Bin directory
if not os.path.exists(bin_dir_name):
	os.makedirs(bin_dir_name)

# Build
build_command_args = ["gcc"]
for src_file_name in src_file_names:
	build_command_args.append(src_file_name)
build_command_args.append("-o")
build_command_args.append(os.path.join(bin_dir_name, bin_name))
build_command_args.append("-I" + src_dir_name)
if option_mac:
	build_command_args.append("-I$(brew --prefix sdl2)/include")
build_command_args.append("-std=c11")
build_command_args.append("-Wall")
build_command_args.append("-Wextra")
build_command_args.append("-pedantic")
build_command_args.append("-Wno-overlength-strings")
build_command_args.append("-Wno-maybe-uninitialized")
if option_debug:
	build_command_args.append("-DDEBUG")
	build_command_args.append("-g")
if release_build:
	build_command_args.append("-O2")
	build_command_args.append("-fno-stack-protector")
	build_command_args.append("-flto")
	build_command_args.append("-s")
if option_opengl_notifications:
	build_command_args.append("-DENABLE_OPENGL_NOTIFICATIONS")
if option_mac:
	build_command_args.append("-framework OpenGL")
else:
    build_command_args.append("-lGL")
if option_use_glew:
	build_command_args.append("-DGLEW_STATIC") # Doesn't seem to be enough ><
	build_command_args.append("-lGLEW")
	build_command_args.append("-DUSE_GLEW")
	if option_mac:
		build_command_args.append("-I$(brew --prefix glew)/include")
build_command_args.append("`sdl2-config --cflags --libs`") # See the SDL2 doc
build_command_args.append("-lm")
build_command = " ".join(build_command_args)
print(("RELEASE" if release_build else "DEBUG") + " BUILD")
print_blue(build_command)
build_exit_status = os.system(build_command)

# Launch if -l
if option_launch and build_exit_status == 0:
	launch_command_args = ["./" + bin_name]
	if option_debug:
		launch_command_args.append("-d")
	for launch_arg in launch_args:
		launch_command_args.append(launch_arg)
	launch_command = " ".join(launch_command_args)
	os.chdir(bin_dir_name)
	print_blue(launch_command)
	launch_exit_status_raw = os.system(launch_command)
	launch_exit_status = launch_exit_status_raw >> 8
	if bin_dir_name != ".":
		os.chdir("..")
	if launch_exit_status != 0:
		print_blue(f"exit status {launch_exit_status}")
