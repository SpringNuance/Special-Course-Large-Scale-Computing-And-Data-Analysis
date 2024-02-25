import os
import sqlite3
import subprocess
from enum import Enum
from functools import wraps
from itertools import product
from pathlib import Path
from typing import List, Dict, Union

AnyPath = Union[Path, str, bytes]
RETRY = 3

class Tool(Enum):
    SWIFT = "swift"
    S3CMD = "s3cmd"
    RCLONE = "rclone"


def retry(f):
    """
    Retry a function for a fixed amount of time if failed.

    Parameters
    ----------
    f:
        Function to retry if failed.

    Returns
    -------
    decorated:
        Decorated function.

    """
    @wraps(f)
    def decorated(*args, **kwargs):
        for _ in range(RETRY):
            try:
                result = f(*args, **kwargs)
                return result
            except Exception as e:
                print(e)
                print("Retrying ...")
        print(f"Retried {RETRY} times but all failed.")
        return result
    return decorated


# TODO: Implement function to extract transfer rate.
def get_network_transfer_rate():
    return 0


def get_parameters_dicts(**kwargs) -> List[Dict]:
    """
    Get a list of parameters dictionaries.

    Parameters
    ----------
    kwargs:

    Returns
    -------
    parameters: List[namedtuple]
        List of parameters dictionaries.

    """
    parameters = [
        dict(zip(kwargs.keys(), param))
        for param in product(*kwargs.values())
    ]
    return parameters


def save_to_db(db: AnyPath, table: str, *args) -> bool:
    """
    Save data to database.

    Parameters
    ----------
    db: AnyPath
        Path to sqlite database.
    table: str
        Name of table to save data to.
    args:
        Positional arguments.

    Returns
    -------
    bool

    """
    connection = sqlite3.connect(db)

    try:
        cursor = connection.cursor()
        # TODO: INSECURE !!! Use cursor.execute second parameters instead !!!
        values = ','.join([f'\'{arg}\'' for arg in args])
        sql = f"INSERT INTO {table} VALUES ({values})"
        cursor.execute(sql)
        connection.commit()
        connection.close()
        return True
    except Exception as e:
        print(e)
        print("Rolling back ...")
        connection.rollback()
        connection.close()
        return False


def get_file_of_size(size: int, save_dir: AnyPath) -> Path:
    """
    Create a binary file with a specific size.

    Parameters
    ----------
    size: int
        Size (in gigabyte) of the file to be created.
    save_dir: AnyPath
        Directory to save the created file.

    Returns
    -------
    created_file_path: Path
        Path to the created file.

    """
    if not isinstance(size, int) or size <= 0:
        raise ValueError(f"Expected size to be positive integer, but got "
                         f"{size} instead.")

    save_dir = Path(save_dir)
    save_dir.mkdir(parents=True, exist_ok=True)
    created_file_path = save_dir / f"{size}GB.h5"

    if created_file_path.exists():
        print(f"File {created_file_path} already exists.")
        return created_file_path
    else:
        print(f"Creating file {created_file_path} of size {size}")
        with open(created_file_path, "wb") as f:
            f.truncate(size * (1024 ** 3))
        print(f"File {created_file_path} created.")
        return created_file_path


def split_file(file: AnyPath, file_split_size: int = 0,
               file_split_chunk: int = 0,
               suffix_length: int = 4) -> List[Path]:
    """
    Split a file into smaller files. All the split files will locate in dedicated
    folder to separate from other splits (from other experiments).

    The name format of dedicated folder:
        <file_name>-<file_extension>-<file_split_size>G-splits

    The name format of split file:
        <file_name>-<file_extension>-<file_split_size>G-<number>

    Parameters
    ----------
    file: AnyPath
        Path to the file to be split.
    file_split_size: int
        Size of a split file. Measured in Gigabyte.
    file_split_chunk: int
        Number of split files.
    suffix_length: int
        Length of suffix for split files.

    Returns
    -------
    split_files: List[Path]
        List of path to split files.

    """
    if not isinstance(file_split_size, int) or file_split_size < 0:
        raise ValueError(f"Non-negataive integer file_split_size expected, "
                        f"but got {type(file_split_size)} instead.")
    if not isinstance(file_split_size, int) or file_split_chunk < 0:
        raise ValueError(f"Non-negataive integer file_split_chunk expected, "
                        f"but got {type(file_split_chunk)} instead.")
    if not isinstance(suffix_length, int) or suffix_length <= 0:
        raise TypeError(f"Positive integer suffix_length expected, but got "
                        f"{type(suffix_length)} instead.")

    file = Path(file)
    prefix = f"{file.name.replace('.', '-')}-{file_split_size}G-"

    # Create directory to store split files. This is for easy management:
    parent_folder = file.parent / (prefix + "splits")
    parent_folder.mkdir(parents=True, exist_ok=True)

    # Call linux "split" to split the file. This is for convenience and reliability.
    # Might need check for performance compared to native python code in different settings:
    if file_split_size > 0:
        cmd = f"split -d -a {suffix_length} -b {file_split_size}GB " \
              f"{file.as_posix()} {parent_folder / prefix}"
        p = subprocess.run(
            cmd.split(),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
            check=False
        )
    elif file_split_chunk > 0:
        cmd = f"split -d -a {suffix_length} -n {file_split_chunk} " \
              f"{file.as_posix()} {parent_folder / prefix}"
        p = subprocess.run(
            cmd.split(),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
            check=False
        )
    else:
        # If file_split_size is 0, then don't split the file, only move to new directory:
        file.rename(parent_folder / (prefix + "0"*suffix_length))

    # Glob all paths to split files:
    split_files = list(parent_folder.glob(f"*{prefix}*"))

    return split_files


def upload_file_swift(file: AnyPath, bucket: str, segment_size: int,
                      segment_thread: int) -> subprocess.CompletedProcess:
    """
    Upload file to cloud storage using Swift.

    Parameters
    ----------
    file: AnyPath
        Path to the file to be upload.
    bucket: str
        Bucket name.
    segment_size: int
        Size of a file segment (in Gigabyte).
    segment_thread: int
        Number of threads to upload segments.

    Returns
    -------
    result: subprocess.CompletedProcess

    """
    cmd = f"swift upload " \
          f"--use-slo " \
          f"--segment-size {segment_size}G " \
          f"--segment-threads {segment_thread} " \
          f"{bucket} " \
          f"{file}"
    result = subprocess.run(
        cmd.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        check=False
    )
    return result


def delete_bucket_swift(bucket: str) -> subprocess.CompletedProcess:
    """
    Delete bucket in cloud using Swift.

    Parameters
    ----------
    bucket: str
        Bucket name.

    Returns
    -------
    result: subprocess.CompletedProcess

    """
    cmd = f"swift delete {bucket}"
    result = subprocess.run(
        cmd.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        check=False
    )
    return result


def upload_file_s3cmd(file: AnyPath, bucket: str,
                      segment_size: int) -> subprocess.CompletedProcess:
    """
    Upload file to cloud storage using s3cmd.

    Parameters
    ----------
    file: AnyPath
        Path to the file to be upload.
    bucket: str
        Bucket name.
    segment_size: int
        Size of a file segment (in Megabyte).

    Returns
    -------
    result: subprocess.CompletedProcess

    """
    cmd = f"s3cmd mb s3://{bucket}"
    result = subprocess.run(
        cmd.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        check=False
    )

    cmd = f"s3cmd put " \
          f"--multipart-chunk-size-mb {segment_size}000 " \
          f"{file} " \
          f"s3://{bucket}"
    result = subprocess.run(
        cmd.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        check=False
    )
    return result


def delete_bucket_s3cmd(bucket: str) -> subprocess.CompletedProcess:
    """
    Delete bucket in cloud using s3cmd.

    Parameters
    ----------
    bucket: str
        Bucket name.

    Returns
    -------
    result: subprocess.CompletedProcess

    """
    cmd = f"s3cmd rb --recursive s3://{bucket}"
    result = subprocess.run(
        cmd.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        check=False
    )
    return result


def upload_file_rclone(file: AnyPath, bucket: str, segment_size: int,
                       segment_thread: int) -> subprocess.CompletedProcess:
    """
    Upload file to cloud storage using rclone.

    Parameters
    ----------
    file: AnyPath
        Path to the file to be upload.
    bucket: str
        Bucket name.
    segment_size: int
        Size of a file segment (in Megabyte).

    Returns
    -------
    result: subprocess.CompletedProcess

    """
    cmd = f"rclone copy " \
          f"--multi-thread-cutoff {segment_size}000 " \
          f"--multi-thread-streams {segment_thread} " \
          f"{file} " \
          f"allas:{bucket}"
    result = subprocess.run(
        cmd.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        check=False
    )
    return result


def delete_bucket_rclone(bucket: str) -> subprocess.CompletedProcess:
    """
    Delete bucket in cloud using rclone.

    Parameters
    ----------
    bucket: str
        Bucket name.

    Returns
    -------
    result: subprocess.CompletedProcess

    """
    cmd = f"rclone purge allas:{bucket}"
    result = subprocess.run(
        cmd.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        check=False
    )
    return result
