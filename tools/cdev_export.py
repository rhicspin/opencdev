#!/usr/bin/env python2
# vim: set sw=4 ts=4 :

import datetime
import os
import subprocess
import tempfile

import Sybase


CAD_HOST = "acnlina5.pbn.bnl.gov"
CAD_SDDS_BASE = "/operations/app_store/RunData/run_fy13"
CAD_LOGREQ_BASE = "/operations/app_store/Gpm"
OUTPUT_DIR = "./run_fy13"


def fetch(conn, logreq):
    print " [>] %s" % logreq
    print "  [*] Fetching metadata ..."
    cur = conn.cursor()
    logreq_full = os.path.join(CAD_LOGREQ_BASE, logreq)
    #cur.execute("SELECT * FROM rhicFileHeaderV WHERE requestFile = @name", {'@name': logreq_full})
    cur.execute("SELECT * FROM rhicFileHeaderV WHERE requestFile = '%s'" % logreq_full)
    res = cur.fetchall()
    field_list = map(lambda x: x[0], cur.description)
    fields = ", ".join(field_list)

    sqldump_path = os.path.join(OUTPUT_DIR, "sql", "%s.sql" % logreq)
    print "  [*] Dumping metadata to %s ..." % sqldump_path
    sqldump_dir = os.path.dirname(sqldump_path)
    if not os.path.isdir(sqldump_dir):
        os.makedirs(sqldump_dir)
    with open(sqldump_path, "w") as fp:
        for row in res:
            def show(val):
                if isinstance(val, datetime.datetime):
                    return "'%s'" % val.isoformat(" ")
                else:
                    return repr(val)
            values = ", ".join(map(show, row))
            query = "INSERT INTO rhicFileHeaderV (%s) VALUES (%s)" % (fields, values)
            fp.write("%s\n" % query)

    print "  [*] Copying SDDS files ..."
    with tempfile.NamedTemporaryFile(delete=True) as filelist:
        for row in res:
            remote_path = row[field_list.index("filePath")]
            # Check remote path prefix CAD_SDDS_BASE
            assert(os.path.commonprefix([remote_path, CAD_SDDS_BASE]) == CAD_SDDS_BASE)
            local_path = remote_path[len(CAD_SDDS_BASE):]
            filelist.write("%s\n" % local_path)
        filelist.flush()
        subprocess.call(["rsync", "-ptgo", "--files-from=%s" % filelist.name, "%s:%s" % (CAD_HOST, CAD_SDDS_BASE), OUTPUT_DIR])

if __name__ == '__main__':
    print " [*] Connecting to the database ..."
    conn = Sybase.connect("localhost:5000", "harmless", "harmless", "run_fy13_fill", outputmap = Sybase.DateTimeAsPython)
    fetch(conn, "RHIC/Polarimeter/Yellow/biasReadbacks.logreq")
    fetch(conn, "RHIC/Polarimeter/Blue/biasReadbacks.logreq")
