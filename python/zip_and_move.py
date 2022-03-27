import os
import shutil
from zipfile import *
from tqdm import tqdm


def findAllFiles(path, extension):
    dirs = os.listdir(path)
    paths = []
    for dir in dirs:
        if (os.path.isdir(path + "/" + dir)):
            paths += findAllFiles(path + "/" + dir, extension)
        else:
            if (extension in dir):
                paths.append(path + "/" + dir)
    return paths



def compressFolders(paths, zipPath):
    print("COMPRESSING")

    zf = ZipFile(os.path.join(zipPath, "logs.zip"), "w", compression=ZIP_DEFLATED, compresslevel=5)

    for path in tqdm(paths):
        zf.write(path, path.replace(zipPath, ""))

    size = sum([zinfo.file_size for zinfo in zf.filelist])
    return float(size)


if __name__ == "__main__":
    home = os.path.expanduser("~")
    base_path = home + "/logs"

    files = findAllFiles(base_path, ".csv")
    files += findAllFiles(base_path, ".log")
    files += findAllFiles(base_path, ".json")

    print("Found {} files.".format(len(files)))
    print("Start zipping")

    compressFolders(files, base_path)

    for i in range(10):
        path = "/media/usb" + str(i)
        if os.path.ismount(path):
            shutil.copy(base_path + "/logs.zip", path + "/logs.zip")
            break
