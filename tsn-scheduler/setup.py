from setuptools import setup, Extension
import sys
import os

# Platform specific flags
extra_compile_args = []
if sys.platform == 'win32':
    extra_compile_args = ['/D_PLATFORM_MOCK', '/W4']
else:
    extra_compile_args = ['-D__linux__', '-Wall', '-Wextra']

# Define the Extension
tsn_module = Extension(
    'tsn_scheduler',
    sources=[
        'src/python_api/bindings.c',
        'src/core/scheduler.c',
        'src/core/queue.c',
        'src/core/flow.c',
        'src/core/bandwidth.c',
        'src/linux/netlink_socket.c'
    ],
    include_dirs=['src'],
    extra_compile_args=extra_compile_args,
)

setup(
    name='tsn_scheduler',
    version='0.1.0',
    description='TSN Scheduler Python Bindings for OpenNeuro',
    ext_modules=[tsn_module],
)
