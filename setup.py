#!/usr/bin/env python3

import glob
import os
import shutil
import subprocess
import sys


class ConsoleColors:
    blue = '\033[94m'
    cyan = '\033[96m'
    green = '\033[92m'
    yellow = '\033[93m'
    red = '\033[91m'
    reset = '\033[0m'


def gen_skia_args(debug):
    args = {'is_official_build': True,
            'is_debug': False,
            'skia_use_metal': False,
            'skia_use_gl': True,
            'skia_use_vulkan': False,
            'skia_use_direct3d': False,
            'skia_use_system_libjpeg_turbo': False,
            'skia_use_system_zlib': False,
            'skia_use_system_harfbuzz': False,
            'skia_use_system_libpng': False,
            'skia_use_system_libwebp': False,
            'skia_use_system_expat': False,
            'skia_use_system_icu': False}

    if sys.platform == 'darwin':
        args['skia_use_metal'] = True
    elif sys.platform == 'win32':
        args['skia_use_direct3d'] = True
        args['skia_use_vulkan'] = True
    else:
        args['skia_use_vulkan'] = True

    if debug:
        args['is_debug'] = True
        args['is_official_build'] = False

    st = ' '.join([f'{key}={str(value).lower()}' for key, value in args.items()])
    
    if sys.platform == "win32":
        if debug:
            return st + ' extra_cflags=["/MDd"]'
        else:
            return st + ' extra_cflags=["/MD"]'
        
    return st



def build_skia(debug):
    depot_tools_path = os.path.join(os.path.expanduser('~'), 'depot_tools')

    if not os.path.exists(depot_tools_path):

        git_path = shutil.which('git')

        if git_path is None:
            raise Exception('Git could not be found. Make sure it is installed.')

        proc = subprocess.run(
            ['git', 'clone', 'https://chromium.googlesource.com/chromium/tools/depot_tools.git', depot_tools_path])

        if proc.returncode != 0:
            raise Exception('Failed to clone depot_tools! Are you connected to the internet?')

    os.chdir('skia')

    print("Syncing git dependencies for skia. This might take a while.")

    proc = subprocess.run(['python3', 'tools/git-sync-deps'], capture_output=True, text=True)

    if proc.returncode != 0:
        print(proc.stderr)
        print(f'{ConsoleColors.yellow}Failed to sync skia deps!{ConsoleColors.reset}')

    print('Now building skia. This might take a while.')

    build_type = 'Debug' if debug else 'Release'

    proc = subprocess.run(['./bin/gn', 'gen', f'.build/{build_type}', f'--args={gen_skia_args(debug)}'], capture_output=True, text=True)

    if proc.returncode != 0:
        print(proc.stdout)
        raise Exception('Failed to generate project!')

    if shutil.which('ninja') is None:
        raise Exception('Could not find ninja! Please check the README for instructions on how to install.')

    proc = subprocess.run(['ninja', '-C', f'.build/{build_type}', 'skia'])

    if proc.returncode != 0:
        raise Exception('Failed to build skia!')

    print(f'{ConsoleColors.green}Successfully built skia.{ConsoleColors.reset}')

    os.chdir('..')


def cmake_build(name, debug, params=None):
    if params is None:
        params = []

    os.chdir(name)

    build_type = 'Debug' if debug else 'Release'

    proc = subprocess.run(['cmake', '-S', '.', '.build'] + params, capture_output=True, text=True)

    if proc.returncode != 0:
        print(proc.stderr)
        raise Exception(f'{name} cmake generation failed!')

    os.chdir('.build')

    print(f'Now building {name}.')

    proc = subprocess.run(['cmake', '--build', '.', '--config', build_type])

    if proc.returncode != 0:
        raise Exception(f'Failed to build {name}!')

    print(f'{ConsoleColors.green}Successfully built {name}.{ConsoleColors.reset}')

    os.chdir('../..')


def build(debug):

    os.chdir('Libraries')

    try:
        build_skia(debug)
        cmake_build('rtmidi', debug, ['-DRTMIDI_BUILD_STATIC_LIBS=1'])
        if sys.platform == "win32":
            cmake_build('rtaudio', debug, ['-DRTAUDIO_BUILD_STATIC_LIBS=1', '-DRTAUDIO_STATIC_MSVCRT=0', '-DRTAUDIO_API_DS=1', '-DRTAUDIO_API_ASIO=1'])
        else:
            cmake_build('rtaudio', debug, ['-DRTAUDIO_BUILD_STATIC_LIBS=1'])
        cmake_build('glfw', debug)
        cmake_build('Nucleus', debug)
    except Exception as message:
        print(f'{ConsoleColors.red}{message}{ConsoleColors.reset}')
        sys.stdout.flush()
        return 1

    path = "Debug" if debug else "Release"

    try:
        os.makedirs(f'.build/{path}')
    except FileExistsError:
        pass

    if sys.platform == "win32":

        postfix = "d" if debug else ""

        shutil.copy(f'skia/.build/{path}/skia.lib', f'.build/{path}')
        shutil.copy(f'rtmidi/.build/{path}/rtmidi.lib', f'.build/{path}')
        shutil.copy(f'rtaudio/.build/{path}/rtaudio{postfix}.lib', f'.build/{path}')
        shutil.copy(f'glfw/.build/src/{path}/glfw3.lib', f'.build/{path}')
        shutil.copy(f'Nucleus/.build/{path}/Nucleus{postfix}.lib', f'.build/{path}')

    else:
        shutil.copy(f'skia/.build/{path}/libskia.a', f'.build/{path}')
        shutil.copy('rtmidi/.build/librtmidi.a', f'.build/{path}')
        shutil.copy('rtaudio/.build/librtaudio.a', f'.build/{path}')
        shutil.copy('glfw/.build/src/libglfw3.a', f'.build/{path}')
        shutil.copy('Nucleus/.build/libNucleus.a', f'.build/{path}')

    print(f'{ConsoleColors.green}Successfully built all dependencies.{ConsoleColors.reset}')

    os.chdir('..')

    return 0


def setup():
    cmake_path = shutil.which("cmake")

    if cmake_path is None:
        print("Could not find CMake. Make sure it is installed")
        return

    if not os.path.exists('Libraries/.build'):
        print('Dependencies binary folder not found. Now building dependencies...')
        if build(True) != 0:
            return 1

    try:
        os.mkdir(".cmake")
    except FileExistsError:
        pass

    os.chdir(".cmake")

    generator = "Unix Makefiles"

    if sys.platform == "darwin":
        generator = "Xcode"
    elif sys.platform == "win32":
        generator = "Visual Studio 17 2022"

    proc = subprocess.run([cmake_path, "..", "-G", generator])

    if proc.returncode != 0:
        print(f"{ConsoleColors.red}CMake generation failed!{ConsoleColors.reset}")
        return 1

    os.chdir("..")

    if sys.platform == "darwin":
        try:
            os.symlink(f'.cmake/Flux.xcodeproj', f'Flux.xcodeproj')
        except FileExistsError:
            pass

    elif sys.platform == "win32":
        os.system('pwsh -command "$Location = Get-Location; $WScriptObj = New-Object -ComObject (""WScript.Shell""); '
                  '$Shortcut = $WscriptObj.CreateShortcut("""$Location\Flux.lnk"""); '
                  '$Shortcut.TargetPath="""$Location\.cmake\Flux.sln"""; $Shortcut.Save()"')


def purge(path):
    for file in glob.glob(path):
        try:
            shutil.rmtree(file)
        except FileNotFoundError:
            pass


def clean():
    purge(".cmake")
    purge("./Build")
    purge("./cmake-*")

    for file in glob.glob("./Flux.*"):
        os.unlink(file)

    print("Cleaned cmake and Build directories.")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        setup()
    elif sys.argv[1] == "clean":
        clean()
    elif sys.argv[1] == "build":
        if len(sys.argv) >= 3:
            if sys.argv[2].lower() == 'debug':
                build(True)
            elif sys.argv[2].lower() == 'release':
                build(False)
            else:
                print('Invalid argument. Expected debug or release')
        else:
            print('Not enough arguments. Expected debug or release')
    else:
        print(f'Unrecognized argument: {sys.argv[1]}')
