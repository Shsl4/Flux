#!/usr/bin/env python3

import os, sys, shutil, subprocess, glob, pathlib

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
            'skia_use_system_icu': False }

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

    return ' '.join([ f'{key}={str(value).lower()}' for key, value in args.items() ] )
    

def build_skia(debug):

    depot_tools_path = os.path.join(os.path.expanduser('~'), 'depot_tools')

    if not os.path.exists(depot_tools_path):

        git_path = shutil.which('git')

        if git_path is None:
            raise Exception('Git could not be found. Make sure it is installed.')

        proc = subprocess.run(['git', 'clone', 'https://chromium.googlesource.com/chromium/tools/depot_tools.git', '~/depot_tools'])

        if proc.returnCode != 0:
            raise Exception('Failed to clone depot_tools! Are you connected to the internet?')

    os.environ['PATH'] += os.pathsep + depot_tools_path

    os.chdir('skia')
        
    print("Syncing git dependencies for skia...")

    proc = subprocess.run(['python', 'tools/git-sync-deps'], capture_output=True, text=True)

    if proc.returncode != 0:
        print(proc.stderr)
        print(f'{ConsoleColors.yellow}Failed to sync skia deps!{ConsoleColors.reset}')

    print("Fetching ninja...")

    proc = subprocess.run('bin/fetch-ninja', capture_output=True, text=True)

    if proc.returncode != 0:
        print(proc.stdout)
        raise Exception('Failed to fetch ninja!')

    proc = subprocess.run(['bin/gn', 'gen', 'out/build', f'--args={gen_skia_args(debug)}'], capture_output=True, text=True)

    if proc.returncode != 0:
        print(proc.stdout)
        raise Exception('Failed to generate project!')

    print('Now building skia. This might take a while.')

    proc = subprocess.run(['ninja', '-C', 'out/build'])

    if proc.returncode != 0:
        raise Exception('Failed to build skia!')

    print(f'{ConsoleColors.green}Successfully built skia.{ConsoleColors.reset}')

    os.chdir('..')

def build_rt(name, debug):

    os.chdir(name)

    proc = subprocess.run('./autogen.sh', capture_output=True, text=True)

    if proc.returncode != 0:
        print(proc.stdout)
        raise Exception(f'{name} autogen failed!')

    print(f'Now building {name}.')

    proc = subprocess.run('make')

    if proc.returncode != 0:
        raise Exception(f'Failed to build {name}!')

    print(f'{ConsoleColors.green}Successfully built {name}.{ConsoleColors.reset}')

    os.chdir('..')

def cmake_build(name, debug):

    os.chdir(name)

    proc = subprocess.run(['cmake', '-S', '.', '.build'], capture_output=True, text=True)

    if proc.returncode != 0:
        print(proc.stdout)
        raise Exception(f'{name} cmake generation failed!')

    os.chdir('.build')

    print(f'Now building {name}.')

    proc = subprocess.run('make')

    if proc.returncode != 0:
        raise Exception(f'Failed to build {name}!')

    print(f'{ConsoleColors.green}Successfully built {name}.{ConsoleColors.reset}')

    os.chdir('../..')


def build(debug):

    os.chdir('Libraries')

    try:
        build_skia(debug)
        build_rt('rtmidi', debug)
        build_rt('rtaudio', debug)
        cmake_build('glfw', debug)
        cmake_build('Nucleus', debug)
    except Exception as message:
        print(f'{ConsoleColors.red}{message}{ConsoleColors.reset}')
        return 1

    try:
        os.mkdir(".build")
    except FileExistsError:
        pass

    shutil.copy('skia/out/build/libSkia.a', '.build')
    shutil.copy('rtmidi/.libs/librtmidi.a', '.build')
    shutil.copy('rtaudio/.libs/librtaudio.a', '.build')
    shutil.copy('glfw/.build/src/libglfw3.a', '.build')
    shutil.copy('Nucleus/.build/libNucleus.a', '.build')

    print(f'{ConsoleColors.green}Successfully built all dependencies.{ConsoleColors.reset}')

    os.chdir('..')

    return 0

def setup():

    cmake_path = shutil.which("cmake")

    if(cmake_path is None):
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
    ext = ""

    if sys.platform == "darwin":
        generator = "Xcode"
        ext = ".xcodeproj"
    elif sys.platform == "win32":
        generator = "Visual Studio 17 2022"
        ext = ".sln"

    proc = subprocess.run([cmake_path, "..", "-G", generator])

    if proc.returncode != 0:
        print("\033[91mCMake generation failed!")
        return 1

    os.chdir("..")

    if ext != "":
        try:
            os.symlink(f'.cmake/Flux{ext}', f'Flux{ext}')
        except FileExistsError:
            pass


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
        build(True)
