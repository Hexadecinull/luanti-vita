#include "vita_fs.h"

#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/io/dirent.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

extern "C" {

int vita_fs_mkdir(const char *path)
{
    return sceIoMkdir(path, 0777);
}

int vita_fs_mkdir_recursive(const char *path)
{
    if (!path || path[0] == '\0') return -1;

    std::string p(path);
    size_t pos = 0;

    if (p.size() >= 5 && p[3] == ':') {
        pos = 5;
    }

    while (pos < p.size()) {
        size_t slash = p.find('/', pos);
        if (slash == std::string::npos) slash = p.size();

        std::string sub = p.substr(0, slash);
        if (!sub.empty()) {
            SceIoStat st;
            if (sceIoGetstat(sub.c_str(), &st) < 0) {
                int r = sceIoMkdir(sub.c_str(), 0777);
                if (r < 0 && r != 0x80010011) return r;
            }
        }
        pos = slash + 1;
    }
    return 0;
}

bool vita_fs_exists(const char *path)
{
    if (!path) return false;
    SceIoStat st;
    return sceIoGetstat(path, &st) >= 0;
}

bool vita_fs_is_dir(const char *path)
{
    if (!path) return false;
    SceIoStat st;
    if (sceIoGetstat(path, &st) < 0) return false;
    return SCE_S_ISDIR(st.st_mode);
}

int vita_fs_copy(const char *src, const char *dst)
{
    if (!src || !dst) return -1;

    SceUID in = sceIoOpen(src, SCE_O_RDONLY, 0);
    if (in < 0) return in;

    SceUID out = sceIoOpen(dst, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0666);
    if (out < 0) {
        sceIoClose(in);
        return out;
    }

    static char buf[65536];
    int ret = 0;

    while (true) {
        int n = sceIoRead(in, buf, sizeof(buf));
        if (n < 0) { ret = n; break; }
        if (n == 0) break;
        int w = sceIoWrite(out, buf, n);
        if (w < 0) { ret = w; break; }
    }

    sceIoClose(in);
    sceIoClose(out);
    return ret;
}

int vita_fs_copy_if_missing(const char *src, const char *dst)
{
    if (vita_fs_exists(dst)) return 0;
    return vita_fs_copy(src, dst);
}

int vita_fs_remove(const char *path)
{
    if (!path) return -1;
    if (vita_fs_is_dir(path)) return sceIoRmdir(path);
    return sceIoRemove(path);
}

int vita_fs_rename(const char *src, const char *dst)
{
    if (!src || !dst) return -1;
    return sceIoRename(src, dst);
}

long vita_fs_file_size(const char *path)
{
    if (!path) return -1;
    SceIoStat st;
    if (sceIoGetstat(path, &st) < 0) return -1;
    return static_cast<long>(st.st_size);
}

}
