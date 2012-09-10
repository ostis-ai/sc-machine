TEMPLATE = subdirs

CONFIG += ordered
DESTDIR = ./bin

SUBDIRS = sc-memory \
          sc-memory/test \
          tools/sc-store-visual
