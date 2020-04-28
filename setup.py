
from setuptools import setup, Extension

module1 = Extension(
    'pylibgeohash',
    include_dirs=['/usr/local/include'],
    libraries=[],
    library_dirs=['/usr/local/lib'],
    sources=['pylibgeohash.c', 'geohash.c']
)

setup(
    name='pylibgeohash',
    version='0.2.2',
    description='Thin wrapper around the geohash codebase',
    author='Dan Bauman',
    author_email='bauman.85@osu.edu',
    url='https://github.com/bauman/libgeohash',
    keywords=['geohash', 'geohashes'],
    classifiers=[
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'License :: OSI Approved :: BSD License',
        'Programming Language :: Python :: 3',
    ],
    ext_modules=[module1]
)
