import os
import sys
import sysconfig
from shutil import copyfile
from importlib import util
from distutils.core import run_setup

SOURCEPATH = os.path.dirname(os.path.abspath(__file__)).replace('application/common', '')

def distutils_path():
    f = "{dirname}.{platform}-{version[0]}.{version[1]}"
    return [f.format(dirname='temp',
                    platform=sysconfig.get_platform(),
                    version=sys.version_info),
            f.format(dirname='lib',
                    platform=sysconfig.get_platform(),
                    version=sys.version_info) ]

def is_module(module_name):
    mod_value = util.find_spec(module_name)
    if mod_value:
        print('\nFound module \'{}\':'.format(module_name))
        for item in mod_value.__dict__.items():
            print('\t{} ::: {}'.format(item[0], item[1]))
        return True
    else:
        print('\nCan not found module {}'.format(module_name))
        return False

def build_indoorino_module():
    for path in distutils_path():
        buildpath=os.path.join(SOURCEPATH, 'build/build/', path)
        if os.path.isdir(buildpath):
            print('\nRemoving folder {}'.format(buildpath))
            os.system('rm -r {}'.format(buildpath))

    buildpath = os.path.join(SOURCEPATH, 'build/')
    currentpwd= os.getcwd()
    os.chdir(buildpath)

    d = run_setup('setup.py', script_args=['build'])

    os.chdir(currentpwd)

    path_build_dir = os.path.join(SOURCEPATH, 'build/build/' + distutils_path()[1])
    path_build_lib = os.path.join(path_build_dir, 'IndoorinoModule' + sysconfig.get_config_var('EXT_SUFFIX'))
    path_target = os.path.join(SOURCEPATH, 'application/common/IndoorinoModule.so')

    if os.path.isfile(path_build_lib):
        print('Copying module {} --> {}'.format(path_build_lib, path_target))
        copyfile(path_build_lib, path_target)
    else:
        raise ModuleNotFoundError ('\nERROR:Indoorino: can not build libraries\n Check compiling errors')

def build_setup():
    packs=( ('tkinter',   'python3-tk'),
            ('numpy',     'python3-numpy'),
            ('serial',    'python3-serial'),
            ('matplotlib','python3-matplotlib')
          )

    for req in packs:
        if not is_module(req[0]):
            if sys.platform == 'linux':
                raise ModuleNotFoundError(
                    '\nERROR:Indoorino: {} is needed to run the program.\n Please try sudo apt-get install {}'.format(
                        req[0], req[1]
                    ))

    build_indoorino_module()

if __name__ == "__main__":

    build_setup()
    try:
        import common.IndoorinoModule as Module
        Module.benchmark()

        print('\033[1;32m\tcommon.IndoorinoModule loaded correctly!\033[0m')
    except ModuleNotFoundError:
        try:
            import IndoorinoModule
            print('\033[1;32m\tIndoorinoModule loaded correctly!\033[0m')
        except ModuleNotFoundError:
            print('\033[1;31m\tCan not find IndoorinoModule!\033[0m')

        except ImportError as error:
            print('\033[1;31m\tIndoorinoModule loading failed!\n{}\033[0m'.format(error))
    except ImportError as error:
        print('\033[1;31m\tIndoorinoModule loading failed!\n{}\033[0m'.format(error))
