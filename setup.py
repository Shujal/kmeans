from distutils.core import setup, Extension

module = Extension("kmeans",sources=["kmeans.cpp"])

setup(name = 'PackageName', version = '1.337', description = 'K-Means algorithm for int point clouds. Returns int points.', ext_modules = [module])
