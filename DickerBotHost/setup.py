from setuptools import setup

APP = ['Host.py']
DATA_FILES = ['Host.ui']
OPTIONS = {
    'argv_emulation': False,
    'includes': ['serial', 'serial.tools.list_ports', 'asyncio'],
    'packages': ['PyQt5', 'websockets'],
    'resources': ['Host.ui'],
    'excludes': ['packaging', 'setuptools', 'pkg_resources'],
    'iconfile': 'dickerbot.icns',
    'plist': {
        'CFBundleName': 'DickerBotHost',
        'CFBundleShortVersionString': '1.0.0',
        'CFBundleIdentifier': 'com.keshavshankar.dickerbot',
    },
}

setup(
    app=APP,
    name='DickerBotHost',
    data_files=DATA_FILES,
    options={'py2app': OPTIONS},
    setup_requires=['py2app'],
)
