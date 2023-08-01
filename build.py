from dataclasses import dataclass
from glassy import atMain, utils
import sys
import shutil
import os

output_folder = r"E:\Assets\CppLibs\Bite\\"

def pyprint(*args):
  print('PY>>', *args)

@dataclass(slots=True, frozen=True)
class ProcedurePaths:
  lib: str
  libout_name: str


folders: dict[tuple[bool, bool], ProcedurePaths] =\
{
  (False, True): ProcedurePaths("x64\\Debug\\Bite", 'bite64-d'),
  (True, True): ProcedurePaths("x64\\Release\\Bite", 'bite64'),
  (False, False): ProcedurePaths("Win32\\Debug\\Bite", 'bite32-d'),
  (True, False): ProcedurePaths("Win32\\Release\\Bite", 'bite32'),
}

def process(release: bool, win64x: bool):
  proc: ProcedurePaths = folders[release, win64x]
  if not os.path.exists(proc.lib + '.lib'):
    pyprint(f"FAILED: building {'release' if release else 'debug'} {'64' if win64x else '86'} configuration: no .lib file found")
    return
  shutil.copy(proc.lib + '.lib', output_folder + 'lib/' + proc.libout_name + '.lib')
  os.remove(proc.lib + '.lib')
  pyprint(
    f"SUCCEED: building {'release' if release else 'debug'} {'64' if win64x else '86'} configuration!")


@atMain
def main():
  pyprint("updating include")
  shutil.copy('src/Bite.h', output_folder + 'include/Bite.h')
  
  if sys.argv.__len__() == 1:
    pyprint("building all configurations")
    for i in (False, False), (False, True), (True, False), (True, True):
      process(*i)
    return
  
  output_release = sys.argv[1] == 'r'
  if sys.argv[1] != 'd' and not output_release:
    raise ValueError(f"invalid output type(argument 1): '{sys.argv[1]}', must be either 'd' debug or 'r' release")
  if sys.argv.__len__() == 2:
    pyprint(f"building all {'release' if output_release else 'debug'} configurations")
    process(output_release, False)
    process(output_release, True)
    return
  
  output_64x = sys.argv[2] == '64'
  if sys.argv[2] != '32' and sys.argv[2] != '86' and not output_64x:
    raise ValueError(f"invalid output binary(argument 2): '{sys.argv[2]}', must be either '64' or '32' or '86'")

  pyprint(f"building {'release' if output_release else 'debug'} {'64' if output_64x else '86'} configuration")
  process(output_release, output_64x)
