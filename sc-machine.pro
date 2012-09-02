TEMPLATE = subdirs

CONFIG += ordered
DESTDIR = ./bin

SUBDIRS = sc-store \
          sc-store/test \
          tools/sc-store-visual
