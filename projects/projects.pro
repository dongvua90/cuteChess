CONFIG += ordered

TEMPLATE = subdirs
SUBDIRS = lib gui

cli.depends = lib
gui.depends = lib
