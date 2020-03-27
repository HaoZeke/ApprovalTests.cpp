#! /usr/bin/env python3

# pip3 install -r requirements.txt

from scripts import version
from scripts.release_details import build

if __name__ == '__main__':
    build(version.update_minor, deploy=False)
