#!/usr/bin/env python2

from __future__ import print_function
import posix as posix
import sys
# from typing import Optional, List, Tuple, Dict, Any, cast
import json

import json


class AppConfiguration(object):
    def __init__(self, data=None):
        if data is None:
            with open('json.txt') as fh:
                data = json.loads(fh.read())
        else:
            data = dict(data)

        for key, val in data.items():
            setattr(self, key, self.compute_attr_value(val))

    def compute_attr_value(self, value):
        if type(value) is list:
            return [self.compute_attr_value(x) for x in value]
        elif type(value) is dict:
            return AppConfiguration(value)
        else:
            return value


    def recursive_iteration(input, result ='') :
        if type(input) == dict:
            for k, v in input.items():
                if type(v) == str:
                    result += '%s:%s\n\t' % (str(k), str(v))
                elif type(v) in (dict, list):
                    result += '%s:\n\t' % str(k)
                    result = input.recursive_iteration(v, result)
                    result += '\n\t'
        elif type(input) == list:
            for item in input:
                if type(item) == str:
                    result += item
                    continue
                result = input.recursive_iteration(item, result) + '\n\t'
        else:
            result += input + '\n\t'
        return result

def main(argv):
    if len(argv) == 1:
        print("1")
    elif len(argv) == 2:
        print(argv[2])
    elif len(argv) == 3:
        print("sss")
    else:
        instance = AppConfiguration()
        print(instance.article[0].id)
        print(instance.blog[0].name)
        # print(AppConfiguration.recursive_iteration(instance))
        if argv[2] == "-parse":
            f = open(argv[3], 'r')
            str = f.read()
            jsonObject = json.loads(str)
            print(jsonObject['blog'])
        print(len(argv))


if __name__ == '__main__':
    try:
        main(sys.argv)
    except RuntimeError as e:
        print('FATAL: %s' % e, file=sys.stderr)
        sys.exit(1)
