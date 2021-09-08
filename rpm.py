#!/usr/bin/env python3
"""
Use Cases
1. Troubleshooting systems - what changed with rpm time stamps
2. Auditing
3. Testing
4. Cloning
"""

"""
Copyright 2021 Richard 'Rick' Hernandez 619-847-3241

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""
import sys
import datetime
import subprocess

def cmd(command):
    try:
        output = subprocess.check_output(command, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"command process error, exiting\n    {command}")
        sys.exit(1)
    except FileNotFoundError:
        print(f"File not found: {command}")
        sys.exit(2)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(3)

    return output.decode("utf-8")

listing = cmd("rpm -qa --qf '%{INSTALLTIME}:%{NAME}|%{VERSION}-%{RELEASE}\n' | sort -nr").split('\n')

for rpm in listing:
    if not ':' in rpm:
        continue
    ts, name = rpm.split(':')
    ts = datetime.datetime.fromtimestamp(int(ts))
    print(ts.strftime('%Y-%m-%d %H:%M:%S'), end='|')
    print(name)
