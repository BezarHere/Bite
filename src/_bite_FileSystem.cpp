#include "_bite_defines.h"
#include "_bite_FileSystem.h"
#include <sys/stat.h>
#include <exception>

using bite::filesys::info_t;

constexpr int ValidStat = 0;

__forceinline info_t get_info(const char *const path)
{
  info_t i;
  if (const int j = stat(path, &i))
    throw std::exception("bad stat retrival", j);
  return i;
}

__forceinline info_t get_info(const std::string &path)
{
  return get_info(path.c_str());
}

namespace bite
{
  namespace filesys
  {

    bool exists(const std::string &path)
    {
      info_t tmp;
      return stat(path.c_str(), &tmp) != ValidStat;
    }

    bool exists(const char *const path)
    {
      info_t tmp;
      return stat(path, &tmp) != ValidStat;
    }

    size_t size(const std::string &path)
    {
      return get_info(path).st_size;
    }

    size_t size(const char *const path)
    {
      return get_info(path).st_size;
    }

    bool isdir(const std::string &path)
    {
      return get_info(path).st_mode & DirFlags::IsDir;
    }

    bool isdir(const char *const path)
    {
      return get_info(path).st_mode & DirFlags::IsDir;
    }

    DirStat::DirStat(const std::string &path, const info_t &&info)
      : filepath{ path }, flags((DirFlags)info.st_mode), device(info.st_dev)
    {
    }

    DirStat::DirStat(const std::string &path)
      : DirStat(path, get_info(path))
    {
    }

    DirStat::DirStat(const char *const path)
      : DirStat(std::string{path}, get_info(path))
    {
    }

    DirStat::DirStat(const DirStat &copy)
      : filepath(copy.filepath), flags(copy.flags), device(copy.device)
    {
    }

  }
}
