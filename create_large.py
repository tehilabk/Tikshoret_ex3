#!/usr/bin/env python

with open("1mb.txt", "wb") as out:
    out.seek(( 1024 * 1024) - 1)
    out.write(b'\0')

