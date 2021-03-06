#!/usr/bin/env python3

""" Compiles the ipsys source code.

Usage:
  {this_script} [options]

Options:
  -h   --help       Prints this docstring and halts.
  -l   --launch     Executes the bin if compiled, with what follows as args.
  -d   --debug      Standard debuging build, defines DEBUG, launches with -d.
  -c=X --compiler=X Use the X compiler, where X is gcc or clang.
  --mac             Build on mac.
  --sdl2-static     Statically link to the SDL2, default is dynamic.
  --use-glew        Use the GLEW OpenGL extention loader.
  --opengl-notifs   Enables OpenGL notifications.

Example usage for debug:
  {this_script} -d --compiler=gcc -l
"""

import sys
import os
import re

def print_blue(*args, **kwargs):
	print("\x1b[36m", end = "")
	print(*args, **kwargs)
	print("\x1b[39m", end = "", flush = True)

def print_error(error, *args, **kwargs):
	print("\x1b[1m", end = "")
	print(f"\x1b[31m{error} error:\x1b[39m ", end = "")
	print(*args, **kwargs)
	print("\x1b[22m", end = "", flush = True)

## Options

# Launch option -l
option_launch = False
for i in range(1, len(sys.argv)):
	if sys.argv[i] in ("-l", "--launch"):
		option_launch = True
		launch_index = i
		break
if option_launch:
	options = sys.argv[1:launch_index]
	launch_args = sys.argv[launch_index+1:]
else:
	options = sys.argv[1:]

def cmdline_has_option(expects_value, *option_names):
	global options
	# Find option
	found, value = None, None
	for option in options:
		option_name, *option_value = option.split("=", 1)
		if option_name in option_names:
			if expects_value and len(option_value) == 0:
				print_error("Cmdline",
					f"Argument option \"{option_name}\" expects a value.")
			elif not expects_value and len(option_value) == 1:
				print_error("Cmdline",
					f"Argument option \"{option_name}\" " +
					"doesn't expect a value.")
			found = option
			value = option_value[0] if len(option_value) == 1 else True
			break
	if found == None:
		return None if expects_value else False
	# Check for duplicates
	options.remove(found)
	for option in options:
		option_name, *_ = option.split("=", 1)
		if option_name in option_names:
			print_error("Cmdline",
				f"Argument option \"{option_name}\" is redundant.")
			options.remove(option)
	return value

option_help = cmdline_has_option(False, "-h", "--help")
option_debug = cmdline_has_option(False, "-d", "--debug")
option_compiler = cmdline_has_option(True, "-c", "--compiler")
if option_compiler == None:
	option_compiler = "gcc"
if option_compiler not in ("gcc", "clang"):
	print_error("Cmdline", f"The \"{option_compiler}\" compiler " +
		"is not supported.")
	sys.exit(-1)
if option_debug:
	print(f"Using compiler {option_compiler}")
option_mac = cmdline_has_option(False, "--mac")
option_sdl2_static = cmdline_has_option(False, "--sdl2-static")
option_use_glew = cmdline_has_option(False, "--use-glew")
option_opengl_notifs = cmdline_has_option(False, "--opengl-notifs")

for unknown_option in options:
	print_error("Cmdline", f"Unknown argument option \"{unknown_option}\".")

src_dir_name = "src"
bin_dir_name = "bin"
bin_name = "ipsys"

# Help message if -h
if option_help:
	this_script = sys.argv[0]
	python = "" if this_script.startswith("./") else "python3 "
	print(__doc__.strip().format(this_script = python + sys.argv[0]))
	sys.exit(0)

## Embed content

embedded_header_file_name = "embedded.h" # See this file for some explanations
embedded_source_file_name = "embedded.c" # This one will be overwritten
embed_re = r"EMBEDDED\s*\(\s*\"([^\"]+)\"\s*\)\s*([^\s][^;]+[^\s])\s*;"

def escaped_file_content(filepath):
	if option_debug:
		print(f"Embed file \"{filepath}\"")
	try:
		with open(filepath, "r") as file:
			# Perform the string escaping
			return file.read().translate({
				ord("\""): "\\\"", ord("\\"): "\\\\",
				ord("\n"): "\\n", ord("\t"): "\\t"})
	except FileNotFoundError as error:
		print_error("Embedded file",
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

## Compile

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
build_command_args = [option_compiler]
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
build_command_args.append("-Wno-unused-result")
if option_compiler == "gcc":
	build_command_args.append("-Wno-maybe-uninitialized")
if option_debug:
	build_command_args.append("-DDEBUG")
	build_command_args.append("-g")
else:
	build_command_args.append("-O3")
	build_command_args.append("-no-pie")
	build_command_args.append("-fno-stack-protector")
	build_command_args.append("-flto")
	build_command_args.append("-s")
if option_opengl_notifs:
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
if option_sdl2_static:
	build_command_args.append("`sdl2-config --cflags`")
	build_command_args.append("-static")
	build_command_args.append("-Wl,-Bstatic")
	build_command_args.append("`sdl2-config --static-libs`")
	build_command_args.append("-Wl,-Bdynamic")
	# TODO: At least confirm that it works, which requiers to statically
	# link to a lot of libraries..
else:
	build_command_args.append("`sdl2-config --cflags --libs`")
build_command_args.append("-lm")
build_command = " ".join(build_command_args)
print(("DEBUG" if option_debug else "RELEASE") + " BUILD")
print_blue(build_command)
build_exit_status = os.system(build_command)

## Launch

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
	if launch_exit_status != 0:
		print_blue(f"exit status {launch_exit_status}")
	if bin_dir_name != ".":
		os.chdir("..")
