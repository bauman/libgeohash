from setuptools import setup, Extension
from os import environ
from pathlib import Path

include_dirs = [
    str(Path(__file__).parent)
]
include_dirs.extend(environ.get("INCLUDE_DIR", ".").split(":"))
setup_args = dict(
    ext_modules = [
        Extension('pylibgeohash',
                  sources=['pylibgeohash.c', 'geohash.c'],
                  include_dirs=include_dirs
                  )
    ]
)
setup(**setup_args)
