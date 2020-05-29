#!/usr/bin/env python2

from __future__ import print_function

import sys


def main(argv):
    if len(argv) == 1:
        print("1")
    elif len(argv) == 2:
        print("2")
    elif len(argv) == 3:
        print("3")
    else:
        print("else")


if __name__ == '__main__':
    try:
        main(sys.argv)
    except RuntimeError as e:
        print('FATAL: %s' % e, file=sys.stderr)
        sys.exit(1)
