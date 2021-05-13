import os
import scipy.io
from zipfile import *
from tqdm import tqdm
from pandas import read_csv
from browseTerminal import terminalBrowser


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

    zf = ZipFile(os.path.join(zipPath, "CSV.zip"), "w", ZIP_DEFLATED)

    for path in tqdm(paths):
        zf.write(path, path.replace(zipPath, ""))


if __name__ == "__main__":
    browser = terminalBrowser(startPath="/home/filippo/Downloads")
    base_path = browser.browse()

    files = findAllFiles(base_path, ".csv")

    to_zip = []
    for file in files:
        print(file)
        try:
            csv = read_csv(file, sep=';', index_col=False)
        except:
            pass

        values = {}
        for i, col in enumerate(csv.columns):
            values[col] = csv.iloc[:,i].values

        new_fname = file.replace(".csv", ".mat")
        scipy.io.savemat(new_fname, values)
        to_zip.append(new_fname)
    compressFolders(to_zip, base_path)
