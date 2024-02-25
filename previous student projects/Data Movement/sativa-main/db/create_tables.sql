CREATE TABLE IF NOT EXISTS File (
    name    TEXT NOT NULL PRIMARY KEY,
    ext     TEXT NOT NULL,
    size    TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS Experiment (
    file            TEXT NOT NULL,
    version         TEXT NOT NULL,
    bucket          TEXT NOT NULL,
    cluster         TEXT NOT NULL,
    node            TEXT NOT NULL,
    tool            TEXT NOT NULL,
    file_split_size UNSIGNED BIG INT,   -- in gigabyte
    segment_size    INTEGER,            -- in gigabyte
    thread          INTEGER,
    core            INTEGER,
    transfer_rate   INTEGER,            -- in MB/s
    start_time      DATETIME,           -- in iso format
    end_time        DATETIME,           -- in iso format
    status          TEXT NOT NULL,      -- successful or failed

    FOREIGN KEY (file) REFERENCES File(name)
);

