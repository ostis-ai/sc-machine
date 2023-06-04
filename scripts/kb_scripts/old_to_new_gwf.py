import sys
import os
from xml.dom.minidom import parse

dictionary = {
    "node/-/not_define": "node/-/-/not_define",
    "node/var/symmetry": "node/var/perm/tuple",
    "node/const/general_node": "node/const/perm/general",
    "node/const/relation": "node/const/perm/relation",
    "node/const/attribute": "node/const/perm/role",
    "node/const/nopredmet": "node/const/perm/struct",
    "node/const/material": "node/const/perm/terminal",
    "node/const/asymmetry": "node/const/perm/tuple",
    "node/var/general_node": "node/var/perm/general",
    "node/var/relation": "node/var/perm/relation",
    "node/var/attribute": "node/var/perm/role",
    "node/var/nopredmet": "node/var/perm/struct",
    "node/var/material": "node/var/perm/terminal",
    "node/const/predmet": "node/const/temp/struct",
    "node/const/group": "node/const/perm/group",
    "node/var/predmet": "node/var/temp/struct",
    "node/var/group": "node/var/perm/group",
    "pair/const/synonym": "pair/const/-/perm/noorien",
    "pair/const/orient": "pair/const/-/perm/orient",
    "arc/const/fuz": "pair/const/fuz/perm/orient/membership",
    "arc/const/fuz/temp": "pair/const/fuz/temp/orient/membership",
    "arc/const/neg": "pair/const/neg/perm/orient/membership",
    "arc/const/neg/temp": "pair/const/neg/temp/orient/membership",
    "arc/const/pos": "pair/const/pos/perm/orient/membership",
    "arc/const/pos/temp": "pair/const/pos/temp/orient/membership",
    "pair/var/noorient": "pair/-/-/-/noorient",
    "pair/var/orient": "pair/-/-/-/orient",
    "arc/var/fuz": "pair/var/fuz/perm/orient/membership",
    "arc/var/fuz/temp": "pair/var/fuz/temp/orient/membership",
    "arc/var/neg": "pair/var/neg/perm/orient/membership",
    "arc/var/neg/temp": "pair/var/neg/temp/orient/membership",
    "arc/var/pos": "pair/var/-/perm/noorien",
    "arc/var/pos/temp": "pair/var/pos/temp/orient/membership",
    "pair/noorient": "pair/-/-/-/noorient",
    "pair/orient": "pair/-/-/-/orient",
    "arc/-/-": "pair/-/-/-/orient",
}


def updateExistingTypesByTag(elem, tag: str):
    elements = elem.getElementsByTagName(tag)
    for element in elements:
        if (tag == "arc" and element.getAttribute("type") == "node/var/perm/general"):
            raise Exception("wrong arc type: node/var/perm/general")
        if (dictionary.get(element.getAttribute("type"))):
            element.setAttribute("type", dictionary.get(
                element.getAttribute("type")))


def updateTags(elem):
    print("Update Tags")
    updateExistingTypesByTag(elem, "node")
    updateExistingTypesByTag(elem, "pair")
    updateExistingTypesByTag(elem, "arc")

    contours = elem.getElementsByTagName("contour")
    for contour in contours:
        if (contour.getAttribute("type") == ""):
            contour.setAttribute("type", dictionary.get(
                contour.getAttribute("type")))

    buses = elem.getElementsByTagName("bus")
    for bus in buses:
        if (bus.getAttribute("type") == ""):
            bus.setAttribute("type", dictionary.get(bus.getAttribute("type")))


def createDir(address, to_dir):
    path_list = address.split("/")
    path_list.pop(0)
    path = "/".join(path_list)
    os.makedirs("/".join([to_dir, path]), 511, True)
    return "/".join([to_dir, path])


def translate(from_dir, to_dir):
    if not os.path.isdir(to_dir):
        os.mkdir(to_dir)
    if os.path.isdir(from_dir):
        for address, dirs, files in os.walk(from_dir):
            for from_file in files:
                if from_file.endswith(".gwf"):
                    with open(os.path.join(address, from_file)) as filegwf:
                        print("Begin to translate:"+from_file)
                        document = parse(filegwf)
                        updateTags(document)
                        document.normalize()
                        with open("/".join([createDir(address, to_dir), from_file]), "w+") as file_to_write:
                            file_to_write.write(document.toprettyxml())
                            print("Translated text written in:"+from_file)
                else:
                    print("Incorrect file:"+from_file)
    else:
        print("Incorrect param input dir")
        exit(1)


if __name__ == '__main__':
    if len(sys.argv) == 3:
        translate(sys.argv[1], sys.argv[2])
    else:
        print("Incorrect params, to run this script: python3 old_to_new_gwf.py <path to input dir> <path to output dir>")
        exit(1)
