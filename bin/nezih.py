#!/usr/bin/env python2

from __future__ import print_function
import posix as posix
import sys
# from typing import Optional, List, Tuple, Dict, Any, cast
import json
import urllib

import json


class Json_Parser_Recursive(object):
    def __init__(self, data=None, text=None):
        if data is None and text is not None:
            with open(text) as fh:
                data = json.loads(fh.read())
        else:
            data = dict(data)

        for key, val in data.items():
            setattr(self, key, self.compute_attr_value(val))


    def compute_attr_value(self, value):
        if type(value) is list:
            return [self.compute_attr_value(x) for x in value]
        elif type(value) is dict:
            return Json_Parser_Recursive(data=value)
        else:
            return value

    def recursive_iteration(input, result=''):
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


def parse(data):
    field_value_list = []
    res_list = []
    if type(data) == list:
        for row in data:
            field_value_list.append(parsejson(row, JsonFields={}))
        if (len(field_value_list)) > 1:
            # Uneven json structure
            for i in range(len(field_value_list)):
                if field_value_list[i] not in field_value_list[i + 1:]:
                    res_list.append(field_value_list[i])
                #            field_value_list = list({frozenset(item.items()) : item for item in field_value_list}.values())

    elif type(data) == dict:
        res_list.append(parsejson(data, JsonFields={}))
    return res_list


def parsejson(data, JsonFields, category=None):
    for key, value in data.items():
        if isinstance(value, dict):
            parsejson(value, JsonFields, key)
        else:
            JsonFields.update({(key if category == None else category + '.' + key): type(value).__name__})
    return JsonFields


def recursive_iteration(input, result=''):
    if type(input) == dict:
        for k, v in input.items():
            if type(v) == str:
                result += '%s:%s\n\t' % (str(k), str(v))
            elif type(v) in (dict, list):
                result += '%s:\n\t' % str(k)
                result += '%s:\n\t' % str(k)
                result += recursive_iteration(v, result)
                result += '\n\t'
        return result
    elif type(input) == list:
        for item in input:
            if type(item) == str:
                result += item
                continue
            elif type(item) == unicode:
                result += item
                continue
            result += recursive_iteration(item, result) + '\n\t'
        return result
    elif type(input) == Json_Parser_Recursive:
        temp = ''
        for attr, value in input.__dict__.iteritems():
            if type(value) == list:
                temp += attr + recursive_iteration(value, result)
            elif type(value) == dict:
                temp += attr + recursive_iteration(value, result)
            elif type(value) == Json_Parser_Recursive:
                temp += attr + recursive_iteration(value, result)
            elif type(value) == unicode:
                temp += '%s:%s\n\t' % (str(attr), str(value))
                return temp
        result = temp
    return result


def main(argv):
    if len(argv) == 1 | len(argv) == 0:
        print('FATAL: %s' % e, file=sys.stderr)
        sys.exit(1)
    elif len(argv) == 2:
        print('FATAL: %s' % e, file=sys.stderr)
        sys.exit(1)
    elif len(argv) == 3:
        print('FATAL: %s' % e, file=sys.stderr)
        sys.exit(1)
    else:
        if argv[2] == "-parse":
            if argv[3] == '-show':
                data = ''
                with open(argv[4]) as fh:
                    data = json.loads(fh.read())
                instance = Json_Parser_Recursive(text=argv[4])
                print (data)
            else:
                instance = Json_Parser_Recursive(text=argv[3])
        if argv[2] == "-parseurl":
            if argv[3] == '-show':
                url = argv[4]
                response = urllib.urlopen(url)
                data = json.load(response)
                instance = Json_Parser_Recursive(data=data)
                print (data)
            else:
                instance = Json_Parser_Recursive(text=argv[4])

if __name__ == '__main__':
    try:
        main(sys.argv)
    except RuntimeError as e:
        print('FATAL: %s' % e, file=sys.stderr)
        sys.exit(1)
