//
// Created by cpasjuste on 17/11/17.
//

#include <unistd.h>
#include <cstring>

#include "physfs.h"
#include "cross2d/skeleton/physfs_io.h"

using namespace c2d;

PHYSFSIo::PHYSFSIo() : Io() {
    const std::string cwd = PHYSFSIo::getDataPath();
    PHYSFS_init(cwd.c_str());
    PHYSFS_mount("myzip.zip", "/romfs", 1);
    PHYSFS_mount("/", "/", 1);
}

PHYSFSIo::~PHYSFSIo() {
    PHYSFS_deinit();
}

std::string PHYSFSIo::getDataPath() {

    char buf[1024];
    if (getcwd(buf, sizeof(buf)) != nullptr) {
        std::string str = buf;
        if (!Utility::endsWith(str, "/")) {
            str += "/";
        }
        return str;
    }

    return Io::getDataPath();
}

std::string PHYSFSIo::getRomFsPath() {
    return "/romfs/";
}

Io::File PHYSFSIo::getFile(const std::string &path) {

    File file{};
    PHYSFS_Stat st{};

    if (PHYSFS_stat(path.c_str(), &st) == 0) {
        return file;
    }

    file.name = Utility::baseName(path);
    file.path = path;
    file.size = (size_t) st.filesize;
    file.type = st.filetype == PHYSFS_FILETYPE_DIRECTORY ? Type::Directory : Type::File;

    return file;
}

size_t PHYSFSIo::getSize(const std::string &path) {

    PHYSFS_Stat st{};

    if (PHYSFS_stat(path.c_str(), &st) == 0) {
        return 0;
    }

    return (size_t) st.filesize;
}

Io::Type PHYSFSIo::getType(const std::string &path) {

    PHYSFS_Stat st{};

    if (PHYSFS_stat(path.c_str(), &st) == 0) {
        return Type::Unknown;
    }

    return st.filetype == PHYSFS_FILETYPE_DIRECTORY ? Type::Directory : Type::File;
}

bool PHYSFSIo::exist(const std::string &path) {
    return PHYSFS_exists(path.c_str()) != 0;
}

bool PHYSFSIo::create(const std::string &path) {
    return PHYSFS_mkdir(path.c_str()) != 0;
}

bool PHYSFSIo::removeFile(const std::string &path) {
    return PHYSFS_delete(path.c_str()) != 0;
}

bool PHYSFSIo::removeDir(const std::string &path) {

    //struct dirent *ent;
    //DIR *dir;
    char **fileList;
    File file = getFile(path);
    if (file.type != Type::Directory) {
        return false;
    }

    fileList = PHYSFS_enumerateFiles(path.c_str());
    for (char **i = fileList; *i != nullptr; i++) {

        printf(" * We've got [%s].\n", *i);
    }

    /*
    if ((dir = opendir(file.path.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {

            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

            std::string newPath =
                    file.path + (Utility::endsWith(file.path, "/") ? "" : "/") + std::string(ent->d_name);

            if (getType(newPath) == Type::Directory) {
                if (!removeDir(newPath)) {
                    closedir(dir);
                    return false;
                }
            } else {
                if (!removeFile(newPath)) {
                    closedir(dir);
                    return false;
                }
            }
        }
        closedir(dir);
    } else {
        return false;
    }
    */

    return rmdir(path.c_str()) == 0;
}

char *PHYSFSIo::read(const std::string &file, size_t offset, size_t size) {

    PHYSFS_File *fp;
    PHYSFS_sint64 file_size;
    char *buffer;

    fp = PHYSFS_openRead(file.c_str());
    if (fp == nullptr) {
        printf("PHYSFSIo::read: can't open %s\n", file.c_str());
        return nullptr;
    }

    file_size = PHYSFS_fileLength(fp);
    if (size <= 0) {
        size = file_size;
    }

    if (offset + size > (PHYSFS_uint64) file_size) {
        size = file_size - offset;
    }

    if (offset > 0) {
        PHYSFS_seek(fp, offset);
    }

    buffer = (char *) malloc(size);
    if (PHYSFS_readBytes(fp, buffer, size) != (PHYSFS_sint64) size) {
        PHYSFS_close(fp);
        free(buffer);
        printf("PHYSFSIo::read: can't read %s\n", file.c_str());
        return nullptr;
    }

    PHYSFS_close(fp);

    return buffer;
}

bool PHYSFSIo::write(const std::string &file, const char *data, size_t size) {

    PHYSFS_File *fp = PHYSFS_openWrite(file.c_str());
    if (fp == nullptr) {
        return false;
    }

    PHYSFS_sint64 len = PHYSFS_writeBytes(fp, data, size);
    PHYSFS_close(fp);

    return len == (PHYSFS_sint64) size;
}

std::vector<Io::File> PHYSFSIo::getDirList(const std::string &path, bool sort, bool showHidden) {

    std::vector<Io::File> files;
    PHYSFS_Stat st{};
    char **fileList = PHYSFS_enumerateFiles(path.c_str());

    for (char **i = fileList; *i != nullptr; i++) {

        const char *name = *i;

        // skip "."
        if ((path == "/" || strlen(name) == 1) && name[0] == '.') {
            continue;
        }

        // skip "hidden" files
        if (!showHidden && name[0] == '.') {
            if (strlen(name) != 2 && name[1] != '.') {
                continue;
            }
        }

        File file;
        file.name = name;
        file.path = Utility::removeLastSlash(path) + "/" + file.name;
        if (PHYSFS_stat(file.path.c_str(), &st) != 0) {
            file.size = (size_t) st.filesize;
            file.type = st.filetype == PHYSFS_FILETYPE_DIRECTORY ? Type::Directory : Type::File;
        }

        file.color = file.type == Type::Directory ? Color::Yellow : Color::White;
        files.push_back(file);
    }

    PHYSFS_freeList(fileList);

    if (sort) {
        std::sort(files.begin(), files.end(), compare);
    }

    return files;
}

Io::File PHYSFSIo::findFile(const std::string &path,
                            const std::vector<std::string> &whitelist, const std::string &blacklist) {
    return File{};
#if 0
    struct dirent *ent;
    DIR *dir;
    File file{};

    if (path.empty()) {
        return file;
    }

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            for (const auto &search : whitelist) {
                if (Utility::contains(ent->d_name, search)) {
                    if (!blacklist.empty() && Utility::contains(ent->d_name, blacklist)) {
                        continue;
                    }
                    file.name = ent->d_name;
                    file.path = Utility::removeLastSlash(path) + "/" + file.name;
#if 0

                    auto *dirSt = (fsdev_dir_t *) dir->dirData->dirStruct;
                    FsDirectoryEntry *entry = &dirSt->entry_data[dirSt->index];
                    file.type = entry->type == ENTRYTYPE_DIR ? Type::Directory : Type::File;
                    file.size = entry->fileSize;
#else
                    struct stat st{};
                    if (stat(file.path.c_str(), &st) == 0) {
                        file.size = (size_t) st.st_size;
                        file.type = S_ISDIR(st.st_mode) ? Type::Directory : Type::File;
                    }
#endif
                    file.color = file.type == Type::Directory ? Color::Yellow : Color::White;
                    break;
                }
            }
        }
        closedir(dir);
    }

    return file;
#endif
}

bool PHYSFSIo::copy(const std::string &src, const std::string &dst,
                    const std::function<void(File, File, float)> &callback) {

    bool res = _copy(src, dst, callback);

    if (callback != nullptr) {
        callback(File{}, File{}, res ? 2 : -1);
    }

    return res;
}

bool PHYSFSIo::_copy(const std::string &src, const std::string &dst,
                     const std::function<void(File, File, float)> &callback) {
#if 0
    File srcFile;
    File dstFile;
    struct dirent *ent;
    DIR *dir;

    if (src == dst) {
        if (callback != nullptr) {
            callback(srcFile, dstFile, -1);
        }
        return false;
    }

    // The destination is a sub-folder of the source folder
    size_t size = src.size();
    if (src.compare(0, size, dst) == 0
        && (dst[size] == '/' || dst[size - 1] == '/')) {
        if (callback != nullptr) {
            callback(srcFile, dstFile, -1);
        }
        return false;
    }

    srcFile = getFile(src);
    dstFile = srcFile;
    dstFile.name = Utility::baseName(src);
    dstFile.path = dst;
    if (!Utility::endsWith(dstFile.path, "/")) {
        dstFile.path += "/";
    }
    dstFile.path += dstFile.name;

    if (srcFile.type == Type::File) {
        bool res = _copyFile(srcFile, dstFile, callback);
        if (callback != nullptr) {
            callback(srcFile, dstFile, 2);
        }
        return res;
    }

    if (!create(dstFile.path)) {
        if (callback != nullptr) {
            callback(srcFile, dstFile, -1);
        }
        return false;
    }

    if ((dir = opendir(srcFile.path.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {

            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

            std::string newSrcPath =
                    srcFile.path + (Utility::endsWith(dstFile.path, "/") ? "" : "/") + std::string(ent->d_name);
            File newSrcFile = getFile(newSrcPath);
            File newDstFile = newSrcFile;
            newDstFile.path = dstFile.path;

            if (newSrcFile.type == Type::File) {
                newDstFile.path += (Utility::endsWith(dstFile.path, "/") ? "" : "/") + std::string(ent->d_name);
                bool success = _copyFile(newSrcFile, newDstFile, callback);
                if (!success) {
                    if (callback != nullptr) {
                        callback(srcFile, dstFile, -1);
                    }
                    closedir(dir);
                    return false;
                }
            } else {
                bool success = _copy(newSrcFile.path, newDstFile.path, callback);
                if (!success) {
                    if (callback != nullptr) {
                        callback(srcFile, dstFile, -1);
                    }
                    closedir(dir);
                    return false;
                }
            }
        }
        closedir(dir);
    } else {
        return false;
    }
#endif
    return true;
}

bool PHYSFSIo::_copyFile(const File &src, const File &dst,
                         const std::function<void(File, File, float)> &callback) {

#if 0
    if (src.path == dst.path) {
        if (callback != nullptr) {
            callback(src, dst, -1);
        }
        return false;
    }

    // The destination is a sub-folder of the source folder
    size_t len = src.path.size();
    if (src.path.compare(0, len, dst.path) == 0
        && (dst.path[len] == '/' || dst.path[len - 1] == '/')) {
        if (callback != nullptr) {
            callback(src, dst, -1);
        }
        return false;
    }

    FILE *srcFd = fopen(src.path.c_str(), "r");
    if (srcFd == nullptr) {
        if (callback != nullptr) {
            callback(src, dst, -1);
        }
        return false;
    }

    FILE *dstFd = fopen(dst.path.c_str(), "w");
    if (dstFd == nullptr) {
        fclose(srcFd);
        if (callback != nullptr) {
            callback(src, dst, -1);
        }
        return false;
    }

    auto buf = (unsigned char *) malloc(C2D_IO_COPY_BUFFER_SIZE);
    if (buf == nullptr) {
        fclose(srcFd);
        fclose(dstFd);
        if (callback != nullptr) {
            callback(src, dst, -1);
        }
        return false;
    }

    if (callback != nullptr) {
        callback(src, dst, 0);
    }

    size_t readBytes, writeBytes, totalBytes = 0;
    while ((readBytes = fread(buf, 1, C2D_IO_COPY_BUFFER_SIZE, srcFd)) > 0) {
        writeBytes = fwrite(buf, 1, readBytes, dstFd);
        if (writeBytes < 0) {
            free(buf);
            fclose(srcFd);
            fclose(dstFd);
            if (callback != nullptr) {
                callback(src, dst, -1);
            }
            return false;
        }

        if (callback != nullptr) {
            totalBytes += writeBytes;
            float progress = (float) totalBytes / (float) src.size;
            callback(src, dst, progress);
        }
    }

    free(buf);
    fclose(srcFd);
    fclose(dstFd);
#endif

    return true;
}
